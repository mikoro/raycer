#!/bin/sh

if [ ! -d bin/data ]; then
  cp -R data bin/
fi

if [ ! -f bin/settings.ini ]; then
  cp misc/settings.ini bin/
fi
