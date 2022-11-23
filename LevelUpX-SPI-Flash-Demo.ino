#include <SPI.h>

/*
 This section defines constants that we'll use to reference physical pins on the Arduino.
 */
#define DATAOUT 11//MOSI
#define DATAIN  12//MISO
#define SPICLOCK  13//sck
#define SLAVESELECT 10//ss
#define HOLD 9

/*
 This section defines constants that we'll use to send SPI commands to the flash chip.
 */
#define READ  3


/*
 This section declares variables that we'll use throughout the code.
 */
int data = 0;
int count = 0;
char buf[512];
int address;


/*
 The setup() function runs once when the Arduino powers up.
 */
void setup()
{
  // We'll use the "address" variable to track which address we're reading from flash, so we initialize it to 0.
  address = 0;
  
  // We'll send each byte read from the flash over to the computer via the Arduino serial port, so we must initialize serial communications.
  Serial.begin(115200);

  // We set the pin mode for each pin that we'll use to commmunicate with the SPI flash chip.
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);

  // Set the CS pin to high, which pauses the SPI chip while we get some other stuff ready.
  digitalWrite(SLAVESELECT,HIGH);

  // Now we'll tell the Arduino SPI library that we're about to begin SPI communications.
  SPI.begin();
}

void loop()
{
  Serial.print( "\nStarting flash read...\n" );

  /*
   This section reads the first byte from the SPI flash chip.
   We handle it in its own code block because we want to perform
   some initial SPI communication setup, such as calling beginTransaction()
   and then setting the CS pin to LOW, which tells the SPI flash chip
   we want to talk to it.
   */
  if( address == 0 ){
    digitalWrite(SLAVESELECT, HIGH);
    SPI.endTransaction();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    
    digitalWrite(SLAVESELECT, LOW);
    SPI.transfer( READ );
    data = SPI.transfer( (char) (address>>16) );
    data = SPI.transfer( (char) (address>>8) );
    data = SPI.transfer( (char) (address) );
  }

  /*
   For all other addresses (bytes), just read the byte and print it to the Arduino serial port.
   */
  while( address < 5000000 ) {
    data = SPI.transfer( 0xFF );
    sprintf(buf, "%02x ", data );
    Serial.print( buf );
    count++;

    /*
     This section just neatly formats the output in 64-byte rows.
     */
    if( count == 64 ) {
      Serial.print( "\n" );
      count = 0;
    }

    // Now we increment the address to read the next byte from the SPI flash chip.
    address++;
  }

  /*
   Now we're done reading the contents, so cleanup SPI communication.
   */
  if( address >= 1000000 ) {
    digitalWrite(SLAVESELECT, HIGH);
    SPI.endTransaction();
    Serial.print( "\nFinished flash read...\n" );
  }
  
delay( 60000 );

}
