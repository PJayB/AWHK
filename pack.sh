#!/bin/sh

mkdir tmp
cd tmp

cp ../Release/*.exe .
cp ../README.md README.txt

zip -9 ../AdvancedWindowHotkeys.zip *

cd ..
rm -rf tmp
