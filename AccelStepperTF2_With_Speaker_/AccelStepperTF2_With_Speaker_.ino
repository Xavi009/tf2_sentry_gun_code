#include <AccelStepper.h>
#include <SD.h>                           //include SD module library
#include <TMRpcm.h>                       //include speaker control library
#include <SPI.h>
#define HALFSTEP 8
#define FULLSTEP 4

/////AccelStepper/////
//The first 4 means FULLSTEP, then the coil end pins Blue, Yellow, Pink, Orange
AccelStepper stepper(4, 2, 4, 3, 5);

int targetPosition = 509;      //2048 steps per rotation when full stepping
int speed = 180;               //sets speed to 180; max achievable speed for me
bool clockwise = true;


/////BUTTON/////
int buttonPin = 7;   // input pin for push button
int buttonState = 0;


/////Speaker+Audio/////
#define SD_ChipSelectPin 10               //define CS pin
TMRpcm tmrpcm;                            //create an object for speaker library

void setup() {
  stepper.setCurrentPosition(targetPosition/2);  //sets current position to be in the center (half of targetPosition)
  stepper.setMaxSpeed(speed);         //Positive speeds are clockwise, Negative Speeds are counter-clockwise
  stepper.moveTo(targetPosition);
  stepper.setSpeed(speed);
  pinMode(buttonPin, INPUT_PULLUP);    // declare pushbutton as input

  tmrpcm.speakerPin = 9;
  if (!SD.begin(SD_ChipSelectPin)) {      //see if the card is present and can be initialized
    return;                               //don't do anything more if not
  }
  tmrpcm.setVolume(4);
}

void loop() {
  //Check to see if the stepper has reached the target:
  if (stepper.distanceToGo() == 0) {
    tmrpcm.play("1.wav");
    if (clockwise == true) {
      clockwise = false;  //Go counterclockwise now
      stepper.moveTo(0); //Go back to the "home" (original) position
      stepper.setSpeed(-speed);
    }
    else {
      clockwise = true;   //Go clockwise now
      stepper.moveTo(targetPosition);  //Go to the target position
      stepper.setSpeed(speed);
    }
  }

  stepper.runSpeed();                    //If the stepper still needs to move (distanceToGo() != 0)
                                         //continue to advance (step) the motor
  buttonState = digitalRead(buttonPin);  // read input value
  if (buttonState == HIGH) {             // check if the input is HIGH (button released)
    stepper.moveTo(targetPosition/2);
    while (stepper.distanceToGo() < 0) {
      stepper.setSpeed(-speed);
      stepper.runSpeed();
    }
    while (stepper.distanceToGo() > 0) {
      stepper.setSpeed(speed);
      stepper.runSpeed();
    }
    tmrpcm.stopPlayback();
    delay(6000);
  }
  else {
    stepper.runSpeed();
  }
}
