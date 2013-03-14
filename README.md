X10ABOT_DB
==========

Daughterboard Library for the X10ABOT Robotics Platform

Example Code
------------
```C++
    #include <X10ABOT_DB.h>

    #define LOGGING 1
    #define DB_ADDRESS 9
    #include <Wire.h>

    X10ABOT_DB bot(DB_ADDRESS, LOGGING);
    void setup(){
      Serial.begin(9600);
    }
    void loop(){
    }
```
