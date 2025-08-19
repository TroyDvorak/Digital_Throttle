#include <Servo.h>
#include <IRremote.h>
Servo myservo;
int sensorvalue;
//const int lopePin = 7;
const int cruisePin = 6;
const int launchPin = 8;
const int numReadings = 50;
int lastThrottle = 0;
int readings[numReadings];  
int readIndex = 0;          
int total = 0;             
int average = 0;  
int i = 0;
const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;
const int THROTTLE_THRESHOLD = 60; // to indicate if throttle is being pressed

// Button values for remote 
const int button0 = 16738455;
const int button1 = 16724175;
const int button2 = 16718055;
const int button3 = 16743045;
const int button4 = 16716015;
const int button5 = 16726215;
const int button6 = 16734885;
const int button7 = 16728765;
const int button8 = 16730805;
const int button9 = 16732845;
const int buttonPause = 16712445;
const int buttonUp = 16748655;
const int buttonDown = 16769055;

// Servo position in degrees corresponding to RPM (roughly)
const int RPM3K = 7;
const int RPM4K = 11;
const int RPM5K = 20;
const int RPM6K = 23;
const int RPM7K = 65;
const int RPM8K = 105;
const int RPMMAX = 180;

// Servo position corresponding to 45 mph on actual sled (last tested was around 6500 rpm)
const int MPH45 = 36; 


void setup() {
Serial.begin(9600);
//pinMode(lopePin, OUTPUT);
pinMode(cruisePin, OUTPUT);
pinMode(launchPin, OUTPUT);
myservo.attach(9);
myservo.write(0);
irrecv.enableIRIn();
  irrecv.blink13(true);
for (int j = 0; j < numReadings; j++) {
    readings[j] = 0;
  }
  myservo.write(0);
}



void loop() {
  
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(A0);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;
  //Serial.println(average); 
  int servoPosition = map(average, 52, 307, 0, 180); //Maps 0 percent throttle (52) to 0 degrees and 100% throttle (307) to 180 degrees
  myservo.write(servoPosition); 
  //Serial.println(servoPosition);
 int throttlePosition = map(average, 52, 307, 0, 100);
  //Serial.println(throttlePosition);
  
  if (irrecv.decode(&results)){
 
        if (results.value == 0XFFFFFFFF)
          results.value = key_value;
          //Serial.println(results.value);
        switch(results.value){
          case 0xFFA25D:
          Serial.println("CH-");
          break;
          case 0xFF629D:
          Serial.println("CH");
          break;
          case 0xFFE21D:
          Serial.println("CH+");
          break;
          case 0xFF22DD:
          Serial.println("|<<");
          break;
          case 0xFF02FD:
          Serial.println(">>|");
          break ;  
          case 0xFFC23D:
          Serial.println(">|");
          break ;               
          case 0xFFE01F:
          Serial.println("-");
          decrement(servoPosition);
          break ;  
          case 0xFFA857:
          Serial.println("+");
          break ;  
          case 0xFF906F:
          Serial.println("^");
          increment(servoPosition);
          break ;  
          case 0xFF6897:
          Serial.println("0");
          myservo.write(0);
          break ;  
          case 0xFF9867:
          Serial.println("EQ"); //45 mph
          hold(MPH45);
          break ;
          case 0xFFB04F:
          Serial.println("200+");
          break ;
          case 0xFF30CF:
          Serial.println("1");
          hold(45);
          break ;
          case 0xFF18E7:
          Serial.println("2");
          hold(90);
          break ;
          case 0xFF7A85:
          Serial.println("3");
          hold(RPM3K);
          break ;
          case 0xFF10EF:
          Serial.println("4");
          hold(RPM4K);
          break ;
          case 0xFF38C7:
          Serial.println("5");
          hold(RPM5K);
          break ;
          case 0xFF5AA5:
          Serial.println("6");
          hold(RPM6K);
          break ;
          case 0xFF42BD:
          Serial.println("7");
          hold(RPM7K);
          break ;
          case 0xFF4AB5:
          Serial.println("8");
          hold(RPM8K);
          break ;
          case 0xFF52AD:
          Serial.println("9");
          hold(RPMMAX);
          break ;      
        }
        key_value = results.value;
        irrecv.resume(); 
  }


//LOPE SWITCH
// int lopeSwitch = digitalRead(lopePin);
// if (lopeSwitch == HIGH) {
//   myservo.write(40);
//   delay(100);
//   myservo.write(0);
//   delay(1000);
// }

//CRUISE CONTROL
int cruiseButton = digitalRead(cruisePin);
if (cruiseButton == HIGH) {
  delay(1000);
  cruiseButton = digitalRead(cruisePin);
  while (1) {
    myservo.write(sensorvalue);
    cruiseButton = digitalRead(cruisePin);
    lastThrottle = analogRead(A0);
    //Serial.println(analogRead(A0));
    if (lastThrottle > THROTTLE_THRESHOLD) {
      break;
    }
    if (cruiseButton == HIGH) {
      delay(1000);
      break;
    }
    delay(1);
  }
}

//LAUNCH CONTROL
int launchButton = digitalRead(launchPin);
while (launchButton == HIGH && analogRead(A0) >= 114) {
  myservo.write(40);
  launchButton = digitalRead(launchPin);
  delay(1);
}

 delay(5); //loop delay for stability
 }

