local UsbExplorer = require "ldpf4l.UsbExplorer"
local log = require "ldpf4l.Logging"

log.setLevel(0)

function printTable(table)
    for k, v in pairs(table) do
        local s = v or "nil"
        print(k .. " : " .. tostring(s))
    end
end

print("LibUsb Api Version:")
local version = UsbExplorer.apiVersion()
print(tostring(version))

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

