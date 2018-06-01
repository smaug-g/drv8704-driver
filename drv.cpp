/*
  Drv.h - Library for communicating with a DRV8704
  Created by Sergio Mauricio Guerrero for REV. March 15, 2018

  Usage:
  
    "drv sailboat(CSC pin);"
    void setup() {

    drv.setBridge("on");
    drv.setBridge("off");

    drv.setLogging("info");

    }
    

  Dependencies:

    REV Logger Library. (should be included)

  ** see drv.h for full doc **

*/
#include <SPI.h>
#include <Arduino.h>
#include "drv.h"
#include "Logger.h"

// initialize logging object
Logger logger("DRV8704", "info");

// register addresses (for internal functions)
const int CTRL = 0x0;
const int TORQUE = 0x1;
const int OFF = 0x2;
const int BLANK = 0x3;
const int DECAY = 0x4;
const int DRIVE = 0x6;
const int STATUS = 0x7;

// constructor
drv::drv(int select) {

  // pins
  _SCS = select;

  // updated with currentRegisterValues
  unsigned int currentRegisterValues[8];

  unsigned int initRegs[] = {
      0x301, // B001100000001  CTRL
      0x0FF, // B000011111111  TORQUE
      0x130, // B000100110000  OFF
      0x080, // B000010000000  BLANK
      0x010, // B000000010000  DECAY
      0x000, // B000000000000  RESERVED register (unused)
      0xFA5, // B111110100101  DRIVE
      0x000, // B000000000000  STATUS
  };    

  bool faults[] = {0, 0, 0, 0, 0, 0};
    
  
}

/*
PUBLIC FUNCTIONS
*/
void drv::open() {
  digitalWrite(_SCS, HIGH);
  SPI.beginTransaction(SPISettings(140000, MSBFIRST, SPI_MODE0));  
}

void drv::close() {
  SPI.endTransaction();
  digitalWrite(_SCS, LOW);
}

unsigned int drv::read(unsigned int address) {
    /*
     Read from a register over SPI using Arduino SPI library.

     Args: address -> int 0xX where X <= 7
     Return: integer representing register value. 

     Example:  data = spiReadReg(0x6);
    */ 
    unsigned int value;
    address = address << 12; // allocate zeros for data
    address |= 0x8000; // set MSB to read (1)
    open();
    value = SPI.transfer16(address); // transfer read request, recieve data
    close();
    
    return value;
}


void drv::write(unsigned int address, unsigned int value) {
 /*
  Write to register over SPI using Arduino SPI library.

  address : int 0xX where X <= 0x7, 
  value : int to be written (as binary) to register. (12 bits)
  returns : true if write successful, false otherwise
  Example:  spiWriteReg(0x6, 0x0FF0);

  */
  unsigned int packet=0;

  address = address << 12; // build packet skelleton
  address &= ~0x8000; // set MSB to write (0)
  packet = address | value;
  open();  // open comms
  SPI.transfer16(packet);
  close(); // close
}

void drv::setLogging(char* level) {
  // sets logging level for the drv logger
  logger.setLevel(level);
  Serial.println("REV - DRV8704 driver loaded");
  Serial.print("DRV8704 - Log level set: ");
  Serial.println(level);
}

// *** SETTERS ***

bool drv::setHbridge(char* value) {
  // cleat bits 16-13 from the read data (not used)
  unsigned int current = read(CTRL) & ~0xF000;
  unsigned int outgoing;

  if (value == "off") {
    outgoing = current & ~0x001; // clear bit 0
  } else if (value == "on") {
    outgoing = current | 0x001; // set bit 0
  } else {
    outgoing = current; // do nothing
    logger.loge("ENBL set: invalid input");
    return false;
  }

  write(CTRL, outgoing);

  return logger.logSet("CTRL", "ENBL", value, getHbridge() == value);
}

bool drv::setISGain(int value) {
  unsigned int current = read(CTRL) & ~0xF000;
  unsigned int outgoing;

  if (value == 5) {
    outgoing = current & ~0x300; // clear bits 9-8
  } else if (value == 10) {
    outgoing = current | 0x100; // set bit 8
    outgoing &= ~0x200; // clear bit 9
  } else if (value == 20) {
    outgoing = current | 0x200; // set bit 9
    outgoing &= ~0x100; // clear bit 8
  } else if (value == 40) {
    outgoing = current | 0x300; // set bits 9-8
  } else {
    outgoing = current; // do nothing
    logger.loge("ISGAIN set: invalid input");
    return false;
  }
  
  write(CTRL, outgoing);

  return logger.logSet("CTRL", "ISGAIN", value, getISGain() == value);
}

