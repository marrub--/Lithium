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

def proc_monster ofp, flg, lst
   ofp << "   {"
   if lst[0] =~ /[[:digit:]]/
      ofp << "#{lst.shift},#{lst.shift},"
   else
      name = lst.shift
      ofp << "Exp_#{name},Score_#{name},"
   end
   ofp << %(mtype_#{lst.shift},"#{lst.shift}")
   ofp << ", #{flg}" unless flg.empty?
   ofp << "},\n"
end

def proc_file ifp, ofp
   it = ifp.read.lines.each

   ofp.puts generated_header "monc"
   ofp.puts "enum {"

   loop do
      ln = it.next
      break if ln == "end flags\n"
      if ln[0] == "+"
         sp = ln[1..-1].chomp.split
         ofp << "   #{sp[0]} = 1 << #{sp[1]},\n"
      end
   end

   ofp << <<~_end_
   };

   StrEntON
   static struct monster_info const monsterinfo[] = {
   _end_

   flg = ""
   loop do
      ln = it.next
      txt = ln[1..-1].chomp
      case ln[0]
      when "%"; flg = txt
      when "+"; proc_monster ofp, flg, txt.split
      end
   end

   ofp << <<~_end_
   };
   StrEntOFF

   /* EOF */
   _end_
end

proc_file open(ARGV[0], "rt"), open(ARGV[1], "wt")

## EOF
