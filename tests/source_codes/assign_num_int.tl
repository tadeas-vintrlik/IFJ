require "ifj21"

function foo(x : integer, y : integer) : integer, integer
  local i : integer = x
  local j : integer = (y + 2) * 3
  i, j = j+1, i+1  -- vyhodnocuj zprava a pøiøazuj pozdìji
  return i, j 
end

function main() 
  local a : integer = 1
  local b : integer = 2
  local c : number
  local d : number
  c, d = foo(a, b)  -- returns 13, 2
  write (c, d)
end
main()
