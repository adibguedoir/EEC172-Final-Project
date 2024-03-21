//includes
#include <stdio.h>
#include <stdlib.h>
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "gpio.h"
#include "utils.h"
#include "gpio_if.h"
#include "pin_mux_config.h"
#include "uart.h"
#include "i2c_if.h"

// definition
#define APPLICATION_VERSION     "1.4.0"
#define APP_NAME                "Final Project"
#define UART_PRINT              Report

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//*****************************************************************************
void DisplayPrompt() {
    UART_PRINT("\n\rcmd#");
}

//*****************************************************************************
// Parses the user input command and invokes the I2C APIs
int ParseNProcessCmd(char *pcCmdBuffer) {
    // Code for command parsing and I2C API invocation
    return 0; // Placeholder return
}

//*****************************************************************************
// Board Initialization & Configuration
static void BoardInit(void) {
    MAP_IntMasterEnable();
}

//*****************************************************************************
// Main function handling the I2C example
void lastlab() {
    // Initialize board configurations
    BoardInit();

    // Configure the pinmux settings for the peripherals exercised
    PinMuxConfig();

    // Initialize UART
    InitTerm();

    // Initialize I2C
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    // Display prompt
    DisplayPrompt();

    // Variables for accelerometer data and tilt detection
    unsigned char xtilt, ytilt, alert;
    unsigned char xoff = (unsigned char)0x5;
    unsigned char yoff = (unsigned char)0x3;
    int mode = 1;

    GPIO_IF_LedConfigure(LED1|LED2|LED3);
    GPIO_IF_LedOff(MCU_ALL_LED_IND);

    // Main loop
    while(1){

        // Procedure for getting the x and y acceleration data
        I2C_IF_Write(0x18, &xoff, 1, 0);
        I2C_IF_Read(0x18, &xtilt, 1);
        I2C_IF_Write(0x18, &yoff, 1, 0);
        I2C_IF_Read(0x18, &ytilt, 1);

        // Prints the collected data to the terminal
        //UART_PRINT("xtilt: %d\n\r", xtilt);
        //UART_PRINT("ytilt: %d\n\r", ytilt);


        // Polling SW3
        if((GPIOPinRead(GPIOA1_BASE, 0x20) & 0x20) && (mode == 0 || mode == 2)) {
            mode = 1;
        }
        // Polling SW2
        if((GPIOPinRead(GPIOA2_BASE, 0x40) & 0x40) && (mode == 0 || mode == 1)) {
            mode = 2;
            MAP_UtilsDelay(8000000);
        }

        // Resets the alert flag
        alert = 0;

        if (mode == 2){
            // Checks if we are tilting down
            if (ytilt >= 200 && ytilt <= 240) {
                alert = 1;
            }
            // Checks if we are tilting up
            else if (ytilt >= 20 && ytilt <= 60) {
                alert = 1;
            }
            // Checks if we are tilting to the left
            if (xtilt >= 200 && xtilt <= 240) {
                alert = 1;
            }
            // Checks if we are tilting to the right
            else if (xtilt >= 20 && xtilt <= 60) {
                alert = 1;
            }
        }

        // Prints the stuff
        UART_PRINT("alert: %d\n\r", alert);
        UART_PRINT("mode: %d\n\r", mode);

        if (alert == 1){
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            while(mode == 2){
                if((GPIOPinRead(GPIOA1_BASE, 0x20) & 0x20) && (mode == 0 || mode == 2)) {
                    mode = 1;
                }
            }
        }
        else{
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        }

    }
}
