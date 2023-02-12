Recently, I've been playing a lot at home with devices based on ESP8266, and sometimes it happens that I change the name of the network and there is a problem, because the device has to be removed and new data must be entered and hidden. It can be irritating to say the least, so I came up with the idea that I would write a simple program that will connect to the last WiFi network when ESP is started, and in case of problems, it will issue an AP and a website where we can enter our new data.
All in PlatformIO, file content below:

[env:d1_mini_pro]
platform = espressif8266
board = d1_mini_pro
board_build.filesystem = littlefs
framework = arduino
monitor_speed = 115200
upload_speed = 921600
lib_deps = 


We must remember to put: board_build.filesystem = littlefs, it creates a partition on our ESP where the data will be kept - you can also store other data there, except for the file with the WiFi network, but remember that the write can not be done too often. We can also update our board via OTA, remotely, by adding under platformio.ini:
upload_protocol = espota
upload_port = our IP address
