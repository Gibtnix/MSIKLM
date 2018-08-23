/**
 * @file msiklm.c
 *
 * @brief source file that contains controller functions to configure the SteelSeries keyboard in MSI gaming notebooks
 */

#include "msiklm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief parses a string into a color value
 * @param color_str the color value as a string (red, green, blue, etc.), hex code (0xFFFFFF) or in [r;g;b] notation where r;g;b are the respective channel values
 * @param result the parsed color
 * @returns 0 if parsing succeeded, -1 on error
 */
int parse_color(const char* color_str, struct color* result)
{
    int ret = -1;
    if (color_str != NULL && result != NULL)
    {
        switch (color_str[0])
        {
            case 'b':
                if (strcmp(color_str, "blue") == 0)
                {
                    result->profile = blue;
                    result->red = 0;
                    result->green = 0;
                    result->blue = 255;
                    ret = 0;
                }
                break;

            case 'g':
                if (strcmp(color_str, "green") == 0)
                {
                    result->profile = green;
                    result->red = 0;
                    result->green = 255;
                    result->blue = 0;
                    ret = 0;
                }
                break;

            case 'n':
                if (strcmp(color_str, "none") == 0)
                {
                    result->profile = none;
                    result->red = 0;
                    result->green = 0;
                    result->blue = 0;
                    ret = 0;
                }
                break;

            case 'o':
                if (strcmp(color_str, "orange") == 0)
                {
                   result->profile = orange;
                   result->red = 255;
                   result->green = 100;
                   result->blue = 0;
                   ret = 0;
                }
                else if (strcmp(color_str, "off") == 0)
                {
                    result->profile = none;
                    result->red = 0;
                    result->green = 0;
                    result->blue = 0;
                    ret = 0;
                }
                break;

            case 'p':
                if (strcmp(color_str, "purple") == 0)
                {
                    result->profile = purple;
                    result->red = 255;
                    result->green = 0;
                    result->blue = 255;
                    ret = 0;
                }
                break;

            case 'r':
                if (strcmp(color_str, "red") == 0)
                {
                    result->profile = red;
                    result->red = 255;
                    result->green = 0;
                    result->blue = 0;
                    ret = 0;
                }
                break;

            case 's':
                if (strcmp(color_str, "sky") == 0)
                {
                    result->profile = sky;
                    result->red = 0;
                    result->green = 255;
                    result->blue = 255;
                    ret = 0;
                }
                break;

            case 'w':
                if (strcmp(color_str, "white") == 0)
                {
                    result->profile = white;
                    result->red = 255;
                    result->green = 255;
                    result->blue = 255;
                    ret = 0;
                }
                break;

            case 'y':
                if (strcmp(color_str, "yellow") == 0)
                {
                    result->profile = yellow;
                    result->red = 255;
                    result->green = 255;
                    result->blue = 0;
                    ret = 0;
                }
                break;

            case '[': //[red;green;blue]
                {
                    result->profile = custom;

                    size_t length = strlen(color_str);
                    char* color_rgb = (char*)malloc(length * sizeof(char));
                    strcpy(color_rgb, &color_str[1]); //copy the string and skip the '['-sign
                    color_rgb[length] = '\0';

                    char* saved_ptr = NULL;
                    char* end_ptr = NULL;

                    //try to parse the red value
                    long val = strtol(strtok_r(color_rgb, ";", &saved_ptr), &end_ptr, 10);
                    if (*end_ptr == '\0' && *saved_ptr != '\0' && val >= 0 && val <= 255)
                    {
                        result->red = (byte)val;

                        //try to parse the green value
                        val = strtol(strtok_r(NULL, ";", &saved_ptr), &end_ptr, 10);
                        if (*end_ptr == '\0' && *saved_ptr != '\0' && val >= 0 && val <= 255)
                        {
                            result->green = (byte)val;

                            //finally try to parse the blue value
                            val = strtol(strtok_r(NULL, "]", &saved_ptr), &end_ptr, 10);
                            if (*end_ptr == '\0' && *saved_ptr == '\0' && val >= 0 && val <= 255)
                            {
                                result->blue = (byte)val;
                                ret = 0;
                            }
                        }
                    }
                    free(color_rgb);
                }
                break;

            case '0': //color in hex format
                if (strlen(color_str) == 8  && color_str[1] == 'x')
                {
                    result->profile = custom;

                    int red1 = parse_hex(color_str[2]);
                    if (red1 >= 0)
                    {
                        int red2 = parse_hex(color_str[3]);
                        if (red2 >= 0)
                        {
                            result->red = 16 * red1 + red2;
                            int green1 = parse_hex(color_str[4]);
                            if (green1 >= 0)
                            {
                                int green2 = parse_hex(color_str[5]);
                                if (green2 >= 0)
                                {
                                    result->green = 16 * green1 + green2;
                                    int blue1 = parse_hex(color_str[6]);
                                    if (blue1 >= 0)
                                    {
                                        int blue2 = parse_hex(color_str[7]);
                                        if (blue2 >= 0)
                                        {
                                            result->blue = 16 * blue1 + blue2;
                                            ret = 0;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
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
    if (brightness_str != NULL)
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

            case 'r':
                if (strcmp(brightness_str, "rgb") == 0)
                    ret = rgb;
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
    if (mode_str != NULL)
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
    hid_device* dev = open_keyboard();
    bool ret = dev != NULL;
    if (ret)
        hid_close(dev);
    return ret;
}

/**
 * @brief tries to open the MSI gaming notebook's SteelSeries keyboard
 * @returns a corresponding hid_device, null if the keyboard was not detected
 */
hid_device* open_keyboard()
{
    hid_device* dev = NULL;
    if (hid_init() == 0)
        dev = hid_open(0x1770, 0xff00, 0);
    return dev;
}

/**
 * @brief sets the selected color for a specified region (the colors will only be set as soon as set_mode() is called in advance)
 * @param dev the hid device
 * @param color the color value
 * @param region the region where the color should be set
 * @param brightness the selected brightness (note that it also defines the kind of command that is send to the keyboard)
 * @returns the actual number of bytes written, -1 on error
 */
int set_color(hid_device* dev, struct color color, enum region region, enum brightness brightness)
{
    int ret = -1;
    if ((region == left || region == middle || region == right || region == logo || region == front_left || region == front_right || region == mouse) && //valid region
        (brightness == rgb || brightness == off || color.profile != custom)) //explicit brightness is only valid for predefined colors (i.e. rgb-selection mixed with brightness makes little sense)
    {
        byte buffer[8];
        buffer[0] = 1;
        buffer[1] = 2;
        buffer[3] = (byte)region;
        buffer[7] = 236; //EOR (end of request)

        if (brightness == rgb) //full rgb selection -> rgb-command
        {
            buffer[2] = 64; //rgb
            buffer[4] = color.red;
            buffer[5] = color.green;
            buffer[6] = color.blue;
        }
        else //predefined color with explicit brightness -> set-command
        {
            buffer[2] = 66; //set
            buffer[4] = brightness != off ? (byte)color.profile : 0;
            buffer[5] = (byte)brightness;
            buffer[6] = 0;
        }

        ret = hid_send_feature_report(dev, buffer, 8);
    }
    return ret;
}

/**
 * @brief sets the selected mode
 * @param dev the hid device
 * @param mode the selected mode
 * @returns the actual number of bytes written, -1 on error
 */
int set_mode(hid_device* dev, enum mode mode)
{
    int ret = -1;
    if (mode == normal || mode == gaming || mode == breathe || mode == demo || mode == wave) //check for a valid mode
    {
        byte buffer[8];
        buffer[0] = 1;
        buffer[1] = 2;
        buffer[2] = 65; //commit
        buffer[3] = (byte)mode; //set hardware mode
        buffer[4] = 0;
        buffer[5] = 0;
        buffer[6] = 0;
        buffer[7] = 236; //EOR (end of request)
        ret = hid_send_feature_report(dev, buffer, 8);
    }
    return ret;
}

/**
 * @brief utility function for hex code parsing
 * @param hex the hex code in question
 * @returns the parsed integer value
 */
int parse_hex(unsigned char hex)
{
    int ret = -1;
         if (hex >= '0' && hex <= '9') ret = hex - '0';
    else if (hex >= 'A' && hex <= 'F') ret = 10 + hex - 'A';
    else if (hex >= 'a' && hex <= 'f') ret = 10 + hex - 'a';
    return ret;
}
