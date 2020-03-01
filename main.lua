function printTable(table)
    for k,v in pairs(table) do
        local s = v or "nil"
        print(k.." : "..tostring(s))
    end
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

local device = nil
for k,v in pairs(devices) do
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
local dpf = Dpf.open(device)
if dpf then
    print("Connected to Dpf device.")
    local size = dpf:dimensions()
    print("Screen size:")
    printTable(size)
    dpf:setBrightness(7)
    print("Set brightness to "..dpf:brightness())
    print("Buffer Size: "..dpf:bufferSize())
else
    print("Failed to open Dpf device.")
end