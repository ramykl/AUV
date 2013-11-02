/*
 Calculate the longitudinal scale -----------------------------------------------------------------------------------------
 */
static float lon_scale(float *loc){
  static float last_lat;
  static float scale = 1.0;
  if (fabs(last_lat - loc[0]) < 100000) {
    // we are within 0.01 degrees (about 1km) of the
    // same latitude. We can avoid the cos() and return
    // the same scale factor.
    return scale;
  }
  scale = cos((fabs((float)loc[0])/1.0e7) * 0.0174532925);
  last_lat = loc[0];
  return scale;
}


/*
 Calculates the bearing in centi-degrees between two locations -----------------------------------------------------------
 */
float bearing_cd(float *loc1, float *loc2){
  float off_x = loc2[1] - loc1[1];
  float off_y = (loc2[0] - loc1[0]) / lon_scale(loc2);
  float bearing = 9000 + atan2(-off_y, off_x) * 5729.57795;
  if (bearing < 0) bearing += 36000;
  return bearing;
}

/*
 Calculate the distance between 2 points ----------------------------------------------------------------------------------
 */
float c_dist(float *loc1, float *loc2){
  // the standard version of formulae yields unacceptable precision
  // return acos(sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon2-lon1));
  // alternative formulae (haversine)
  const float two=2.0;
  return two*asin(sqrt(square(sin((loc1[0]-loc2[0])/two)) + cos(loc1[0])*cos(loc2[0])*square(sin((loc2[1]-loc1[1])/two))));
}

/*
 see if location is past a line perpendicular to
 the line between point1 and point2. If point1 is
 our previous waypoint and point2 is our target waypoint
 then this function returns true if we have flown past
 the target waypoint
 */
boolean location_passed_point(float *location, float *point1, float *point2){
  // the 3 points form a triangle. If the angle between lines
  // point1->point2 and location->point2 is greater than 90
  // degrees then we have passed the waypoint
  float *loc = location;
  float *p1 = point1;
  float *p2 = point2;
  Vector2f loc1(loc[0], loc[1]);
  Vector2f pt1(p1[0], p1[1]);
  Vector2f pt2(p2[0], p2[1]);
  float angle = (loc1 - pt2).angle(pt1 - pt2);
  if (isinf(angle)) {
    // two of the points are co-located.
    // If location is equal to point2 then say we have passed the
    // waypoint, otherwise say we haven't
    if (c_dist(loc, p2) == 0) {
      return true;
    }
    return false;
  } 
  else if (angle == 0) {
    // if we are exactly on the line between point1 and
    // point2 then we are past the waypoint if the
    // distance from location to point1 is greater then
    // the distance from point2 to point1
    return c_dist(loc, p1) > c_dist(p2, p1);
  }
  if (degrees(angle) > 90) {
    return true;
  }
  return false;
}

/*
 check if a location is inside a polygon ----------------------------------------------------------------------------------------
 */
bool pointInPolygon( float x, float y ){
  int i, j = points - 1;
  bool oddNodes = false;
  for (i = 0; i < points; i++){
    if ((location[i][1] < y && location[j][1] >= y || location[j][1] < y && location[i][1] >= y) &&  (location[i][0] <= x || location[j][0] <= x)){
      oddNodes ^= (location[i][0] + (y - location[i][1]) / (location[j][1] - location[i][1]) * (location[j][0] - location[i][0]) < x);
    }
    j = i;
  }
  return oddNodes;
}

