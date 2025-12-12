// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  1.0
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
#include "jet_brains_mono.h"

struct position {
  uint16_t x;
  uint16_t y;
};

struct position g_cursor_pos = {0, 25};


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function initializes the ILI9341 LCD display by configuring the necessary
//  GPIO pins, sending initialization commands, and setting up the display
//  settings. The Adafruit ILI9341 library is used as a reference for the
//  initialization commands.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
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
    ILI9341_MADCTL  , 1, 0xE8,             // Memory Access Control
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


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function performs both a hardware and software reset of the ILI9341.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_reset(void)
{
  GPIOA->DOUTCLR31_0 = RESET_MASK;
  msec_delay(200);
  GPIOA->DOUTSET31_0 = RESET_MASK;
  msec_delay(200);
  ili9341_write_command(ILI9341_SWRESET);
  msec_delay(200);
} /* ili9341_reset */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function sends a command to the ILI9341 LCD display via SPI. The DC pin
//  is set low to indicate that the data being sent is a command.
//
// INPUT PARAMETERS:
//  cmd - The command to be sent to the display
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_write_command(uint8_t cmd) {
  GPIOA->DOUT31_0 &= ~DC_MASK;
  spi1_write_data(cmd);
  while (!spi1_xfer_done());
} /* ili9341_write_command */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function sends 8-bit data to the ILI9341 LCD display via SPI. The DC
//  pin is set high to indicate that the data being sent is data.
//
// INPUT PARAMETERS:
//  data - The 8-bit data to be sent to the display
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_write_data8(uint8_t data) {
  GPIOA->DOUT31_0 |= DC_MASK;
  spi1_write_data(data);
  while (!spi1_xfer_done());
} /* ili9341_write_data8 */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function sends 16-bit data to the ILI9341 LCD display via SPI. The DC
//  pin is set high to indicate that the data being sent is data.
//
// INPUT PARAMETERS:
//  data - The 16-bit data to be sent to the display
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_write_data16(uint16_t data) {
  GPIOA->DOUT31_0 |= DC_MASK;
  spi1_write_data((data >> 8) & 0xFF);
  while (!spi1_xfer_done());
  spi1_write_data(data & 0xFF);
  while (!spi1_xfer_done());
} /* ili9341_write_data16 */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function sends 32-bit data to the ILI9341 LCD display via SPI. The DC
//  pin is set high to indicate that the data being sent is data.
//
// INPUT PARAMETERS:
//  data - The 32-bit data to be sent to the display
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function sends 8-bit data to the ILI9341 LCD display via SPI. The DC
//  pin is set high to indicate that the data being sent is data.
//
// INPUT PARAMETERS:
//  x - The starting X coordinate of the rectangle
//  y - The starting Y coordinate of the rectangle
//  w - The width of the rectangle
//  h - The height of the rectangle
//  color - The color to fill the rectangle with
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  // Set column address (X)
  ili9341_write_command(0x2A);
  ili9341_write_data16(x);
  ili9341_write_data16(x + w - 1);

  // Set page address (Y)
  ili9341_write_command(0x2B);
  ili9341_write_data16(y);
  ili9341_write_data16(y + h - 1);

  // Write to RAM
  ili9341_write_command(0x2C);

  GPIOA->DOUT31_0 |= DC_MASK;
  // Fill rectangle with color
  for (uint32_t i = 0; i < (w * h); i++) {
    spi1_write_data(color >> 8);
    spi1_write_data(color);
    while (!spi1_xfer_done());
  } /* for */
} /* ili9341_fill_rect */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function fills the entire ILI9341 LCD display with a specified color.
//
// INPUT PARAMETERS:
//  color - The color to fill the entire screen with
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_fill_screen(uint16_t color) {
  ili9341_fill_rect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, color);
} /* ili9341_fill_screen */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function draws a single pixel on the ILI9341 LCD display at the 
//  specified coordinates with the specified color.
//
// INPUT PARAMETERS:
//  x - The X coordinate of the pixel
//  y - The Y coordinate of the pixel
//  color - The color of the pixel
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function draws a character on the ILI9341 LCD display at the specified
//  coordinates using the JetBrains Mono font. The character is drawn using the
//  draw_pixel function.
//
// INPUT PARAMETERS:
//  c - The character to draw
//  x - The starting X coordinate of the character (upper left corner)
//  y - The starting Y coordinate of the character (upper left corner)
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_draw_char(char c, uint16_t x, uint16_t y)
{
  if (c < 32 || c > 126) return; // Unsupported character

  const glyph_dsc_t *glyph = &font_dsc.glyph_dsc[c - 32 + 1];
  const uint8_t *bitmap = &font_dsc.glyph_bitmap[glyph->bitmap_index];

  int16_t top_y = y - (glyph->box_h + glyph->ofs_y);

  for (uint8_t row = 0; row < glyph->box_h; row++)
  {
    for (uint8_t col = 0; col < glyph->box_w; col++)
    {
      uint8_t byte_index = (col + row * glyph->box_w) / 8;
      uint8_t bit_index = 7 - ((col + row * glyph->box_w) % 8);
      bool pixel_on = (bitmap[byte_index] >> bit_index) & 0x01;

      if (pixel_on)
      {
        ili9341_draw_pixel(x + glyph->ofs_x + col, top_y + row, ILI9341_BLACK);
      } /* if */
    } /* for */
  } /* for */
} /* ili9341_draw_char */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function erases a character on the ILI9341 LCD display at the cursor
//  position. The character is erased by covering it with the specified color.
//
// INPUT PARAMETERS:
//  c - The character to erase
//  color - The color to use for erasing the character
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_erase_char(char c, uint16_t color)
{
  if (c < 32 || c > 126) return;

  const glyph_dsc_t *glyph = &font_dsc.glyph_dsc[c - 32 + 1];
  int16_t top_y = g_cursor_pos.y - (glyph->box_h + glyph->ofs_y);

  /* cover glyph with color */
  ili9341_fill_rect(g_cursor_pos.x, top_y, GLYPH_WIDTH, glyph->box_h, color);
} /* ili9341_erase_char */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function draws a character on the ILI9341 LCD display at the current
//  cursor position. If the character is a newline, it moves the cursor down by
//  one line height. If the character is a carriage return, it moves the cursor
//  to the beginning of the line. For other characters, it draws the character
//  and moves the cursor to the right by the width of the character.
//
// INPUT PARAMETERS:
//  c - The character to draw at the cursor position
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void ili9341_draw_char_at_cursor(char c)
{
  if (c == '\n')
  {
    g_cursor_pos.y += 25;
  } /* if */
  else if (c == '\r')
  {
    g_cursor_pos.x = 0;
  } /* else if */
  else
  {
    ili9341_draw_char(c, g_cursor_pos.x, g_cursor_pos.y);
    g_cursor_pos.x += GLYPH_WIDTH;
  } /* else */
} /* ili9341_draw_char_at_cursor */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function sets the cursor position on the ILI9341 LCD display. The cursor
//  position is updated to the specified X and Y coordinates.
//
// INPUT PARAMETERS:
//  x - The new X coordinate of the cursor
//  y - The new Y coordinate of the cursor
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void set_cursor_position(uint16_t x, uint16_t y)
{
  g_cursor_pos.x = x;
  g_cursor_pos.y = y;
} /* set_cursor_position */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function retrieves the current global cursor variable position and
//  stores it in the provided pointers to X and Y coordinates.
//
// INPUT PARAMETERS:
//  x - Pointer to store the current X coordinate of the cursor
//  y - Pointer to store the current Y coordinate of the cursor
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void get_cursor_position(uint16_t* x, uint16_t* y)
{
  *x = g_cursor_pos.x;
  *y = g_cursor_pos.y;
} /* get_cursor_position */
