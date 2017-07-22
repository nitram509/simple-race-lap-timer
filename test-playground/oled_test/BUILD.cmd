@echo off

SET "ARDUINO=c:\local\arduino-1.8.3\arduino_debug.exe"
SET BUILD_DIR=.build

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

%ARDUINO% --verbose-build --board arduino:avr:nano --pref "build.path=%BUILD_DIR%" --verify  oled_test.ino
