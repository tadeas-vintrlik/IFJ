require "ifj21"


function x() : integer, string
	return 2
end

function main()
    local a : integer
    local b : string

    a, b = x()

    write(a, "\n", b)
end

main()
