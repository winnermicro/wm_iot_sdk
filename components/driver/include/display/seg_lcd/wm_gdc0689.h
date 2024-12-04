/**
 * @file wm_gdc0689.h
 *
 * @brief WM_GDC0689 Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __WM_GDC0689_H__
#define __WM_GDC0689_H__

#include "wm_hal_seg_lcd.h"
#include "wm_dt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_GDC0689_Enumerations WM DRV_SEG_LCD Enumerations
 * @brief WinnerMicro DRV_SEG_LCD Enumerations
 */

/**
 * @addtogroup WM_GDC0689_Enumerations
 * @{
 */

/**
 * @brief Enumeration for service icon display states on the segment LCD.
 *
 * This enum defines the possible states for displaying service icons on the
 * segment LCD, such as turning the service icon on or off.
 */
typedef enum {
    WM_GDC0689_SERVICE_ICON_OFF = 0, /**< Turns the service icon off */
    WM_GDC0689_SERVICE_ICON_ON  = 1, /**< Turns the service icon on */
    WM_GDC0689_SERVICE_ICON_MAX = 2  /**< Maximum value for service icon states (should not be used directly) */
} wm_gdc0689_service_icon_t;

/**
 * @brief Enumeration for battery level display states on the segment LCD.
 *
 * This enum represents the different battery level states that can be displayed
 * on the segment LCD, ranging from 0% to 100% battery level, as well as an 'OFF' state.
 */
typedef enum {
    WM_GDC0689_BATTERY_LEVEL_OFF         = 0, /**< Represents the battery level display is turned off */
    WM_GDC0689_BATTERY_LEVEL_PERCENT_0   = 1, /**< Represents a 0% battery level on the display */
    WM_GDC0689_BATTERY_LEVEL_PERCENT_25  = 2, /**< Represents a 25% battery level */
    WM_GDC0689_BATTERY_LEVEL_PERCENT_50  = 3, /**< Represents a 50% battery level */
    WM_GDC0689_BATTERY_LEVEL_PERCENT_75  = 4, /**< Represents a 75% battery level */
    WM_GDC0689_BATTERY_LEVEL_PERCENT_100 = 5, /**< Represents a 100% full battery level */
    WM_GDC0689_BATTERY_LEVEL_MAX         = 6  /**< Maximum value for battery level states (should not be used directly) */
} wm_gdc0689_battery_level_t;

/**
 * @brief Enumeration for signal level display states on the segment LCD.
 *
 * This enum is used to define the different signal level states that can be
 * displayed on the segment LCD, with specific percentages indicating the strength.
 */
typedef enum {
    WM_GDC0689_SIGNAL_LEVEL_OFF         = 0, /**< Represents the signal level display is turned off */
    WM_GDC0689_SIGNAL_LEVEL_PERCENT_25  = 1, /**< Represents a signal level at 25% */
    WM_GDC0689_SIGNAL_LEVEL_PERCENT_50  = 2, /**< Represents a signal level at 50% */
    WM_GDC0689_SIGNAL_LEVEL_PERCENT_75  = 3, /**< Represents a signal level at 75% */
    WM_GDC0689_SIGNAL_LEVEL_PERCENT_100 = 4, /**< Represents a signal level at 100% (full signal strength) */
    WM_GDC0689_SIGNAL_LEVEL_MAX         = 5  /**< Maximum value for signal level states (should not be used directly) */
} wm_gdc0689_signal_level_t;

/**
 * @brief Enumeration for unit display types on the segment LCD.
 *
 * This enum lists the different unit types that can be displayed on the segment
 * LCD to represent various measurements, such as temperature, force, pressure, etc.
 */
typedef enum {
    WM_GDC0689_UNIT_OFF          = 0, /**< Represents no unit display or 'OFF' state */
    WM_GDC0689_UNIT_MILLIVOLT    = 1, /**< Unit for millivolts: mV */
    WM_GDC0689_UNIT_NEWTON_METER = 2, /**< Unit for Newton meter: N.m, a measure of torque */
    WM_GDC0689_UNIT_KILOGRAM     = 3, /**< Unit for kilograms: kg */
    WM_GDC0689_UNIT_GRAM         = 4, /**< Unit for grams: g */
    WM_GDC0689_UNIT_MILLIMETER   = 5, /**< Unit for millimeters: mm */
    WM_GDC0689_UNIT_MICROSTRAIN  = 6, /**< Unit for microstrain: με, a measure of deformation */
    WM_GDC0689_UNIT_KILOPASCAL   = 7, /**< Unit for kilopascals: kPa, a measure of pressure */
    WM_GDC0689_UNIT_MEGAPASCAL   = 8, /**< Unit for megapascals: MPa, a higher measure of pressure */
    WM_GDC0689_UNIT_CELSIUS      = 9, /**< Unit for Celsius: ℃, a measure of temperature */
    WM_GDC0689_UNIT_MAX          = 10 /**< Maximum value for unit types (should not be used directly) */
} wm_gdc0689_unit_t;

