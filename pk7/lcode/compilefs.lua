#!/usr/bin/env lua

function trim(s)
   return s:gsub("^%s*(.-)%s*$", "%1")
end

function procHead(alias)
   out:write("\"" .. alias .. "\" =")
end

function procLine(ln, last)
   out:write("\n   \"" .. ln:gsub("\\([^c\\]+)", "\\\\\\%1"):gsub("\"", "\\\""))
   if not last then out:write("\\n") end
   out:write("\"")
end

function procEnd()
   out:write(";\n")
end

function procData(data, alias)
   print("data> '" .. data .. "' -> '" .. alias .. "'")
   
   procHead(alias)
   procLine(data, true)
   procEnd()
end

function procFile(fname, alias)
   local lns = io.lines(fname)
   print("file> '" .. fname .. "' -> '" .. alias .. "'")
   procHead(alias)
   for ln in lns do
      procLine(ln)
   end
   procEnd()
end

for ln in io.lines("FileData/dir.txt") do
   if ln:sub(1, 3) == "in " then
      print("proc> " .. ln:sub(4))
      out = io.open("../" .. ln:sub(4), "w")
      out:write("[default]\n\n")
   elseif out then
      local _,  s1 = ln:find("data ", 1, true)
      local e1, s2 = ln:find("->", 1, true)
      if s1 and e1 and s2 then
         procData(trim(ln:sub(s1+1, e1-1)), trim(ln:sub(s2+1)))
      else
         local s, e = ln:find("->", 1, true)
         if s and e then
            procFile("FileData/" .. trim(ln:sub(1, s-1)), trim(ln:sub(e+1)))
         end
      end
   end
end

print()

-- EOF
