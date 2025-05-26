#ifndef __TP_CALI_ON_LCD_H__
#define __TP_CALI_ON_LCD_H__

#include "wm_drv_tft_lcd.h"
#include "wm_drv_sdh_spi.h"

// need match the “spi_device_name” which defined in the device table for selected TFT-LCD panel.
#define LCD_SPI_CONTROLLER_DEVICE_NAME "sdspi"
#define LCD_DEVICE_NAME                WM_CFG_TFT_LCD_DEVICE_NAME
// The number of lines drawn in a single LCD clear/fill operation
#define LCD_DATA_DRAW_LINE_UNIT        40
#define LCD_RGB565_BLUE                0x001F
#define LCD_RGB565_RED                 0xF800
#define LCD_RGB565_GREEN               0x07E0
#define LCD_RGB565_YELLOW              0xFFE0
#define LCD_RGB565_WHITE               0xFFFF

/**
  * @brief Init LCD screen
  *
  * @return
  *    - WM_ERR_SUCCESS : success
  *    - WM_ERR_FAILED: failed
  */
int example_lcd_init(void);

/**
 * @brief Deinit LCD screen
 */
void example_lcd_deinit(void);

/**
 * @brief Clear the LCD screen to the specified color
 *
 * @param[in] color Color values in RGB565 format
 */

void example_lcd_clear(uint16_t color);
/**
 * @brief Draw a cross mark on the LCD (for touch calibration)
 *
 * @param[in] x     The X coordinate of the center point of the cross (unit: pixel)
 * @param[in] y     The Y coordinate of the center point of the cross (unit: pixel)
 * @param[in] size  The size of the cross (in pixels)
 * @param[in] color Color values in RGB565 format
 */
void example_lcd_draw_cross(uint16_t x, uint16_t y, uint16_t size, uint16_t color);

#endif