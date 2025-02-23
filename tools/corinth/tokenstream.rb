## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Token stream.                                                            │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

class TokenStream
   def initialize inner, eof
      @inner = inner
      @eof   = eof
   end

   def peek
      begin
         @inner.peek
      rescue StopIteration
         @eof
      end
   end

   def next
      begin
         @inner.next
      rescue StopIteration
         @eof
      end
   end

   def peek_or type, default = nil
      if self.peek.type.tk_is? type
         tok = self.next
         if block_given?
            yield tok
         else
            tok.text
         end
      else
         default
      end
   end

   def drop type
      if self.peek.type.tk_is? type
         self.next
      else
         nil
      end
   end

   def drop? type
      drop(type) != nil
   end

   def while_is type
      if block_given?
         loop do
            break unless self.peek.type.tk_is? type
            yield self.next
         end
      else
         to_enum :while_is, type
      end
   end

   def while_drop type
      loop do
         yield
         break unless self.drop type
      end
   end

   def until_drop type
      loop do
         break if self.drop type
         yield
      end
   end
end

## EOF
