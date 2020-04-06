#!/usr/bin/env ruby
# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Monster info text compiler.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

ChColors =
   %w"Common Green Blue Cyan Purple Yellow FireBlu Red Gray Abyss Black White"

def ch_stuff monsters, expscr, mtype, match
   for cr in ChColors
      monsters.push({exp:     "Exp_" + expscr,
                     scr:     "Score_" + expscr,
                     mtype:   mtype,
                     match:   cr + match,
                     flags:   %w"mif_full",
                     exclude: true})
   end
end

common_main do
   tks = tokenize ARGV.shift

   flags    = []
   enums    = []
   monsters = []

   loop do
      tok = tks.next.expect_in_top [:plus, :modulo, :assign, :eof]

      case tok.type
      when :plus
         tok = tks.next.expect_after tok, [:identi, :number]
         exp = nil
         scr = nil
         if tok.type == :number
            exp = tok.text
            tok = tks.next.expect_after tok, :number
            scr = tok.text
         else
            exp = "Exp_" + tok.text
            scr = "Score_" + tok.text
         end
         tok = tks.next.expect_after tok, :identi
         mtype = tok.text
         tok = tks.next.expect_after tok, :identi
         match = tok.text
         exclude = tks.drop? :dollar
         monsters.push({exp:     exp,
                        scr:     scr,
                        mtype:   mtype,
                        match:   match,
                        flags:   flags,
                        exclude: exclude})
      when :modulo
         flags = []
         if tks.peek.type == :identi
            tks.while_drop :bar do
               tok = tks.next.expect_after tok, :identi
               flags.push tok.text
            end
         end
      when :assign
         tok = tks.next.expect_after tok, :identi
         enums.push tok.text
      when :eof
         break
      end
   end

   ch_stuff monsters, "Revenant",      "revenant",      "Revenant"
   ch_stuff monsters, "ZombieMan",     "zombie",        "Zombie"
   ch_stuff monsters, "Imp",           "imp",           "Imp"
   ch_stuff monsters, "ShotgunGuy",    "zombiesg",      "SG"
   ch_stuff monsters, "Demon",         "demon",         "Demon"
   ch_stuff monsters, "Spectre",       "demon",         "Spectre"
   ch_stuff monsters, "ChaingunGuy",   "zombiecg",      "CGuy"
   ch_stuff monsters, "Cacodemon",     "cacodemon",     "Caco"
   ch_stuff monsters, "HellKnight",    "hellknight",    "HK"
   ch_stuff monsters, "Arachnotron",   "arachnotron",   "SP1"
   ch_stuff monsters, "Mancubus",      "mancubus",      "Fatso"
   ch_stuff monsters, "Archvile",      "archvile",      "Arch"
   ch_stuff monsters, "BaronOfHell",   "baron",         "Baron"
   ch_stuff monsters, "LostSoul",      "lostsoul",      "LSoul"
   ch_stuff monsters, "PainElemental", "painelemental", "PE"
   ch_stuff monsters, "CyberDemon",    "cyberdemon",    "Cybie"
   ch_stuff monsters, "SpiderDemon",   "mastermind",    "Mind"

   for mon in monsters
      cnt = monsters.count do |x|
         if x[:exclude]
            false
         else
            x[:match].include? mon[:match]
         end
      end

      if cnt > 1
         raise "string `#{mon[:match]}' appears multiple times in monsters"
      end
   end

   open(ARGV.shift, "wt").puts <<_end_
#{generated_header "monc"}
enum {
#{
res = String.new
for enum, i in enums.each_with_index
   res.concat "   #{enum} = 1 << #{i},\n"
end
res
}
};

StrEntON
static struct monster_info const monsterinfo[] = {
#{
res = String.new
for mon in monsters
   res.concat %(   {#{mon[:exp]}, #{mon[:scr]}, mtype_#{mon[:mtype]}, "#{mon[:match]}")
   unless mon[:flags].empty?
      res.concat ", " + mon[:flags].join("|")
   end
   res.concat "},\n"
end
res
}
};
StrEntOFF

/* EOF */
_end_
end

## EOF
