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
            "<color> OR <color_left>,<color_middle>[,<color_right>,<color_logo>,<color_front_left>,<color_front_right>,<color_mouse>]\n"
           KDEFAULT
            "    set a color for the respective zone at full brightness;\n"
            "    if multiple values are supplied, they have to be separated with commas without spaces,\n"
            "    e.g. red or red,green,blue are valid while red,green, blue or red, green, blue are not\n"
            "    it is valid to supply three times the same value, but the result will be the same as supplying it only once\n"
            "    valid predefined colors are: none, off (equivalent to none), red, orange, yellow, green, sky, blue, purple, white\n"
            "    additionally it is possible to supply a color in full RGB notation; in this case it has to be supplied in the format\n"
            "    [red;green;blue] where the brackets are required and 'red', 'green' and 'blue'' are the respective color values (range 0 to 255)\n"
            "    it is also supported to mix the predefined colors with explicit definitions\n"
            "    please note that it might be necessary to put quotation marks around explicit color definitions,\n"
            "    otherwise the argument might not be properly processed by the shell; cf. Readme.md for more detailed information\n"
            "\n"
           KMAG
            "<colors> <brightness>\n"
           KDEFAULT
            "    set the respective zone's color(s) at the specified brightness; valid brightnesses are: low, medium, high\n"
            "    remark: to disable the illumination, use off or none as global color\n"
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
 * @brief application's entry point
 * @param argc number of command line arguments
 * @param argv command line argument array; the first value is always the program's name
 * @return 0 if everything succeeded, -1 otherwise
 */
int main(int argc, char** argv)
{
    //the color values (at most 7); num_regions tracks the actual number of parsed colors (i.e. regions to set)
    struct color colors[7];
    int num_regions = 0;
    int ret;

    if (argc > 1) //if colors are supplied, they are always the first argument, so try to parse them
    {
        ret = 0;

        char color_arg[strlen(argv[1])];
        strcpy(color_arg, argv[1]);

        char* saved_ptr = NULL;
        const char* color_str = strtok_r(color_arg, ",", &saved_ptr);
        while (color_str != NULL && ret == 0) //parse into next color slot as long as a color is available for parsing (color_str != NULL) and previous parsing succeeded (ret == 0)
        {
            ret = parse_color(color_str, &(colors[num_regions]));
            if (ret == 0 && num_regions < 7)
            {
                ++num_regions;
                color_str = strtok_r(NULL, ",", &saved_ptr);
            }
            else
            {
                ret = -1;
            }
        }

        if (ret == 0 && num_regions == 1) //special case: one color will be used for the first three regions
        {
            colors[2] = colors[1] = colors[0];
            num_regions = 3;
        }
    }
    else
    {
        ret = -1;
    }

    //the brightness and the mode; initialize them according to the parsed from command line arguments
    enum brightness br = ret == 0 ? high : -1;
    enum mode md = ret == 0 ? normal : -1;

    //it holds: if ret == 0, the num_regions color values are all valid (and will be set) or brithness and mode are -1 (colors will not be modified, only maybe the mode)

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
                        printf("Invalid argument '%s' - use 'msiklm help' to list an overview of valid commands\n", argv[1]);
                }
            }
            break;

        case 3:
            // two command line arguments; valid inputs are:
            // '<color> <brightness>' -> set the respective color(s) and brightness
            // '<color> <mode>'       -> set the respective color(s) and mode

            if (ret==0)
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
                        printf("Invalid brightness / mode argument '%s' - use 'msiklm help' to list an overview of valid commands\n", argv[2]);
                        ret = -1;
                    }
                }
            }
            else
            {
                printf("Invalid color argument '%s' - use 'msiklm help' to list an overview of valid commands\n", argv[1]);
                ret = -1;
            }
            break;

        case 4:
            // three command line arguments; valid input is only <color> <brightness> <mode>

            if (ret==0)
            {
                br = parse_brightness(argv[2]);
                if ((int)br >= 0)
                {
                    md = parse_mode(argv[3]);
                    if ((int)md < 0)
                    {
                        printf("Invalid mode argument '%s' - use 'msiklm help' to list an overview of valid commands\n", argv[3]);
                        ret = -1;
                    }
                }
                else
                {
                    printf("Invalid brightness argument '%s' - use 'msiklm help' to list an overview of valid commands\n", argv[2]);
                    ret = -1;
                }
            }
            else
            {
                printf("Invalid color argument '%s' - use 'msiklm help' to list an overview of valid commands\n", argv[1]);
                ret = -1;
            }
            break;

        default:
            printf("Invalid arguments supplied; use 'msiklm help' to show a list of valid arguments\n");
            break;
    }

    if (ret == 0)
    {
        if ((int)md >= 0)
        {
            hid_device* dev = open_keyboard();

            if (dev != NULL)
            {
                for (int i=0; i<num_regions; ++i)
                    set_color(dev, colors[i], i+1, br);
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
