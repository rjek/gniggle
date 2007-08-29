#!/usr/bin/lua

-- metserver.c
-- This file is part of Gniggle
--
-- Copyright (C) 2007 - Rob Kendrick <rjek@rjek.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to
-- deal in the Software without restriction, including without limitation the
-- rights to use, copy, modify, merge, publish, distribute, and/or sell copies
-- of the Software, and to permit persons to whom the Software is furnished to
-- do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
-- IN THE SOFTWARE.

config = {
	bind = { host = "localhost", port = 1234 },
	allowNew = true,
}

require "socket"

-- servers.host, servers.port, servers.width, servers.height, servers.comment
-- servers.lastseen.  Keyed by "host:port"
local servers = { }

local s = socket.tcp()
local connections = { s }
local buffers = { }
s:setoption("reuseaddr", true)
s:bind(config.bind.host, config.bind.port)
s:listen(10)

function SendList(c)
	for i, v in pairs(servers) do
		c:send(string.format("%s:%d:%d:%d:%s\n",
			v.host, v.port, v.width, v.height, v.comment));
	end
end

function ProcessLine(c, l)
	
	local _, _, host, port, width, height, comment =
		string.find(l, "^(.-):(%d+):(%d+):(%d+):(.*)$")
	
	if host ~= nil then
		local t = { host = host, port = port, width = width,
				height = height, comment = comment }
		servers[host .. port] = t;
	end

	buffers[c] = nil
	connections[c] = nil
	c:close();
end

while true do
	local read, send, err = socket.select(connections, connections, 1)

	if err == "timeout" then
		-- do some tidying of idling connections and expire old
		-- entries in the server list
	end

	for _, r in ipairs(read) do
		if r == s then
			local c = s:accept()
			connections[#connections + 1] = c
			buffers[c] = ""
			SendList(c)
			c:settimeout(0)
		else
			local l, err = r:receive(1)
			if l == nil and err ~= nil then
				buffers[r] = nil
				connections[r] = nil
				r:close()
			else
				buffers[r] = buffers[r] .. l
				local _, _, cl, re = string.find(
					buffers[r], "^(.-)\n(.*)$")
				if cl ~= nil then
					buffers[r] = re
					ProcessLine(r, cl)
				end
			end
		end
	end
end
