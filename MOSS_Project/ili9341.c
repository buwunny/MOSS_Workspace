// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  0.1
//
//       FILE NAME:  ili9341.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains a collection of functions for initializing and 
//    configuring the ILI9341 LCD display on the LP-MSPM0G3507 LaunchPad
//    and the MOSS Expansion Board.
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    This code was developed for educational purposes as part of the CSC202 
//    course and is provided "as is" without warranties of any kind, whether 
//    express, implied, or statutory.
//
//    The author and organization do not warrant the accuracy, completeness, or
//    reliability of the code. The author and organization shall not be liable
//    for any direct, indirect, incidental, special, exemplary, or consequential
//    damages arising out of the use of or inability to use the code, even if
//    advised of the possibility of such damages.
//
//    Use of this code is at your own risk, and it is recommended to validate
//    and adapt the code for your specific application and hardware requirements.
//
// Copyright (c) 2024 by TBD
//    You may use, edit, run or distribute this file as long as the above
//    copyright notice remains
// *****************************************************************************
//******************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "ti/devices/msp/peripherals/hw_iomux.h"
#include "ili9341.h"
#include "spi.h"
#include "clock.h"
#include "uart.h"


void ili9341_init(void)
{
  // Configure reset pin and DC pin

  // Set IOMUX for GPIO function
  IOMUX->SECCFG.PINCM[DC_IOMUX] = IOMUX_PINCM_PC_CONNECTED |
                                  PINCM_GPIO_PIN_FUNC;
  IOMUX->SECCFG.PINCM[RESET_IOMUX] = IOMUX_PINCM_PC_CONNECTED |
                                     PINCM_GPIO_PIN_FUNC;

  // Enable GPIO output port
  GPIOA->DOE31_0 |= DC_MASK;
  GPIOA->DOE31_0 |= RESET_MASK;

  // Clear DOUT
  GPIOA->DOUT31_0 &= ~RESET_MASK;
  GPIOA->DOUT31_0 &= ~DC_MASK;


  // Pin reset
  ili9341_reset();

  ili9341_write_command(ILI9341_SWRESET);


  // https://github.com/adafruit/Adafruit_ILI9341
  static const uint8_t initcmd[] = {
    0xEF, 3, 0x03, 0x80, 0x02,
    0xCF, 3, 0x00, 0xC1, 0x30,
    0xED, 4, 0x64, 0x03, 0x12, 0x81,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 1, 0x20,
    0xEA, 2, 0x00, 0x00,
    ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
    ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
    ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
    ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
    ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
    ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
    ILI9341_PIXFMT  , 1, 0x55,
    ILI9341_FRMCTR1 , 2, 0x00, 0x18,
    ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
    0xF2, 1, 0x00,                         // 3Gamma Function Disable
    ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
    ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
      0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
      0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    ILI9341_SLPOUT  , 0x80,                // Exit Sleep
    ILI9341_DISPON  , 0x80,                // Display on
    0x00                                   // End of list
  };


  uint8_t cmd, x, numArgs;
  const uint8_t *addr = initcmd;

  while ((cmd = *addr++) > 0) {
    x = *addr++;
    numArgs = x & 0x7F;

    // Send command
    GPIOA->DOUT31_0 &= ~DC_MASK;
    ili9341_write_command(cmd);

    // Send command arguments (data)
    GPIOA->DOUT31_0 |= DC_MASK;
    for (uint8_t i = 0; i < numArgs; i++) {
      ili9341_write_data8(*addr++);
    }

    if (x & 0x80) {
      msec_delay(150);
    }
  }
} /* ili9341_init */


void ili9341_reset(void)
{
  GPIOA->DOUTCLR31_0 = RESET_MASK;
  msec_delay(200);
  GPIOA->DOUTSET31_0 = RESET_MASK;
  msec_delay(200);
} /* ili9341_reset */


void ili9341_write_command(uint8_t cmd) {
  GPIOA->DOUT31_0 &= ~DC_MASK;
  spi1_write_data(cmd);
  while (!spi1_xfer_done());
} /* ili9341_write_command */


void ili9341_write_data8(uint8_t data) {
  GPIOA->DOUT31_0 |= DC_MASK;
  spi1_write_data(data);
  while (!spi1_xfer_done());
} /* ili9341_write_data8 */


void ili9341_write_data16(uint16_t data) {
  GPIOA->DOUT31_0 |= DC_MASK;
  spi1_write_data((data >> 8) & 0xFF);
  while (!spi1_xfer_done());
  spi1_write_data(data & 0xFF);
  while (!spi1_xfer_done());
} /* ili9341_write_data16 */

void ili9341_write_data32(uint32_t data) {
  GPIOA->DOUT31_0 |= DC_MASK;
  spi1_write_data((data >> 24) & 0xFF);
  while (!spi1_xfer_done());
  spi1_write_data((data >> 16) & 0xFF);
  while (!spi1_xfer_done());
  spi1_write_data((data >> 8) & 0xFF);
  while (!spi1_xfer_done());
  spi1_write_data(data & 0xFF);
  while (!spi1_xfer_done());
} /* ili9341_write_data32 */


void ili9341_fill_screen(uint16_t color) {
  
  // Set column address (X)
  ili9341_write_command(0x2A);
  ili9341_write_data32((0x0000 << 16) | (ILI9341_TFTWIDTH - 1));

  // Set page address (Y)
  ili9341_write_command(0x2B);
  ili9341_write_data32((0x0000 << 16) | (ILI9341_TFTHEIGHT - 1));

  // Write to RAM
  ili9341_write_command(0x2C);

  GPIOA->DOUT31_0 |= DC_MASK;
  // Fill entire screen with color
  for (uint32_t i = 0; i < (ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT); i++) {
    spi1_write_data(color >> 8);
    spi1_write_data(color);
  }
}

void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
  // Set column address (X)
  ili9341_write_command(0x2A);
  ili9341_write_data8(x >> 8);
  ili9341_write_data8(x & 0xFF);
  ili9341_write_data8(x >> 8);
  ili9341_write_data8(x & 0xFF);

  // Set page address (Y)
  ili9341_write_command(0x2B);
  ili9341_write_data8(y >> 8);
  ili9341_write_data8(y & 0xFF);
  ili9341_write_data8(y >> 8);
  ili9341_write_data8(y & 0xFF);

  // Write to RAM
  ili9341_write_command(0x2C);

  // Write pixel color
  ili9341_write_data16(color);
} /* ili9341_draw_pixel */