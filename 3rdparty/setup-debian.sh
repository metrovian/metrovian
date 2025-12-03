#!/bin/bash
git submodule foreach --recursive "git reset --hard"
git submodule foreach --recursive "git clean -fd"
git submodule update --init --recursive
sudo apt update
sudo apt install \
        nlohmann-json3-dev \
        libasound2-dev \
        libcli11-dev \
        libeigen3-dev \
        libmuparser-dev \
        libmicrohttpd-dev \
        libspdlog-dev \
        libavcodec-dev \
        libavformat-dev \
        libavutil-dev \
        libswscale-dev \
        libswresample-dev \
        libgpiod-dev \