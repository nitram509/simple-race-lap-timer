@echo off

SET "AVR_PATH=c:\local\arduino-1.6.7\hardware\tools\avr"
SET "AVRDUDE=%AVR_PATH%\bin\avrdude.exe"
SET "AVRDUDE_CONF=%AVR_PATH%\etc\avrdude.conf"
SET BUILD_DIR=.build

%AVRDUDE% -C "%AVRDUDE_CONF%" -c usbasp-clone -p atmega328p -U "flash:w:%BUILD_DIR%/oled_test.ino.with_bootloader.hex"
