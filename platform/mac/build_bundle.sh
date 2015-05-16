#!/bin/sh

rm -rf Raycer.app

mkdir -p Raycer.app/Contents/MacOS
mkdir -p Raycer.app/Contents/Resources
mkdir -p Raycer.app/Contents/Frameworks

cp platform/mac/Info.plist Raycer.app/Contents
cp misc/icons/raycer.icns Raycer.app/Contents/Resources
cp -R bin/* Raycer.app/Contents/MacOS
ln -s ../MacOS/data Raycer.app/Contents/Resources/data
platform/mac/dylibbundler -od -b -x ./Raycer.app/Contents/MacOS/raycer -d ./Raycer.app/Contents/Frameworks/ -p @executable_path/../Frameworks/
