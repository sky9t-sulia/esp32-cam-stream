# Freenove ESP32-S3 Camera Module Project

This project uses the Freenove ESP32-S3 Camera module to capture images and stream video.

## Images

![Camera Module](https://github.com/sky9t-sulia/esp32-cam-stream/blob/main/images/1.jpg?raw=true)
![Back Side](https://github.com/sky9t-sulia/esp32-cam-stream/blob/main/images/2.jpg?raw=true)
![Web Interface](https://github.com/sky9t-sulia/esp32-cam-stream/blob/main/images/3.jpg?raw=true)

## Hardware Requirements

- Freenove ESP32-S3 Camera module
- MicroSD card

## Software Requirements

- ESP-IDF v5.0 or later
- CMake 3.13 or later

## Connect Display

- SDA -> 47
- SCL -> 21

## Default GPIO for SDMMC

- CMD -> 38
- CLK -> 39
- D0  -> 40

## Configuration

Before building the project, you may need to configure some settings. This can be done using the `menuconfig` tool provided by ESP-IDF.

1. Navigate to the project directory in your terminal.
2. Run `idf.py menuconfig`.
3. You need to configure WiFi settings, SSD1306 display and Camera

## Building the Project

1. After configuring your settings, you can build the project by running `idf.py build` in the project directory.
2. Once the build is complete, you can flash the firmware to the ESP32-S3 by running `idf.py -p PORT flash`, replacing `PORT` with the appropriate port for your system.

## Preparing the SD Card

Before using the module, you need to prepare an SD card with the necessary files:

1. Format the SD card as FAT32.
2. Copy all files from the `webserver` folder in the project directory to the root directory of the SD card.
3. Safely eject the SD card from your computer.
4. Insert the SD card into the SD card slot on the module.

Please ensure that the SD card is inserted into the module before powering it on.

## Usage

After flashing the firmware, the ESP32-S3 will connect to the configured WiFi network and start the web server. The device's IP address will be displayed on the SSD1306 display.

To view the web interface:

1. Note down the IP address displayed on the SSD1306 display.
2. Open a web browser on a device connected to the same network as the ESP32-S3.
3. Enter the IP address into the browser's address bar and press Enter.

You should now see the web interface.