#!/usr/bin/env ruby
# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Monster info text compiler.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

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
         monsters.push({exp:   exp, scr: scr, mtype: mtype, match: match,
                        flags: flags})
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
