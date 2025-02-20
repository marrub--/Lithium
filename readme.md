![Lithium](https://mab.greyserv.net/f/Lithium_logo.png)

# Running

If you wish to run an in-development version of the mod, you can click
the "Code" button at the top of the page and then "Download ZIP". Once
you've done that, and extracted the ZIP file, you can then load the
`pk7` folder into GZDoom as you would any other mod.

If you want to load a single file instead of a folder, for instance if
you're using Delta Touch, zip the contents of the `pk7` folder and
optionally [change the
extension](https://zdoom.org/wiki/Using_ZIPs_as_WAD_replacement) to
create a `pk3` file.

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

- Ruby
- ImageMagick
- [RMagick](https://rmagick.github.io/)
- OptiPNG

# Licensing

Respect content creators and do not use the content of this mod
elsewhere without permission. See [credits.txt](credits.txt) for full
licensing information and attribution.
