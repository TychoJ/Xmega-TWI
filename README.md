# Xmega-TWI
This is a library for the Xmega devices that support TWI. At the moment it's only been tested on the AtXmega256A3U. But if your Xmega device has a TWI module it should still work.  
The library is meant to make I2C communication easier.  
At the moment this library can only be used by a I2C/TWI master.


## How to use

Add the c and h file to your project and include the h file in the file you want to use this library.

## Usage

```c
#include <TWI.h>

int main(void){

  enable_twi(&TWIx, BAUD_400K, TIMEOUT_DIS);
  
  while(1){
    // your program
    asm("nope");
  }
}
```

## Questions
Make use of the forum if you have questions on how to use the library.  

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
