#!/usr/bin/env sh
if [ -f tools/ttfuck ]
then
   rm -rf tools/ttfuck
fi

mkdir tools/ttfuck
wget http://mab.greyserv.net/f/ttfuck.7z
7za x ttfuck.7z -otools/ttfuck
rm ttfuck.7z
