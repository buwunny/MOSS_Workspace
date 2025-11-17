//******************************************************************************
//*****************************    C Source Code    ****************************
//******************************************************************************
//  DESIGNER NAME:  Rafael Ortiz
//
//        VERSION:  1.0
//
//      FILE NAME:  main.c
//
//------------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the MOSS Expansion board. This is my final project for CSC 202.
//
//******************************************************************************
//******************************************************************************

//------------------------------------------------------------------------------
// Loads standard C include files
//------------------------------------------------------------------------------
#include <stdio.h>

//------------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//------------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"
#include "uart.h"
#include "lcd1602.h"
#include "adc.h"
#include "spi.h"
#include "kernel.h"
#include "shell.h"

//------------------------------------------------------------------------------
// Define function prototypes used by the program
//------------------------------------------------------------------------------
void system_init(void);


//------------------------------------------------------------------------------
// Define symbolic constants used by the program
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//------------------------------------------------------------------------------


int main(void)
{
  system_init();
  
  // Endless loop to prevent program from ending
  while (1);

} /* main */


void system_init(void)
{
  kernel_init();
  shell_init();
  shell_loop();
} /* system_init */
