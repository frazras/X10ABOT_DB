/*
  X10ABOT_DB.h - Library for flashing X10ABOT_DB code.
  Created by Rohan A. Smith, January 31, 2012.
  Released into the public domain.
*/


#ifndef X10ABOT_DB_H
#define X10ABOT_DB_H


#include "Arduino.h" //"WProgram.h"
//#include <Wire.h>


//Functions
static const byte FN_IO = 1;
static const byte FN_PWM  = 2;
static const byte FN_SERIAL = 3;

//IO Operands
static const byte OP_IO_HI = 2;
static const byte OP_IO_LOW = 1;
static const byte OP_IO_INP = 0;

//PWM Operands
static const byte OP_PWM_A = 0;
static const byte OP_PWM_B = 1;

//port to pin assignment

typedef struct {
      byte io_pin[2];  //Input/Output pins
      byte analog; //Analog input pin
    }INPUT_PORT;

typedef struct {
      byte pwm_pin[2]; //Pulse Width Modulation pins
      byte io_pin[2]; //Input/Output pins
    }OUTPUT_PORT;

typedef struct {
      byte fn;   //function
      byte op;   //operand
      byte port; //port #
      byte db; //daughterboard address
      byte data; //databyte(s)
      byte pin; //pin selector
    }MicroCode;

//free pin analog 3

class X10ABOT_DB {

  public:
    X10ABOT_DB(byte logging);
    X10ABOT_DB(byte db_address, byte logging);
    ~X10ABOT_DB();

    void receiveEvent(int numBytes);
    static void receiveEvent_wrapper(int numBytes);
    int execParse(MicroCode instr);
    /**
    * Logging Functions
    **/
    void i2cStatusLog(byte var);

  private:
    int _logging;

};

  #endif
