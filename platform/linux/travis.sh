#!/bin/sh

sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo add-apt-repository 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.6 main'
sudo add-apt-repository ppa:boost-latest/ppa
wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|sudo apt-key add -

sudo apt-get purge gcc g++ llvm clang

sudo apt-get update -qq

sudo apt-get install gcc-4.9 g++-4.9 llvm-3.6 clang-3.6
sudo apt-get install libglew-dev
sudo apt-get install opencl-headers
sudo apt-get install libboost1.55-all-dev
#sudo apt-get install libfreetype6-dev

sudo update-alternatives --remove-all gcc
sudo update-alternatives --remove-all g++
sudo update-alternatives --remove-all clang
sudo update-alternatives --remove-all clang++
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 20
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.6 20
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-3.6 20
sudo update-alternatives --config gcc
sudo update-alternatives --config g++
sudo update-alternatives --config clang
sudo update-alternatives --config clang++

wget https://dl.dropboxusercontent.com/u/41666920/raycer-travis/travis.tar.gz -P /var/tmp
tar xf /var/tmp/travis.tar.gz -C /var/tmp
