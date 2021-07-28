#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## General validity checker.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"
require "set"

def exclude_lang?(ali, txt) =
    txt =~ /\$.*/                  ||
    ali =~ /LITH_st_name_.*/       ||
    ali =~ /LITH_INFO_SSIZE_.*/    ||
    ali =~ /LITH_PK_GET_..._FLAGS/ ||
    ali =~ /LITH_PK_.*_NUM/        ||
    ali =~ /BGFLAT../

common_main do
   errors = []
   # check language strings for duplicates
   language = {}

   curlang = nil
   IO.readlines("pk7/language.txt").each do |ln|
      case ln
      when /^"([^"]*)" = "(.*)";$/
         language[curlang][$1.to_sym] = $2
      when /^\[([a-z ]*)\]$/
         curlang = $1.to_sym
         language[curlang] = {}
      end
   end

   duplicate_cross = Set[]
   duplicate_content = {}
   for lang, defs in language
      dupes = duplicate_content[lang] = {}
      for ali, txt in defs
         # check for duplicate cross-language definitions, text and all
         for olang, odefs in language
            duplicate_cross.add ali if olang != lang && odefs[ali] == txt
         end

         # check for duplicate text among the same language
         unless exclude_lang? ali, txt
            dupes[txt] = [] unless dupes[txt]
            dupes[txt].push ali
         end
      end
   end

   for dupe in duplicate_cross.to_a.sort
      errors.push "duplicate cross-language def: \"#{dupe}\""
   end

   for lang, dupes in duplicate_content.sort
      for txt, aliases in dupes.sort
         if aliases.length > 1
            errors.push "duplicate in-language def (#{lang}):\n\"" +
                        aliases.sort.join("\"\n\"") +
                        "\""
         end
      end
   end

   if errors.empty?
      puts "no errors detected"
   else
      puts errors.sort.join "\n"
      exit false
   end
end

## EOF
