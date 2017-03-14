# speedometer

![Arduino Speedometer](http://bob.gritch.org/misc/speedometer.jpg)

This is an arduino sketch that acts as a really dumb speedometer (and other GPS features). The basic bits of the solution are an Arduino board (most any board is adequate), a 4800bps NMEA-0183 GPS module (pretty much any GPS module available for the Arduino), and two TM1638 LED driver boards (mine have eight 7-segment displays, eight red/green LEDs, and eight pushbutton switches). Why LEDs instead of a multi-line LCD display? Because they look cool. No good reason. I just like them.

By default, when you apply power to the system, once the GPS obtains a position lock, the first (left) display shows your latitude, and the second (right) display shows your longitude. There is no indication of N/S or E/W due to the limited number of digits available. It's assumed that you know which hemisphere you're in. There is also no decimal point displayed. This is due to a limitation in the TM1638 driver. It's technically possible to do, but painful. It's assumed you can mentally insert the decimal point. The series of eight LEDs on the second (right) display are used as a bar graph to show how many satellites are currently used (obviously, with a limit of eight). Displays are updated at the same rate the GPS provides new data, which is typically 1hz.

The buttons underneath each display can alter the displayed data. The buttons on the left display do the following:

* Button 1: Speed (in MPH)
* Button 2: Course (in Degrees)
* Button 3: Altitude (in Feet)
* Button 4: Latitude (as DDddddd) on left, Longitude (as DDddddd) or right
* Button 5: HDOP
* Button 6: GMT Time
* Button 7: Local Time
* Button 8: Seconds since boot

The buttons on the right display do the following:

* Button 1: Speed (in MPH)
* Button 2: Course (in Degrees)
* Button 3: Altitude (in Feet)
* Button 4: Distance to Home (left display), Bearing to Home (right display)
* Button 5: HDOP
* Button 6: GMT Time
* Button 7: Local Time
* Button 8: Seconds since boot

In order to have the correct local time, you need to set your offset from GMT in the source code as the variable TZ. In order to set the definition of Home, change the HOME_LAT and HOME_LON variables to the desired location. Then change the code to display this data when the desired button is pressed (I usually use Button 8, since I really don't care how many seconds it's been since the system booted).

Distance to home is displayed in Miles until the distance is less than 1 Mile, at which point it switches to feet.
