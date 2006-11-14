#!/usr/bin/lua

grid = "kwoejhqfishitdot"
word = "shoe"

strsub = string.sub

function getpos(x, y, width)
	return ((x - 1) * width) + y
end

function getchar(grid, x, y, width)
	return strsub(grid, getpos(x, y, width), getpos(x, y, width))
end

function look(grid, width, height, word, x, y)
	local eaten = strsub(grid, getpos(x, y, width), getpos(x, y, width))
	grid = strsub(grid, 1, getpos(x, y, width) - 1)
		.. "-"
		.. strsub(grid, getpos(x, y, width) + 1, -1)
	
	print("|"..word.."|")
	
	if (word == "") then
		print "YES!  This word is in the cube."
		return true
	end
	
	for sx = -1, 1 do
		for sy = -1, 1 do
			if (sx ~=0 or sy ~=0)
				and (x + sx > 0)
				and (y + sy > 0)
				and (x + sx <= width)
				and (y + sy <= height) then
				print(getchar(grid, x + sx, y + sy, width).."?")
				if getchar(grid, x + sx, y + sy, width) == strsub(word, 1, 1) then
					print("found '"..strsub(word, 1, 1).."' at "..x,y)
					if look(grid, width, height, strsub(word, 2, -1), x + sx, y + sy) then
						return true
					end
				end
				
			end	
		end
	end
	
	grid = strsub(grid, 1, getpos(x, y, width) - 1)
		.. eaten
		.. strsub(grid, getpos(x, y, width) + 1, -1)
		
	return false
end

function find(grid, width, height, word)
	firstchar = strsub(word, 1, 1)
	word = strsub(word, 2, -1)

	for x = 1, width do
		for y = 1, height do
			io.write(" ", getchar(grid, x, y, width), " ")
		end
		io.write("\n\n")
	end

	for x = 1, width do
		for y = 1, height do
			if getchar(grid, x, y, width) == firstchar then
				print("found '"..firstchar.."' at "..x,y)
				if look(grid, width, height, word, x, y) then
					return true
				end
			end
		end
	end
	
	return false
	
end

repeat
	word = io.read("*l")
	if word and string.len(word) > 2 and find(grid, 4, 4, word) then
		print(word)
	end
until not word
