Canvas = require "ldpf4l.Canvas"
UsbExplorer = require "ldpf4l.UsbExplorer"
Screen = require "ldpf4l.Screen"
Image = require "ldpf4l.Image"
log = require "ldpf4l.Logging"

log.setLevel(2)

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

    local rects = {
        { rect = { 0, (320 // 4), 239, 239 }, color = { r = 255, g = 255, b = 255, a = 128 } },
        { rect = { 50, 50, 230, 200 }, color = { r = 255, g = 100, b = 100, a = 128 } },
        { rect = { 0, 180, 180, 320 }, color = { r = 100, g = 255, b = 100, a = 128 } },
        { rect = { 40, 60, 141, 222 }, color = { r = 100, g = 255, b = 255, a = 128 } },
        { rect = { 0, 162, 120, 319 }, color = { r = 255, g = 255, b = 100, a = 128 } },
        { rect = { 125, 0, 239, 319 }, color = { r = 255, g = 100, b = 255, a = 128 } }
    }

    for _, v in pairs(rects) do
        local rect = v.rect
        local color = v.color
        canvas:fillColor(rect[1], rect[2], rect[3], rect[4], color.r, color.g, color.b, color.a)
        screen:draw(canvas)
    end

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

function testDrawFrame(screen, canvas)
    print("Test draw frames")
    local frames = {
        { rect = { 10, 10, 12, 50 }, color = { r = 255, g = 0, b = 0, a = 255 } },
        { rect = { 15, 0, 90, 120 }, color = { r = 0, g = 255, b = 0, a = 255 } },
        { rect = { 100, 50, 130, 180 }, color = { r = 0, g = 0, b = 255, a = 255 } },
        { rect = { 50, 30, 190, 200 }, color = { r = 255, g = 0, b = 255, a = 255 } },
        { rect = { 200, 60, 240, 300 }, color = { r = 255, g = 255, b = 0, a = 255 } },
        { rect = { 60, 100, 230, 320 }, color = { r = 0, g = 255, b = 255, a = 255 } },
    }

    for _, v in pairs(frames) do
        local rect = v.rect
        local color = v.color
        canvas:drawFrame(rect[1], rect[2], rect[3], rect[4], color.r, color.g, color.b, color.a, 1);
        screen:draw(canvas);
    end
end

function testDrawImages(screen, canvas)

    local images = {
        "img1.jpg",
        "img2.jpg",
        "img3.jpg",
        "img4.jpg",
        "img5.png",
        "img6.png",
        "img7.png",
        "img8.png",
        "img9.jpeg",
    }

    for _, v in pairs(images) do
        canvas:clear()
        local image = Image.loadFile("../images/pics/" .. v)
        local imageSize = image:getSize()
        print("Image loaded. Size " .. imageSize.width .. "x" .. imageSize.height)
        local imageInfo = image:getInfo()
        print("Color channels: origin " .. imageInfo.channels .. ", opened as " .. imageInfo.requiredChannels)
        canvas:drawImage(image, 0, 0, true)
        screen:draw(canvas)
    end
end

function testDrawAnimate(screen, canvas)
    local totalFrames = (87 - 30)
    local frames = {};
    for frame = 1, totalFrames do
        frames[frame] = Image.loadFile("../images/animate/00" .. frame + 29 .. ".png")
    end

    local x = (240 / 2) - (32 / 2)
    local y = (320 / 2) - (32 / 2)

    local bound = 32;

    print("Total " .. totalFrames .. " frames.")
    print("Set an " .. (32 + bound) .. "px width invisible frame to limit frame rate")

    for frame = 1, (totalFrames * 5) do
        canvas:fillColor(x - bound, y - bound, x + 32 + bound, y + 32 + bound, 0, 0, 0, 255)
        canvas:drawFrame(x - bound, y - bound, x + 32 + bound, y + 32 + bound, 255, 255, 255, 0, 2)
        local current = frame % totalFrames + 1
        canvas:drawImage(frames[current], x, y, false)
        screen:draw(canvas)
    end
end

function testCanvasCopy(screen, canvas)
    local screenSize = screen:getSize()
    local layer1 = Canvas.new(screenSize.width, screenSize.height);
    local layer2 = Canvas.new(screenSize.width, screenSize.height);

    layer1:fillColor(0, 180, 180, 320, 100, 255, 100, 128)
    layer2:fillColor(40, 60, 141, 222, 100, 255, 255, 50)

    print("Compose all layer in order")
    canvas:clear()
    canvas:drawCanvas(layer1, true)
    canvas:drawCanvas(layer2, true)
    screen:draw(canvas);
    print("Compose all layer in reversed order")
    canvas:clear()
    canvas:drawCanvas(layer1, true)
    canvas:drawCanvas(layer2, true)
    screen:draw(canvas);
    print("Only show layer 1")
    canvas:clear()
    canvas:drawCanvas(layer1, true)
    screen:draw(canvas);
    print("Only show layer 2")
    canvas:clear()
    canvas:drawCanvas(layer2, true)
    screen:draw(canvas);

end

function testComposing(screen, canvas)

end

function testBadApple(screen, canvas)
    local screenSize = screen:getSize()
    local totalFrames = 1822
    local x = (screenSize.width - 98) // 2
    local y = (screenSize.height - 98) // 2
    --local totalFrames = 3068
    --local x = (screenSize.width - 114) // 2
    --local y = (screenSize.height - 86) // 2

    canvas:setBackground(0, 0, 0, 255)
    canvas:clear()

    for frame = 1, totalFrames do
        --local img = Image.loadFile("../images/bad_apple/f" .. frame .. ".jpg")
        local img = Image.loadFile("../images/daisuke/f" .. frame .. ".jpg")
        canvas:drawImage(img, x, y, false)
        screen:draw(canvas)
        --io.write("frame[" .. frame .. "]\r")
        --io.flush()
    end
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
    screen:setBackLightLevel(7);

    local canvas = Canvas.new(screenSize.width, screenSize.height);
    print("Canvas created.")
    local canvasSize = canvas:getSize()
    print("Size       : " .. canvasSize.width .. "x" .. canvasSize.height)
    local background = canvas:getBackground()
    print("Background : { red=" .. background.red .. ", green=" .. background.green .. ", blue=" .. background.blue .. ",alpha=" .. background.alpha .. " }")

    --canvas:clear()
    --screen:draw(canvas)
    --testForceFlush(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testDrawRectAndForceFlush(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testDrawRect(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testDrawDots(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testDrawFrame(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testDrawImages(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testDrawAnimate(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testCanvasCopy(screen, canvas)
    --canvas:clear()
    --screen:draw(canvas)
    --testComposing(screen, canvas)

    canvas:clear()
    screen:draw(canvas)
    testBadApple(screen, canvas)

    --screen:setBackLightLevel(0);
end

local screen = Screen.openDpf(device)

if (screen) then
    useScreen(screen)
end
