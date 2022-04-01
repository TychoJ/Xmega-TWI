# Xmega-TWI
This is a library for the Xmega devices that support TWI. At the moment it's only been tested on the AtXmega256A3U. But if your Xmega device has a TWI module it should still work.  
The library is meant to make I2C communication easier.  
At the moment this library can only be used by a I2C/TWI master.


## How to use

Add the c and h file to your project and include the h file in the file you want to use this library.

## Usage

```c
#define F_CPU xx          // xx is the clock speed of the Xmega device
#define TWI_ADRESS xxxx   // The adress of the I2C device

#define REG1 xx   // A register of the I2C device
#define REG2 xx   // A register of the I2C device

#include <avr/io.h>
#include <TWI.h>

int main(void){
  
  /*
   *  TWIx is the TWI module of the Xmega device you want to use.
   *  BAUD_X00K is the speed of the I2C/TWI communication. You can use BAUD_100K and BAUD_400K.
   *  You can find out which one you need from the datasheet of the device you want to communicate with.
   *  For single master operations you can leave TIMEOUT_DIS as is. Other options are: TIMEOUT_50US, 
   *                                                                 TIMEOUT_100US and TIMEOUT_200US.
   */
  enable_TWI(&TWIx, BAUD_X00K, TIMEOUT_DIS);
  
  uint8_t write = x // x is the data that is going to be written to a register of the I2C device
  uint8_t read;     // The data read from the I2C device register is going to be stored in this variable 
  
  while(1){
    write_8bit_register_TWI(&TWIx, TWI_ADRESS, write, REG1);
    read_8bit_register_TWI(&TWIx, TWI_ADRESS, &read, REG2);
  }
}
```

## For HvA students
Use `TWIE` see example below

```c
#define F_CPU xx          // xx is the clock speed of the Xmega device
#define TWI_ADRESS xxxx   // The adress of the I2C device

#define REG1 xx   // A register of the I2C device
#define REG2 xx   // A register of the I2C device

#include <avr/io.h>
#include <TWI.h>

int main(void){
  
  /*
   *  TWIE is the TWI module of the Xmega board developed by Jan Derk Bakker you want to use.
   *  BAUD_X00K is the speed of the I2C/TWI communication. You can use BAUD_100K and BAUD_400K.
   *  You can find out which one you need from the datasheet of the device you want to communicate with.
   *  For single master operations you can leave TIMEOUT_DIS as is. Other options are: TIMEOUT_50US, 
   *                                                                 TIMEOUT_100US and TIMEOUT_200US.
   */
  enable_TWI(&TWIE, BAUD_X00K, TIMEOUT_DIS);
  
  uint8_t write = x // x is the data that is going to be written to a register of the I2C device
  uint8_t read;     // The data read from the I2C device register is going to be stored in this variable 
  
  while(1){
    write_8bit_register_TWI(&TWIE, TWI_ADRESS, write, REG1);
    read_8bit_register_TWI(&TWIE, TWI_ADRESS, &read, REG2);
  }
}
```

## Questions
Make use of the forum if you have questions on how to use the library. Or if you have an idea to improve the useability of the library.  

## Contributing
If you find an issue with the library please open an issue and explain what you think doesn't work.  

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## State of development

### Before V1.0.0
  - add doxygen documentation
  - fix restart function
  
### After V1.0.0
  - add interrupt based functions
  - add the possibility to use the library as a slave device
  
## License
[MIT](https://choosealicense.com/licenses/mit/)
