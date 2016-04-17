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
            "<color> OR <color_left,color_middle,color_right>\n"
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
 * @brief: converts a color command line argument into the three color values
 * @param color_str the color string, valid is either "color" or "color_left,color_middle,color_right"
 * @param color_left the assigned left color (either color or color_left)
 * @param color_middle the assigned middle color (either color or )
 * @param color_right the assigned right color
 * @return 0 if everything succeeded, 1 on partly matching input, -1 on non-color input
 */
int string_to_colors(char* color_str, enum color* color_left, enum color* color_middle, enum color* color_right)
{
    *color_left = parse_color(strtok(color_str, ","));
    if ((int)*color_left < 0)
        return -1;

    char* color_middle_str = strtok(NULL, ",");
    if (color_middle_str == NULL)
    {
        //one color for all three zones
        *color_middle = *color_right = *color_left;
        return 0;
    }
    else
    {
        //explicit color for each zone
        *color_middle = parse_color(color_middle_str);
        if ((int)*color_middle < 0)
            return 1;

        *color_right = parse_color(strtok(NULL, ","));
        if ((int)*color_right >= 0 && !strtok(NULL, ",")) //no more parts allowed
            return 0;
        else
            return 1;
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
    //the color values, brightness and mode; will all be initialized to -1 and modified depending on the parsed from command line arguments
    //the keyboard's color conifguration will only be modified if all color and brightness are valid after parsing the command line arguments
    //the keyboard's mode will only be modified if the mode value is valid after parsing the command line arguments
    enum color color_left=-1, color_middle=-1, color_right=-1;
    enum brightness br = high;
    enum mode md = normal;

    if (argc == 1)
    {
        printf("No arguments supplied; enter help to show a list of valid arguments\n");
        return -1;
    }

    //if the color is supplied, it is always the first argument, so try to parse it;
    //if it is a valid color, the parse status is 0, and the color will only be set if the parse status stays 0
    int parse_status = string_to_colors(argv[1], &color_left, &color_middle, &color_right);

    if (parse_status > 0)
    {
        printf("Invalid color arguments supplied; enter help to show a list of valid arguments\n");
        return -1;
    }

    //parse the command line arguments
    switch (argc)
    {
        case 2:
        {
            // only one command line argument; valid values are:
            //  '<color>' -> set the respective color(s)
            //  '<mode>'  -> set the respective mode
            //  'help'    -> show the help
            //  'test'    -> try to find a compatible SteelSeries MSI Gaming Notebook
            //  'list'    -> list all found HID devices

            if (parse_status != 0) //nothing to do if colors are parsed successfully
            {
                md = parse_mode(argv[1]);
                if ((int)md >= 0)
                {
                    //set only the mode
                    hid_device* hiddev = open_keyboard();
                    set_mode(hiddev, md);
                    hid_close(hiddev);
                    return 0;
                }
                else if (strcmp(argv[1], "help")==0)
                {
                    show_help();
                    return 0;
                }
                else if (strcmp(argv[1], "test")==0)
                {
                    if (keyboard_found())
                    {
                        printf(KMAG"Compatible keyboard found!\n"KDEFAULT);
                        return 0;
                    }
                    else
                    {
                        printf(KMAG"No compatible keyboard found!\n"KDEFAULT);
                        return -1;
                    }
                }
                else if (strcmp(argv[1], "list")==0)
                {
                    enumerate_hid();
                    return 0;
                }
                else
                {
                    printf("Invalid argument '%s' - use 'help' to list an overview of valid commands\n", argv[1]);
                    return -1;
                }
            }
            break;
        }

        case 3:
        {
            // two command line arguments; valid inputs are:
            // '<color> <brightness>' -> set the respective color(s) and brightness
            // '<color> <mode>'       -> set the respective color(s) and mode

            if (parse_status==0)
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
                        return -1;
                    }
                }
            }
            else
            {
                printf("Invalid color argument '%s' - use 'help' to list an overview of valid commands\n", argv[1]);
                return -1;
            }

            break;
        }

        case 4:
        {
            // three command line arguments; valid input is only <color> <brightness> <mode>

            if (parse_status==0)
            {
                br = parse_brightness(argv[2]);
                if ((int)br >= 0)
                {
                    md = parse_mode(argv[3]);
                    if ((int)md < 0)
                    {
                        printf("Invalid mode argument '%s' - use 'help' to list an overview of valid commands\n", argv[3]);
                        return -1;
                    }
                }
                else
                {
                    printf("Invalid brightness argument '%s' - use 'help' to list an overview of valid commands\n", argv[2]);
                    return -1;
                }
            }
            else
            {
                printf("Invalid color argument '%s' - use 'help' to list an overview of valid commands\n", argv[1]);
                return -1;
            }

            break;
        }

        default:
        {
            printf("Invalid arguments supplied; enter help to show a list of valid arguments\n");
            break;
        }
    }

    if (parse_status == 0)
    {
        if (br == off) //no color if brightness is set to off
            color_left = color_middle = color_right = none;

        hid_device* hiddev = open_keyboard();
        if (hiddev == NULL)
        {
            printf("No compatible keyboard found!\n");
            return -1;
        }

        set_color(hiddev, color_left, left, br);
        set_color(hiddev, color_middle, middle, br);
        set_color(hiddev, color_right, right, br);
        set_mode(hiddev, md);
        hid_close(hiddev);
    }

    hid_exit();
    return 0;
}
