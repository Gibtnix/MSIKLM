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

    if (sudo /usr/local/bin/msiklm $@); then
        echo "Activating MSIKLM autostart..."
        sleep 1

        #redirection with '>' or '>>' takes place before 'sudo' is applied, hence not directly usable here
        sudo sh -c "echo '# run MSIKLM to configure the keyboard illumination' > $file"
        sudo sh -c "echo 'ACTION==\"add\", ATTRS{idVendor}==\"1770\", ATTRS{idProduct}==\"ff00\", ' >> $file"
        run="echo '  RUN+=\"/usr/local/bin/msiklm $@\"'"
        sudo sh -c "$run >> $file"

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
