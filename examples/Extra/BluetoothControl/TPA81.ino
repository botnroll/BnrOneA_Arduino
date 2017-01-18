byte getData(byte reg)
{                                   // Function to receive one byte of data from TPA81
    Wire.beginTransmission(ADDRESSTPA81);                        // Begin communication with TPA81
    Wire.write(reg);                                        // Send reg to TPA81
    Wire.endTransmission();
    Wire.requestFrom(ADDRESSTPA81, 1);                           // Request 1 byte
    while(Wire.available() < 1);                            // Wait for byte to arrive
    byte data = Wire.read();                                // Get byte
    return(data);                                           // return byte
}
