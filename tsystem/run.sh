#!/bin/sh
# Define some constants
BIN_PATH=bin
SRC_PATH=ticketingsystem
# First remove the sources.list file if it exists and then create the sources file of the project
find $SRC_PATH -name *.java > $SRC_PATH/sources.list
# First remove the ONSServer directory if it exists and then create the bin directory of ONSServer
rm -rf $BIN_PATH
mkdir $BIN_PATH
# Compile the project
javac -d $BIN_PATH @$SRC_PATH/sources.list

java -cp ${BIN_PATH} ticketingsystem.Test # or somepackage.someclass
