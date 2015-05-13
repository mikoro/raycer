#!/bin/sh

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -y 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.6 main'
sudo add-apt-repository -y ppa:boost-latest/ppa
wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|sudo apt-key add -

sudo apt-get autoremove --purge -y gcc g++

sudo apt-get update -qq

sudo apt-get install -y gcc-4.9 g++-4.9 llvm-3.6 clang-3.6
sudo apt-get install -y libglew-dev
sudo apt-get install -y opencl-headers
sudo apt-get install -y libboost1.55-all-dev

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 20
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.6 20
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-3.6 20

wget https://dl.dropboxusercontent.com/u/41666920/raycer-travis/travis.tar.gz -P /var/tmp
tar xf /var/tmp/travis.tar.gz -C /var/tmp