bool drv::setDTime(int value) {
  unsigned int current = read(CTRL) & ~0xF000;
  unsigned int outgoing;
  
  if (value == 410) {
    outgoing = current & ~0xC00; // clear bits 11-10
  } else if (value == 460) {
    outgoing = current | 0x400; // set bit 10
    outgoing &= ~0x800; // clear bit 11
  } else if (value == 670) {
    outgoing = current | 0x800; // set bit 11
    outgoing &= ~0x400; // clear bit 10
  } else if (value == 880) {
    outgoing = current | 0xC00; // set bits 11-10
  } else {
    outgoing = current; // do nothing
    logger.loge("DTIME set: invalid input");
  }

  write(CTRL, outgoing);

  return logger.logSet("CTRL", "DTIME", value, getDTime() == value);
}

bool drv::setTorque(unsigned int value) {
  unsigned int current = read(TORQUE) & ~0xF000;
  unsigned int outgoing;

  if(value <= 255 && value >= 0) {
    outgoing = current & 0xF00; // clear bits 7-0
    outgoing |= value; // set bits 7-0
  } else {
    outgoing = current; // do nothing
    logger.loge("TORQUE set: invalid input");
    return false;
  }

  write(TORQUE, outgoing);
  return logger.logSet("TORQUE", "TORQUE", value, getTorque() == value);
}

bool drv::setTOff(unsigned int value) {
  unsigned int current = read(OFF) & ~0xF000;
  unsigned int outgoing;

  if(value <= 255 && value >= 0) {
    outgoing = current & 0xF00; // clear bits 7-0
    outgoing |= value; // set bits 7-0
  } else {
    outgoing = current; // do nothing
    logger.loge("TOFF set: invalid input");
    return false;
  }
  
  write(OFF, outgoing);
  return logger.logSet("OFF", "TOFF", value, getTOff() == value);
}

bool drv::setTBlank(unsigned int value) {
  unsigned int current = read(BLANK) & ~0xF000;
  unsigned int outgoing;

  if(value <= 255 && value >= 0) {
    outgoing = current & 0xF00; // clear bits 7-0
    outgoing |= value; // set bits 7-0
  } else {
    outgoing = current; // do nothing
    logger.loge("TBLANK set: invalid input");
    return false;
  }
  
  write(BLANK, outgoing);
  return logger.logSet("BLANK", "TBLANK", value, getTBlank() == value);
}

bool drv::setTDecay(unsigned int value) {
  unsigned int current = read(DECAY)  & ~0xF000;
  unsigned int outgoing;

  if(value <= 255 && value >= 0) {
    outgoing = current & 0xF00; // clear bits 7-0
    outgoing |= value; // set bits 7-0
  } else {
    outgoing = current; // do nothing
    logger.loge("TDECAY set: invalid input");
    return false;
  }
  
  write(DECAY, outgoing);
  return logger.logSet("DECAY", "TDECAY", value, getTDecay() == value);
}

bool drv::setDecMode(char* value) {
  unsigned int current = read(DECAY)  & ~0xF000;
  unsigned int outgoing;

  if(value == "slow") {
    outgoing = current & ~0x700;// clear bits 10-8
  } else if (value == "fast") {
    outgoing = current & ~0x700; // clear bits 10-8
    outgoing |= 0x200; // set bit 9
  } else if (value == "mixed") {
    outgoing = current & ~0x700; // clear bits 10-8
    outgoing |= 0x300; // set bits 9-8
  } else if (value == "auto") {
    outgoing = current & ~0x700; // clear bits 10-8
    outgoing |= 0x500; // set bits 10 and 8
  } else {
    outgoing = current; // do nothing
    logger.loge("DECMOD set: invalid input");
    return false;
  }

  write(DECAY, outgoing);
  return logger.logSet("DECAY", "DECMOD", value, getDecMode() == value);
}

bool drv::setOCPThresh(int value) {
  unsigned int current = read(DRIVE)  & ~0xF000;
  unsigned int outgoing;

  if (value == 250) {
    outgoing = current & ~0x003; // clear bits 1-0
  } else if (value == 500) {
    outgoing = current & ~0x003; // clear bits 1-0
    outgoing |= 0x001; // set bit 0
  } else if (value == 750) {
    outgoing = current & ~0x003; // clear bits 1-0
    outgoing |= 0x002; // set bit 1
  } else if (value == 1000) {
    outgoing = current | 0x003; // set bits 1-0
  } else {
    outgoing = current;
    logger.loge("OCPTH set: invalid input");
    return false;
  }
  
  write(DRIVE, outgoing);
  return logger.logSet("DRIVE", "OCPTH", value, getOCPThresh() == value);
}

