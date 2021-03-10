/*
 * TWI.c
 *
 * Created: 8-3-2021 15:08:48
 *  Author: tycho
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "TWI.h"

void set_baud(TWI_t *twi, uint32_t TWI_speed){
	twi->MASTER.BAUD = TWI_BAUD(F_CPU, TWI_speed);
}

void enable_TWI(TWI_t *twi, uint32_t TWI_speed, uint8_t timeout){
	set_baud(twi, TWI_speed);
	set_acknowledge(twi, ACK);
	twi->MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
	set_timeout(twi, timeout);	
}

void disable_TWI(TWI_t *twi){
	twi->MASTER.CTRLA = (0 << TWI_MASTER_ENABLE_bp);
}

void set_timeout(TWI_t *twi, uint8_t time_out){
	switch(time_out){
		case TIMEOUT_DIS:
		twi->MASTER.CTRLB = TIMEOUT_DIS;
		twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
		break;
		
		case TIMEOUT_50US:
		twi->MASTER.CTRLB = TIMEOUT_50US;
		twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_UNKNOWN_gc;
		break;
		
		case TIMEOUT_100US:
		twi->MASTER.CTRLB = TIMEOUT_100US;
		twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_UNKNOWN_gc;
		break;
		
		case TIMEOUT_200US:
		twi->MASTER.CTRLB = TIMEOUT_200US;
		twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_UNKNOWN_gc;
		break;
		
		default:
		twi->MASTER.CTRLB = TIMEOUT_DIS;
		twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
		break;
	}
}

void set_acknowledge(TWI_t *twi, uint8_t ack){
	(ack == ACK) ? (twi->MASTER.CTRLC = (0 << 2)) : (twi->MASTER.CTRLC = (1 << 2));
}

uint8_t bus_state(TWI_t *twi){
	uint8_t ret = twi->MASTER.STATUS;
	ret = (000000 << 2);
	return ret;
}

void set_bus_state_TWI(TWI_t *twi, uint8_t state){
	twi->MASTER.STATUS = state;
}

uint8_t start_TWI(TWI_t *twi, uint8_t addr, uint8_t rw){
	if(bus_state(twi) != BUS_NOT_IN_USE) return bus_state(twi);
	
	if( !( (rw == READ) || (rw == WRITE) ) ) return INVALID_RW;
	twi->MASTER.ADDR = (addr << 1) | rw;
	
	//when RXACK is 0 an ACK has been received
	if(twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) return NACK;
	
	return TWI_STATUS_OK;
}

uint8_t repeated_start_TWI(TWI_t *twi, uint8_t addr, uint8_t rw){
	if(bus_state(twi) != OWNER_OF_BUS) return bus_state(twi);
	
	if( !( (rw == READ) || (rw == WRITE) ) ) return INVALID_RW;
	twi->MASTER.ADDR = (addr << 1) | rw;
	
	//when RXACK is 0 an ACK has been received
	if(twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) return NACK;
	
	return TWI_STATUS_OK;
}

void stop_TWI(TWI_t *twi){
	twi->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
	
}

uint8_t send_TWI(TWI_t *twi, uint8_t data){
	twi->MASTER.DATA = data;
	
	uint8_t send_suc = 0;
	uint8_t time_passed = 0; 
	while ( !send_suc){
		if(time_passed > 10) return DATA_NOT_SEND;
		_delay_us(20);
		
		if(twi->MASTER.STATUS & TWI_MASTER_WIF_bm) send_suc = 0;
		
		time_passed++;
	}
	
	//when RXACK is 0 an ACK has been received
	if(twi->MASTER.STATUS & TWI_MASTER_RXACK_bm) return NACK;
	
	return ACK;
}

uint8_t read_TWI(TWI_t *twi, uint8_t *data){
	uint8_t rec_suc = 0;
	uint8_t time_passed = 0;
	while ( !rec_suc){
		if(time_passed > 10) return DATA_NOT_RECEIVED;
		_delay_us(20);
		
		if(twi->MASTER.STATUS & TWI_MASTER_RIF_bm) rec_suc = 0;
		
		time_passed++;
	}
	(*data) = twi->MASTER.DATA;
	
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
	
	//check bus state 
	//if still master repeat a stop condition
	//if different state do nothing
	
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
	
	err = repeated_start_TWI(twi, addr, READ);
	
	//check for errors
	if(err == BUS_IN_USE) return BUS_IN_USE;
	if(err == NACK) return NACK;
	
	err = read_TWI(twi, data);
	
	if(err == DATA_NOT_RECEIVED){
		stop_TWI(twi);
		return DATA_NOT_RECEIVED; 
	}
	
	stop_TWI(twi);
	
	return TWI_STATUS_OK;
}