/**
 * @}
 */

/**
 * @defgroup WM_GDC0689_Functions WM DRV_SEG_LCD Functions
 * @brief WinnerMicro DRV_SEG_LCD Functions
 */

/**
 * @addtogroup WM_GDC0689_Functions
 * @{
 */

/**
 * @brief Initializes the segment LCD device.
 *
 * This function performs the necessary steps to initialize the segment LCD device,
 * preparing it for use. This typically includes configuring the hardware interface
 * and setting up the display parameters.
 *
 * @param[in] dev_name The name of the device to initialize.
 *
 * @return
 *    - A pointer to the initialized device structure if the initialization is successful.
 *    - NULL if the initialization fails or the device name is not recognized, or the device has already been initialized.
 */
wm_device_t *wm_gdc0689_init(char *dev_name);

/**
 * @brief Deinitializes the segment LCD device.
 *
 * This function is used to release resources and perform cleanup operations for the
 * segment LCD device that was previously initialized with wm_gdc0689_init. It is important
 * to call this function when the device is no longer needed to avoid resource leaks.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD that needs to be deinitialized.
 *
 * @return
 *    - WM_ERR_SUCCESS if the device is successfully deinitialized.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_deinit(wm_device_t *dev);

/**
 * @brief Clears the display on the segment LCD.
 *
 * This function is used to clear the content displayed on the segment LCD, effectively
 * turning off all segments. This can be used to blank the display or reset the LCD
 * to a known state.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD to be cleared.
 *
 * @return
 *    - WM_ERR_SUCCESS if the display is successfully cleared.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_clear(wm_device_t *dev);

/**
 * @brief Displays a service icon on the segment LCD.
 *
 * This function updates the display to show a specified service icon, which can
 * indicate various statuses or information to the user.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] service_icon The type of service icon to display, as defined in the enumeration.
 *
 * @return
 *    - WM_ERR_SUCCESS if the service icon is successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer or service icon type is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_service_icon(wm_device_t *dev, wm_gdc0689_service_icon_t service_icon);

/**
 * @brief Displays the battery level on the segment LCD.
 *
 * This function is used to indicate the current battery level on the LCD.
 * It is important for users to be aware of the device's battery status.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] battery_level The battery level to display, as defined in the enumeration.
 *
 * @return
 *    - WM_ERR_SUCCESS if the battery level is successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer or battery level value is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_battery_level(wm_device_t *dev, wm_gdc0689_battery_level_t battery_level);

/**
 * @brief Displays the signal level on the segment LCD.
 *
 * This function updates the LCD to show the current signal strength,
 * which is crucial for communication devices to inform the user about
 * the quality of the network connection.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] signal_level The signal level to display, as defined in the enumeration.
 *
 * @return
 *    - WM_ERR_SUCCESS if the signal level is successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer or signal level value is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_signal_level(wm_device_t *dev, wm_gdc0689_signal_level_t signal_level);

/**
 * @brief Displays a unit of measurement on the segment LCD.
 *
 * This function allows the display of various units of measurement on the
 * segment LCD, which can be useful for devices that measure different physical
 * quantities.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] unit The unit of measurement to display, as defined in the enumeration.
 *
 * @return
 *    - WM_ERR_SUCCESS if the unit is successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer or unit value is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_unit(wm_device_t *dev, wm_gdc0689_unit_t unit);

/**
 * @brief Displays the current time on the segment LCD.
 *
 * This function is used to show the time in hours and minutes on the LCD,
 * which is essential for devices that need to display the current time to the user.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] tm_hour The hour component of the time to display.
 * @param[in] tm_min The minute component of the time to display.
 *
 * @return
 *    - WM_ERR_SUCCESS if the time is successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer or time values are invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_time(wm_device_t *dev, int tm_hour, int tm_min);

/**
 * @brief Displays an integer value on the segment LCD.
 *
 * This function allows the display of integer values on the LCD, which can be
 * used to show numerical data such as scores, counts, or measurements.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] value The integer value to be displayed.
 *
 * @return
 *    - WM_ERR_SUCCESS if the integer value is successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_integer(wm_device_t *dev, uint32_t value);

/**
 * @brief Displays a decimal value on the segment LCD.
 *
 * This function enables the display of decimal (floating-point) values on the
 * segment LCD, which can be used for precise measurements or other data that
 * requires fractional representation.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] value The decimal (floating-point) value to be displayed.
 *
 * @return
 *    - WM_ERR_SUCCESS if the decimal value is successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_decimal(wm_device_t *dev, float value);

/**
 * @brief Displays all segments on the segment LCD.
 *
 * This function turns on all segments of the LCD to their display state.
 * It is typically used for testing the functionality of the LCD or to
 * ensure that all segments are visible and working correctly.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 *
 * @return
 *    - WM_ERR_SUCCESS if all segments are successfully displayed.
 *    - WM_ERR_NO_INITED if the device is not initialized.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_gdc0689_display_all(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_GDC0689_H__ */
