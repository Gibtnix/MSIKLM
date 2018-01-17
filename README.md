# GENERAL

The MSI Keyboard Light Manager (MSIKLM) is an easy-to-use tool that allows to configure the
SteelSeries keyboards of MSI gaming notebooks with Linux in almost the same way as the
SteelSeries Engine can do using Windows.


# INSTALLATION & REQUIREMENTS
## Manual installation

I tried to keep the external dependencies to a minimum level, however there are some unavoidable
ones. These are:

 * GCC     - the C compiler
 * make    - the main build tool of the Linux world
 * LIBUSB  - MSIKLM needs to communicate with the keyboard, for this LIBUSB is required

Besides there are no others, no Qt, no Java, not even a C++ compiler is required. To install the
program, there is an installation script 'install.sh' which can be run by opening the respective
folder in a terminal and typing

    ./install.sh

or if there are any problems you can try

    bash install.sh

as well which will most certainly will work on most Linux distributions. This script will do the
following steps, if you do not want to use the installation script for some reason, you can use
the manual commands instead:

 * installation of the dependencies
   ```
   sudo apt install -y gcc make libhidapi-dev
   ```

 * compiling of MSIKLM
   ```
   make
   ```

 * clean up
   ```
   make clean
   ```

 * copy the built program to '/usr/local/bin/msiklm' and set its permissions
   ```
   sudo mv -fv msiklm /usr/local/bin/msiklm
   sudo chmod 755 /usr/local/bin/msiklm
   ```

 * test the connection
   ```
   sudo msiklm test
   ```

Whenever MSIKLM is used, it should always be run as root because otherwise, the communication with
the keyboard is not possible, hence always use the sudo prefix (only 'msiklm help' will work as
non-root).

## Distribution package

Currently, the only linux distribution providing an install method is Archlinux, via the AUR repository

 * Archlinux : https://aur.archlinux.org/packages/msiklm-git/


# USABILITY

MSIKLM is a pure command line application, however its keyboard illumination control functionality
is encapsulated such that it could easily be integrated into a graphical user interface. However,
I neither wrote one for it nor I plan to do so. It is quite easy to use, and here is how to use
it. It always has to be called with at least one argument, i.e. running it without one will result
in an error. Here is an overview over the valid commands:

|command                                                       | valid arguments                                                                                | example                              |
|--------------------------------------------------------------|------------------------------------------------------------------------------------------------|--------------------------------------|
|sudo msiklm \<color\>                                         | either a predefined color or arbitrary RGB values ([R;G;B] or hex code), cf. explanation below | sudo msiklm green                    |
|sudo msiklm \<color1\>[,\<color2\>,\<color3\>,\<color4\>,...] | same as single color (important: no space between the colors!), cf. explanation below          | sudo msiklm green,blue,red           |
|sudo msiklm \<mode\>                                          | normal, gaming, breathe, demo, wave                                                            | sudo msiklm wave                     |
|sudo msiklm \<color\> \<brightness\>                          | color as above, brightness can be off, low, medium, high                                       | sudo msiklm green high               |
|sudo msiklm \<color\> \<mode\>                                | same as above                                                                                  | sudo msiklm green,blue,red wave      |
|sudo msiklm \<color\> \<brightness\> \<mode\>                 | same as above                                                                                  | sudo msiklm green,blue,red high wave |

The predefined supported colors are: none, off (equivalent to none), red, orange, yellow, green,
sky, blue, purple and white. The color configuration can also be performed in an more advanced way:
At most seven zones are supported (as long as supported by your device) and the respective colors
have to be supplied in the following order: left, middle, right, logo, front_left, front_right and
mouse. If there is only one supplied color, it is reused for the first three zones, the remaining
ones stay unchanged (i.e. green as single argument is equivalent to green,green,green). The colors
have to be separated with no spaces between the colors, simply add a comma for a new zone. The last
four colors are fully optional, i.e. they are set if and only if they are supplied. Consequently,
if you want to change the last color (mouse), you have to specify a color for all zones. Instead of
a predefined color, each color can alternatively be set in full RGB notation; the color values have
to be either enclosed by brackets and separated by semicolons, e.g. 'green' is equivalent to using
[0;255;0], or hex code notation can be used (0x000000 to 0xFFFFFF) where the respective values have
to be selected accordingly. It is possible to mix these explicit color definitions with predefined
ones, e.g. you can select a custom color for the left zone and use predefined for the others by
supplying [R;G;B],green,blue. Please note that it might be necessary to put quotation marks around
explicit color definitions, otherwise the argument might not be properly processed by the shell.

Additionally, there are three extra commands that might be useful if something does not work:

    msiklm help         -> shows the program's help
    sudo msiklm test    -> tests if a compatible keyboard is found
    sudo msiklm list    -> lists all found hid devices, this might be helpful if your keyboard is not detected by MSIKLM


# AUTOSTART

An important additional feature is the optional autostart functionality since the keyboard will
reset itself to its default color configuration whenever you reboot it or resume from standby.
Hence, it is really useful to automatically reconfigure the keyboard to your configuration of
choice. To do this, there is an extra script called 'autostart.sh' that can do this for you. This
script registers MSIKLM to the udev service (more precisely it registers the keyboard to the udev
service which calls MSIKLM as soon as the keyboard is detected) by creating a rule file:

    /etc/udev/rules.d/99-msiklm.rules

To create this file including your MSIKLM arguments of choice, run:

    ./autostart.sh <your arguments>

Try if everything works by first rebooting your system and then try a standby and wakeup. If
everything works, we are done here. If not, please report an issue. :-)

Finally the autostart can be disabled by running

    ./autostart.sh --disable

which will disable the autostart by removing the rule file.

Important remark: If you already used a previous version of MSIKLM, please undo the previous
autostart functionality by removing MSIKLM calls from 'etc/rc.local' as well as simply delete
MSIKLM wakeup scripts from the paths:

    /lib/systemd/system-sleep/
    /usr/lib/pm-utils/sleep.d/


# UNINSTALLATION

MSIKLM also comes with an uninstallation script uninstall.sh which will remove the program file
/usr/local/bin/msiklm as well as running ./autostart --disable, i.e. it disables the autostart.
If you want to use it, simply run:

    ./uninstall.sh


# DEVELOPER INFORMATION

The source code is split into three files:
- Main application (`main.c`) that converts the input
- Small library that contains the main features (`msiklm.h` and `msiklm.c`).
This provides a simple C API and hence allows an easy integration into different programs like maybe
a small graphical user interface.
