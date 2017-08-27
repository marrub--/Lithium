#!/usr/bin/sh
args='-xr!*.gitignore' '-xr!*.bat' '-xr!*.dbs' '-xr!*.wad.b*'
(cd pk7      && 7za a ../Lithium.pk7 ./* $args)
(cd pk7_cpk1 && 7za a ../Lithium_cpack1.pk7 ./* $args)
