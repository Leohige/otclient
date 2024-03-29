/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/leohige/otclient>
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

#pragma once

#include <framework/global.h>

#include "framework/xml/tinyxml.h"
#include "itemtype.h"
#include "thingtype.h"

class ThingTypeManager
{
public:
    void init();
    void terminate();

    bool loadDat(std::string file);
    bool loadOtml(std::string file);
    void loadOtb(const std::string& file);
    void loadXml(const std::string& file);
    bool loadAppearances(const std::string& file);
    void parseItemType(uint16_t id, TiXmlElement* elem);

    void saveDat(const std::string& fileName);

    void addItemType(const ItemTypePtr& itemType);
    const ItemTypePtr& findItemTypeByClientId(uint16_t id);
    const ItemTypePtr& findItemTypeByName(const std::string& name);
    ItemTypeList findItemTypesByName(const std::string& name);
    ItemTypeList findItemTypesByString(const std::string& name);

    const ThingTypePtr& getNullThingType() { return m_nullThingType; }
    const ItemTypePtr& getNullItemType() { return m_nullItemType; }

    const ThingTypePtr& getThingType(uint16_t id, ThingCategory category);
    const ItemTypePtr& getItemType(uint16_t id);

    ThingTypeList findThingTypeByAttr(ThingAttr attr, ThingCategory category);
    ItemTypeList findItemTypeByCategory(ItemCategory category);

    const ThingTypeList& getThingTypes(ThingCategory category);
    const ItemTypeList& getItemTypes() { return m_itemTypes; }

    uint32_t getDatSignature() { return m_datSignature; }
    uint32_t getOtbMajorVersion() { return m_otbMajorVersion; }
    uint32_t getOtbMinorVersion() { return m_otbMinorVersion; }
    uint16_t getContentRevision() { return m_contentRevision; }

    bool isDatLoaded() { return m_datLoaded; }
    bool isXmlLoaded() { return m_xmlLoaded; }
    bool isOtbLoaded() { return m_otbLoaded; }

    bool isValidDatId(uint16_t id, ThingCategory category) { return id >= 1 && id < m_thingTypes[category].size(); }
    bool isValidOtbId(uint16_t id) { return id >= 1 && id < m_itemTypes.size(); }

private:
    ThingTypeList m_thingTypes[ThingLastCategory];
    ItemTypeList m_reverseItemTypes;
    ItemTypeList m_itemTypes;

    ThingTypePtr m_nullThingType;
    ItemTypePtr m_nullItemType;

    bool m_datLoaded{ false };
    bool m_xmlLoaded{ false };
    bool m_otbLoaded{ false };

    uint32_t m_otbMinorVersion{ 0 };
    uint32_t m_otbMajorVersion{ 0 };
    uint32_t m_datSignature{ 0 };
    uint16_t m_contentRevision{ 0 };
};

extern ThingTypeManager g_things;
