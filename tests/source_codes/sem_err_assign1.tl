require "ifj21"

function foo(x : integer, y : integer) : integer, integer
  local i : integer = x
  local j : integer = (y + 2) * 3
  i, j = j+1, i+1  -- vyhodnocuj zprava a p�i�azuj pozd�ji
  return i, j 
end

function main() 
  local a : integer = 1
  local b : integer = 2
  local c : string
  a, c = foo(a, b)  -- prirazujeme do spatnych typu
  if a < b then
    write(a, "<", b, "\n")
  else
    write(a, ">=", b, "\n")
    local a : integer = 666
    write(a, "\n")
  end
  write(a, "\n")  --[[ prints 13 ]]
end
main()
