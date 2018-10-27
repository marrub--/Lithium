#!/usr/bin/env ruby
## Copyright © 2018 Alison Sanderson, all rights reserved.
## mkfont: Creates fonts and downloads necessary TTFs if needed.
require 'shellwords'

FONTS=%w(MisakiG MisakiM JFDotG)
CMAP=`tools/getcmap.rb`

for f in FONTS
   unless FileTest.exist? "tools/#{f}.ttf"
      `wget 'http://mab.greyserv.net/f/#{f}.ttf' -O tools/#{f}.ttf`
   end

   system %W(tools/ttfuck/ttfuck.exe tools/#{f}.ttf 8 #{CMAP} pk7/lfiles/Font_#{f}.txt pk7/lgfx/Font/#{f}).shelljoin
end

## EOF
