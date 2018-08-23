/**
 * @file main.c
 *
 * @brief the main application
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msiklm.h"

//the following macros can be used for colored text output
#ifndef _WIN32
    #define KDEFAULT "\033[0m"
    #define KNRM     "\x1B[0m"
    #define KRED     "\x1B[31m"
    #define KGRN     "\x1B[32m"
    #define KYEL     "\x1B[33m"
    #define KBLU     "\x1B[34m"
    #define KMAG     "\x1B[35m"
    #define KCYN     "\x1B[36m"
    #define KWHT     "\x1B[37m"
#else
    #define KDEFAULT ""
    #define KNRM     ""
    #define KRED     ""
    #define KGRN     ""
    #define KYEL     ""
    #define KBLU     ""
    #define KMAG     ""
    #define KCYN     ""
    #define KWHT     ""
#endif

/**
 * @brief prints help information
 */
void show_help()
{
    printf(KRED
            "MSIKLM - MSI Keyboard Light Manager\n"
            "###################################\n"
           KYEL
            "\n"
            "utility to configure the SteelSeries keyboard in MSI Gaming Notebooks - possible arguments:\n"
            "\n"
           KMAG
            "help\n"
           KDEFAULT
            "    show this help\n"
            "\n"
           KMAG
            "test\n"
           KDEFAULT
            "    test if a compatible SteelSeries MSI Gaming Notebook is detected\n"
            "\n"
           KMAG
            "list\n"
           KDEFAULT
            "    list all found HID devices\n"
            "\n"
           KMAG
            "<color> OR <color_left>,<color_middle>[,<color_right>,<color_logo>,<color_front_left>,<color_front_right>,<color_mouse>]\n"
           KDEFAULT
            "    sets a color for the respective zone at full brightness;\n"
            "    if multiple values are supplied, they have to be separated with commas without spaces,\n"
            "    e.g. red or red,green,blue are valid while red,green, blue or red, green, blue are not\n"
            "    it is valid to supply three times the same value, but the result will be the same as supplying it only once\n"
            "    valid predefined colors are: none, off (equivalent to none), red, orange, yellow, green, sky, blue, purple, white\n"
            "    additionally it is possible to supply a color in full RGB notation; in this case it has to be supplied either in the format\n"
            "    [red;green;blue] where the brackets are required and 'red', 'green' and 'blue' are the respective color values (range 0 to 255)\n"
            "    or in hex code (0x000000 to 0xFFFFFF) notations where the respective values have to be selected accordingly\n"
            "    it is also supported to mix the predefined colors with explicit definitions\n"
            "    please note that it might be necessary to put quotation marks around explicit color definitions,\n"
            "    otherwise the argument might not be properly processed by the shell; cf. Readme.md for more detailed information\n"
            "    remark: to disable the illumination, use off or none as global color\n"
            "\n"
           KMAG
            "<colors> <brightness>\n"
           KDEFAULT
            "    additionally to the color, also a brightness can be specified for all zones where a predefined color is used;\n"
            "    thus, the respective brightness selection will be ignored for all zones where a custom rgb selection has been given\n"
            "    so valid brightnesses are 'low', 'medium', 'high' and 'rgb' where 'rgb' is the default value\n"
            "    important: as soon as a brightness has been set, a different command is used, so it might solve problems to explicitly define it\n"
            "\n"
           KMAG
            "<colors> <mode>\n"
           KDEFAULT
            "    sets the respective zone's color(s) and activates the specified mode\n"
            "    mode is one of the following effects: normal, gaming, breathe, demo, wave\n"
            "\n"
           KMAG
            "<colors> <brightness> <mode>\n"
           KDEFAULT
            "    simultaneously sets the respective zone's color(s) with a specified brightness and a mode\n"
            "\n"
           KMAG
            "<mode>\n"
           KDEFAULT
            "    only set a mode and keep the colors unchanged\n"
    );
}

/**
 * @brief this function is called whenever parsing of an argument failed in some way and prints a respective error message
 * @param value the string that could not be parsed (might be null)
 * @param expected_type string that informs about the expected input type (might be null)
 */
void on_parse_error(const char* value, const char* expected_type)
{
    if (value == NULL)
        printf(KRED"Invalid arguments supplied; use 'msiklm help' to show a list of valid arguments\n"KDEFAULT);
    else if (expected_type == NULL)
        printf(KRED"Invalid argument '%s' - use 'msiklm help' to list an overview of valid commands\n"KDEFAULT, value);
    else
        printf(KRED"Invalid %s argument '%s' - use 'msiklm help' to list an overview of valid commands\n"KDEFAULT, expected_type, value);
}

/**
 * @brief similar to parse_brightness() but additionally checks if the brightness is valid for the current color selection
 * @param brightness_str the brightness value as a string
 * @param with_rgb explicit brightnesses other than 'rgb' are only valid if there is no explicit rgb-color defined
 * @returns the parsed brightness value, -1 if the string is not a valid brightness or -2 if its only invalid for the current color selection
 */
enum brightness parse_check_brightness(const char* brightness_str, bool with_rgb)
{
    enum brightness br = parse_brightness(brightness_str);
    if ((int)br >= 0 && with_rgb && (br == high || br == medium || br == low))
    {
        on_parse_error(brightness_str, "rgb-color brightness");
        br = -2;
    }
    return br;
}

