/*
 * File TWI.h
 * Author: Tycho Jöbsis
 * Date: 13-02-2021
 */ 

/*
*	MIT License
*
*	Copyright (c) 2021 TychoJ
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
*/


#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 2000000UL
#endif

#ifndef TWI_H_
#define TWI_H_

#define BAUD_100K        100000UL
#define BAUD_400K        400000UL

#define TIMEOUT_DIS   TWI_MASTER_TIMEOUT_DISABLED_gc
#define TIMEOUT_50US  TWI_MASTER_TIMEOUT_50US_gc
#define TIMEOUT_100US TWI_MASTER_TIMEOUT_100US_gc
#define TIMEOUT_200US TWI_MASTER_TIMEOUT_200US_gc

#define	UNKNOWN_BUS_STATE	0
#define	BUS_NOT_IN_USE		1
#define	OWNER_OF_BUS		2
#define	BUS_IN_USE			3

#define	UNKNOWN_BUS_STATE_GR	TWI_MASTER_BUSSTATE_UNKNOWN_gc
#define	BUS_NOT_IN_USE_GR		TWI_MASTER_BUSSTATE_IDLE_gc
#define	OWNER_OF_BUS_GR			TWI_MASTER_BUSSTATE_OWNER_gc
#define	BUS_IN_USE_GR			TWI_MASTER_BUSSTATE_BUSY_gc

#define INVALID_RW	4

#define WRITE 0 
#define READ  1

#define NACK 0
#define ACK	 1
#define DATA_NOT_SEND 10
#define DATA_NOT_RECEIVED 7
#define TWI_STATUS_OK 5

//inline function to calculate the baud value
#define TWI_BAUD(F_SYS, F_TWI)   ((F_SYS / (2 * F_TWI)) - 5)

//set the baud rate of a TWI module
void set_baud(TWI_t *twi, uint32_t TWI_speed);

//enables a TWI module
//TWI_speed is used to calculate the baud rate
//timeout: if you are the only master you should use TIMEOUT_DIS
void enable_TWI(TWI_t *twi, uint32_t TWI_speed, uint8_t timeout);

//disables a TWI module
void disable_TWI(TWI_t *twi);

//time in us after which it's assumed the TWI bus is free

// disabled(normally used for i2c), 50us, 100us, 200us  
void set_timeout(TWI_t *twi, uint8_t time_out);

//selects if ACK or NACK will be used
void set_acknowledge(TWI_t *twi, uint8_t ack);

//returns in what state the bus is
uint8_t bus_state(TWI_t *twi);  

//function used for setting the bus state
void set_bus_state_TWI(TWI_t *twi, uint8_t state);

uint8_t wait_till_send(TWI_t *twi, uint8_t rw);

uint8_t wait_till_received(TWI_t *twi, uint8_t rw);

//issues an start condition and send an address
//returns 1 if an acknowledge is received
//returns 0 if a not acknowledge is received
//returns 3 if the bus is not free
uint8_t start_TWI(TWI_t *twi, uint8_t addr, uint8_t rw);

uint8_t repeated_start_TWI(TWI_t *twi, uint8_t addr, uint8_t rw);

//issues a stop condition
void stop_TWI(TWI_t *twi);

//internal function used for sending data
uint8_t send_TWI(TWI_t *twi, uint8_t data);

//internal function used to read data
//twi for what twi module
//data pointer to store read data
//go_on to continue or stop reading data
//returns 7 when data is not received
//returns 5 if data is received and no errors have occurred
//go_on 1 continue 0 stop reading
uint8_t read_TWI(TWI_t *twi, uint8_t *data, uint8_t go_on);

//send 8bits to the address 
uint8_t send_8bit_TWI(TWI_t *twi, uint8_t addr, uint8_t data);

//twi is the TWI port
//addr is the address of the TWI device were you want to write to a register
//data is the data that you want to write in the register
//reg is the register to which you want to write the data
uint8_t write_8bit_register_TWI(TWI_t *twi, uint8_t addr, uint8_t data, uint8_t reg);

//twi is the TWI port
//addr is the address of the TWI device were you want to read a register
//data is the variable where you want to store the data 
//reg is the register you want to read data from
uint8_t read_8bit_register_TWI(TWI_t *twi, uint8_t addr, uint8_t *data, uint8_t reg);


#endif /* TWI_H_ */