bool drv::setOCPDeglitchTime(float value) {
  unsigned int current = read(DRIVE) & ~0xF000;
  unsigned int outgoing;

  if (value == 1.05) {
    outgoing = current & ~0x00C; // clear bits 3-2
  } else if (value == 2.1) {
    outgoing = current & ~0x00C; // clear bits 3-2
    outgoing |= 0x004; // set bit 2
  } else if (value == 4.2) {
    outgoing = current & ~0x00C; // clear bits 3-2
    outgoing |= 0x008; // set bit 3
  } else if (value == 8.4) {
    outgoing = current | 0x00C; // set bits 3-2
  } else {
    outgoing = current;
    logger.loge("OCPDEG set: invalid input");
    return false;
  }

  write(DRIVE, outgoing);
  return logger.logSet("DRIVE", "OCPTH", value, getOCPDeglitchTime() == value);
}

bool drv::setTDriveN(int value) {
  unsigned int current = read(DRIVE) & ~0xF000;
  unsigned int outgoing;

  if (value == 263) {
    outgoing = current & ~0x030; // clear bits 5-4
  } else if (value == 525) {
    outgoing = current & ~0x030; // clear bits 5-4
    outgoing |= 0x010; // set bit 4
  } else if (value == 1050) {
    outgoing = current & ~0x030; // clear bits 5-4
    outgoing |= 0x020; // set bit 5
  } else if (value == 2100) {
    outgoing = current | 0x030; // set bits 5-4
  } else {
    outgoing = current;
    logger.loge("TDRIVEN set: invalid input");
    return false;
  }

  write(DRIVE, outgoing);
  return logger.logSet("DRIVE", "TDRIVEN", value, getTDriveN() == value);
}

bool drv::setTDriveP(int value) {
  unsigned int current = read(DRIVE) & ~0xF000;
  unsigned int outgoing;

  if (value == 263) {
    outgoing = current & ~0x0C0; // clear bits 7-6
  } else if (value == 525) {
    outgoing = current & ~0x0C0; // clear bits 7-6
    outgoing |= 0x040; // set bit 6
  } else if (value == 1050) {
    outgoing = current & ~0x0C0; // clear bits 7-6
    outgoing |= 0x080; // set bit 7
  } else if (value == 2100) {
    outgoing = current | 0x0C0; // set bits 7-6
  } else {
    outgoing = current;
    logger.loge("TDRIVEP set: invalid input");
    return false;
  }
  
  write(DRIVE, outgoing);
  return logger.logSet("DRIVE", "TDRIVEP", value, getTDriveP() == value);
}

bool drv::setIDriveN(int value) {
  unsigned int current = read(DRIVE) & ~0xF000;
  unsigned int outgoing;

  if (value == 100) {
    outgoing = current & ~0x300; // clear bits 9-8
  } else if (value == 200) {
    outgoing = current & ~0x300; // clear bits 9-8
    outgoing |= 0x100; // set bit 8
  } else if (value == 300) {
    outgoing = current & ~0x300; // clear bits 9-8
    outgoing |= 0x200; // set bit 9
  } else if (value == 400) {
    outgoing = current | 0x300; // set bits 9-8
  } else {
    outgoing = current;
    logger.loge("IDRIVEN set: invalid input");
    return false;
  }

  write(DRIVE, outgoing);
  return logger.logSet("DRIVE", "IDRIVEN", value, getIDriveN() == value);
}

bool drv::setIDriveP(int value) {
  unsigned int current = read(DRIVE) & ~0xF000;
  unsigned int outgoing;

  if (value == 50) {
    outgoing = current & ~0xC00; // clear bits 11-10
  } else if (value == 100) {
    outgoing = current & ~0xC00; // clear bits 11-10
    outgoing |= 0x400; // set bit 8
  } else if (value == 150) {
    outgoing = current & ~0xC00; // clear bits 11-10
    outgoing |= 0x800; // set bit 9
  } else if (value == 200) {
    outgoing = current | 0xC00; // set bits 11-10
  } else {
    outgoing = current;
    logger.loge("IDRIVEP set: invalid input");
    return false;
  }

  write(DRIVE, outgoing);
  return logger.logSet("DRIVE", "IDRIVEP", value, getIDriveP() == value);
}

