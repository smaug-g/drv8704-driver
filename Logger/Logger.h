/*
    Logger.h - simple logging library for REV projects

    Created by Sergio Mauricio Guerrero for REV. March 16, 2018

    Outputs log messages to serial port

    Usage:
    initialize a Logger object:
    Logger logger(char* level);
    logger.setLevel(error/info/off);
    logger.logi("info message");
    logger.loge("error message");

*/

#pragma once

#include <Arduino.h>

class Logger {

    public: 
     /*
     initializes Serial communications (baud rate: 9600)
     */
     Logger(char* tagg, char* level);

     /*
     stores logging level
     */
     char* lvl;

     char* tag;

     /*
     sets logging:
     "off" - nothing except globals
     "error" - only errors
     "info" - all messages
     */
     void setLevel(char* level);

     /*
     info log message
     */
     void logi(char* message);

     /*
     error log message
     */
     void loge(char* message);

     /*
     global log message
     */
     void logg(char* message);


     // DRV Specific Functions

     /*
     logging for Setter functions of DRV
     if success : logs info - "TAG - reg register subreg setting write success"
     else : logs error - "TAG - reg register subreg setting write fail"
    
     Usage:
        bool success = drv.write(CTRL, value));
        logSet("CTRL", "ENBL", "on", success);
    

     */
     bool logSet(char* reg, char* subreg, char* setting, bool success);

     bool logSet(char* reg, char* subreg, int setting, bool success);

     bool logSet(char* reg, char* subreg, float setting, bool success);

     bool logSet(char* reg, char* subreg, unsigned int setting, bool success);



};
