local array = require "boolarray"

a = array.new(100); 

print(array.size(a))

for i = 1, 100 do
    array.set(a, i, true)
end

print(array.get(a, 10))

array.set(a, 10, false)

print(array.get(a, 10))
