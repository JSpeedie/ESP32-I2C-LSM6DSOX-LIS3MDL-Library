# ESP32-I2C-LSM6DSOX-LIS3MDL-Library

## Making use of the Library

If you wish to use this library in a projects of yours, you can simply clone
the repo and copy the contents of the `components/` directory into the
components directory of your ESP IDF project, and modify the `CMakeLists.txt`
of your `main` directory of your project to include the contents found in
`main/CMakeLists.txt` of this repo. That's it!


## Example Program Setup

If you simply wish to run the example program here on an ESP32, here are the
steps.

### Software setup

I used the ESP IDF for development of this project. In order to recreate
this example project, you will have to install the ESP IDF as explained at
[https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html)

Once you've set it up, all you need to do for development (after making the
necessary hardware connections, of course) is:

```bash
cd ~/esp/esp-idf
get_idf
mkdir projects
cd projects
git clone git@github.com:JSpeedie/ESP32-I2C-LSM6DSOX-LIS3MDL-Library.git 9-dof-test
cd 9-dof-test
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
