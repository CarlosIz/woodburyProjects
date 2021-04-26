// program that uses sensor input and finite state machine (FSM) logic 
// to fade between red, green and blue colors of an RGB LED 

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;        // the number of the pushbutton pin
const int ledPinRed =  6;       // the number of the red LED pin
const int ledPinGreen =  5;     // the number of the green LED pin
const int ledPinBlue =  3;      // the number of the blue LED pin

const int analogInPin = A0;     // sensor attached to pin A0

int sensorValue = 0;            // value read from the sensor
int sensorValueOld = 0;         // previous value read from the sensor

// program state machine definitions
#define STATE_RED 1
#define STATE_RED2 2
#define STATE_GREEN 3
#define STATE_YELLOW1 4
#define STATE_YELLOW 5


// variables that will change:
int buttonState = 0;            // variable for reading the pushbutton status    
int buttonStateOld = 0;    
int programState = STATE_RED;  // variable to keep track of program state
int timer = 5;
int programState2 = 0;
unsigned long sensorReadMs = 0;


void setup() 
{
  pinMode(ledPinRed, OUTPUT); // set ledPinRed as an output
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
  pinMode(buttonPin, INPUT);  // set btnPin as an input
  Serial.begin(9600);
}

void loop()
{
  buttonState = digitalRead(buttonPin);  // read digital input of btnPin

  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  
  // print the results to the serial monitor:
  Serial.print("sensor = " );
  Serial.println(sensorValue);
  
  // high-to-low change in button state 
  if ((sensorValue >= 350) && (timer == 5))  
  {
    
    programState = STATE_RED;
    
    if(programState == STATE_RED)
    {
      digitalWrite(ledPinRed, HIGH);
      delay(1000);
      digitalWrite(ledPinRed, LOW);
      delay(1000);
      Serial.println("RED");
      
    }
  }
    if((sensorValue >= 200) && (sensorValueOld >= 350))
    {
      programState = STATE_RED2;
      timer = 0;
      if(programState == STATE_RED2)
      {
      Serial.print(sensorValue);
      Serial.println("programState -> RED2");
      for (int i = 0; i <= 255; i++)
      {
        analogWrite(ledPinRed, i);
        if (i == 255)
        {
          for (i = 255; i >= 0; i--)
          {
          analogWrite(ledPinRed, i);
          }
        }
      }
    }
    if((sensorValue <= 100) && (sensorValueOld >= 200))
    {
      programState = STATE_YELLOW1;
    }
    if(programState == STATE_YELLOW1)
    {
      if((sensorValue >= 150) && (sensorValueOld <= 100))
      {
        programState = STATE_YELLOW;
      }
    }
    if(programState == STATE_YELLOW)
    {
      digitalWrite(ledPinRed, HIGH);
      digitalWrite(ledPinGreen, HIGH);
      Serial.print(sensorValue);
      Serial.println("programState -> YELLOW");
    }
    if (buttonState == HIGH)
    {
      programState2 = 1;
      if(programState2 == 1)
      {
        digitalWrite(ledPinGreen, HIGH);
        digitalWrite(ledPinRed, LOW);
        for (int i = 0; i <= 5; i++)
      {
        if (i == 5)
        {
          timer = 5;
        }
      }
      }
      
    }
    delay(10);
  }
  

  if(millis() > sensorReadMs + 500)  // if last sensor read was > 500ms ago
  {
//    Serial.println("update sensorValOld");
    sensorValueOld = sensorValue;
    sensorReadMs = millis();      // remember the time we read sensor
  }
  
  buttonStateOld = buttonState;   // save the latest button state
}