// *** GETTERS ***

char* drv::getHbridge() {
  unsigned int current = read(CTRL) & 0x001;
  char* get = "none";

  if (current == 0) {
    get = "off";
  } else if (current == 1) {
    get = "on";
  }

  return get;
}

int drv::getISGain() {
  unsigned int current = read(CTRL) & 0x300;
  int get = 0;

  if (current == 0x000) {
    get = 5;
  } else if (current == 0x100) {
    get = 10;
  } else if (current == 0x200) {
    get = 20;
  } else if (current == 0x300) {
    get = 40;
  }

  return get;
}

int drv::getDTime() {
  unsigned int current = read(CTRL) & 0xC00;
  int get = 0;

  if (current == 0x000) {
    get = 410;
  } else if (current == 0x400) {
    get = 460;
  } else if (current == 0x800) {
    get = 670;
  } else if (current == 0xC00) {
    get = 880;
  }

  return get;
}

unsigned int drv::getTorque() {
  return read(TORQUE) & 0x0FF;
}

unsigned int drv::getTOff() {
  return read(OFF) & 0x0FF;
}

unsigned int drv::getTBlank() {
  return read(BLANK) & 0x0FF;
}

unsigned int drv::getTDecay() {
  return read(DECAY) & 0x0FF;
}

char* drv::getDecMode() {
  unsigned int current = read(DECAY) & 0x700;
  char* get = "none";

  if (current == 0x000) {
    get = "slow";
  } else if (current == 0x200) {
    get = "fast";
  } else if (current == 0x300) {
    get = "mixed";
  } else if (current == 0x500) {
    get = "auto";
  }

  return get;
}

int drv::getOCPThresh() {
  unsigned int current = read(DRIVE) & 0x003;
  int get = 0;

  if (current == 0x000) {
    get = 250;
  } else if (current == 0x001) {
    get = 500;
  } else if (current == 0x002) {
    get = 750;
  } else if (current == 0x003) {
    get = 1000;
  }

  return get;
}

float drv::getOCPDeglitchTime() {
  unsigned int current = read(DRIVE) & 0x00C;
  float get = 0;

  if (current == 0x000) {
    get = 1.05;
  } else if (current == 0x002) {
    get = 2.1;
  } else if (current == 0x004) {
    get = 4.2;
  } else if (current == 0x00C) {
    get = 8.4;
  }

  return get;
}

int drv::getTDriveN() {
  unsigned int current = read(DRIVE) & 0x030;
  int get = 0;

  if (current == 0x000) {
    get = 263;
  } else if (current == 0x010) {
    get = 525;
  } else if (current == 0x020) {
    get = 1050;
  } else if (current == 0x030) {
    get = 2100;
  }

  return get;
}

int drv::getTDriveP() {
  unsigned int current = read(DRIVE) & 0x0C0;
  int get = 0;

  if (current == 0x000) {
    get = 263;
  } else if (current == 0x040) {
    get = 525;
  } else if (current == 0x080) {
    get = 1050;
  } else if (current == 0x0C0) {
    get = 2100;
  }

  return get;
}

int drv::getIDriveN() {
  unsigned int current = read(DRIVE) & 0x300;
  int get = 0;

  if (current == 0x000) {
    get = 100;
  } else if (current == 0x100) {
    get = 200;
  } else if (current == 0x200) {
    get = 300;
  } else if (current == 0x300) {
    get = 400;
  }

  return get;
}

int drv::getIDriveP() {
  unsigned int current = read(DRIVE) & 0xC00;
  int get = 0;

  if (current == 0x000) {
    get = 50;
  } else if (current == 0x400) {
    get = 100;
  } else if (current == 0x800) {
    get = 150;
  } else if (current == 0xC00) {
    get = 200;
  }

  return get;
}

void drv::getFault() {
  unsigned int current = read(STATUS) & 0x03F;
  if (current & 0x1) {
    faults[0] = true;
  }
  if (current & 0x2) {
    faults[1] = true;
  }
  if (current & 0x4) {
    faults[2] = true;
  }
  if (current & 0x8) {
    faults[3] = true;
  }
  if (current & 0x10) {
    faults[4] = true;
  }
  if (current & 0x20) {
    faults[5] = true;
  }

}

void drv::clearFault(int value) {
  write(STATUS, 0<<value); 
}