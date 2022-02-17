#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Formatted text → LANGUAGE processor.                                     │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

require_relative "corinth.rb"

Alias         = Struct.new :name, :text
Language      = Struct.new :name, :data
TxtParseState = Struct.new :cwd, :out, :langs

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

   state.cwd.push(*dirname)

   lns = read_lines unsplit_name state, [readname]
   lan = state.langs[language]

   buf        = nil
   name       = nil
   in_concat  = false
   last_empty = true

   do_close_buf = lambda do
      lan.data.push Alias.new name, buf if buf
      buf = nil
   end

   for ln, linenum in lns.each_with_index
      ln = ln.chomp.gsub "\\#", "LITH_"
      case ln
      when /^##.*$/
         do_close_buf.call
      when /^@@$/
         in_concat = !in_concat
         buf.concat "\n" if buf and !last_empty
      when /^== (.+)\|(.*)$/
         m = $~
         do_close_buf.call
         name = m[1].strip
         text = m[2].strip
         lan.data.push Alias.new name, text
      when /^%% (.+)$/
         m = $~
         do_close_buf.call
         name      = m[1].strip
         buf       = String.new
         in_concat = false
      when /^@@ (.+)$/
         m = $~
         do_close_buf.call
         name      = m[1].strip
         buf       = String.new
         in_concat = true
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
            unless lnam == "default"
               lname = [lnam] + name
               parse_file state, lname, lnam
            end
         end
      when /^!!include (.+)$/
         m = $~
         do_close_buf.call
         name = split_name m[1]
         parse_file(state, name, language || "default")
      else
         if buf
            if in_concat
               if ln.empty?
                  buf.concat "\n\n"
                  last_empty = true
               elsif last_empty || language == "jp"
                  buf.concat ln.strip
                  last_empty = false
               else
                  buf.concat " " + ln.strip
                  last_empty = false
               end
            else
               buf.concat ln + "\n"
               last_empty = true
            end
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

common_main do
   filename = split_name ARGV.shift
   out      = open ARGV.shift, "w"
   state    = TxtParseState.new [], nil, {}

   parse_file state, filename, nil

   out.puts generated_header "txtc"

   sorted = state.langs.sort_by do |k, v| k end
   for _, lang in sorted
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
