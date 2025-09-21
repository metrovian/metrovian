#!/bin/bash

git submodule foreach --recursive "git reset --hard"
git submodule foreach --recursive "git clean -fd"
git submodule update --init --recursive

sudo apt update
sudo apt install \
        libssl-dev \
        libgsl-dev \
        libgmp-dev \
        libasound2-dev \
        libavcodec-dev \
        libavformat-dev \
        libavutil-dev \
        libswscale-dev \
        libswresample-dev \