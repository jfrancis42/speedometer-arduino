// 0==speed, 1==course, 2==altitude, 3==lat/lon, 4==hdop, 5=gmt time, 6=local time, 7=uptime, 8=home

// Wiring:
// + -
// 9 8
// 7 10
// x x
// x x

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TM1638.h>

static const int RXPin=2, TXPin=3;
static const uint32_t GPSBaud=4800;
#define TZ 7
static const double HOME_LAT = 31.9033924, HOME_LON = -110.9994065;

TinyGPSPlus gps;
TM1638 module0(8,9,7);
TM1638 module1(8,9,10);

// The colors are swapped. Either a bug in the library or a bug in my board.
#define RED TM1638_COLOR_GREEN
#define GRN TM1638_COLOR_RED
#define YEL (TM1638_COLOR_GREEN+TM1638_COLOR_RED)
#define OFF 0

// GPS software serial port.
SoftwareSerial ss(RXPin, TXPin);
int mode0=3,mode1=3;

void setup() {
  module0.setDisplayToString("..GPS..");
  module0.setLEDs(0x0000);
  module1.setDisplayToString("........");
  module1.setLEDs(0x0000);
  
  ss.begin(GPSBaud);
}

void loop() {
  int i,b;
  int b1,b2;
  unsigned long s;
  
  b1=module0.getButtons();
  b2=module1.getButtons();

  if(b1==1)
    mode0=0;
  else if(b1==2)
    mode0=1;
  else if(b1==4)
    mode0=2;
  else if(b1==8) {
    mode0=3;
    mode1=3;
  }
  else if(b1==16)
    mode0=4;
  else if(b1==32)
    mode0=5;
  else if(b1==64)
    mode0=6;
  else if(b1==128)
    mode0=7;

  if(b2==1)
    mode1=0;
  else if(b2==2)
    mode1=1;
  else if(b2==4)
    mode1=2;
  else if(b2==8) {
    mode0=8;
    mode1=8;
  }
  else if(b2==16)
    mode1=4;
  else if(b2==32)
    mode1=5;
  else if(b2==64)
    mode1=6;
  else if(b2==128)
    mode1=7;

  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {  
      // Display speed in mph on 7-seg LEDs
      if (gps.location.isValid()) {
	      if(mode0==0)
	        s=gps.speed.mph();
	      else if(mode0==1)
	        if(gps.speed.mph()>=1)
	          s=gps.course.deg();
	        else
	          s=0;
	      else if(mode0==2)
	        s=gps.altitude.feet();
	      else if(mode0==3)
	        s=gps.location.lat()*1000000;
	      else if(mode0==4)
	        s=gps.hdop.value();
	      else if(mode0==5)
	        s=gps.time.value()/100;
	      else if(mode0==6) {
	        s=gps.time.value()/100;
	        if(s>=(unsigned long)TZ*(unsigned long)10000)
	        s=s-((unsigned long)TZ*(unsigned long)10000);
	      else
	        s=s-(unsigned long)TZ*(unsigned long)10000+(unsigned long)240000;
	      }
	      else if(mode0==7)
	        s=millis()/1000;
        else if(mode0==8) 
          s=TinyGPSPlus::courseTo(
              gps.location.lat(),
              gps.location.lng(),
              HOME_LAT, 
              HOME_LON);
      
	      // Bug (?) where setDisplayToDecNumber display blank screen with value 0
	      if(s==0)
	        module0.setDisplayToString("       0");
	      else
	        module0.setDisplayToDecNumber(s, false, false);

        if(mode1==0)
          s=gps.speed.mph();
        else if(mode1==1)
          if(gps.speed.mph()>=1)
            s=gps.course.deg();
          else
            s=0;
        else if(mode1==2)
          s=gps.altitude.feet();
        else if(mode1==3)
          s=gps.location.lng()*-100000;
        else if(mode1==4)
          s=gps.hdop.value();
        else if(mode1==5)
          s=gps.time.value()/100;
        else if(mode1==6) {
          s=gps.time.value()/100;
          if(s>=(unsigned long)TZ*(unsigned long)10000)
            s=s-((unsigned long)TZ*(unsigned long)10000);
          else
            s=s-(unsigned long)TZ*(unsigned long)10000+(unsigned long)240000;
        }
        else if(mode1==7)
          s=millis()/1000;
        else if(mode0==8) {
          float d=TinyGPSPlus::distanceBetween(
                  gps.location.lat(),
                  gps.location.lng(),
                  HOME_LAT, 
                  HOME_LON)*0.000621371; // meters to feet
          if(d<=1) // if we're less than a mile, use feet
            s=d*5280;
          else
            s=d;
        }
        
        // Bug (?) where setDisplayToDecNumber display blank screen with value 0
        if(s==0)
          module1.setDisplayToString("       0");
        else
          module1.setDisplayToDecNumber(s, false, false);

      } else {
  	    // All RED if invalid fix
	      //module0.setLEDs(0xFF00);
        module1.setLEDs(0xFF00);
      }

      // Display num of sats on row of LEDs on the second board in GRN.
      b=gps.satellites.value();
      if(b==0) {
	      // All RED if no sats
	      module1.setLEDs(0xFF00);
      } else {
	      for(i=0;i<b;i++) {
	        if(b<3)
	          module1.setLED(RED,i);
	        else if(b<4)
	          module1.setLED(YEL,i);
	        else
	          module1.setLED(GRN,i);
	      }
	      if(b<8) {
	        for(i=b;i<8;i++) {
	          module1.setLED(OFF,i);
	        }
	      }
      }
    }
  }
  
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    module0.setDisplayToString("NO GPS");
    // All RED if no GPS.
    module0.setLEDs(0xFF00);
    module1.setLEDs(0xFF00);
    while(true);
  } 
}

