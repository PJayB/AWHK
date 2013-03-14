#!/bin/sh

mkdir tmp
cd tmp

cp ../Release/*.exe .
cp ../README.md README.txt

zip -9 AdvancedWindowsHotkeys.zip *

scp AdvancedWindowsHotkeys.zip pjblewis@pjblewis.com:~/gibbering.net/

cd ..
rm -rf tmp
