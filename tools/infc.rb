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
## Info page text compiler.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

common_main do
   tks = tokenize ARGV.shift

   cat = {}
   pgs = nil

   loop do
      tok = tks.next.expect_in_top [:semico, :plus, :eof]

      case tok.type
      when :semico
         tok = tks.next.expect_after tok, :identi
         aut = tks.drop? :period
         pgs = {}
         cat[tok.text] = {pgs: pgs, aut: aut}
      when :plus
         pcl = []
         tks.while_drop :bar do
            tok = tks.next.expect_after tok, :identi
            pcl.push tok.text
         end
         pcl = pcl.join " | "
         tok = tks.next.expect_after tok, :identi
         nam = tok.text
         aut = tks.drop? :period
         unl = []
         if tks.drop? :colon
            while tks.peek.type == :identi
               tok = tks.next
               unl.push tok.text
            end
         end
         pgs[nam] = {pcl: pcl, aut: aut, unl: unl}
      when :eof
         break
      end
   end

   open(ARGV.shift, "wt").puts <<_end_h_; open(ARGV.shift, "wt").puts <<_end_c_
#{generated_header "infc"}

#if defined(Categ)
#{
res = String.new
for cna, cda in cat
   res.concat %(Categ(#{cna})\n)
end
res
}
#undef Categ
#elif !defined(p_bipname_h)
#define p_bipname_h

enum {
#{
res = String.new
for cna, cda in cat
   for pna, pag in cda[:pgs]
      res.concat %(   BIP_#{cna}_#{pna},\n)
   end
end
res
}
   BIP_MAX,
};

enum {
   #define Categ(name) BIPC_##name,
   #include "p_bipname.h"
   BIPC_MAX,
   BIPC_MAIN,
   BIPC_SEARCH,
};

#endif
_end_h_
#{generated_header "infc"}

#include "p_bip.h"
#include "p_player.h"

/* Extern Objects ---------------------------------------------------------- */

struct pageinfo bipinfo[BIP_MAX] = {
#{
res = String.new
for cna, cda in cat
   for pna, pag in cda[:pgs]
      unl = pag[:unl].map do |s| '"' + s + '"' end.join ","
      res.concat %(   {"#{pna}", BIPC_#{cna}, #{pag[:pcl]}, #{cda[:aut] || pag[:aut]}, {#{unl}}},\n)
   end
end
res
}
};

/* EOF */
_end_c_
end

## EOF
