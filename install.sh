#!/bin/sh

# script to automatically do the following steps:
#   - install the dependencies to compile the program, i.e. install 'gcc', 'make' and 'libhidapi-dev'
#     remark: once the program is compiled, 'libhidapi-dev' isn't required any more, only 'libhidapi-libusb0' is
#   - compile the program
#   - copy the program 'msiklm' to '/usr/local/bin/msiklm'
#   - run 'msiklm' with 'test' option to check if the keyboard is detected

echo "Installation script for msiklm - MSI Keyboard Light Manager"
echo "###########################################################"
sleep 1

echo "Installing dependencies..."
sleep 2

sudo apt-get install -y gcc make libhidapi-dev
if [ $? -eq 0 ]; then
    echo "Dependencies installed successfully"
    sleep 2
else
    echo "Dependencies could not be installed"
    exit 1
fi

echo "Compiling 'msiklm'..."
sleep 2

# compile
make
if [ $? -eq 0 ]; then
    make clean
    echo "Compiling finished successfully"
    sleep 2
else
    echo "Compiling failed"
    exit 1
fi

# install 'msiklm' to '/usr/local/bin/msiklm' which also makes it callable via the msiklm command
make install
if [ $? -eq 0 ]; then
    echo "Installation of 'msiklm' finished successfully"
    sleep 2
else
    echo "Installation of 'msiklm' failed"
    exit 1
fi;

echo "Testing 'msiklm'..."
sleep 1

sudo msiklm test
sleep 1

if [ $? -eq 0 ]; then
    echo "To configure the keyboard run 'msiklm' with your configuartion of choice, for a list of valid commands run 'msiklm help'."
    echo "If you want to enable the autostart, run 'autostart.sh' with your configuartion of choice, to uninstall 'msiklm' run 'uninstall.sh'."
    echo "Thank you for using 'msiklm' and have fun with it! :-)"
else
    echo "Testing of 'msiklm' failed"
fi