/**
 * @brief iterates through all found HID devices and prints some output about them
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

/**
 * @brief application's entry point
 * @param argc number of command line arguments
 * @param argv command line argument array; the first value is always the program's name
 * @return 0 if everything succeeded, -1 otherwise
 */
int main(int argc, char** argv)
{
    //the color values (at most 7); num_regions tracks the actual number of parsed colors (i.e. regions to set) while with_rgb indicates if there is any rgb-color used
    struct color colors[7];
    int num_regions = 0;
    bool with_rgb = false;
    int ret = argc > 1 ? 0 : -1;

    //if colors are supplied, they are always the first argument, so try to parse them
    if (ret == 0)
    {
        size_t length = strlen(argv[1]);
        char* color_arg = (char*)malloc((length+1) * sizeof(char));
        strcpy(color_arg, argv[1]);
        color_arg[length] = '\0';

        char* saved_ptr = NULL;
        const char* color_str = strtok_r(color_arg, ",", &saved_ptr);
        while (color_str != NULL && ret == 0) //parse into next color slot as long as a color is available for parsing (color_str != NULL) and previous parsing succeeded (ret == 0)
        {
            ret = parse_color(color_str, &(colors[num_regions]));
            if (ret == 0 && num_regions < 7)
            {
                if (colors[num_regions].profile == custom)
                    with_rgb = true;
                ++num_regions;
                color_str = strtok_r(NULL, ",", &saved_ptr);
            }
            else
            {
                ret = -1;
            }
        }
        free(color_arg);
    }

    //the brightness and the mode; initialize them according to the parsed command line arguments
    enum brightness br = ret == 0 ? rgb : -1;
    enum mode md = ret == 0 ? normal : -1;

    //it holds: if ret == 0, the num_regions color values are all valid (and will be set) or brightness and mode are -1 (colors will not be modified, only maybe the mode)

    switch (argc)
    {
        case 2:
            // only one command line argument; valid values are:
            //  '<color>' -> set the respective color(s)
            //  '<mode>'  -> set the respective mode
            //  'help'    -> show the help
            //  'test'    -> try to find a compatible SteelSeries MSI Gaming Notebook
            //  'list'    -> list all found HID devices

            if (ret != 0) //nothing to do if colors are parsed successfully
            {
                md = parse_mode(argv[1]);
                if ((int)md >= 0)
                {
                    ret = 0;
                }
                else //invalid mode: check for 'help', 'test' or 'list'
                {
                    switch (argv[1][0])
                    {
                        case 'h':
                            if (strcmp(argv[1], "help") == 0)
                            {
                                show_help();
                                ret = 0;
                            }
                            break;

                        case 't':
                            if (strcmp(argv[1], "test") == 0)
                            {
                                if (keyboard_found())
                                    printf(KMAG"Compatible keyboard found!\n"KDEFAULT);
                                else
                                    printf(KMAG"No compatible keyboard found!\n"KDEFAULT);

                                ret = 0;
                            }
                            break;

                        case 'l':
                            if (strcmp(argv[1], "list") == 0)
                            {
                                enumerate_hid();
                                ret = 0;
                            }
                            break;
                    }

                    if (ret != 0)
                        on_parse_error(argv[1], NULL);
                }
            }
            break;

        case 3:
            // two command line arguments; valid inputs are:
            // '<color> <brightness>' -> set the respective color(s) and brightness
            // '<color> <mode>'       -> set the respective color(s) and mode

            if (ret == 0)
            {
                enum brightness b = parse_check_brightness(argv[2], with_rgb);
                if ((int)b >= 0)
                {
                    br = b;
                }
                else if ((int)b >= -1)
                {
                    enum mode m = parse_mode(argv[2]);
                    if ((int)m >= 0)
                    {
                        md = m;
                    }
                    else
                    {
                        on_parse_error(argv[2], "brightness / mode");
                        ret = -1;
                    }
                }
                else
                {
                    ret = -1;
                }
            }
            else
            {
                on_parse_error(argv[1], "color");
                ret = -1;
            }
            break;

        case 4:
            // three command line arguments; valid input is only <color> <brightness> <mode>

            if (ret == 0)
            {
                br = parse_check_brightness(argv[2], with_rgb);
                if ((int)br >= 0)
                {
                    md = parse_mode(argv[3]);
                    if ((int)md < 0)
                    {
                        on_parse_error(argv[3], "mode");
                        ret = -1;
                    }
                }
                else
                {
                    if ((int)br >= -1)
                        on_parse_error(argv[2], "brightness");
                    ret = -1;
                }
            }
            else
            {
                on_parse_error(argv[1], "color");
                ret = -1;
            }
            break;

        default:
            // too many or no command line arguments
            on_parse_error(NULL, NULL);
            break;
    }

    if (ret == 0 && (int)md >= 0)
    {
        hid_device* dev = open_keyboard();

        if (dev != NULL)
        {
            if (num_regions == 1 && md != gaming) //special case: one color will be used for the first three regions (gaming mode requires only one region)
            {
                colors[2] = colors[1] = colors[0];
                num_regions = 3;
            }

            for (int i=0; i<num_regions && ret == 0; ++i)
                if (set_color(dev, colors[i], i+1, br) <= 0)
                    ret = -1;

            if (ret == 0 && set_mode(dev, md) <= 0)
                ret = -1;

            hid_close(dev);
        }
        else
        {
            printf("No compatible keyboard found!\n");
            ret = -1;
        }

        if (hid_exit() != 0)
            ret = -1;
    }

    return ret;
}
