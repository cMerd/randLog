#!/usr/bin/bash

g++ main.cpp -o randLog -Wall
sudo cp randLog /usr/bin/randLog
rm -f randLog
cp -r ./randLogConfig ~/.config/randLog

