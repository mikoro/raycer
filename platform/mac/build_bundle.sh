#!/bin/sh

rm -rf Raycer.app

mkdir -p Raycer.app/Contents/MacOS
mkdir -p Raycer.app/Contents/Resources
mkdir -p Raycer.app/Contents/Libs

cp platform/mac/Info.plist Raycer.app/Contents
cp -R data Raycer.app/Contents/Resources
cp misc/icons/raycer.icns Raycer.app/Contents/Resources
cp misc/settings.ini Raycer.app/Contents/Resources
cp bin/raycer Raycer.app/Contents/MacOS

platform/mac/dylibbundler -od -b -x ./Raycer.app/Contents/MacOS/raycer -d ./Raycer.app/Contents/Libs/ -p @executable_path/../Libs/
chmod a+x Raycer.app/Contents/Libs/*
