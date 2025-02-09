/**
 @file FaBoGPIO_PCA9698.cpp
 @brief This is a library for the FaBo GPIO I2C Brick.

   Released under APACHE LICENSE, VERSION 2.0

   http://www.apache.org/licenses/

 @author FaBo<info@fabo.io>
*/

#include "FaBoGPIO40_PCA9698-mod.h"

/**
 @brief Constructor
*/
FaBoGPIO40::FaBoGPIO40(uint8_t addr) {
  _i2caddr = addr;
  _output_port[0] = 0x00;
  _output_port[1] = 0x00;
  Wire.begin();
}

/**
 @brief Configure Device
*/
void FaBoGPIO40::configuration() {
  Wire.beginTransmission(_i2caddr);
  Wire.write(PCA9698_CONFIG_PORT0);
  byte mode[2];
  mode[0] = 0x00;
  mode[1] = 0x00;
  Wire.write(mode, sizeof(mode));
  Wire.endTransmission();
}

/**
 @brief write GPIO
 @param [in] pin pinNo
 @param [in] output output
*/
void FaBoGPIO40::digitalWrite(uint8_t pin, uint8_t output) {
  if(8 > pin && pin >=0) {
    if (output == HIGH) {
      _output_port[0] |= 1<<pin;
    } else if (output == LOW){
      _output_port[0] &= ~(1<<pin);
    }
    writeI2c(PCA9698_OUTPUT_PORT0, _output_port[0]);
  } else if(16 > pin && pin >= 8) {
    if (output == HIGH) {
      _output_port[1] |= 1<<(pin - 8);
    } else if (output == LOW){
      _output_port[1] &= ~(1<<(pin - 8));
    }
    writeI2c(PCA9698_OUTPUT_PORT1, _output_port[1]);
  } 
}

void FaBoGPIO40::portWrite(uint8_t port, uint8_t value) { 
	if (port == 0) {
		_output_port[port] = value;
		writeI2c(PCA9698_OUTPUT_PORT0, value);
	} else if (port == 1) {
		_output_port[port] = value;
		writeI2c(PCA9698_OUTPUT_PORT1, value);
	}
}

void FaBoGPIO40::pinWrite(uint8_t pin, uint8_t value) {
	int port;

	if (pin >= 16)
		return;

	port = (pin < 8) ? 0 : 1;
	if (value == HIGH) {
		portWrite(port, _output_port[port] | (1 << (pin % 8)));
	} else {
		portWrite(port, _output_port[port] & ~(1 << (pin % 8)));
	}
}

/**
 @brief read GPIO
 @param [in] pin pinNo
*/
int FaBoGPIO40::digitalRead(uint8_t pin) {
  if(8 > pin && pin >=0) {
    byte data[1];
    readI2c(PCA9698_INPUT_PORT0, sizeof(data), data);
    if((data[0] & (1<<pin)) == (1<<pin)) {
      return HIGH;
    } else {
      return LOW;
    }
  } else if(16 > pin && pin >= 8) {
    byte data[1];
    readI2c(PCA9698_INPUT_PORT1, sizeof(data), data);
    if((data[0] & (1<<pin-8)) == (1<<pin-8)) {
      return HIGH;
    } else {
      return LOW;
    }
    
  }
}

/**
 @brief All Port to LOW
*/
void FaBoGPIO40::setAllClear() {
	portWrite(0, 0);
	portWrite(1, 0);
}

/**
 @brief set Port to GPIO
 @param [in] output output
*/
void FaBoGPIO40::portMode(uint8_t port, uint8_t mode){  
  if(port == 0) { 
    if(mode == INPUT) {
      writeI2c(PCA9698_CONFIG_PORT0, 0xff);
    } else if(mode == OUTPUT) {
      writeI2c(PCA9698_CONFIG_PORT0, 0x00);
    }
  } else if(port == 1) { 
    if(mode == INPUT) {
      writeI2c(PCA9698_CONFIG_PORT1, 0xff);
    } else if(mode == OUTPUT) {
      writeI2c(PCA9698_CONFIG_PORT1, 0x00);
    }
  } 
}
  
  

/**
 @brief Write I2C
 @param [in] address register address
 @param [in] data write data
*/
void FaBoGPIO40::writeI2c(uint8_t address, uint8_t data) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

/**
 @brief Read I2C
 @param [in] address register address
 @param [in] num read length
 @param [out] data read data
*/
void FaBoGPIO40::readI2c(uint8_t address, uint8_t num, uint8_t * data) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(address);
  Wire.endTransmission();
  uint8_t i = 0;
  Wire.requestFrom(_i2caddr, num);
  while( Wire.available() ) {
    data[i++] = Wire.read();
  }
}
