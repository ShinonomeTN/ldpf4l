print("Module composing loaded.")

Component = {
    width = 0,
    height = 0
}

function Component:new(p)
    local obj = p or {}
    self.__index = self
    local result = setmetatable(obj, self)

    result.canvas = Canvas.new(result.width, result.height)

    return result;
end

Layout = {

}

return {
    Component = Component
}