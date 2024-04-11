# ESP32-I2C-LSM6DSOX-LIS3MDL-Library

## Setup

### Software setup

I used the `esp-idf` for development of this project. In order to recreate
this example project, you will have to install the esp idf as explained at
[https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html)

Once you've set it up, all you need to do for development (after making the
necessary hardware connections, of course) is:

```bash
cd ~/esp/esp-idf
get_idf
cd [project] # In this case, "9-dof-test"
idf.py -p /dev/ttyUSB0 flash monitor
```

### Hardware connections

Below is the schematic I used for the example program.

<p align="center">
  <img src="https://raw.githubusercontent.com/wiki/JSpeedie/ESP32-I2C-LSM6DSOX-LIS3MDL-Library/images/ESP32-I2C-9-DOF.png" width="50%"/>
</p>

Of course you will also need to connect a micro usb to usb cable between the
ESP32 and your development machine in order to flash the program to the ESP32
and to give it power.
