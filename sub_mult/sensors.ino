int tR1 = 330000.0;
int sR2 = 4000.0;
int T = 25.0;
float tVs = 9.2;
float sVs = 3.4;
float gain = 0.82;
float areaGain = 0.247;// 0.28*1.5/1.7 // Cross Section Area/Distance

float value_to_volt(int value){
  float volts = ((float(value)*5.0)/1024.0);
  return volts;
}

void get_measurements(){
  float temp = -10.0;
  float con;
  float k;
  int salinityValue = 0;
  int tempValue = 0;
  float saltVolt = 0;
  float tempVolt = 0;
  salinityValue = analogRead(A2);
  tempValue = analogRead(A3);
  //  Serial.print("sv: ");
  //  Serial.print(salinityValue);
  //  Serial.print("\t\t\ttv: ");
  //  Serial.println(tempValue);

  saltVolt = value_to_volt(salinityValue);
  tempVolt = value_to_volt(tempValue);
  uint32_t saltRes = sR2*(gain*sVs/tempVolt - 1.0);
  uint32_t tempRes = 10*(tR1/(tVs/tempVolt - 1.0));
  Serial.print("svolt: ");
  Serial.print(saltVolt);
  Serial.print("\t\ttvolt: ");
  Serial.println(tempVolt);
  Serial.print("sres: ");
  Serial.println(saltRes); 
  //  Serial.print("\t\tres: ");
  //  Serial.println(tempRes);
  con = (1000.0/saltRes)/areaGain; // mS
  Serial.print("con (mS/cm): ");
  Serial.print(con);

  if (tempRes >= 52589 && tempRes <= 64759){
    temp = map(tempRes, 52589, 64759, 40, 35); // 40 - 35
  } 
  else if (tempRes > 64759 && tempRes <= 80223){
    temp = map(tempRes, 64759, 80223, 35, 30); // 35 - 30
  } 
  else if (tempRes > 80223 && tempRes <= 100000){
    temp = map(tempRes, 80223, 100000, 30, 25); // 30 - 25
  } 
  else if (tempRes > 100000 && tempRes <= 125468){
    temp = map(tempRes, 100000, 125468, 25, 20); // 25 - 20
  } 
  else if (tempRes > 125468 && tempRes <= 158499){
    temp = map(tempRes, 125468, 158499, 20, 15); // 20 - 15
  } 
  else if (tempRes > 158499 && tempRes <= 201659){
    temp = map(tempRes, 158499, 201659, 15, 10); // 15 - 10
  } 
  else if (tempRes > 201659 && tempRes <= 258497){
    temp = map(tempRes, 201659, 258497, 10, 5); // 10 - 5
  } 
  else if (tempRes > 258497 && tempRes <= 333964){
    temp = map(tempRes, 258497, 333964, 5, 0); // 5 - 0
  } 
  else if (tempRes > 333964 && tempRes <= 435026){
    temp = map(tempRes, 333964, 435026, 0, -5); // 0 - -5
  }

  if (temp > -10){
    Serial.print("\ttemp: ");
    Serial.println(temp);
    //    k = log(con)+0.029*(T - temp)-0.00019*(T*T-temp*temp);
    //    k = exp(k);
    ////    Serial.print("K: ");
    ////    Serial.print(k);
    //    Serial.print("%salt: ");
    //    Serial.println(k*0.055);
  } 
  else {
    temp = 24;
    Serial.println("\ttemp error");
  }
  k = log(con)+0.029*(T - temp)-0.00019*(T*T-temp*temp);
  k = exp(k);
  //    Serial.print("K: ");
  //    Serial.print(k);
  Serial.print("%salt: ");
  Serial.println(k*0.055); 
  //  delay(300);
}

