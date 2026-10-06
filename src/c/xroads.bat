@echo off

rem show xroads cmd line options
rem xroads.exe -h

rem show xroads debug information
rem xroads.exe -d

rem create log file from output for debugging
rem xroads.exe > xroads.log

rem reduce car velocity to 80%
rem xroads.exe -v 80

rem hide rail tracks
rem xroads.exe -r

rem hide power lines 
rem xroads.exe -p

rem hide bikes, bins and people
rem xroads.exe -b

rem hide street lights
rem xroads.exe -s

rem hide highway lights
rem xroads.exe -w

rem force X-Plane default roads
rem xroads.exe -x

rem an example combination
rem xroads.exe -v 80 -w -s -p -x

rem use xroads default settings
xroads.exe

