/**
 * @file msiklm.h
 *
 * @brief header file for the controller functions to configure the SteelSeries keyboard in MSI gaming notebooks
 */

#ifndef MSIKLM_H
#define MSIKLM_H

#include <stdbool.h>
#include <hidapi/hidapi.h>

typedef unsigned char byte;

/**
 * @brief color struct
 */
typedef struct
{
    byte red;
    byte green;
    byte blue;
} color_t;


/**
 * @brief color constants
 */
static const color_t none   = { 0x00, 0x00, 0x00 };
static const color_t red    = { 0xff, 0x00, 0x00 };
static const color_t orange = { 0xff, 0x60, 0x00 };
static const color_t yellow = { 0xff, 0xff, 0x00 };
static const color_t green  = { 0x00, 0xff, 0x00 };
static const color_t cyan   = { 0x00, 0xff, 0xff };
static const color_t blue   = { 0x00, 0x00, 0xff };
static const color_t purple = { 0xff, 0x00, 0xff };
static const color_t white  = { 0xff, 0xff, 0xff };

/**
 * @brief region enum
 */
enum region
{
    left        = 1,
    middle      = 2,
    right       = 3,
    logo        = 4,
    front_left  = 5,
    front_right = 6,
    mouse       = 7
};

/**
 * @brief brightness enum
 */
enum brightness
{
    off    = 3,
    low    = 2,
    medium = 1,
    high   = 0
};

/**
 * @brief modes enum (where a mode is an animation or some kind of special effect)
 */
enum mode
{
    normal  = 1,
    gaming  = 2,
    breathe = 3,
    demo    = 4,
    wave    = 5
};


/**
 * @brief parses a string into a color value
 * @param color_str the color value as a string (red, green, blue, etc.) or in [r;g;b] notation where r;g;b are the respective channel values
 * @param result the parsed color
 * @returns 0 if parsing succeeded, -1 on error
 */
int parse_color(const char* color_str, color_t* result);

/**
 * @brief parses a string into a brightness value
 * @param brightness_str the brightness value as a string
 * @returns the parsed brightness value or -1 if the string is not a valid brightness
 */
enum brightness parse_brightness(const char* brightness_str);

/**
 * @brief parses a string into a mode value
 * @param mode_str the mode value as a string
 * @returns the parsed mode value or -1 if the string is not a valid mode
 */
enum mode parse_mode(const char* mode_str);

/**
 * @brief tries to open the MSI gaming notebook's SteelSeries keyboard and if it succeeds, it will be closed
 * @returns true, if the keyboard could be opened, false otherwise
 */
bool keyboard_found();

/**
 * @brief tries to open the MSI gaming notebook's SteelSeries keyboard
 * @returns a corresponding hid_device, null if the keyboard was not detected
 */
hid_device* open_keyboard();

/**
 * @brief sets the selected color for a specified region (the colors will only be set as soon as set_mode() is called in advance)
 * @param dev the hid device
 * @param color the color value
 * @param region the region where the color should be set_color
 * @param brightness the selected brightness
 * @returns the acutal number of bytes written, -1 on error
 */
int set_color(hid_device* dev, color_t color, enum region region, enum brightness brightness);

/**
 * @brief sets the selected mode
 * @param dev the hid device
 * @param mode the selected mode
 * @returns the acutal number of bytes written, -1 on error
 */
int set_mode(hid_device* dev, enum mode mode);

/**
 * @brief iterates through all found HID devices
 */
void enumerate_hid();

#endif //MSIKLM_H
