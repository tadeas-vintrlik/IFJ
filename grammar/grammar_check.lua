#!/bin/lua

local tex_path = "grammar.tex"


local file = io.open(tex_path, "r")

local start_symbol = "prog"
local non_terminals = {}
local terminals = {}

local rules = {}

function parse_tex_file() 
	for line in file:lines() do
		local rule = line:match("^%s*\\grule(.*)$")
		if rule then
			local left = rule:match("{([^{}]+)}"):lower():gsub("\\", "")
			rule = rule:gsub("^{[^{}]+}", "")
			rule = rule:gsub("%$\\epsilon%$", "")
			rule = rule:gsub(",", "char_comma")
			rule = rule:gsub("=", "char_equals")

			local right = rule:gmatch("\\n?ter{[^{}]+}")

			rule = {left = left, right = {}}

			for symbol in right do
				local name = symbol:match("{([^{}]+)}"):lower():gsub("\\", "")
				local is_terminal = symbol:find("^\\ter")

				if is_terminal then
					terminals[name] = true
				else
					non_terminals[name] = true
				end

				rule.right[#rule.right + 1] = {
					[is_terminal and "T" or "N"] = name
				}
			end

			rules[#rules + 1] = rule
		end
	end
end





function print_set(name, set, capitalize)
	local sorted = {}
	for k, _ in pairs(set) do
		sorted[#sorted+1] = k
	end

	table.sort(sorted)

	local s = "{"
	for _, v in ipairs(sorted) do
		if #s > 1 then
			s = s .. ", "
		end

		s = s .. (capitalize and v:upper() or v:lower())
	end

	s = s .. "}"

	print(name.. " = " ..s)
end

function print_rules()
	for _, rule in ipairs(rules) do 
		local s = ""

		for _, symbol in ipairs(rule.right) do
			if #s > 1 then
				s = s .. " "
			end

			local name = symbol.T or symbol.N

			s = s .. (symbol.T and name:lower() or name:upper())
		end

		print(rule.left:upper() .. " -> " .. s .. ".")
	end
end

-- Extract rules from the .tex file
parse_tex_file()

print_set("T", terminals, false)
print_set("N", non_terminals, true)

print("BELOW ARE RULES PASTABLE INTO THE WEB GRAMMAR CHECKER AT https://smlweb.cpsc.ucalgary.ca/start.html")

print_rules()
