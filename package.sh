#!/usr/bin/sh
args='-xr!*.gitignore -xr!*.bat -xr!*.dbs -xr!*.wad.b* -tzip'
(cd pk7      && 7za a ../Lithium.pk3 ./* $args)
(cd pk7_cpk1 && 7za a ../Lithium_cpack1.pk3 ./* $args)
