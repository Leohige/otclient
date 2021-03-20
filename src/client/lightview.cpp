/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "lightview.h"
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/image.h>
#include <framework/graphics/painter.h>
#include "mapview.h"
#include "map.h"

#define DEBUG_BUBBLE 0

LightView::LightView(const MapViewPtr& mapView)
{
    m_mapView = mapView;
    m_lightbuffer = g_framebuffers.createFrameBuffer();

    generateLightTexture();
    generateBorderTexture();

    reset();
    resize();
}

void LightView::generateBorderTexture()
{
    const uint8 intensityVariant = 0xFF;
    const float centerFactor = .5;

    const uint16 borderRadius = 4,
        centerRadius = borderRadius * centerFactor,
        bubbleDiameter = borderRadius * 2;

    ImagePtr lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));

    for(int_fast16_t x = -1; ++x < bubbleDiameter;) {
        for(int_fast16_t y = -1; ++y < bubbleDiameter;) {
            const float radius = (borderRadius - x) * (borderRadius - x) + (borderRadius - y) * (borderRadius - y);
            const float intensity = stdext::clamp<float>((borderRadius - radius) / static_cast<float>(borderRadius - centerRadius), .0, 1.);
            uint8_t pixel[4] = { 255, 255, 255, static_cast<uint8>(intensity < .05 ? 0 : 255) };
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_borderTexture = TexturePtr(new Texture(lightImage, true));
    m_borderTexture->setSmooth(true);
}

void LightView::generateLightTexture()
{
    const uint8 intensityVariant = 0xFF;
    const float centerFactor = .0;

    const uint16 bubbleRadius = 5,
        centerRadius = bubbleRadius * centerFactor,
        bubbleDiameter = bubbleRadius * 2;

    ImagePtr lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));

    for(int_fast16_t x = -1; ++x < bubbleDiameter;) {
        for(int_fast16_t y = -1; ++y < bubbleDiameter;) {
            const float radius = std::sqrt((bubbleRadius - x) * (bubbleRadius - x) + (bubbleRadius - y) * (bubbleRadius - y));
            const float intensity = stdext::clamp<float>((bubbleRadius - radius) / static_cast<float>(bubbleRadius - centerRadius), .0f, 1.0f);

            // light intensity varies inversely with the square of the distance
            const uint8_t colorByte = std::min<float>(intensity * intensity, 0.7) * intensityVariant;

            uint8_t pixel[4] = { 255, 255, 255, colorByte };
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_lightTexture = TexturePtr(new Texture(lightImage, true));
    m_lightTexture->setSmooth(true);
}

void LightView::addLightSource(const Point& mainCenter, float scaleFactor, const Light& light, const bool isMoving)
{
    const uint8 intensity = std::min<uint8>(light.intensity, MAX_LIGHT_INTENSITY);

#if DEBUG_BUBBLE == 1
    const float extraRadius = 1;
#else
    const float extraRadius = intensity > 1 ? 2.5 : 1.1;
#endif

    const uint16 radius = (Otc::TILE_PIXELS * scaleFactor) * extraRadius;
    const auto& dimension = getDimensionConfig(intensity);
    for(const auto& position : dimension.positions)
    {
        const Point& center = mainCenter + (position.point * m_mapView->m_tileSize);

        auto& lightPoint = getLightPoint(center);
        if(!lightPoint.isValid) continue;

        float brightness = position.brightness;

        bool gotoNextLight = false;
        for(auto& prevLight : lightPoint.floors[m_currentFloor]) {
            if(prevLight.color == light.color && prevLight.center == center) {
                prevLight.brightness = std::max<float>(prevLight.brightness, brightness);
                gotoNextLight = true;
                break;
            }
        }

        if(gotoNextLight) continue;


        lightPoint.floors[m_currentFloor].push_back({ center , light.color, brightness , radius, isMoving });
    }
}

const DimensionConfig& LightView::getDimensionConfig(const uint8 intensity)
{
    auto& dimension = m_dimensionCache[intensity - 1];
    if(dimension.positions.empty()) {
        const uint8 size = std::max<int>(1, std::floor(static_cast<float>(intensity) / 1.1)),
            middle = (size / 2);
        const int8 start = size * -1;

        // TODO: REFATORATION REQUIRED
        // Ugly algorithm
        {
#if DEBUG_BUBBLE == 1
            const float startBrightness = 3;
#else
            const float startBrightness = intensity == 1 ? .15 : .5 + (static_cast<float>(intensity) / 20);
#endif

            auto pushLight = [&](const int8 x, const int8 y) -> void {
                const float brightness = startBrightness - ((std::max<float>(std::abs(x), std::abs(y))) / intensity);
                dimension.positions.push_back({ x, y, brightness });
            };

            uint8 i = 1;
            for(int_fast8_t x = start; x < 0; ++x) {
                for(int_fast8_t y = i * -1; y <= i; ++y) {
                    if(x == start || y == start || y == size) continue;
                    pushLight(x, y);
                }
                ++i;
            }

            i = 1;
            for(int_fast8_t x = size; x >= 0; --x) {
                for(int_fast8_t y = i * -1; y <= i; ++y) {
                    if(y >= size || y <= start || x == size) continue;
                    pushLight(x, y);
                }
                ++i;
            }
        }

        for(auto& pos : dimension.positions)
        {
            for(const auto& posAround : pos.getPositionsAround())
            {
                if(std::find(dimension.positions.begin(), dimension.positions.end(), posAround) == dimension.positions.end()) {
                    dimension.edges.push_back(pos);
                    pos.isEdge = true;
                    break;
                }
            }
        }
    }

    return dimension;
}

static LightPoint INVALID_LIGHT_POINT(false);
LightPoint& LightView::getLightPoint(const Point& point)
{
    size_t index = (m_mapView->m_drawDimension.width() * (point.y / m_mapView->m_tileSize)) + (point.x / m_mapView->m_tileSize);
    if(index >= m_lightMap.size()) return INVALID_LIGHT_POINT;
    return m_lightMap[index];
}

void LightView::resetBrightness(const Point& point)
{
    auto& lightPoint = getLightPoint(point);
    if(lightPoint.isValid)
        lightPoint.resetBrightness = { m_currentFloor, point };
}

void LightView::drawLights()
{
    // GlobalLight
    Color globalColor = Color::from8bit(m_globalLight.color);
    g_painter->setCompositionMode(Painter::CompositionMode_Replace);
    {
        const float brightness = m_globalLight.intensity / static_cast<float>(MAX_AMBIENT_LIGHT_INTENSITY);
        globalColor.setRed(globalColor.rF() * brightness);
        globalColor.setGreen(globalColor.gF() * brightness);
        globalColor.setBlue(globalColor.bF() * brightness);
        g_painter->setColor(globalColor);
    }
    g_painter->drawFilledRect(Rect(0, 0, m_lightbuffer->getSize()));

    // Lights
    g_painter->setCompositionMode(Painter::CompositionMode_Normal);
    for(int_fast8_t z = m_mapView->m_floorMax; z >= m_mapView->m_floorMin; --z) {
        if(z < m_mapView->m_floorMax) {
            g_painter->setBlendEquation(Painter::BlendEquation_Add);
            for(LightPoint& lightPoint : m_lightMap) {
                const auto& floor = lightPoint.resetBrightness.first;
                if(floor != z) continue;

                const auto& point = lightPoint.resetBrightness.second;
                bool isEdge = false;
                for(auto& pointAround : point.getPointsAround(m_mapView->m_tileSize)) {
                    const auto& lightPointAround = getLightPoint(pointAround);
                    if(lightPointAround.resetBrightness.first != z) {
                        isEdge = true;
                        break;
                    }
                }

                g_painter->setColor(globalColor);
                if(isEdge) {
                    const uint8 size = 61 * (m_mapView->m_renderScale / 100);
                    const Rect dest = Rect((point + (m_mapView->m_tileSize / 4)) - Point(size, size), Size(size * 2, size * 2));
                    g_painter->drawTexturedRect(dest, m_borderTexture);
                } else {
                    g_painter->drawFilledRect(Rect(point, Size(m_mapView->m_tileSize, m_mapView->m_tileSize)));
                }
            }
        }

        g_painter->setBlendEquation(Painter::BlendEquation_Add);
        for(auto& lightPoint : m_lightMap) {
            const bool isCovered = lightPoint.isCovered(z);
            for(const auto& light : lightPoint.floors[z]) {
                if(isCovered && !light.isMoving) continue;

                const Rect dest = Rect(light.center - Point(light.radius, light.radius), Size(light.radius * 2, light.radius * 2));
                const auto brightness = light.brightness - ((m_globalLight.intensity / static_cast<float>(MAX_AMBIENT_LIGHT_INTENSITY) / 4));

                Color color = Color::from8bit(light.color);
                color.setAlpha(brightness);

                g_painter->setColor(color);
                g_painter->drawTexturedRect(dest, m_lightTexture);
            }
        }
    }

    for(LightPoint& lightPoint : m_lightMap) {
        lightPoint.reset();
    }
}

void LightView::resize()
{
    m_lightbuffer->resize(m_mapView->m_frameCache.tile->getSize());
    m_lightMap.resize(m_mapView->m_drawDimension.area());
}

void LightView::draw(const Rect& dest, const Rect& src)
{
    // draw light, only if there is darkness
    if(!isDark() || m_lightbuffer->getTexture() == nullptr) return;

    g_painter->saveAndResetState();
    if(m_lightbuffer->canUpdate()) {
        m_lightbuffer->bind();
        drawLights();
        m_lightbuffer->release();
    }
    g_painter->setCompositionMode(Painter::CompositionMode_Light);

    m_lightbuffer->draw(dest, src);
    g_painter->restoreSavedState();
}