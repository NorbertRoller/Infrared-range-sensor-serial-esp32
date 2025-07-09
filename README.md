# Ultrasonic-range-sensor-serial-esp32
briidea HRWL-01W Ultrasonic range sensor for (water) tank.

briidea HRWL-01W used for freshwater tank in camper.
Ultrasonic range sensor for water tank.
7-32V DC
The display device saves Min Max values, after power was cut off.
The display has 10 LED's (10% steps)
The sensor is ultrasonic and works in the range up to 400mm.

Communication between the sensor and display is via 4 pin serial cable.
The parameters are: 9600 baud, 8 bits, No parity, 1 stop bit

The serial port of the ESP32 is used for communication and that makes the display obsolete.

The sensor needs >1s after power up to be able to accept commands.

The command (8 bytes + checksum) to request a data set is: 0x62, 0x33, 0x09, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x58
The sensor returns 0x62, 0x33, 0x09, 0x00, 0x00,.... At position 5 + 6 is the water level in mm.
Value= byte 5 * 0xFF + byte 6
The last Byte is the CheckSum8 Xor

Example:

0x62,0x33,0x09,0xFF,0xFF,0x00,0x00,0x00,0x58		Requests Data from Sensor after Init

0x62,0x33,0x09,0x00,0x00,0x00,0xA1,0x66,0x9F		Answer from Sensor with mm height

The power for the sensor can be switched on / off to conserve battery in the camper.

Below is a fully functioning test program that will be imported into a Camper-Monitoring-Display project.



## Important:
The ultrasonic sensor needs to stay free of water drops and mist. I use a coating against mist with success.

As you have to drill a hole into the watertank, it is important to seal it with the sensor. However the tine rubber seal is not able to seal properly.

But any DIY will carry rubber seals in the plumping section. I used Dekaseal 8936 on both sides.

The sensor has a thread and a nut unlike other sensors.
