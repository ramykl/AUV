// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: t -*-
// Updated 16/10/13

#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Compass.h> // Compass Library
#include <AP_GPS.h>
#include <AP_Math.h>
#include <I2C.h>
#include <Servo.h>

FastSerialPort0(Serial);
FastSerialPort1(Serial1);
AP_GPS_UBLOX gps(&Serial1);

#define debug false
#define right 1
#define left 2
#define straight 0

#define turn_angle 10
#define neutral_angle 90

Servo wingL;
Servo wingR;
//Servo prop;
int prop_speed = 120;
int prop = 8; // attached to output pin 3


#define T6 1000000
#define T7 10000000
#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi


AP_Compass_HMC5843 compass;
unsigned long timer;
uint32_t time;
uint16_t hours, minutes, seconds, day;


boolean flag = true;

/*
locations: [-27.498620, 153.017141; -27.498451, 153.017001; -27.498203, 153.017301; -27.49847, 153.017436; -27.498608, 153.017336]
 */
/*
 Location data and parameters
 with poly = true location needs to be stored in series around boarder
 else location in series
 */
boolean poly = false; // flag for stay in polygon
const int points = 1;
// location = {{lat, lon}, {lat, lon},..., {lat, lon}}
// polygon of small lake at uq
// {-27.500906, 153.027553} = 0 bear, {-27.504237,153.030664} = 90 bear, 
// float location[points][2] = {{-27.498620, 153.017141}, {-27.498451, 153.017001}, {-27.498203, 153.017301}, {-27.49847, 153.017436}, {-27.498608, 153.017336}};
float location[points][2] = {
  {
    -27.498065,153.017095  }
};
float locVar = 0.000005;
int loc = 0;

float currentPos[2];
float heading, bearing, difBearing, degHeading;
uint8_t dir;

uint8_t count = 0;
uint8_t count2 = 0;

/*
  Initialise compass
 */
void compass_init(){
  I2c.begin();
  I2c.timeOut(20);
  I2c.setSpeed(true);
  if (!compass.init()) {
    Serial.println("compass initialisation failed!");
    while (1) ;
  }
  compass.set_orientation(AP_COMPASS_COMPONENTS_DOWN_PINS_FORWARD); // set compass's orientation on aircraft.
  compass.set_offsets(0,0,0); // set offsets to account for surrounding interference
  compass.set_declination(ToRad(0.0)); // set local difference between magnetic north and true north

  Serial.print("Compass auto-detected as: ");
  switch( compass.product_id ) {
  case AP_COMPASS_TYPE_HIL:
    Serial.println("HIL");
    break;
  case AP_COMPASS_TYPE_HMC5843:
    Serial.println("HMC5843");
    break;
  case AP_COMPASS_TYPE_HMC5883L:
    Serial.println("HMC5883L");
    break;
  default:
    Serial.println("unknown");
    break;
  }
}

/*
 Setup function --------------------------------------------------------------------------------------------------------------
 */
void setup(){
  Serial.begin(9600);
  Serial1.begin(38400);
  stderr = stdout;
  gps.print_errors = true;
  wingL.attach(6);  // attached to output pin 5
  wingR.attach(7);  // attached to output pin 4
  //  prop.attach(8); // attached to output pin 3
  pinMode(prop,OUTPUT);



  Serial.println("GPS UBLOX library test");
  gps.init(GPS::GPS_ENGINE_AIRBORNE_2G);       // GPS Initialization
  delay(1000);

  compass_init();
  delay(10000);
  wingL.write(90);
  wingR.write(90);
  //  prop.write(0);
  analogWrite(prop, prop_speed);

  timer = micros();
}



/*
 Main control loop----------------------------------------------------------------------------------------------------------
 */
void loop(){
  static float min[3], max[3], offset[3];
  float lat, lon;
  unsigned long read_time;
  delay(20);
  if (count == 50){
    count = 0;
    get_measurements();
    Serial.print("Lat:");
    Serial.print(lat, DEC);
    Serial.print(" Lon:");
    Serial.println(lon, DEC);
    Serial.printf("bearing: %.2f heading: %.2f \r\n", bearing, degHeading);
    Serial.printf("dif: %.2f\r\n", difBearing);
  }
  count +=1;
  // compass data update
  compass.accumulate();
  if((micros()- timer) > 100000L) {
    timer = micros();
    compass.read();
    read_time = micros() - timer;
    if(!compass.healthy) {
      Serial.println("not healthy");
      return;
    }
    heading = compass.calculate_heading(0,0); // roll = 0, pitch = 0 for this example
    compass.null_offsets();
    // capture min
    if(compass.mag_x < min[0]){
      min[0] = compass.mag_x;
    }
    if(compass.mag_y < min[1]){
      min[1] = compass.mag_y;
    }
    if(compass.mag_z < min[2]){
      min[2] = compass.mag_z;
    }
    // capture max
    if(compass.mag_x > max[0]){
      max[0] = compass.mag_x;
    }
    if(compass.mag_y > max[1]){
      max[1] = compass.mag_y;
    }
    if(compass.mag_z > max[2]){
      max[2] = compass.mag_z;
    }
    // calculate offsets
    offset[0] = -(max[0]+min[0])/2;
    offset[1] = -(max[1]+min[1])/2;
    offset[2] = -(max[2]+min[2])/2;

    degHeading = ToDeg(heading);
    //Serial.printf("heading: %.2f \r\n", degHeading);
  }


  gps.update();
  if(gps.new_data) {
    time = gps.time/1000;
    seconds = time%60;
    minutes = (time/60)%60;
    hours = (time/3600)%24;
    day = (time/(3600*24));
    lat = (float)gps.latitude / T7;
    lon = (float)gps.longitude / T7;
    currentPos[0] = lat;
    currentPos[1] = lon;
    // stay in polygon area 
    if(poly){
      // if outside turn back to the area
      if(!pointInPolygon(lat, lon)){
        // Serial.println("outside");

      }
      // path following
    } 
    else {
      // follow path
      if(loc != 0){
        if(location_passed_point(currentPos, location[loc], location[loc])){
          loc += 1;
        }
      }
      bearing = ((float) bearing_cd(currentPos, location[loc]))/100.0;
      if (bearing > 180){
        bearing -= 360;
      }
      difBearing = bearing - degHeading;


      steering();
    }
    update_motor();
    gps.new_data = 0; // We have readed the data
  }

  if(debug){
    print_gps(lat, lon);
    print_compass(heading, read_time, offset);
  }
}

