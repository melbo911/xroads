@echo off

rem stands for "remark" or comment to show examples !!

rem show xroads help and cmd line options
rem xroads.exe -h

rem print additional debug information
rem xroads.exe -d

rem create log file from output for debugging
rem xroads.exe > xroads.log

rem reduce car velocity to 80%
rem xroads.exe -v 80

rem enable left-hand-driving support 
rem xroads.exe -l

rem hide rail tracks
rem xroads.exe -r

rem hide lights on primary and secondary streets 
rem xroads.exe -s
rem xroads.exe -s -s      ( hides even more lights )

rem hide highway lights
rem xroads.exe -w

rem example of how to combine multiple options
rem xroads.exe -v 80 -r -s

rem use xroads default settings
xroads.exe

