void steering(){
  if(difBearing < -1){
    dir = right;
    //Serial.print("right\r\n");
  } 
  else if(difBearing > 1){
    dir = left;
    //Serial.print("left\r\n");
  } 
  else {
    dir = straight;
    //Serial.print("straight\r\n");
  }
  if (abs(difBearing >180)){
    if(dir == right){
      dir = left;
      //Serial.print("left\r\n");
    } 
    else {
      dir = right;
      //Serial.print("right\r\n");
    }
  }
}


void update_motor(){
  int value = (difBearing*2)/5;
  if (dir == right){
    //    wingL.write(turn_angle);
    wingL.write(neutral_angle-value);
    wingR.write(neutral_angle);
  } 
  else if(dir == left){
    //    wingR.write(180-turn_angle);
    wingR.write(neutral_angle+value);
    wingL.write(neutral_angle);
  } 
  else {
    wingR.write(neutral_angle);
    wingL.write(neutral_angle);
  }
  //  prop.write(prop_speed);
  //  wingL.write(position);
  //  wingR.write(position);
  analogWrite(prop, prop_speed);
}

