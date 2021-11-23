require "ifj21"

function main()
    local x : integer = (#6 + 12) * 27 -- used #6 here since identififers do not yet work this would be a semantic err
    if x > 0 then
        local x : string = "hello frame"
    else end
end

main()
