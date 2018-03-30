/**
 * @file msiklm.h
 *
 * @brief header file for the controller functions to configure the SteelSeries keyboard in MSI gaming notebooks
 */

#ifndef MSIKLM_H
#define MSIKLM_H

#ifdef _WIN32
    #define strtok_r(s,d,p) strtok_s(s,d,p)
#endif

#include <stdbool.h>
#include <hidapi/hidapi.h>

typedef unsigned char byte;

/**
 * @brief color profile enum: the color profile either defines a default color or indicates a custom selection
 */
enum color_profile
{
    none   = 0,
    red    = 1,
    orange = 2,
    yellow = 3,
    green  = 4,
    sky    = 5,
    blue   = 6,
    purple = 7,
    white  = 8,
    custom = 64 //custom rgb-color selection (64 defines the keyboard's rgb-command)
};

/**
 * @brief color struct: each color is represented by a profile value and its respective rgb-values
 */
struct color
{
    enum color_profile profile;
    byte red;
    byte green;
    byte blue;
};

/**
 * @brief region enum: each value refers to a region whose color could be set
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
 * @brief brightness enum: the brightness is either defined by the rgb selection or one of four predefined values
 */
enum brightness
{
    high   = 0,
    medium = 1,
    low    = 2,
    off    = 3,
    rgb    = custom //brightness defined by custom rgb-color selection
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
 * @param color_str the color value as a string (red, green, blue, etc.), hex code (0xFFFFFF) or in [r;g;b] notation where r;g;b are the respective channel values
 * @param result the parsed color
 * @returns 0 if parsing succeeded, -1 on error
 */
int parse_color(const char* color_str, struct color* result);

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
 * @param region the region where the color should be set
 * @param brightness the selected brightness (note that it also defines the kind of command that is send to the keyboard)
 * @returns the actual number of bytes written, -1 on error
 */
int set_color(hid_device* dev, struct color color, enum region region, enum brightness brightness);

/**
 * @brief sets the selected mode
 * @param dev the hid device
 * @param mode the selected mode
 * @returns the actual number of bytes written, -1 on error
 */
int set_mode(hid_device* dev, enum mode mode);

/**
 * @brief utility function for hex code parsing
 * @param hex the hex code in question
 * @returns the parsed integer value
 */
int parse_hex(unsigned char hex);

#endif //MSIKLM_H
