function printTable(table)
    for k, v in pairs(table) do
        local s = v or "nil"
        print(k .. " : " .. tostring(s))
    end
end

function testForceFlush(screen, canvas)
    print("Test Canvas clear and force flush..")
    canvas:setBackground(255, 0, 0, 255)
    canvas:clear()
    screen:draw(canvas)

    canvas:setBackground(0, 255, 0, 255)
    canvas:clear()
    screen:draw(canvas)

    canvas:setBackground(0, 0, 255, 255)
    canvas:clear()
    screen:draw(canvas)

    canvas:setBackground(255, 255, 0, 255)
    canvas:clear()
    screen:draw(canvas)

    canvas:setBackground(0, 255, 255, 255)
    canvas:clear()
    screen:draw(canvas)

    canvas:setBackground(255, 0, 255, 255)
    canvas:clear()
    screen:draw(canvas)

    canvas:setBackground(255, 255, 255, 255)
    canvas:clear()
    screen:draw(canvas)

    canvas:setBackground(0, 0, 0, 255)
    canvas:clear()
    screen:draw(canvas)
    print("Success")
end

function testDrawRectAndForceFlush(screen, canvas)
    print("Test draw Rect then force flush")
    canvas:fillColor(0, 0, 239, 239, 100, 100, 250, 128)
    canvas:fillColor(20, 20, 127, 140, 100, 100, 250, 128)
    canvas:fillColor(50, 50, 230, 200, 255, 100, 100, 128)
    canvas:fillColor(0, 180, 180, 320, 100, 255, 100, 128)
    canvas:fillColor(40, 60, 141, 222, 100, 255, 255, 128)
    canvas:fillColor(0, 162, 120, 319, 255, 255, 100, 128)
    canvas:fillColor(125, 0, 239, 319, 255, 100, 255, 128)

    screen:draw(canvas)
    print("Finished")
end

function testDrawRect(screen, canvas)
    print("Test draw Rect")
    canvas:fillColor(0, (320 // 4), 239, 239, 255, 255, 255, 128)
    screen:draw(canvas)

    canvas:fillColor(20, 20, 127, 140, 100, 100, 250, 128)
    screen:draw(canvas)

    canvas:fillColor(50, 50, 230, 200, 255, 100, 100, 128)
    screen:draw(canvas)

    canvas:fillColor(0, 180, 180, 320, 100, 255, 100, 128)
    screen:draw(canvas)

    canvas:fillColor(40, 60, 141, 222, 100, 255, 255, 128)
    screen:draw(canvas)

    canvas:fillColor(0, 162, 120, 319, 255, 255, 100, 128)
    screen:draw(canvas)

    canvas:fillColor(125, 0, 239, 319, 255, 100, 255, 128)
    screen:draw(canvas)

    print("Finished")
end

function testDrawDots(screen, canvas)
    print("Test draw points");
    for i = 0, 239 do
        canvas:setPoint(i, i, 255, 255, 255, 128)
    end
    screen:draw(canvas)
    print("Finished")
end

log.level(4)

print("Hello world!")
print("Greeting from lua VM!")

print("LibUsb Api Version:")
local version = UsbExplorer.apiVersion()
print(tostring(version))

print()
local usbContext = UsbExplorer.newContext()

local devices = usbContext:devices()
print(#devices .. " device(s) found.\n")

local device
for _, v in pairs(devices) do
    local info = v:info()
    if info.idVendor == 0x1908 and info.idProduct == 0x0102 then
        device = v
        break
    end
end

if device then
    print("Device DPF found! (Vendor:0x1908, Product: 0x0102)")
    print("-- Information : ")
    local deviceInfo = device:info()
    printTable(deviceInfo)
    print()

    print("-- Configuration :")
    local config = device:config(0)
    printTable(config:info())

    print()
    local interfaces = config:interfaces()
    for ik, iv in pairs(interfaces) do
        print("-- Interface [" .. ik .. "] :")
        printTable(iv)
        print()

        for k, v in pairs(iv.endpointList) do
            print("-- Interface [" .. ik .. "] Endpoint [" .. k .. "] :")
            printTable(v)
            print()
        end
    end
else
    print("Device not found")
end

print()

function useScreen(screen)
    print("Screen created. driver: \"" .. screen:getDriverName() .. "\"")
    local screenSize = screen:getSize()
    print("Dimension  : " .. screenSize.width .. "x" .. screenSize.height)
    print("Color depth: " .. screen:getColorDepth())

    local canvas = Canvas.new(screenSize.width, screenSize.height);
    print("Canvas created.")
    local canvasSize = canvas:getSize()
    print("Size       : " .. canvasSize.width .. "x" .. canvasSize.height)
    local background = canvas:getBackground()
    print("Background : { red=" .. background.red .. ", green=" .. background.green .. ", blue=" .. background.blue .. ",alpha=" .. background.alpha .. " }")

    testForceFlush(screen, canvas)
    testDrawRectAndForceFlush(screen, canvas)
    canvas:clear()
    screen:draw(canvas)
    testDrawRect(screen, canvas)
    testDrawDots(screen, canvas)
end

local screen = Screen.openDpf(device)

if (screen) then
    useScreen(screen)
end