#!/usr/bin/env lua

function trim(s)
   return s:gsub("^%s*(.-)%s*$", "%1")
end

function delr(s)
   return s:gsub("\r", "")
end

function procHead(alias)
   alias = delr(alias)
   out:write("\"" .. alias .. "\" =")
end

function procLine(ln, noeol, nobreak)
   ln = delr(ln)
   if not nobreak then
      out:write("\n   ")
   else
      out:write(" ")
   end
   out:write("\"" .. ln:gsub("\\", "\\\\"):gsub('"', "\\\""):gsub("\\\\c", "\\c"):gsub("\n", "\\n"))
   if not noeol then
      out:write("\\n")
   end
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
   
   local lns = io.open(fname, "rb"):lines()
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
   
   out = io.open("../../" .. fname, "wb")
   out:write("[enu default]\n\n")
end

function procBuf(buf)
   local m
   if trim(buf.lns[#buf.lns]) == "" then
      m = #buf.lns - 1
   else
      m = #buf.lns
   end
   for i = 1, m do
      local ln = buf.lns[i]
      if buf.trimlines then
         if ln == "" then
            ln = "\n\n"
         elseif i ~= m and buf.lns[i + 1] ~= "" then
            ln = ln .. " "
         end
      end
      procLine(ln, i == m or buf.trimlines)
   end
   procEnd()
end

function procFileParse(fname)
   if dbgon then
      print("pfil> '" .. fname .. "'")
   end
   
   local lns = io.open(fname, "rb"):lines()
   local buf = nil
   for ln in lns do
      ln = delr(ln)
      -- single line
      if ln:sub(1, 3) == "== " then
         if buf then
            procBuf(buf)
            buf = nil
         end
         local e, s = ln:find("|", 1, true)
         if e and s then
            procData(trim(ln:sub(s+1)), trim(ln:sub(4, e-1)))
         end
      
      -- info page multiline
      elseif ln:sub(1, 3) == "@@ " then
         if buf then
            procBuf(buf)
         end
         buf = {lns = {}, trimlines = true}
         local alias = trim(ln:sub(4))
         if dbgon then
            print("dmln> '" .. alias .. "'")
         end
         procHead(alias)
      
      -- regular multiline
      elseif ln:sub(1, 3) == "%% " then
         if buf then
            procBuf(buf)
         end
         buf = {lns = {}, trimlines = false}
         local alias = trim(ln:sub(4))
         if dbgon then
            print("dmln> '" .. alias .. "'")
         end
         procHead(alias)
      
      -- comment
      elseif ln:sub(1, 2) == "##" then
         if buf then
            procBuf(buf)
            buf = nil
         end
         out:write("\n// " .. ln:sub(4) .. "\n")
      elseif buf then
         table.insert(buf.lns, ln)
      end
   end
   
   if buf then
      procBuf(buf)
   end
end

function procOutput(ln, e, s)
   local _, sd = ln:find("data ", 1, true) -- find "data" directive
   if sd then
      procData(trim(ln:sub(sd+1, e-1)), trim(ln:sub(s+1)))
   else
      procFile(trim(ln:sub(1, s-2)), trim(ln:sub(e+2)))
   end
end

for ln in io.open("dir.txt", "rb"):lines() do
   ln = delr(ln)
   if ln:sub(1, 3) == "in " then -- find "in" directive
      procIn(trim(ln:sub(4)))
   elseif out and ln:sub(1, 10) == "parsefile " then -- find "parsefile" directive
      procFileParse(trim(ln:sub(11)))
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
