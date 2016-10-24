/**
 * @file main.c
 *
 * @brief the main application
 */

#include <stdio.h>
#include <string.h>
#include "msiklm.h"

//the following macros can be used for colored text output
#define KDEFAULT "\033[0m"
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

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
            "<color> OR <color_left,color_middle,color_right> OR <color_logo,color_front_left,color_front_right,color_mousepad>\n"
           KDEFAULT
            "    set a respective color for all zones / for each zone at full brightness;\n"
            "    if three values are supplied, they have to be separated with commas without spaces,\n"
            "    e.g. red or red,green,blue are valid while red,green, blue or red, green, blue are not\n"
            "    it is valid to supply three times the same value, but the result will be the same as supplying it only once\n"
            "    valid colors are: none, off (equivalent to none), red, orange, yellow, green, sky, blue, purple, white\n"
            "\n"
           KMAG
            "<color> <brightness> OR <color_left,color_middle,color_right> <brightness>\n"
           KDEFAULT
            "    set the respective zone's color(s) at the specified brightness; valid brightnesses are: low, medium, high\n"
            "    remark: to disable the illumination, use off or none as global color\n"
            "\n"
           KMAG
            "<color> <mode> OR <color_left,color_middle,color_right> <mode>\n"
           KDEFAULT
            "    sets the respective zone's color(s) and activates the specified mode\n"
            "    mode is one of the following effects: normal, gaming, breathe, demo, wave\n"
            "\n"
           KMAG
            "<color> <brightness> <mode> OR <color_left,color_middle,color_right> <brightness> <mode>\n"
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
 * @brief converts a color command line argument into the three color values
 * @param color_str the color string, valid is either "color" or "color_left,color_middle,color_right" or "color_logo,color_front_left,color_front_right,color_mousepad"
 * @param color_left the assigned left color (either color or color_left)
 * @param color_middle the assigned middle color (either color or )
 * @param color_right the assigned right color
 * @param color_other the assigned mouse color. This also denotes that color_left is actually color_logo, color_middle is actually color_front_left, and color_right is actually color_front_right
 * @return 0 if everything succeeded, 1 on partly matching input, -1 on non-color input
 */
int string_to_colors(char* color_str, enum color* color_left, enum color* color_middle, enum color* color_right, enum color* color_other)
{
    int ret = 0;

    *color_left = parse_color(strtok(color_str, ","));
    if ((int)*color_left < 0)
        ret = -1;

    if (ret == 0)
    {
        char* color_middle_str = strtok(NULL, ",");
        if (color_middle_str == NULL)
        {
            //one color for all three zones
            *color_other = *color_middle = *color_right = *color_left;
        }
        else
        {
            //explicit color for each zone
            *color_middle = parse_color(color_middle_str);
            if ((int)*color_middle < 0)
                ret = 1;

            *color_right = parse_color(strtok(NULL, ","));
            if ((int)*color_right < 0)
                ret = 1;

            *color_other = parse_color(strtok(NULL, ",")); //This is ok if its -1 by default
            if (strtok(NULL, ",")) //no more parts allowed
                ret = 1;
        }
    }

    return ret;
}

/**
 * @brief application's entry point
 * @param argc number of command line arguments
 * @param argv command line argument array; the first value is always the program's name
 * @return 0 if everything succeeded, -1 otherwise
 */
