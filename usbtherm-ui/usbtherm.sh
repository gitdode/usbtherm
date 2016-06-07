#!/bin/sh

DEVICE=/dev/usbtherm2
BASEDIR=`dirname $0`

cd $BASEDIR
java -jar usbtherm-ui.jar $DEVICE > /tmp/usbtherm.log 2>&1
