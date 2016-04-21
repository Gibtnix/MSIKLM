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

as well which will most certainly will work on most Linux distributions. This script will to the
following steps, if you do not want to use the installation script for some reason, you can use
the manual commands instead:

 * installation of the dependencies
   ```
   sudo apt-get install -y gcc make libhidapi-dev
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

|command                                               | valid arguments                                                                              | example                              |
|------------------------------------------------------|----------------------------------------------------------------------------------------------|--------------------------------------|
|sudo msiklm \<color\>                                   | none, off (equivalent to none), red, orange, yellow, green, sky, blue, purple, white         | sudo msiklm green                    |
|sudo msiklm \<color_left\>,\<color_middle\>,\<color_right\> | same as single color (important: no space between the colors!)                               | sudo msiklm green,blue,red           |
|sudo msiklm \<mode\>                                    | normal, gaming, breathe, demo, wave                                                          | sudo msiklm wave                     |
|sudo msiklm \<color\> \<brightness\>                      | color either one or three values (comma-separated), brightness can be off, low, medium, high | sudo msiklm green high               |
|sudo msiklm \<color\> \<mode\>                            | same as above                                                                                | sudo msiklm green,blue,red wave      |
|sudo msiklm \<color\> \<brightness\> \<mode\>               | same as above                                                                                | sudo msiklm green,blue,red high wave |

Additionally, there are three extra commands that might be useful if something does not work:

    msiklm help         -> shows the program's help
    sudo msiklm test    -> tests if a compatible keyboard is found
    sudo msiklm list    -> lists all found hid devices, this might be helpful if your keyboard is not detected by MSIKLM


# AUTOSTART

An important additional feature is the optional autostart functionality since the keyboard will
reset itself to its default color configuration whenever you reboot it or resume from standby.
Hence, it is really useful to automatically reconfigure the keyboard to your configuration of
choice. To do this, there is an extra script called 'autostart.sh' that can do this for you. This
autostart functionality is two-fold: The first thing is to automatically configure the keyboard
during system boot, the other one is to automatically configure it when resuming from standby.
For the first one, the file /etc/rc.local (this is an initialization script that is called during
system boot) will be modified such that MSIKLM will be called with your arguments of choice, i.e.
the following line will be added to your /etc/rc.local:

    /usr/local/bin/msiklm <your arguments>

To configure the keyboard when resuming from standby, an additional script has to be created
because /etc/rc.local will not be called here. Unluckily the scripts that will be needed depend on
the current Linux/Ubuntu version. For the latest Ubuntu releases, the standby/wakeup scripts have
to be placed in the directory /lib/systemd/system-sleep/ while for older versions probably
/usr/lib/pm-utils/sleep.d/ is the path of choice. Not only this, also the commands that you have
to place in these scripts differ. To do both, modify /etc/rc.local and create a wakeup script in
/lib/systemd/system-sleep/, run:

    ./autostart.sh <your arguments>

Try if everything works by first rebooting your system and then try a standby and wakeup. If
everything works, we are done here. If not, probably your Linux version is an older one, i.e. the
script has to be placed in /usr/lib/pm-utils/sleep.d/ and has to be modified as well. So first
move the script by

    sudo mv /lib/systemd/system-sleep/msiklm-wakeup.sh /usr/lib/pm-utils/sleep.d/99ZZ_msiklm-wakeup.sh

and second modify its content by running

    sudo <editor> /usr/lib/pm-utils/sleep.d/99ZZ_msiklm-wakeup.sh

where <editor> is your favorite text editor (gedit, kate, ...). Then modify the text file to

    #!/bin/sh

    case "$1" in
        resume)
            /usr/local/bin/msiklm <your arguments>
    esac

save it and try a standby resume. Finally the autostart can be disabled by running

    ./autostart.sh --disable

which will undo the modifications of /etc/rc.local as well as removing the created wakeup script.
If you manually moved the wakeup script, it will not be automatically removed.


# UNINSTALLATION

MSIKLM also comes with an uninstallation script uninstall.sh which will remove the program file
/usr/local/bin/msiklm as well as running ./autostart --disable, i.e. it disables the autostart and
removes created wakeup scripts. If you want to use it, simply run:

    ./uninstall.sh


# DEVELOPPER INFORMATION

The source code is splitted into three files: The main application (main.c) that converts the
input as well as a small library that contains the main features (msiklm.h and msiklm.c). It
provides a simple C API and hence allows an easy integration into different programs like maybe
a small graphical user interface.
