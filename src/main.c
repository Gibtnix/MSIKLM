/**
 * @file main.c
 *
 * @brief the main application
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
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
    printf( "Usage: msiklm [options]...\n"
            "MSI Keyboard Light Manager\n"
            "\n"
            "Mandatory arguments to long options are mandatory for short options too.\n"
            "  -M, --mode=MODE\n"
            "      Only set a mode and keep the colors unchanged; MODE can be 'normal'\n"
            "      (default if ommitted), 'gaming', 'breathe', 'demo' or 'wave'\n"
            "  -t, --test\n"
            "      Test if a compatible SteelSeries MSI Gaming Notebook\n"
            "      is detected\n"
            "  -e, --enumerate\n"
            "      List all found HID devices\n"
            "  -a, --color_all=COLOR\n"
            "      Set color for all 3 regions of the keyboard; COLOR can be 'none' (or 'off'),\n"
            "      'red', 'orange', 'yellow', 'green', 'sky', 'blue', 'purple', 'white' or an \n"
            "      hexadecimal code for RRGGBB value\n"
            "  -l, --color_left=COLOR\n"
            "      Set color for left region of the keyboard; COLOR can be 'none' (or 'off'),\n"
            "      'red', 'orange', 'yellow', 'green', 'sky', 'blue', 'purple', 'white' or an\n"
            "      hexadecimal code for RRGGBB value\n"
            "  -m, --color_middle=COLOR\n"
            "      Set color for middle region of the keyboard; COLOR can be 'none' (or 'off'),\n"
            "      'red', 'orange', 'yellow', 'green', 'sky', 'blue', 'purple', 'white' or an\n"
            "      hexadecimal code for RRGGBB value\n"
            "  -r, --color_right=COLOR\n"
            "      Set color for right region of the keyboard; COLOR can be 'none' (or 'off'),\n"
            "      'red', 'orange', 'yellow', 'green', 'sky', 'blue', 'purple','white' or an\n"
            "      hexadecimal code for RRGGBB value\n"
            "  -b, --brightness=BRIGHTNESS\n"
            "      Set the brightness of the color(s) send by options -a, -l, -m and -ri;\n"
            "      BRIGHTNESS can be 'off', 'low', 'medium' or 'high'\n"
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
    color_t color_left = red, color_middle = red, color_right = red;
    enum brightness br = high;
    enum mode md = normal;
    int cl_set = 0, cm_set = 0, cr_set = 0;

    struct option longopts[] = {    {"mode", required_argument, NULL, 'M'},
                                    {"test", no_argument, NULL, 't'},
                                    {"enumerate", no_argument, NULL, 'e'},
                                    {"help", no_argument, NULL, 'h'},
                                    {"color_all", required_argument, NULL, 'a'},
                                    {"color_left", required_argument, NULL, 'l'},
                                    {"color_middle", required_argument, NULL, 'm'},
                                    {"color_right", required_argument, NULL, 'r'},
                                    {"brightness", required_argument, NULL, 'b'},
                                    {0, 0, 0, 0}
                               };
    int i;
    int indexptr;

    if (argc == 1) {
        show_help();
        exit(0);
    }
    // Decode options
    while ((i = getopt_long(argc, argv, "M:teha:l:m:r:b:", longopts, &indexptr)) != -1) {
            switch (i) {
                   case 0:
                           if (longopts[indexptr].flag)
                                  break;
                   case 'M':    // mode
                           md = parse_mode(optarg);
                           break;
                   case 'a':    // color all
                           if (cl_set || cm_set || cr_set) 
                           {
                               printf("Colliding color options. Try --help for more informations.\n");
                               exit(-1);
                           }
                           if (parse_color(optarg, &color_right) == 0)
                           {
                               color_left = color_middle = color_right;
                               cl_set = cm_set = cr_set = -1;
                           }
                           else
                           {
                               printf("Color '%s' could not be parsed. Try --help for more informations.\n", optarg);
                               exit(-1);
                           }
                           break;
                   case 'l':    // color left
                           if (cl_set) {
                               printf("Colliding color options. Try --help for more informations.\n");
                               exit(-1);
                           }
                           if (parse_color(optarg, &color_left) != 0)
                           {
                               printf("Color '%s' could not be parsed. Try --help for more informations.\n", optarg);
                               exit(-1);
                           }
                           cl_set = -1;
                           break;
                   case 'm':    // color middle
                           if (cm_set) {
                               printf("Colliding color options. Try --help for more informations.\n");
                               exit(-1);
                           }
                           if (parse_color(optarg, &color_middle) != 0)
                           {
                               printf("Color '%s' could not be parsed. Try --help for more informations.\n", optarg);
                               exit(-1);
                           }
                           cm_set = -1;
                           break;
                   case 'r':    // color right
                           if (cr_set) {
                               printf("Colliding color options. Try --help for more informations.\n");
                               exit(-1);
                           }
                           if (parse_color(optarg, &color_right) != 0)
                           {
                               printf("Color '%s' could not be parsed. Try --help for more informations.\n", optarg);
                               exit(-1);
                           }
                           cr_set = -1;
                           break;
                   case 'b':    // brightness
                           br = parse_brightness(optarg);
                           break;
                   case 't':    // test
                           if (keyboard_found())
                               printf("Compatible keyboard found!\n");
                           else
                               printf("No compatible keyboard found!\n");
                           break;
                   case 'e':    // enumerate
                           enumerate_hid();
                           break;
                   case 'h':    // help
                   case '?':
                           show_help();
                           exit(0);
                           break;
            }
    }

    hid_device* hiddev = open_keyboard();
    if (hiddev == NULL)
    {
        printf("No compatible keyboard found!\n");
        return -1;
    }
    if (br == off) { //no color if brightness is set to off
        color_left = color_middle = color_right = none;
        cl_set = cm_set = cr_set = -1;
    }
    if (cl_set)
        set_color(hiddev, color_left, left, br);
    if (cm_set)
        set_color(hiddev, color_middle, middle, br);
    if (cr_set)
        set_color(hiddev, color_right, right, br);

    // Send the mode no matter what
    set_mode(hiddev, md);

    hid_close(hiddev);
    hid_exit();
    return 0;


}
