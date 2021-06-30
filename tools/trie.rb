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

require 'rambling-trie'
require_relative "corinth.rb"

OUT = ARGV.shift
TRIES = []
Trie = Struct.new :name, :prefix, :words

common_main do
   trie_file = STDIN.read
   puts trie_file

=begin
   trie = Rambling::Trie.create

   fp = open OUT, "wt"
   fp.puts generated_header "trie"
   fp.puts <<_c_
_c_
=end
end

## EOF
