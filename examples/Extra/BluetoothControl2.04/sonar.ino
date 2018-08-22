int Sonar()
{
    unsigned long duration;  // Duration used to calculate distance
    int distance;
    unsigned long tempo=micros();
  
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2); 
  
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
  
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH, 11640);
    delayMicroseconds(16000 - (micros()-tempo));  // this routine has fixed time (16 milliseconds)
  
    //Calculate the distance (in cm) based on the speed of sound
    distance = (int)(duration/58.2);
    if (distance >= maximumRange || distance <= minimumRange)
      distance=-1;
  
    return distance;
}
