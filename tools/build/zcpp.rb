## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

zcpp = [
   {i: "items.h",        z: "items.zsc"},
   {i: "m_drawing.h",    z: "m_drawing.zsc"},
   {i: "m_version.h",    z: "m_version.zsc"},
   {i: "p_player.h",     z: "p_player.zsc"},
   {i: "p_weaponinfo.h", z: "p_weaponinfo.zsc"},
   {i: "u_names.h",      z: "u_names.zsc"},
   {i: "w_monster.h",    z: "w_monster.zsc"},
   {i: "w_world.h",      z: "w_world.zsc"},
]
@ctx.fp << <<ninja
rule zcpp
 command = gdcc-cpp -o $out #{CPP_ARG} -DZscOn=1 -D'ZscName(x, ...)=Lith_##x' $in
 description = [gdcc-cpp] Pre-process to ZScript
ninja

for ent in zcpp
   i = hdr ent[:i]
   z = zsc ent[:z]

   @ctx.fp << "build #{z}: zcpp #{i} | fakein\n"
   @ctx.fake.push z
end

## EOF
