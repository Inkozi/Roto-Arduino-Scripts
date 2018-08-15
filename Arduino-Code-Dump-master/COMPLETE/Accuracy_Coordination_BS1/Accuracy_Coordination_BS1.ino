/*
 * Roto
 *
 * Project: Akron Psychology
 *
 *
 * Exhibit: Accuracy and Coordination
 *
 *
 * Author(s): Brady Schoeffler
 *
 *
 *
 * Original environment:
 * Arduino 1.0.5 - Roto Libraries xxxx.xx.xx
 *
 * Revision notes:
 *
 *
 *
 *
 */

#define versionString "Roto - [Akron Psychology] [Accuracy and Coordination]"

// Debug Mode (comment to disable)
//#define debugMode

// Libraries:
#include <digits.h>
#include <alarmClock.h>
#include <buttonBoard.h>

//7seg
bool sentinel = true;
const int relay = 4;

// Pin Assignments:
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;
const byte bbDI = 8;
const byte bbDO = 9;
const byte bbCLK = 10;
const byte bbILT = 11;
const byte bbOLT = 12;
const byte bbCount = 1;
const byte segData1 = 14;
const byte segClk1 = 15;
const byte segLat1 = 16;

// Constants:
#define numberOfBoards 1
#define segNumber 3 //number of total digits
#define displayLength 3
#define dp 0
#define updateDisplayIntervalClock 1000
#define timeInterval 5000
#define gameTimeLength 30000
#define attractTime 100
#define pollTime 20

// Global Variables:
boolean gameStart = false;
byte gameSection = 0;
int count = 0;
boolean states[3];
unsigned long updateDisplayInterval;
unsigned long gameTimer;
byte lastButton = 255;
int counter;

// Global Classes:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);
digits dig1 = digits(segData1, segClk1, segLat1, segNumber); //create object of class sevenSegment
digitGroup seg1 = digitGroup(&dig1,0,displayLength);
buttonSelect select = buttonSelect(&bb, 0, 3, true); // this is a helper class for a one of many group of buttons

void pollButtons();
void attractor();

repeatAlarm pollTimer = repeatAlarm(pollButtons); // calls the function pollButtons() on a set interval
repeatAlarm attractMe = repeatAlarm(attractor);

/*
 * 
 *  FxN sevenSegMagic
 *    New intergalactic protocol that 
 *    sets up space-age revision 7segs!!
 *    
 */
void sevenSegSetup(){
  digitalWrite(relay, LOW);
  delay(1000);
  digitalWrite(relay, HIGH);
  delay(1000);
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,LOW);
  }
  delay(100);//obligatory delay...
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,HIGH);
  }
}

void setup()
{
  pinMode(relay, OUTPUT);
  Serial.begin(9600);
  Serial.println(versionString);
  Serial.print("File: ");
  Serial.println(__FILE__);
  attractMe.setInterval(attractTime);
  pollTimer.setInterval(pollTime);
}

void blinkScore(int count, int interval, int counter) {

  for(int i = 0; i < counter; i++) {
    seg1.segDisp(blank);
    dig1.copySection(0, displayLength, displayLength);
    dig1.update();

    delay(interval);

    seg1.segDisp(count,dp);
    dig1.copySection(0, displayLength, displayLength);
    dig1.update();

    delay(interval);
  }
  seg1.segDisp(count,dp);
  dig1.copySection(0, displayLength, displayLength);
  dig1.update();
  delay(3000);
}


void countdownSequence() {
  seg1.segDisp(blank);
  dig1.copySection(0, displayLength, displayLength);
  dig1.update();

  bb.setLamp(4, true);
  delay(500);
  bb.setLamp(4, false);
  delay(2500);
  Serial.println("game start");
}


void pollButtons()
{ 
  select.poll();
  Serial.println(select.getState(), DEC);
  if (!states[select.getState()] && select.getState() != lastButton) {
    states[select.getState()] = true;
    delay(50);
    counter++;
    lastButton = select.getState();
  }
  boolean allFilled = true;
  for (int i = 0; i < 3; i++) {
    if (states[i] == false)
      allFilled = false;
  }
  if (allFilled) {
    for (int i = 0; i<3; i++) {
      states[i] = false;
    }
  }

  //Serial.println(counter);
  seg1.segDisp(counter,dp);
  dig1.copySection(0, displayLength, displayLength);
  dig1.update();
}

void attractor() {
  seg1.chaseAnimation8();
}

void loop()
{
  if (sentinel){
    sevenSegSetup();
    sentinel = false;
  }
  if (!gameStart) {
    attractMe.poll();
    bb.setLamp(3, HIGH);
  }
  if (!gameStart && bb.getButton(3)) {
    Serial.println("Triggered");
    bb.setLamp(3, LOW);
    gameStart = true;
    countdownSequence();
    gameTimer = millis() + gameTimeLength;
  }
  if (gameStart && millis() < gameTimer) {
    pollTimer.poll();
  }
  if (gameStart && millis() > gameTimer) {
    gameStart = false;
    bb.setLamp(7, true);
    delay(500);
    bb.setLamp(7, false);
    blinkScore(counter, 500, 5);
    Serial.println("game ended");
    select.setState(buttonReset);
    counter = 0;
  }
}





