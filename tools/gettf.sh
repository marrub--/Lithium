#!/usr/bin/env sh
rm -rf tools/ttfuck
mkdir tools/ttfuck
wget http://mab.greyserv.net/f/ttfuck.7z
7za x ttfuck.7z -otools/ttfuck
rm ttfuck.7z
