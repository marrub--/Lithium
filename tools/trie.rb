#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Generates the trie file.                                                 │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

require_relative "corinth.rb"
require "yaml"
require "rambling-trie"

def ind indent
   "   " * indent
end

def mk_trie_code out, defs, node, idx = 0, indent = 1, word = String.new
   switch = node.each.all? do |child|
      !child.terminal? && child.letter.length == 1
   end
   pfx  = defs[:prefix] || defs[:enum]
   node = node.sort do |a, b| a.letter <=> b.letter end
   if switch
      out[:code].concat "#{ind indent}switch(s[#{idx}]) {\n"
      idx += 1
   end
   for child in node
      orig_idx  = idx
      orig_word = word.clone
      ltr = child.letter.to_s
      word.concat ltr
      if switch
         out[:code].concat "#{ind indent}case '#{child.letter}':\n"
      else
         ltr = ltr.each_char.collect do |c| c end
         ltr.push '\0' if child.children.empty?
         out[:code].concat "#{ind indent}if("
         chrs = ltr.each_with_index.map do |c, i|
            s = "s[#{idx}] == '#{c}'"
            idx += 1
            s
         end
         chrs = chrs.join " &&\n#{ind indent + 1}"
         out[:code].concat chrs
         out[:code].concat ") {\n"
      end
      if child.terminal?
         al = defs[:aliases] && defs[:aliases][word.to_sym]
         en = al || word.clone
         if pfx
            en.prepend pfx
            out[:enum].push en unless defs[:enum] || al
         end
         if child.children.empty?
            out[:code].concat "#{ind indent + 1}return #{en};\n"
         else
            out[:code].concat "#{ind indent + 1}if(s[#{idx}] == '\\0') {\n"
            out[:code].concat "#{ind indent + 2}return #{en};\n"
            out[:code].concat "#{ind indent + 1}}\n"
         end
      end
      unless child.children.empty?
         mk_trie_code out, defs, child.children, idx, indent + 1, word
      end
      if switch
         out[:code].concat "#{ind indent + 1}break;\n"
      else
         out[:code].concat "#{ind indent}}\n"
      end
      word.replace orig_word
      idx = orig_idx
   end
   if switch
      out[:code].concat "#{ind indent}}\n"
      idx -= 1
   end
end

common_main do
   hsh = YAML.load(STDIN.read, symbolize_names: true)
   all_outputs = hsh.map do |func, defs|
      words = []
      words.concat(defs[:words])                    if defs[:words]
      words.concat(defs[:aliases].keys.map(&:to_s)) if defs[:aliases]
      out  = {enum: [], code: String.new, func: "stkcall i32 #{func}(register cstr s)"}
      trie = Rambling::Trie.create
      trie.concat words.sort
      trie.compress!
      mk_trie_code out, defs, trie.children
      out
   end
   open(ARGV.shift, "wt").puts <<_end_h_; open(ARGV.shift, "wt").puts <<_end_c_
#{generated_header "trie"}

#{
   res = String.new
   all_outputs.each do |out|
      unless out[:enum].empty?
         res.concat "enum {\n"
         out[:enum].each do |en|
            res.concat "   #{en},\n"
         end
         res.concat "};\n"
      end
      res.concat "#{out[:func]};\n"
   end
   res
}
/* EOF */
_end_h_
#{generated_header "trie"}
#include "m_engine.h"
#include "p_player.h"
#include "w_monster.h"
#include "m_trie.h"

#{
   res = String.new
   all_outputs.each do |out|
      res.concat "alloc_aut(0) #{out[:func]} {\n"
      res.concat out[:code]
      res.concat "   return -1;\n"
      res.concat "}\n"
   end
   res
}
/* EOF */
_end_c_
end

## EOF