//Function to hold specified servo position
void hold(int degrees) {
  while (1) {
    myservo.write(degrees);
    //Serial.println(degrees);
    Serial.println(results.value);
    lastThrottle = analogRead(A0);
    if (lastThrottle > THROTTLE_THRESHOLD) {
      break;
      }
    if (irrecv.decode(&results)){
      if (results.value == 0XFFFFFFFF) 
        results.value = key_value;
      
      if (results.value == button1) //1 
        degrees = 45;
      else if (results.value == button2) //2
        degrees = 90;
      else if (results.value == button3) { //3
        degrees = RPM3K;
        
      }
      else if (results.value == button4) //4
        degrees = RPM4K;
      else if (results.value == button5) //5
        degrees = RPM5K;
      else if (results.value == button6) //6
        degrees = RPM6K;
      else if (results.value == button7) //7
        degrees = RPM7K;
      else if (results.value == button8) //8
        degrees = RPM8K;
      else if (results.value == button9) //9
        degrees = RPMMAX;
      else if (results.value == buttonUp) //up arrow
        increment(degrees);
      else if (results.value == buttonDown) //down arrow
        decrement(degrees);
      if (results.value == button0) //0
        break;
      key_value = results.value;
      irrecv.resume();   
  }    
  }
}

//Servo ramp run
void increment(int degrees) {
  while (1) {
    degrees += 2;
    //Serial.println(degrees);
    if (degrees >= 180) 
      break;
    myservo.write(degrees);
    lastThrottle = analogRead(A0);
    if (lastThrottle > THROTTLE_THRESHOLD) {
      break;
      }
    if (irrecv.decode(&results)) {
      if (results.value == 0XFFFFFFFF) 
        results.value = key_value;
      //Serial.println(results.value);
      if (results.value == button1) //1 
        hold(45);
      else if (results.value == button2) //2
        hold(90);
      else if (results.value == button3) //3
        hold(RPM3K);
      else if (results.value == button4) //4
        hold(RPM4K);
      else if (results.value == button5) //5
        hold(RPM5K);
      else if (results.value == button6) //6
        hold(RPM6K);
      else if (results.value == button7) //7
        hold(RPM7K);
      else if (results.value == button8) //8
        hold(RPM8K);
      else if (results.value == button9) //9
        hold(RPMMAX);
      else if (results.value == buttonPause)// >||
        hold(degrees);
      else if (results.value == buttonDown) // down arrow
        decrement(degrees);
      if (results.value == button0) //0
        break;
      key_value = results.value;
      irrecv.resume();   
  }   
  delay(80); 
  }
  hold(0);
}


//Servo deramp run
void decrement(int degrees) {
  
  while (1) {
    
    degrees -= 2;
    //Serial.println(degrees);
    if (degrees <= 0) 
      break;
    myservo.write(degrees);
    lastThrottle = analogRead(A0);
    if (lastThrottle > THROTTLE_THRESHOLD) {
      break;
      }
    if (irrecv.decode(&results)){
      if (results.value == 0XFFFFFFFF) 
        results.value = key_value;
      //Serial.println(results.value);
     if (results.value == button1) //1 
        hold(45);
      else if (results.value == button2) //2
        hold(90);
      else if (results.value == button3) //3
        hold(RPM3K);
      else if (results.value == button4) //4
        hold(RPM4K);
      else if (results.value == button5) //5
        hold(RPM5K);
      else if (results.value == button6) //6
        hold(RPM6K);
      else if (results.value == button7) //7
        hold(RPM7K);
      else if (results.value == button8) //8
        hold(RPM8K);
      else if (results.value == button9) //9
        hold(RPMMAX);
      else if (results.value == buttonPause)// >||
        hold(degrees);
      else if (results.value == buttonUp) //up arrow
        increment(degrees);
      if (results.value == button0) //0
        break;
      key_value = results.value;
      irrecv.resume();   
  }   
  delay(80); 
  }
  hold(0);
}


