local s = require "splitlib"

local t = s.split("hi,ho,there", ","); 

for i, v in pairs(t) do
    print(i, v)
end
