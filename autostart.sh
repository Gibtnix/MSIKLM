#!/bin/sh

# to enable the autostart, execute this script with your configuration of choice (same arguments as used for 'msiklm')
# this script will create a file '/etc/udev/rules.d/99-msiklm.rules' to activate the respective configuration at startup or wakeup
# further, this script can also undo this by calling it with the '--disable' argument

if [ $# -eq 0 ]; then
    echo "At least one argument is required"
    exit 1
fi

# the rules file
file='/etc/udev/rules.d/99-msiklm.rules'

if  [ $1 != "--disable" ]; then
    # activate the autostart

    # autostart requires 'msiklm' to be installed
    if [ ! -f '/usr/local/bin/msiklm' ]; then
        echo "MSI Keyboard Light Manager is not installed, hence no autostart possible"
        exit 1
    fi

    sudo /usr/local/bin/msiklm $@
    if [ $? -eq 0 ]; then
        echo "Activating MSIKLM autostart..."
        sleep 1

        #redirection with '>' or '>>' takes place before 'sudo' is applied, hence not directly usable here
        sudo sh -c "echo '# run MSIKLM to configure the keyboard illumination' > $file"
        sudo sh -c "echo 'ACTION==\"add\", ATTRS{idVendor}==\"1770\", ATTRS{idProduct}==\"ff00\", \' >> $file"
        sudo sh -c "echo '  RUN+=\"/usr/local/bin/msiklm $@\"' >> $file"

        sudo chmod 755 $file

        echo "Autostart rules file '$file' created"
    else
        echo "Input arguments are not valid for 'msiklm', use 'msiklm help' to get an overview of valid arguments"
        exit 1
    fi
else
    # deactivate the autostart

    echo "Deactivating MSIKLM autostart..."
    sleep 1

    if [ -f $file ]; then
        echo "Removing autostart rules file..."
        sleep 1

        sudo rm -v $file
    fi
fi


# # function to remove the current entry in the file '/etc/rc.local'
# # used either to remove an old configuration during updating or to complete deactivate the autostart
# clean_rclocal()
# {
#     sudo sed -i "/^# automatically configure the keyboard illumination/d" /etc/rc.local
#     sudo sed -i "/msiklm/ d" /etc/rc.local
#     #sudo sed -i "/msiklm/,+1 d" /etc/rc.local #removes the next line, too
# }
#
# # the wakeup script that either will be created or removed
# script='/lib/systemd/system-sleep/msiklm-wakeup.sh'
#
# if  [ $1 != "--disable" ]; then
#     # activate the autostart
#
#     # autostart requires 'msiklm' to be installed
#     if [ ! -f '/usr/local/bin/msiklm' ]; then
#         echo "MSI Keyboard Light Manager is not installed, hence no autostart possible"
#         exit 1
#     fi
#
#     sudo /usr/local/bin/msiklm $@
#     if [ $? -eq 0 ]; then
#         echo "Activating autostart..."
#         sleep 1
#
#         # remove an already existing configuration (in case there is one) and add an entry for 'msiklm' into '/etc/rc.local'
#         clean_rclocal
#         sudo sed -i "s'^exit 0'# automatically configure the keyboard illumination at startup\n/usr/local/bin/msiklm $@\nexit 0'g" /etc/rc.local
#
#         echo "Autostart entry added to file '/etc/rc.local'"
#         sleep 1
#
#         dir=$(dirname $script)
#         if [ -d $dir ]; then
#
#             echo "Creating wakeup script..."
#             sleep 1
#
#             #redirection with '>' or '>>' takes place before 'sudo' is applied, hence not directly usable here
#             sudo sh -c "echo '#!/bin/sh' > $script"
#             sudo sh -c "echo '# automatically created script to configure the keyboard illumination at wakeup' >> $script"
#             sudo sh -c "echo '' >> $script"
#             sudo sh -c "echo 'case \$1/\$2 in' >> $script"
#             sudo sh -c "echo '    pre/*)' >> $script"
#             sudo sh -c "echo '        ;;' >> $script"
#             sudo sh -c "echo '    post/*)' >> $script"
#             sudo sh -c "echo '        /usr/local/bin/msiklm' $@ >> $script"
#             sudo sh -c "echo '        ;;' >> $script"
#             sudo sh -c "echo 'esac' >> $script"
#
#             sudo chmod 744 $script
#
#             echo "Wakeup script '$script' created"
#
#         else
#             echo "Directory '$dir' does not exist, hence no wakeup script created"
#         fi
#     else
#         echo "Input arguments are not valid for 'msiklm', use 'msiklm help' to see valid arguments"
#     fi
# else
#     # deactivate the autostart
#
#     echo "Disabling autostart..."
#     sleep 1
#
#     clean_rclocal
#
#     echo "Autostart entry removed from file '/etc/rc.local'"
#     sleep 1
#
#     if [ -f $script ]; then
#         echo "Removing wakeup script..."
#         sleep 1
#
#         sudo rm -v $script
#     fi
# fi
