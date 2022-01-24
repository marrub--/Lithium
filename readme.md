![Lithium](https://mab.greyserv.net/f/Lithium_logo.png)

# Running

If you wish to run an in-development version of the mod, you can click
the "Clone or download" button and then "Download ZIP".

Once you've done that you can then load the `pk7` folder into ZDoom as
you would any other mod.

# Code

The C code in `source/` is compiled to ACS bytecode with a customized
version of GDCC. It can be rather terse at times, and is scarcely
commented, but it is hopefully understandable on its own. All text
sources are CC0 public domain unless otherwise specified.

To compile, generate a build file with `tools/genbuild.rb` and run
`ninja`.

Build requirements:

- Ninja
- Ruby
- [rambling-trie](https://rubygems.org/gems/rambling-trie)
- [GDCC](https://github.com/marrub--/GDCC)

Requirements for `tools/mkfont.rb`:

- wget
- ImageMagick
- OptiPNG
- PNGQuant
- AdvPNG

# Licensing

Respect content creators and do not use the content of this mod
elsewhere without permission. See [credits.txt](credits.txt) for full
licensing information and attribution.
