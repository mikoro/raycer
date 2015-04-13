#!/bin/sh

if [ ! -d bin/data ]; then
  cp -R data bin/
  mkdir -p bin/data/kernels/
  cp src/GpuRaytracing/*.cl bin/data/kernels/
fi

if [ ! -f bin/settings.ini ]; then
  cp misc/settings.ini bin/
fi
