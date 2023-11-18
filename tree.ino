//Pin # defs
#define fullPin 2   //Treestand full float switch
#define lowPin 3    //Treestand low float switch  
#define bucketPin 4 //Bucket low float switch
#define treePin 5   //Treestand critical low float switch
#define killSwitch 6 //Pin for relay that disables pump
#define pumpPin 7     //Pin for MOSFET that controls pump
#define buzzer 8   //Make some noise
#define spillSense A0   //Analog (capacitive) water sensor on floor beneath stand

/* --- THIS TIME NEEDS TO BE CALIBRATED IN SITU --- */
#define fillDurationMax 60000 //Max duration in milliseconds for fill cycle before it assumes a fault and alarms

/* ---  This voltage needs to be calibrated in situ --- */
#define spillThreshold 500    // Analog voltage from spill sensor, lower trips spill condition


bool alarm = false;   //Is the alarm loop running 
bool fillState = false; //Is the fill cycle running

int fillStartTime = 0;  //Time fill function starts 
int fillCurrentTime = 0; //Current time

void alarmAction() {
  digitalWrite(pumpPin, LOW);
  digitalWrite(killSwitch, HIGH);
  digitalWrite(buzzer, HIGH);
}

/* --- Alarm condition check --- */
void alarmCheck() {                             
  if (analogRead(spillSense) < spillThreshold) {  //Check the spill sensor, if it detects water on floor, disable pump and trip alarm
    alarm = true;
    Serial.println("SPILL");
    
  }
  else if (digitalRead(bucketPin) == false) {   //Check that the bucket has enough water in it, alarm if not
    alarm = true;
    Serial.println("BUCKET LOW");

  }
  else if (digitalRead(treePin) == false) {   //Check that the tree stand has a minimum amount of water in it. This might be offloaded to separate circuit
    alarm = true;
    Serial.println("TREE LOW");

  }
  if (alarm == true) {
    alarmAction();
  }
  else {
   alarm = false;
   Serial.println("ALARM CLEAR");
  }
}

/* --- Fill cycle control --- */
void fill() {
  if (fillState == true) {
    if (fillStartTime == 0) {
      fillStartTime = millis();
      digitalWrite(pumpPin, HIGH);
      Serial.println("PUMP START");
    }
    if (fillStartTime > 0) {
      fillCurrentTime = millis() - fillStartTime;
      if (fillCurrentTime >= fillDurationMax) {
        alarm = true;
        return;
      }
      else {
        return;
      }
    }
  }
  if (fillState == false && fillStartTime > 0) {
    digitalWrite(pumpPin, LOW);
    fillStartTime = 0;
    Serial.println("PUMP FINISH");
  }
}


void setup() {
  //Define all the pins and initial pin states
  pinMode(fullPin, INPUT_PULLUP);
  pinMode(lowPin, INPUT_PULLUP);
  pinMode(bucketPin, INPUT_PULLUP);
  pinMode(treePin, INPUT_PULLUP);

  pinMode(killSwitch, OUTPUT);
  digitalWrite(killSwitch, LOW);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
  pinMode(spillSense, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  alarmCheck();
  if (alarm == false) {                                           //If no alarm conditions, run
    if (digitalRead(fullPin) ==  true && fillState == false) {   //Read "full" sensor switch
      if (digitalRead(lowPin) == false) {                          //Read "fill start" sensor switch
      fillState = true;
      }
    }
    if (digitalRead(fullPin) == false && fillState == true) {
      fillState = false;
    }
    fill();    
  }
}
