require "ifj21"

function main()
    local str : string = "Hello World"
    local x : integer = (#str + 12) * 27
    write(x)
end

global main : function()

main()
