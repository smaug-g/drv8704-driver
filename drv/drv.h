/*
  Drv.h - Library for communicating with a DRV8704
  Created by Sergio Mauricio Guerrero for REV. March 15, 2018

  Usage:
  
    "drv sailboat(CSC pin);"
    void setup() {

    drv.setBridge("on");
    drv.setBridge("off");

    drv.setLogging("info");

    }
    

  Dependencies:

    REV Logger Library. (should be included)

  ** see drv.h for full doc **

*/
#pragma once
#include <Arduino.h>
#include <SPI.h>

class drv {
    public:
        
        drv(int out, int in, int clk, int select);
        
        // pins
        int _MOSI;
        int _MISO;
        int _SCLK;
        int _SCS;

        // faults
        bool faults[];
        
        
        // register addresses
        const int CTRL = 0x0;
        const int TORQUE = 0x1;
        const int OFF = 0x2;
        const int BLANK = 0x3;
        const int DECAY = 0x4;
        const int DRIVE = 0x6;
        const int STATUS = 0x7;

        unsigned int currentRegisterValues[8];

        // Default reg values
        unsigned int initRegs[];

        // functions 
        
        /*
        opens SPI bus
        */
        void open();

        /*
        closes SPI bus
        */
        void close();
        
        /*
        reads from given address
        */
        unsigned int read(unsigned int address);

        /*
        writes value to address
        returns true if successful
        */
        void write(unsigned int address, unsigned int value);
        
        /*
        sets logging level for DRV logger object (see Logger.h)
        */
        void setLogging(char* level);
        
        /*
        reads all registers and stores in currentRegisterValues
        */
        void getCurrentRegisters();

        /*
        confirms that all Regs have desired values
        desiredRegs[]: array with 7 entries each with 12 bit values (one for each reg)
        */
        void regDiagnostic(int desiredRegs[]);

        
        // *** SETTERS ***


        // following funcs deal with CTRL register

        /*
        sets ENBL register
        value: 
            "on" - turns on h-bridge
            "off" ("on" default)
        returns true if successful
        */
        bool setHbridge(char* value);

        /*
        sets ISGAIN register
        value: 
            5 - sets ISENSE amplifier gain to 5 V/V
            10/20/40 (40 default)
        returns true if successful
        */
        bool setISGain(int value);

        /*
        sets DTIME register
        value:
            410 - sets dead time to 410 ns
            460/670/880 (410 default)
        returns true if successful
        */
        bool setDTime(int value);

        // following func deals with TORQUE register

        /*
        sets TORQUE register
        value: 0-255 - Sets full-scale output current for both H-bridges (255 default)
        returns true if successful
        */
        bool setTorque(unsigned int value);

        // following func deals with OFF register (only TOFF, PWMMODE shouldnt be modified)

        /*
        sets TOFF register
        value: 0-255 - Sets fixed off time, in increments of 525 ns
            0 - 525 ns
            48 - default
            255 - 133.8 us
        returns true if successful
        */
        bool setTOff(unsigned int value);

        // following func deals with BLANK register

        /*
        sets TBLANK register
        value: 0-255 - Sets current trip blanking time, in increments of 21 ns (128 default)
        returns true if successful
        */
        bool setTBlank(unsigned int value);

        // following funcs deal with DECAY register

        /*
        sets TDECAY register
        value: 0-255 - Sets mixed decay transition time, in increments of 525 ns (16 default)
        returns true if successful
        */
        bool setTDecay(unsigned int value);

        /*
        sets DECMODE register
        value: 
            "slow" - force slow decay at all times (default)
            "fast" - force fast decay at all times
            "mixed" - use mixed decay at all times
            "auto" - use auto mixed decay at all times
        returns true if successful
        */
        bool setDecMode(char* value);

        // following funcs deal with DRIVE register

        /*
        sets OCPTH register
        value:
            250 - sets OCP threshold to 250 mV
            500/750/1000 (500 default)
        returns true if successful
        */
        bool setOCPThresh(int value);

        /*
        sets OCPDEG register 
        value:
            1.05 - sets OCP deglitch time to 1.05 us
            2.1/4.2/8.4 (2.1 default)
        returns true if successful
        */
        bool setOCPDeglitchTime(float value);

        /*
        sets TDRIVEN register
        value:
            263 - sets Gate drive sink time to 263 ns
            525/1050/2100 (1050 default)
        returns true if successful
        */
        bool setTDriveN(int value);
        
        /*
        sets TDRIVEP register
        value:
            263 - sets Gate drive source time to 263 ns
            525/1050/2100 (1050 default)
        returns true if successful
        */
        bool setTDriveP(int value);

        /*
        sets IDRIVEN register
        value:
            100 - sets Gate drive peak sink current to 100 mA
            200/300/400 (400 default)
        returns true if successful
        */
        bool setIDriveN(int value);

        /*
        sets IDRIVEP register
        value:
            50 - sets Gate drive peak source current to 50 mA
            100/150/200 (200 default)
        returns true if successful
        */
        bool setIDriveP(int value);



        // *** GETTERS ***
        // all getters return the value one would pass the corresponding setter

        char* getHbridge();

        int getISGain();

        int getDTime();

        unsigned int getTorque();

        unsigned int getTOff();

        unsigned int getTBlank();

        unsigned int getTDecay();

        char* getDecMode();

        int getOCPThresh();

        float getOCPDeglitchTime();

        int getTDriveN();

        int getTDriveP();

        int getIDriveN();

        int getIDriveP();
        
        /*
        Returns bits 0-5 of STATUS register
        */
  
        void getFault();


                
        /*
        clears a Fault if there is one.
        value: OTS - over temp                  (0) (auto clear)
            AOCP - Channel A over current       (1)     
            AOCP - Channel A over current       (1)
            BOCP - Channel B "      "           (2)
            APDF - Channel A predriver fault    (3)
            BPDF - Channel B "         "        (4)
            UVLO - Undervoltage                 (5) (auto clear)
        */
        void clearFault(int value);

        
};


