#include <Servo.h>

Servo myservo;
Servo myservo1;
void setup() 
{ 
  myservo.attach(8);
  myservo1.attach(9);
} 

void loop() 
{
  for(int i = 0,j = 110; i <= 110,j >= 0; i+=1,j-=1){
    myservo.write(i); 
    delay(10);
    myservo1.write(j); 
    delay(10);
  }
  delay(1000);
   for(int i = 110,j = 0; i >= 0,j <= 110; i-=1,j+=1){
    myservo.write(i); 
    delay(10);
    myservo1.write(j); 
    delay(10);
  }
  delay(1000);
}
