#!/bin/sh

mkdir tmp
cd tmp

cp ../Release/*.exe .
cp ../README.md README.txt

zip -9 ../AdvancedWindowsHotkeys.zip *

cd ..
rm -rf tmp
