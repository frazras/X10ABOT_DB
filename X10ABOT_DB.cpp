/*
  X10ABOT_DB.h - Library for  X10ABOT_DB code.
  Created by Rohan A. Smith, January 31, 2012.
  Released into the public domain.
*/
#include <Wire.h>
#include "X10ABOT_DB.h" //include the declaration for this class


void* pt2Object;

//Used as the most fundamental level of coding
MicroCode instr;

//PWM pins are 3,5, 6,9, 10 & 11
//IO pin are: 2,4, 7,8, 12 & 13 (0,1 =>[rx,tx])
OUTPUT_PORT output[] = {
  //{{PWM a, PWM b}, {IO a, IO b}}
  {{10, 11}, {12, 13}},  //port 1
  {{6, 9}, {7, 8}} //port 2
};

//Analog pins are 0,1,2,3,4 or as Digital {14,15,16,17}
INPUT_PORT input[] = {
  //{{IO a, IO b}, analog}
  {{2, 4}, 0}, //port 1
  {{16, 17}, 1} //port 2
};

//available IO: {0,1} =>[rx,tx])
//available PWM {3,5}

//<<constructor>>
X10ABOT_DB::X10ABOT_DB(byte db_address, byte logging){
  _logging = logging;
  Wire.begin(db_address);                // join i2c bus with address #4
 //Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent_wrapper); // register event
}

X10ABOT_DB::X10ABOT_DB(byte logging){
  _logging = logging;
}


//<<destructor>>
X10ABOT_DB::~X10ABOT_DB(){/*nothing to destruct*/}

/*Byte 1:1111XXXX FUNCTION BYTE
Byte 1:XXXX1111 OPERAND BYTE
Byte 2:11111111 D.B. SELECTION
Byte 3:1111111X PORT SELECTION
Byte 3:XXXXXXX1 PIN SELECTION
Byte 3+n 11111111 DATA BYTES; n> 0*/


/**
* Static wrapper method for the recieve event;
**/
void X10ABOT_DB::receiveEvent_wrapper (int numBytes){
       // explicitly cast to a pointer to TClassA
       X10ABOT_DB* mySelf = (X10ABOT_DB*) pt2Object;

       // call member
       mySelf->receiveEvent(numBytes);
   }

/**
* Implementation of the Wire/I2C recieve event
**/

void X10ABOT_DB::receiveEvent(int numBytes)
{
  byte fn_op, db, port;
  while(0 < Wire.available()) // loop through all but the last
  {

    fn_op = Wire.read(); // receive FUNCTION & OPERATOR byte
    instr.db = Wire.read(); // receive Daughter Board's # byte
    port = Wire.read(); // receive Daughter Board's Port # byte

    byte op_mask = 0b00001111; // operator bitmask
    byte pin_mask =0b00000001;  //pin bitmask

    instr.fn   = fn_op  >> 4;
    instr.port = port >> 1;
    instr.op   = (fn_op & op_mask);
    instr.pin  = (port & pin_mask);

    //Serial.print("instr.fn: ");Serial.println(instr.fn, BIN);
    //Serial.print("instr.op: ");Serial.println(instr.op, BIN);
    //Serial.print("instr.db: ");Serial.println(instr.db, BIN);
    //Serial.print("instr.pt: ");Serial.println(instr.port, BIN);
    //Serial.print("instr.pn: ");Serial.println(instr.pin, BIN);

    if (0<Wire.available())
    {
      while(0 < Wire.available()){ // loop through all but the last
        //Serial.println("DATA");
        instr.data = Wire.read(); // receive byte as a character
        //Serial.print("instr.data: ");Serial.println(instr.data, BIN);
        execParse(instr);      //Calls the function delegator
      }
    }
    else{
      execParse(instr);
    }
  }
}

void X10ABOT_DB::localEvent(byte * message, int numBytes)
{
  byte fn_op, db, port;
  if (numBytes>=3) // loop through all but the last
  {

    fn_op = message[0]; // receive FUNCTION & OPERATOR byte
    instr.db = message[1]; // receive Daughter Board's # byte
    port = message[2]; // receive Daughter Board's Port # byte

    byte op_mask = 0b00001111; // operator bitmask
    byte pin_mask =0b00000001;  //pin bitmask

    instr.fn   = fn_op  >> 4;
    instr.port = port >> 1;
    instr.op   = (fn_op & op_mask);
    instr.pin  = (port & pin_mask);
    numBytes = numBytes - 3;

    //Serial.print("instr.fn: ");Serial.println(instr.fn, BIN);
    //Serial.print("instr.op: ");Serial.println(instr.op, BIN);
    //Serial.print("instr.db: ");Serial.println(instr.db, BIN);
    //Serial.print("instr.pt: ");Serial.println(instr.port, BIN);
    //Serial.print("instr.pn: ");Serial.println(instr.pin, BIN);

    if (numBytes>0)
    {
      byte x = 3;
      while(0 < numBytes){ // loop through all but the last
        //Serial.println("DATA");
        instr.data = message[x]; // receive byte as a character
        //Serial.print("instr.data: ");Serial.println(instr.data, BIN);
        execParse(instr);      //Calls the function delegator
        x++;
        numBytes--;
      }
    }
    else{
      execParse(instr);
    }
  }
}

int X10ABOT_DB::execParse(MicroCode instr){  //byte fn, byte op, byte db, byte port, byte data)
  int val;
  switch( instr.fn )
  {
    case DB_FN_IO:
    //Serial.println("IO");

    switch( instr.op )
    {
      //Initialise output port
      case DB_OP_IO_HI:
      pinMode(output[instr.port].io_pin[instr.pin], OUTPUT);
      //Serial.print("PORT ");Serial.print(instr.port, DEC);Serial.print(" LED ");Serial.print(instr.op, DEC);Serial.print(" ON ");Serial.println(instr.pin, DEC);
      digitalWrite(output[instr.port].io_pin[instr.pin], HIGH);
      break;


    case DB_OP_IO_LOW:
      pinMode(output[instr.port].io_pin[instr.pin], OUTPUT);
      //Serial.print("PORT ");Serial.print(instr.port, DEC);Serial.print(" LED ");Serial.print(instr.pin, DEC);Serial.println(" OFF");
      digitalWrite(output[instr.port].io_pin[instr.pin], LOW);
      break;


      //Initialise input port
    case DB_OP_IO_INP:
    //case 2:
      pinMode(output[instr.port].io_pin[instr.pin], INPUT);
      return digitalRead(output[instr.port].io_pin[instr.pin]);
      break;
    }

    break;

  case DB_FN_PWM:
      analogWrite(output[instr.port].pwm_pin[instr.pin], instr.data);
      //Serial.print("PORT ");Serial.print(instr.port, DEC);
      //Serial.print(" PWM("); Serial.print(instr.pin); Serial.print(") DATA="); Serial.println(instr.data);
      break;

  case DB_FN_SERIAL:
    //Serial.println("SERIAL");
    break;

  default:
    Serial.println("NONE");
  }
  //Serial.println("DONE");
  //delay(5000);
  return val;
}
