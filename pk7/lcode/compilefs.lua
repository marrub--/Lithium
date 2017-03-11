#!/usr/bin/env lua

function procLine(ln)
   return ln:gsub("\\", "\\\\"):gsub("\"", "\\\"")
end

function procFile(fname, alias)
   if not out then
      print(">> No output, skipping '" .. fname .. "' -> '" .. alias .. "'")
      return
   end
   
   local lns = io.lines(fname)
   if lns then
      print("> '" .. fname .. "' -> '" .. alias .. "'")
      out:write("\"" .. alias .. "\" =")
      for ln in lns do
         out:write("\n   \"" .. procLine(ln) .. "\\n\"")
      end
      out:write(";\n")
   else
      print(">> No file named '" .. fname .. "', skipping '" .. alias .. "'")
   end
end

for ln in io.lines("FileData/dir.txt") do
   if ln:sub(1, 3) == "in " then
      print("Processing ", ln:sub(4))
      out = io.open("../" .. ln:sub(4), "w")
      if out then
         out:write("[default]\n\n")
      end
   elseif out then
      local s, e = ln:find(" -> ", 1, true)
      if s and e then
         procFile("FileData/" .. ln:sub(1, s-1), ln:sub(e+1))
      end
   end
end

-- EOF
