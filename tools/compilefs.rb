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
## Formatted text → LANGUAGE processor.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

Alias      = Struct.new :name, :text
Language   = Struct.new :name, :data
ParseState = Struct.new :cwd, :out, :langs

def escape text
   text.gsub(/((?<m>\\)(?!c))|(?<m>")/, "\\\\\\k<m>").gsub("\n", "\\n")
end

def split_name name
   name.strip.split "/"
end

def unsplit_name state, name
   (state.cwd + name).join "/"
end

def parse_file state, filename, language
   readname = filename[-1]
   dirname  = filename[0..-2]

   state.cwd.push *dirname

   lns = read_lines unsplit_name state, [readname]
   lan = state.langs[language]

   buf       = nil
   close_buf = nil

   do_close_buf = lambda do
      close_buf.call if close_buf
      buf       = nil
      close_buf = nil
   end

   for ln, linenum in lns.each_with_index
      ln = ln.chomp.gsub "\\#", "LITH_"
      case ln
      when /^## .+$/
         do_close_buf.call
      when /^== (.+)\|(.*)$/
         m = $~
         do_close_buf.call
         name = m[1].strip
         text = m[2].strip
         lan.data.push Alias.new name, text
      when /^%% (.+)$/
         m = $~
         do_close_buf.call
         name = m[1].strip
         buf  = []
         close_buf = lambda do
            text = buf.join "\n"
            lan.data.push Alias.new name, text
         end
      when /^@@ (.+)$/
         m = $~
         do_close_buf.call
         name = m[1].strip
         buf  = []
         close_buf = lambda do
            text = buf.reduce "" do |memo, ln|
               if ln.empty? then memo + "\n\n"
               else              memo + ln.strip + " " end
            end
            text = text.gsub /[ \t]+\n/, "\n"
            lan.data.push Alias.new name, text
         end
      when /^\+\+ (.+)\|(.+)$/
         m = $~
         do_close_buf.call
         name = m[1].strip
         file = split_name m[2]
         file = unsplit_name state, file
         file = IO.read file
         lan.data.push Alias.new name, file
      when /^!!output (.+)$/
         m = $~
         do_close_buf.call
         state.out = m[1].strip
      when /^!!lang (.+)\|(.+)$/
         m = $~
         do_close_buf.call
         lnam = m[1].strip
         name = m[2].strip
         state.langs[lnam] = Language.new name, []
      when /^!!inclang (.+)$/
         m = $~
         do_close_buf.call
         name = split_name m[1]
         for lnam in state.langs.each_key
            lname = [lnam] + name
            parse_file state, lname, lnam
         end
      when /^!!include (.+)$/
         m = $~
         do_close_buf.call
         name = split_name m[1]
         parse_file state, name, language
      else
         if buf
            buf.push ln
         elsif ln.empty?
            next
         else
            raise "#{filename}@#{linenum}: invalid text"
         end
      end
   end

   do_close_buf.call

   state.cwd.pop dirname.size
end

for filename in ARGV
   filename = split_name filename
   state    = ParseState.new [], nil, {}

   parse_file state, filename, nil

   out = open state.out, "w"
   out.puts generated_header "compilefs"

   sorted = state.langs.sort_by do |k, v| k end
   for lnam, lang in sorted
      out.puts "[" + lang.name + "]"
      for data in lang.data
         if data.is_a? Alias
            out.puts %("#{escape data.name}" = "#{escape data.text.strip}";)
         else
            out.puts data
         end
      end
   end
end

## EOF
