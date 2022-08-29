FROM ubuntu:22.04 AS builder

RUN export DEBIAN_FRONTEND=noninteractive \
	&& ln -fs /usr/share/zoneinfo/America/Sao_Paulo /etc/localtime

RUN apt-get update && apt-get install -y --no-install-recommends \
	git cmake curl zip unzip tar automake ca-certificates build-essential \
	libglew-dev libx11-dev autoconf libtool pkg-config tzdata python3 \
	&& dpkg-reconfigure --frontend noninteractive tzdata \
	&& apt-get clean && apt-get autoclean

RUN pip3 install wheel && pip3 install conan

COPY ./ /otclient/
WORKDIR /otclient/build

RUN conan install -s compiler.libcxx=libstdc++11 -s build_type=Release .. --build=missing
RUN cmake ..
RUN cmake --build . -j$(nproc) --config Release

FROM ubuntu:22.04

COPY --from=builder /otclient /otclient
COPY ./data/ /otclient/data/.
COPY ./mods/ /otclient/mods/.
COPY ./modules/ /otclient/modules/.
COPY ./init.lua /otclient/.
WORKDIR /otclient
CMD ["./otclient"]
