#!/bin/sh

if [ ! -d bin/data ]; then
  cp -R data bin/
  mkdir -p bin/data/opencl/
  cp src/CLRaytracing/*.cl bin/data/opencl/
fi

if [ ! -f bin/settings.ini ]; then
  cp misc/settings.ini bin/
fi
