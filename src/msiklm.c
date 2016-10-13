/**
 * @file msiklm.c
 *
 * @brief source file that contains controller functions to configure the SteelSeries keyboard in MSI gaming notebooks
 */

#include "msiklm.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief parses a string into a color value
 * @param color_str the color value as a string
 * @returns the parsed color value or -1 if the string is not a valid color (special case: "off" will also be accepted as none)
 */
enum color parse_color(const char* color_str)
{
    enum color ret = -1;
    if (color_str)
    {
        switch (color_str[0])
        {
            case 'b':
                if (strcmp(color_str, "blue") == 0)
                    ret = blue;
                break;

            case 'g':
                if (strcmp(color_str, "green") == 0)
                    ret = green;
                break;

            case 'n':
                if (strcmp(color_str, "none") == 0)
                    ret = none;
                break;

            case 'o':
                if (strcmp(color_str, "off") == 0)
                    ret = none; //special case: "off" will also be accepted as none
                else if (strcmp(color_str, "orange") == 0)
                    ret = orange;
                break;

            case 'p':
                if (strcmp(color_str, "purple") == 0)
                    ret = purple;
                break;

            case 'r':
                if (strcmp(color_str, "red") == 0)
                    ret = red;
                break;

            case 's':
                if (strcmp(color_str, "sky") == 0)
                    ret = sky;
                break;

            case 'w':
                if (strcmp(color_str, "white") == 0)
                    ret = white;
                break;

            case 'y':
                if (strcmp(color_str, "yellow") == 0)
                    ret = yellow;
                break;
        }
    }
    return ret;
}

/**
 * @brief parses a string into a brightness value
 * @param brightness_str the brightness value as a string
 * @returns the parsed brightness value or -1 if the string is not a valid brightness
 */
enum brightness parse_brightness(const char* brightness_str)
{
    enum brightness ret = -1;
    if (brightness_str)
    {
        switch (brightness_str[0])
        {
            case 'h':
                if (strcmp(brightness_str, "high") == 0)
                    ret = high;
                break;

            case 'm':
                if (strcmp(brightness_str, "medium") == 0)
                    ret = medium;
                break;

            case 'l':
                if (strcmp(brightness_str, "low") == 0)
                    ret = low;
                break;

            case 'o':
                if (strcmp(brightness_str, "off") == 0)
                    ret = off;
                break;
        }
    }
    return ret;
}

/**
 * @brief parses a string into a mode value
 * @param mode_str the mode value as a string
 * @returns the parsed mode value or -1 if the string is not a valid mode
 */
enum mode parse_mode(const char* mode_str)
{
    enum mode ret = -1;
    if (mode_str)
    {
        switch (mode_str[0])
        {
            case 'n':
                if (strcmp(mode_str, "normal") == 0)
                    ret = normal;
                break;

            case 'g':
                if (strcmp(mode_str, "gaming") == 0)
                    ret = gaming;
                break;

            case 'b':
                if (strcmp(mode_str, "breathe") == 0)
                    ret = breathe;
                break;

            case 'd':
                if (strcmp(mode_str, "demo") == 0)
                    ret = demo;
                break;

            case 'w':
                if (strcmp(mode_str, "wave") == 0)
                    ret = wave;
                break;
        }
    }
    return ret;
}

/**
 * @brief tries to open the MSI gaming notebook's SteelSeries keyboard and if it succeeds, it will be closed
 * @returns true, if the keyboard could be opened, false otherwise
 */
bool keyboard_found()
{
    hid_device* keyboard = open_keyboard();
    bool ret = keyboard != NULL;
    if (ret)
        hid_close(keyboard);
    return ret;
}

/**
 * @brief tries to open the MSI gaming notebook's SteelSeries keyboard
 * @returns a corresponding hid_device, null if the keyboard was not detected
 */
hid_device* open_keyboard()
{
    hid_device* keyboard = NULL;
    if (hid_init() == 0)
        keyboard = hid_open(0x1770, 0xff00, 0);
    return keyboard;
}

/**
 * @brief sets the selected color for a specified region
 * @param dev the hid device
 * @param color the color value
 * @param region the region where the color should be set_color
 * @param brightness the selected brightness
 * @returns the acutal number of bytes written, -1 on error
 */
int set_color(hid_device* dev, enum color color, enum region region, enum brightness brightness)
{
    // check for a valid color
    if (color != none   && color != red    && color != orange &&
        color != yellow && color != green  && color != sky    &&
        color != blue   && color != purple && color != white)
        return -1;

    // check for a valid region
    if (region != left && region != middle && region != right)
        return -1;

    // check for a valid brightness
    if (brightness != off && brightness != low && brightness != medium && brightness != high)
        return -1;

    unsigned char activate[8];
    activate[0] = 1;
    activate[1] = 2;
    activate[2] = 66; // set
    activate[3] = (unsigned char)region;
    activate[4] = (unsigned char)color;
    activate[5] = (unsigned char)brightness;
    activate[6] = 0;
    activate[7] = 236; // EOR (end of request)

    return hid_send_feature_report(dev, activate, 8);
}

/**
 * @brief sets the selected mode
 * @param dev the hid device
 * @param mode the selected mode
 * @returns the acutal number of bytes written, -1 on error
 */
int set_mode(hid_device* dev, enum mode mode)
{
    // check for a valid value, otherwise use normal mode
    if (mode != normal  && mode != gaming && mode != breathe && mode != demo && mode != wave)
        return -1;

    unsigned char commit[8];
    commit[0] = 1;
    commit[1] = 2;
    commit[2] = 65; // commit
    commit[3] = (unsigned char)mode; // set hardware mode
    commit[4] = 0;
    commit[5] = 0;
    commit[6] = 0;
    commit[7] = 236; // EOR (end of request)

    return hid_send_feature_report(dev, commit, 8);
}

/**
 * @brief iterates through all found HID devices
 */
void enumerate_hid()
{
    struct hid_device_info* enumerate = hid_enumerate(0,0);

    if (enumerate != NULL)
    {
        struct hid_device_info* dev = enumerate;
        while (dev != NULL)
        {
            printf("Device: %S\n", dev->product_string);
            printf("    Device Vendor ID:        %i\n", dev->vendor_id);
            printf("    Device Product ID:       %i\n", dev->product_id);
            printf("    Device Serial Number:    %S\n", dev->serial_number);
            printf("    Device Manufacturer:     %S\n", dev->manufacturer_string);
            printf("    Device Path:             %s\n", dev->path);
            printf("    Device Interface Number: %i\n", dev->interface_number);
            printf("    Device Release Number:   %d\n", dev->release_number);
            printf("\n");
            dev = dev->next;
        }
        hid_free_enumeration(enumerate);
    }
    else
    {
        printf("No HID device found!\n");
    }
}
