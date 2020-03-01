
local test = require "test"
local num = test.add(1)
print("Result is "..num)

local array = NumberArray.new(1000)
print("Created a new "..tostring(array))
print("Size of array is "..#array)
for i=1,1000 do
    array[i] = 1/i
end
for i=1,10 do print(array[i]) end