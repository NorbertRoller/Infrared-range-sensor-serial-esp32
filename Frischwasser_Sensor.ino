/*
briidea HRWL-01W Frischwassertankanzeige
Infrared range sensor for water tank.
7-32V DC
The display device saves Min Max values, after power was cut off.
The display has 10 LED's (10% steps)
The sensor is infrared and works in the range up to 400mm.

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



Important:
The surface of the sensor needs to stay free of water drops and mist. I use a coating against mist with quite success.
As you have to drill a hole into the watertank, it is important to seal it with the sensor. However the tine rubber seal is not able to seal properly.
But any DIY will carry rubber seals in the plumping section. I used Dekaseal 8936 on both sides 
The sensor has a thread and a nut unlike other sensors.

*/

//#define RXD1 (21)
//#define TXD1 (22)


#define Frisch_Senor_EIN 5          // Powersuply 12V->5V with enable On/Off

char stringbuffer[17]="";           // Buffer 2x of what the dataset is. Can be smaler
int stringbufferposition=0;         // Current position in buffer for next received byte
unsigned int Frisch_mm=0;           // Millimeter distance between Sensor and surface that refelects the signal.
                                    // Long distance = empty. Short distance = full. 2-5cm safety distance needed 
                                    // between sensor and full position. Coating is important in a camper!
byte xorTemp;                       // checksum xor

void setup() {
  
  Serial.begin(115200); 
  Serial1.begin(9600, SERIAL_8N1, 21, 22);        // Serial Port 1 // Infrared Sensor (Pin )

  // Start 5V für Frischwasser Sensor
  pinMode(Frisch_Senor_EIN, OUTPUT);            // Enable 5V Power
  digitalWrite(Frisch_Senor_EIN, HIGH);          //  As we want to use immediately after setup()
}

void loop()
{

// supply current and wait for the sensor to get ready
digitalWrite(Frisch_Senor_EIN, HIGH);       // Enable 5V Power
Serial.println("[Frischwasser] +5V für Sensor EIN");
delay(1500);        // wait for sensor to get ready booting

// send command to request a meassurment
  Serial1.write(0x62);  //0
  Serial1.write(0x33);  //1
  Serial1.write(0x09);  //2
  Serial1.write(0xFF);  //3
  Serial1.write(0xFF);  //4
  Serial1.write(0x00);  //5
  Serial1.write(0x00);  //6
  Serial1.write(0x00);  //7
  Serial1.write(0x58);  //8 = checksum

  Serial.println("Wait for Sensor to prepare dataset / send 1st char");
  while (Serial1.available() == 0)
    {Serial.print(".");
     delay(100);}        // wait for sensor to prepared data set
  Serial.println("\nFirst byte received");

// read byte by byte until full dataset was received
  stringbufferposition=0;               // Position 0 in buffer
  while (Serial1.available() > 0) 
  {
  stringbuffer[stringbufferposition]=Serial1.read();
  Serial.print(stringbuffer[stringbufferposition],HEX);   // Debug only
  Serial.print(" - ");
  Serial.println(stringbuffer[stringbufferposition]);

// check if it is a valid dataset
  if (stringbufferposition==8 && stringbuffer[0]==0x62 && stringbuffer[1]==0x33 && stringbuffer[2]==0x09)
	  {
    // build checksum 8 XOR of received dataset
    for(int i = 0; i < 8; i++)
      {xorTemp ^= byte(stringbuffer[i]);}
    Serial.print("Checksum:");            // Debug only
    Serial.println(xorTemp,HEX);          // Debug only
    if(xorTemp==stringbuffer[8])          // Is checksum ok ?
      {
      xorTemp = 0;                        // Reset checksum
      Serial.println("[Frischwasser] Checksum ok"); // Debug only
      Serial.println("[Frischwasser] Sensor has send a valid data set");
      Serial.println(stringbuffer[5], DEC);
      Serial.println(stringbuffer[6], DEC);
      Frisch_mm = (stringbuffer[5] * 0xFF) + (stringbuffer[6]);

      Serial.print("Freshwater mm decimal :");  // Display received distance in mm
      Serial.println(Frisch_mm, DEC);
      }
      break;
    }
  else
    {
    stringbufferposition++;
    stringbuffer[stringbufferposition] = 0;   // Null terminate, in case there is no more bytes received.
	  }
  if(stringbufferposition==16)                // end of buffer reached, delete garbage
    {                                         // restart at buffer position 0
    stringbufferposition = 0;                 // Null terminate, in case there is no more bytes received.
    }	
   }

  digitalWrite(Frisch_Senor_EIN, LOW);        // disable 5V for sensor

  delay(20000);                               // Wait 20 sec for next data request
}
