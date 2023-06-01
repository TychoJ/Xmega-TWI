/*
 * File TWI.c
 * Author: Tycho J�bsis
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
#include <util/delay.h>
#include "twi.h"

void set_baud(TWI_t *twi, uint32_t TWI_speed){
	twi->MBAUD = TWI_BAUD(F_CPU, TWI_speed);
}

void enable_TWI(TWI_t *twi, uint32_t TWI_speed, uint8_t timeout){
	set_baud(twi, TWI_speed);
	set_acknowledge(twi, ACK);
    twi->MCTRLA |= 1 << TWI_ENABLE_bp;
	set_timeout(twi, timeout);	
}

void disable_TWI(TWI_t *twi){
	twi->MCTRLA ^= (0 << TWI_ENABLE_bp);
}

void set_timeout(TWI_t *twi, uint8_t time_out){
    // Clear the timeout settings bits in the register 
    twi->CTRLA &= ~(0x3 << TWI_TIMEOUT_0_bp);

    time_out &= 0x03;
	twi->CTRLA |= time_out;

    // Set the bus state to IDLE
	twi->MSTATUS &= ~(0<<1);
    twi->MSTATUS &= ~(1<<0);
}

void set_acknowledge(TWI_t *twi, uint8_t ack){
    ack = (~ack) & 0x01;    // Make it possible to use defines within this library

    if (ack) twi->MCTRLB |= ack << TWI_ACKACT_bp; // Set AKACT in Master CTRLB to 1
    else twi->MCTRLB &= (~ack) << TWI_ACKACT_bp;  // Set AKACT in Master CTRLB to 0
	
}

uint8_t bus_state(TWI_t *twi){
	uint8_t ret = twi->MSTATUS;

    // Filter the bus state
	ret &= ~0x03;

    // Return the bus state
	return ret;
}

void set_bus_state_TWI(TWI_t *twi, uint8_t state){
    twi->MSTATUS |= (twi->MSTATUS & (~0x3)) | state; 
}

uint8_t wait_till_send(TWI_t *twi, uint8_t rw){
	uint8_t send_suc = 0;
	uint16_t time_passed = 0;
	
	while ( !send_suc ) {
		
		if(twi->MSTATUS & (TWI_WIF_bm << rw)) send_suc = 1;
		
		if(time_passed > 1000) return DATA_NOT_SEND;
		_delay_us(1);		
		time_passed++;
	}
	return TWI_STATUS_OK;
}

uint8_t wait_till_received(TWI_t *twi, uint8_t rw){
	if(wait_till_send(twi, rw) == TWI_STATUS_OK) return TWI_STATUS_OK;
	return DATA_NOT_RECEIVED;
}

uint8_t start_TWI(TWI_t *twi, uint8_t addr, uint8_t rw){
	
	if(bus_state(twi) != BUS_NOT_IN_USE) return bus_state(twi);
	
	if( !( (rw == READ) || (rw == WRITE) ) ) return INVALID_RW;
	
	twi->MASTER.ADDR = (addr << 1) | rw;	//send slave address
	if(wait_till_send(twi, rw) == DATA_NOT_SEND) return DATA_NOT_SEND; // wait until sent
	
	//when RXACK is 0 an ACK has been received
	if(twi->MASTER.STATUS & TWI_MASTER_RXACK_bm){
		stop_TWI(twi);
		return NACK;
		} 
	
	return ACK;
}

uint8_t repeated_start_TWI(TWI_t *twi, uint8_t addr, uint8_t rw){
	if(bus_state(twi) != OWNER_OF_BUS) return bus_state(twi);
	
	if( !( (rw == READ) || (rw == WRITE) ) ) return INVALID_RW;
	twi->MASTER.ADDR = (addr << 1) | rw;
	
	wait_till_send(twi, rw);
	
	
	//when RXACK is 0 an ACK has been received
	if(twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) return NACK;
	
	return TWI_STATUS_OK;
}

void stop_TWI(TWI_t *twi){
	twi->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
	
}

uint8_t send_TWI(TWI_t *twi, uint8_t data){
	twi->MASTER.DATA = data;
	
	if( wait_till_send(twi, WRITE) == DATA_NOT_SEND) return DATA_NOT_SEND;
	
	//when RXACK is 0 an ACK has been received
	if(twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) return NACK;
	
	return ACK;
}

uint8_t read_TWI(TWI_t *twi, uint8_t *data, uint8_t go_on){
	if(wait_till_received(twi, READ) == DATA_NOT_RECEIVED) return DATA_NOT_RECEIVED;
	(*data) = twi->MASTER.DATA;
	 twi->MASTER.CTRLC = ((go_on == ACK) ? TWI_MASTER_CMD_RECVTRANS_gc :        // send ack (go on) or
	 TWI_MASTER_ACKACT_bm|TWI_MASTER_CMD_STOP_gc); //     nack (and stop)
	return TWI_STATUS_OK;
}

uint8_t send_8bit_TWI(TWI_t *twi, uint8_t addr, uint8_t data){
	uint8_t err;
	
	err = start_TWI(twi, addr, WRITE);
	
	//check for errors
	if(err == BUS_IN_USE) return BUS_IN_USE;
	if(err == NACK) return NACK;
	
	err = send_TWI(twi, data);
	
	//check for errors
	if(err == DATA_NOT_SEND) return DATA_NOT_SEND;
	if(err == NACK) return NACK;
	
	stop_TWI(twi);
	
	return TWI_STATUS_OK;	
}

uint8_t write_8bit_register_TWI(TWI_t *twi, uint8_t addr, uint8_t data, uint8_t reg){
	uint8_t err;
	
	err = start_TWI(twi, addr, WRITE);
	
	//check for errors
	if(err == BUS_IN_USE) return BUS_IN_USE;
	if(err == NACK) return NACK;
	
	err = send_TWI(twi, reg);
	
	//check for errors
	if(err == DATA_NOT_SEND) return DATA_NOT_SEND;
	if(err == NACK) return NACK;
	
	err = send_TWI(twi, data);
	
	//check for errors
	if(err == DATA_NOT_SEND) return DATA_NOT_SEND;
	if(err == NACK) return NACK;
	
	stop_TWI(twi);
	
	return TWI_STATUS_OK;
}

uint8_t read_8bit_register_TWI(TWI_t *twi, uint8_t addr, uint8_t *data, uint8_t reg){
	uint8_t err;
	
	err = start_TWI(twi, addr, WRITE);
	
	//check for errors
	if(err == BUS_IN_USE) return BUS_IN_USE;
	if(err == NACK) return NACK;
	
	err = send_TWI(twi, reg);
	
	//check for errors
	if(err == DATA_NOT_SEND) return DATA_NOT_SEND;
	if(err == NACK) return NACK;
	
	//err = repeated_start_TWI(twi, addr, READ);
	twi->MASTER.ADDR = (addr << 1) | READ;
	while( ! (twi->MASTER.STATUS & (TWI_MASTER_WIF_bm << READ)) );  // wait until sent
	
	//check for errors
	if(err == BUS_IN_USE) return BUS_IN_USE;
	if(err == NACK) return NACK;
	
	err = read_TWI(twi, data, NACK);
	
	if(err == DATA_NOT_RECEIVED) return DATA_NOT_RECEIVED; 
	
	return TWI_STATUS_OK;
}
