# Download base image ubuntu 22.04
FROM ubuntu:22.04
VOLUME /home/external
CMD chmod a+x /home/external
ARG BUILD_TYPE
ENV BUILD_TYPE=${BUILD_TYPE}
ENV DIRPATH=/
ENV DIRNAME=application
ENV WORKDIR=${DIRPATH}${DIRNAME}
ENV APP_ROOT=${WORKDIR}
ENV BUILD_ROOT=${APP_ROOT}/build

RUN apt-get update && \
apt-get install -y cmake \
                   git \
                   vim \
                   build-essential \
                   python3 \
                   python3.10-distutils \
                   python3-dev \
                   libgtk2.0-dev \
                   libboost-all-dev \
                   pkg-config && \
git clone --depth=1 https://github.com/talvezu/ForMontee.git ${APP_ROOT} && \
mkdir ${APP_ROOT}/build &&\
sed -i 's/GIT_TAG v2.7.1/GIT_TAG 787d2c88cafa4d07fb38c9519c485a86323cfcf4 # v2.7.1/g' ${APP_ROOT}/app/utils/CMakeLists.txt

WORKDIR ${BUILD_ROOT}
RUN cmake .. && \
cmake --build . --target unit_gtests && \
cmake --build . --target main -j$(nproc)
