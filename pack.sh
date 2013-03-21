#!/bin/sh

mkdir tmp
cd tmp

cp ../Release/*.exe .
cp ../index.html .
cp -R ../Help .

zip -9 ../AdvancedWindowHotKeys.zip *
zip -9 ../AdvancedWindowHotKeys.zip Help/*.png
zip -9 ../AdvancedWindowHotKeys.zip Help/*.css


cd ..
rm -rf tmp
