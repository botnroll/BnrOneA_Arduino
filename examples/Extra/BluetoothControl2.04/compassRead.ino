float read_yaw()
{
byte highByte, lowByte;    // highByte and lowByte store the bearing and fine stores decimal place of bearing

   Wire.beginTransmission(ADDRESSCMPS10);     //start communication with CMPS11
   Wire.write(2);                             //Send the register we wish to start reading from
   Wire.endTransmission();

   Wire.requestFrom(ADDRESSCMPS10, 2);        // Request 4 bytes from CMPS11
   while(Wire.available() < 2);               // Wait for bytes to become available
   highByte = Wire.read();
   lowByte = Wire.read();
   
return (float)((highByte<<8)+lowByte)/10;
}

char read_roll()
{
char roll;                 // Store  roll values of CMPS10, chars are used because they support signed value

   Wire.beginTransmission(ADDRESSCMPS10);     //start communication with CMPS11
   Wire.write(5);                             //Send the register we wish to start reading from
   Wire.endTransmission();

   Wire.requestFrom(ADDRESSCMPS10, 1);        // Request 4 bytes from CMPS11
   while(Wire.available() < 1);               // Wait for bytes to become available
   roll =Wire.read();
return roll;
}

char read_pitch()
{
char pitch;                // Store pitch values of CMPS10, chars are used because they support signed value

   Wire.beginTransmission(ADDRESSCMPS10);     //start communication with CMPS11
   Wire.write(4);                             //Send the register we wish to start reading from
   Wire.endTransmission();

   Wire.requestFrom(ADDRESSCMPS10, 1);        // Request 4 bytes from CMPS11
   while(Wire.available() < 1);               // Wait for bytes to become available
   pitch = Wire.read();

return pitch;
}
