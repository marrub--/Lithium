#!/usr/bin/env sh
args='-xr!*.gitignore -xr!*.bat -xr!*.dbs -xr!*.wad.b* -tzip'
(cd pk7 && 7za a ../Lithium.pk3 ./* $args)
