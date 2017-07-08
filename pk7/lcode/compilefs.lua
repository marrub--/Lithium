#!/usr/bin/env lua

function trim(s)
   return s:gsub("^%s*(.-)%s*$", "%1")
end

function procHead(alias)
   out:write("\"" .. alias .. "\" =")
end

function procLine(ln, last, only)
   if not only then
      out:write("\n   ")
   else
      out:write(" ")
   end
   out:write("\"" .. ln:gsub("\\", "\\\\"):gsub('"', "\\\""):gsub("\\\\c", "\\c"))
   if not last then out:write("\\n") end
   out:write("\"")
end

function procEnd()
   out:write(";\n")
end

function procData(data, alias)
   if dbgon then
      print("data> '" .. alias .. "'")
   end
   
   procHead(alias)
   procLine(data, true, true)
   procEnd()
end

function procFile(fname, alias)
   if dbgon then
      print("file> '" .. alias .. "'")
   end
   
   local lns = io.lines(fname)
   procHead(alias)
   for ln in lns do
      procLine(ln)
   end
   procEnd()
end

function procIn(fname)
   if dbgon then
      print("proc> " .. fname)
   end
   
   out = io.open("../" .. fname, "w")
   out:write("[enu default]\n\n")
end

function procBuf(outbuf)
   local m
   if trim(outbuf[#outbuf]) == "" then
      m = #outbuf - 1
   else
      m = #outbuf
   end
   for i = 1, m do
      procLine(outbuf[i], i == m)
   end
   procEnd()
end

function procFileParse(fname)
   if dbgon then
      print("pfil> '" .. fname .. "'")
   end
   
   local lns = io.lines(fname)
   local outbuf
   for ln in lns do
      local sub3 = ln:sub(1, 3)
      if sub3 == "== " then
         if outbuf then
            procBuf(outbuf)
            outbuf = nil
         end
         local e, s = ln:find("|", 1, true)
         if e and s then
            procData(trim(ln:sub(s+1)), trim(ln:sub(4, e-1)))
         end
      elseif sub3 == "@@ " then
         if outbuf then
            procBuf(outbuf)
         end
         outbuf = {}
         local alias = trim(ln:sub(4))
         if dbgon then
            print("dmln> '" .. alias .. "'")
         end
         procHead(alias)
      elseif sub3 == "## " then
         if outbuf then
            procBuf(outbuf)
            outbuf = nil
         end
         out:write("\n// " .. ln:sub(4) .. "\n")
      elseif outbuf then
         table.insert(outbuf, ln)
      end
   end
   
   if outbuf then
      procBuf(outbuf)
   end
end

function procOutput(ln, e, s)
   local _, sd = ln:find("data ", 1, true) -- find "data" directive
   if sd then
      procData(trim(ln:sub(sd+1, e-1)), trim(ln:sub(s+1)))
   else
      procFile("FileData/" .. trim(ln:sub(1, s-2)), trim(ln:sub(e+2)))
   end
end

for ln in io.lines("FileData/dir.txt") do
   if ln:sub(1, 3) == "in " then -- find "in" directive
      procIn(ln:sub(4))
   elseif out and ln:sub(1, 10) == "parsefile " then -- find "parsefile" directive
      procFileParse("FileData/" .. trim(ln:sub(11)))
   elseif out then
      local e, s = ln:find("->", 1, true)
      if e and s then
         procOutput(ln, e, s)
      end
   end
end

if dbgon then
   print()
end

-- EOF
