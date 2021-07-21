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
## Generates the trie file.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"
require "yaml"
require "rambling-trie"

ENUMS = []
FUNCS = []

def ind indent
   "   " * indent
end

def mk_trie_code defs, node, idx = 0, indent = 1, res = String.new, word = String.new
   switch = node.each.all? do |child|
      !child.terminal? && child.letter.length == 1
   end
   pfx  = defs[:prefix] || defs[:enum]
   node = node.sort do |a, b| a.letter <=> b.letter end

   if switch
      res.concat "#{ind indent}switch(s[#{idx}]) {\n"
      idx += 1
   end

   for child in node
      orig_idx  = idx
      orig_word = word.clone

      ltr = child.letter.to_s
      word.concat ltr

      if switch
         res.concat "#{ind indent}case '#{child.letter}':\n"
      else
         ltr = ltr.each_char.collect do |c| c end
         ltr.push '\0' if child.terminal?

         res.concat "#{ind indent}if("
         chrs = ltr.each_with_index.map do |c, i|
            s = "s[#{idx}] == '#{c}'"
            idx += 1
            s
         end
         chrs = chrs.join " &&\n#{ind indent + 1}"
         res.concat chrs
         res.concat ") {\n"
      end

      if child.terminal?
         en = word.clone

         if pfx
            en.prepend pfx
            ENUMS.push en unless defs[:enum]
         end

         res.concat "#{ind indent + 1}return #{en};\n"
      else
         mk_trie_code defs, child.children, idx, indent + 1, res, word
      end

      res.concat "#{ind indent}}\n" unless switch

      word.replace orig_word
      idx = orig_idx
   end

   if switch
      res.concat "#{ind indent}}\n"
      idx -= 1
   end

   res
end

common_main do
   hsh = YAML.load(STDIN.read, symbolize_names: true)

   for func, defs in hsh
      trie = Rambling::Trie.create
      trie.concat defs[:words]
      trie.compress!
      code = mk_trie_code defs, trie.children
      func = "stkcall\ni32 #{func}(register cstr s)"
      FUNCS.push [func, code]
   end

   open(ARGV.shift, "wt").puts <<_end_h_; open(ARGV.shift, "wt").puts <<_end_c_
#{generated_header "trie"}

enum {
   #{ENUMS.join ",\n   "}
};

#{
res = String.new
for func in FUNCS
   res.concat func[0]
   res.concat ";\n"
end
res
}
/* EOF */
_end_h_
#{generated_header "trie"}
#include "common.h"
#include "p_player.h"
#include "w_monster.h"
#include "m_trie.h"

#{
res = String.new
for func in FUNCS
   res.concat "alloc_aut(0) "
   res.concat func[0]
   res.concat " {\n"
   res.concat func[1]
   res.concat "   return -1;\n"
   res.concat "}\n"
end
res
}
/* EOF */
_end_c_
end

## EOF
