/*
  print gps data (for debugging)-------------------------------------------------------------------------------------------------
 */
void print_gps(float lat, float lon){
  Serial.print("gps:");
  Serial.print(" Lat:");
  Serial.print(lat, DEC);
  Serial.print(" Lon:");
  Serial.print(lon, DEC);
  Serial.print(" Alt:");
  Serial.print((float)gps.altitude / 100.0, DEC);
  Serial.print(" GSP:");
  Serial.print(gps.ground_speed / 100.0);
  Serial.print(" COG:");
  Serial.print(gps.ground_course / 100.0, DEC);
  Serial.printf(" VEL: %.2f %.2f %.2f", gps.velocity_north(), gps.velocity_east(), sqrt(sq(gps.velocity_north())+sq(gps.velocity_east())));
  Serial.print(" SAT:");
  Serial.print(gps.num_sats, DEC);
  Serial.print(" FIX:");
  Serial.print(gps.fix, DEC);
  Serial.print(" TIM:");
  Serial.print(gps.time, DEC);
  Serial.println();
  Serial.print("day:");
  Serial.print(day, DEC);
  Serial.print("\t");
  Serial.print(hours, DEC);
  Serial.print(":");
  Serial.print(minutes, DEC);
  Serial.print(":");
  Serial.println(seconds, DEC);
}

/*
  print compass data (for debugging)-------------------------------------------------------------------------------------------------
 */
void print_compass(float heading, unsigned long read_time, float * offset){
  Serial.printf("Heading: %.2f (%3u,%3u,%3u) I2cerr=%u ",
  ToDeg(heading),
  compass.mag_x,
  compass.mag_y,
  compass.mag_z, 
  I2c.lockup_count());

  // display offsets
  Serial.printf("\t offsets(%.2f, %.2f, %.2f)",
  offset[0], offset[1], offset[2]);
  Serial.printf(" t=%u", (unsigned)read_time);
  Serial.println();
}

