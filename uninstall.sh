#!/bin/sh

# removes the program by deleting it from /usr/local/bin
if [ -f '/usr/local/bin/msiklm' ]; then
    sudo rm -v '/usr/local/bin/msiklm'

    if [ -f './autostart.sh' ]; then
        sudo ./autostart.sh --disable
    fi

    echo "All components of 'msiklm' were successfully removed!"
else
    echo "Nothing to remove because 'msiklm' is not installed!"
fi
