function printTable(table)
    for k,v in pairs(table) do
        local s = v or "nil"
        print(k.." : "..tostring(s))
    end
end

function testForceFlush(canvas)
    print("Test Canvas clear and force flush..")
    canvas:setBackground(255,0,0)
    canvas:clear()
    canvas:forceFlush()
    canvas:setBackground(0,255,0)
    canvas:clear()
    canvas:forceFlush()
    canvas:setBackground(0,0,255)
    canvas:clear()
    canvas:forceFlush()
    canvas:setBackground(255,255,0)
    canvas:clear()
    canvas:forceFlush()
    canvas:setBackground(0,255,255)
    canvas:clear()
    canvas:forceFlush()
    canvas:setBackground(255,0,255)
    canvas:clear()
    canvas:forceFlush()
    canvas:setBackground(255,255,255)
    canvas:clear()
    canvas:forceFlush()
    canvas:setBackground(0,0,0)
    canvas:clear()
    canvas:forceFlush()
    print("Success")
end

function testDrawRectAndForceFlush(canvas)
    print("Test draw Rect then force flush")
    canvas:fillRect(0,0, 239, 239, 100, 100, 250, 128)
    canvas:fillRect(20,20, 127, 140, 100, 100, 250, 128)
    canvas:fillRect(50,50, 230, 200, 255, 100, 100, 128)
    canvas:fillRect(0,180, 180, 320, 100, 255, 100, 128)
    canvas:fillRect(40,60, 141, 222, 100, 255, 255, 128)
    canvas:fillRect(0,162, 120, 319, 255, 255, 100, 128)
    canvas:fillRect(125,0, 239, 319, 255, 100, 255, 128)

    canvas:forceFlush()
    print("Finished")
end

function testDrawRect(canvas)
    print("Test draw Rect")
    canvas:fillRect(0,(320 // 4), 239, 239, 255, 255, 255, 128)
    canvas:forceFlush()

    canvas:fillRect(20,20, 127, 140, 100, 100, 250, 128)
    canvas:flush()

    canvas:fillRect(50,50, 230, 200, 255, 100, 100, 128)
    canvas:flush()

    canvas:fillRect(0,180, 180, 320, 100, 255, 100, 128)
    canvas:flush()

    canvas:fillRect(40,60, 141, 222, 100, 255, 255, 128)
    canvas:flush()

    canvas:fillRect(0,162, 120, 319, 255, 255, 100, 128)
    canvas:flush()

    canvas:fillRect(125,0, 239, 319, 255, 100, 255, 128)
    canvas:flush()

    print("Finished")
end

function testDrawDots(canvas)
    print("Test draw points");
    for i=0,239 do
        canvas:setPoint(i, i, 255,255,255,255)
    end
    canvas:flush()
    print("Finished")
end

log.level(4)

print("Hello world!")
print("Greeting from lua VM!")

print("LibUsb Api Version:")
local version = UsbExplorer.apiVersion()
-- print(version.major.."."..version.minor.."."..version.micro.."("..version.nano..")")
print(tostring(version))

print()
local usbContext = UsbExplorer.newContext()

local devices = usbContext:devices()
print(#devices.." device(s) found.\n")

local device
for _,v in pairs(devices) do
    local info = v:info()
    if info.idVendor == 6408 and info.idProduct == 258 then
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
    for ik,iv in pairs(interfaces) do
        print("-- Interface ["..ik.."] :")
        printTable(iv)
        print()

        for k,v in pairs(iv.endpointList) do
            print("-- Interface ["..ik.."] Endpoint ["..k.."] :")
            printTable(v)
            print()
        end
    end
else
    print("Device not found")
end

print()
local dpf = Dpf.openDevice(device)
if dpf then
    print("Connected to Dpf device.")
    local size = dpf:dimensions()
    print("Screen size:")
    printTable(size)
    dpf:setBrightness(1)
    print("Set brightness to "..dpf:brightness())
    print("Buffer Size: "..dpf:bufferSize())
else
    print("Failed to open Dpf device.")
end

local canvas = Dpf.openCanvas(dpf)
print("Canvas created.")
print("Canvas size: "..canvas:width().."x"..canvas:height())
testForceFlush(canvas)
testDrawRectAndForceFlush(canvas)
canvas:clear()
canvas:flush()
testDrawRect(canvas)
testDrawDots(canvas)

--canvas:fillRect(0,0,120,120,255,255,255,255)
--canvas:flush()