int main(int argc, char** argv)
{
    //the color values, the brightness and the mode; will all be initialized according to the parsed from command line arguments
    enum color color_left, color_middle, color_right, color_other;
    enum brightness br;
    enum mode md;
    int ret;

    //if the color is supplied, it is always the first argument, so try to parse it
    if (argc > 1 && string_to_colors(argv[1], &color_left, &color_middle, &color_right, &color_other) == 0)
    {
        ret = 0;
        br = high;
        md = normal;
    }
    else
    {
        ret = -1;
        color_left = color_middle = color_right = color_other = -1;
        br = -1;
        md = -1;
    }
    //it holds: colors, brightness and mode are either all three valid (and will be set) or all -1 (colors will not be modified)

    //parse the command line arguments
    switch (argc)
    {
        case 1:
            printf("No arguments supplied; use 'msiklm help' to show a list of valid arguments\n");
            break;

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
                            if (strcmp(argv[1], "help")==0)
                            {
                                show_help();
                                ret = 0;
                            }
                            break;

                        case 't':
                            if (strcmp(argv[1], "test")==0)
                            {
                                if (keyboard_found())
                                    printf(KMAG"Compatible keyboard found!\n"KDEFAULT);
                                else
                                    printf(KMAG"No compatible keyboard found!\n"KDEFAULT);

                                ret = 0;
                            }
                            break;

                        case 'l':
                            if (strcmp(argv[1], "list")==0)
                            {
                                enumerate_hid();
                                ret = 0;
                            }
                            break;
                    }

                    if (ret != 0)
                        printf("Invalid argument '%s' - use 'help' to list an overview of valid commands\n", argv[1]);
                }
            }
            break;

        case 3:
            // two command line arguments; valid inputs are:
            // '<color> <brightness>' -> set the respective color(s) and brightness
            // '<color> <mode>'       -> set the respective color(s) and mode
            // only if color has 3 arguments

            if (ret==0 && (int)color_other==-1)
            {
                enum brightness b = parse_brightness(argv[2]);
                if ((int)b >= 0)
                {
                    br = b;
                }
                else
                {
                    enum mode m = parse_mode(argv[2]);
                    if ((int)m >= 0)
                    {
                        md = m;
                    }
                    else
                    {
                        printf("Invalid brightness / mode argument '%s' - use 'help' to list an overview of valid commands\n", argv[2]);
                        ret = -1;
                    }
                }
            }
            else
            {
                printf("Invalid color argument '%s' - use 'help' to list an overview of valid commands\n", argv[1]);
                ret = -1;
            }
            break;

        case 4:
            // three command line arguments; valid input is only <color> <brightness> <mode>

            if (ret==0 && (int)color_other==-1)
            {
                br = parse_brightness(argv[2]);
                if ((int)br >= 0)
                {
                    md = parse_mode(argv[3]);
                    if ((int)md < 0)
                    {
                        printf("Invalid mode argument '%s' - use 'help' to list an overview of valid commands\n", argv[3]);
                        ret = -1;
                    }
                }
                else
                {
                    printf("Invalid brightness argument '%s' - use 'help' to list an overview of valid commands\n", argv[2]);
                    ret = -1;
                }
            }
            else
            {
                printf("Invalid color argument '%s' - use 'help' to list an overview of valid commands\n", argv[1]);
                ret = -1;
            }
            break;

        default:
            printf("Invalid arguments supplied; use 'msiklm help' to show a list of valid arguments\n");
            break;
    }

    if (ret == 0)
    {
        if (br == off) //no color if brightness is set to off
            color_left = color_middle = color_right = color_other = none;

        if ((int)md >= 0)
        {
            hid_device* dev = open_keyboard();

            if (dev != NULL)
            {
                if ((int)color_other != -1) //If color_other is set
                {
                    set_rgb_color(dev, color_left, logo);
                    set_rgb_color(dev, color_middle, front_left);
                    set_rgb_color(dev, color_right, front_right);
                    set_rgb_color(dev, color_other, mouse);
                    
                    if ((br == off) || ((color_middle == color_left) && (color_right == color_left) && (color_other == color_left)))
                    {
                        set_color(dev, color_left, left, br);
                        set_color(dev, color_middle, middle, br);
                        set_color(dev, color_right, right, br);
                    }
                }
                else
                {
                    set_color(dev, color_left, left, br);
                    set_color(dev, color_middle, middle, br);
                    set_color(dev, color_right, right, br);
                }

                set_mode(dev, md);
                hid_close(dev);
            }
            else
            {
                printf("No compatible keyboard found!\n");
                ret = -1;
            }
            hid_exit();
        }
    }

    return ret;
}
