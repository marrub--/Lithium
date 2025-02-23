## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

check_uniq "fake dependencies", @ctx.fake

@ctx.fp << <<ninja
build fakein: phony
build fakedeps: phony || #{@ctx.fake.join " "}
default fakedeps
ninja

## EOF
