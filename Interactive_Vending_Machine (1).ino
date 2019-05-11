
/* 
 * This is a customer engagement activity at any experience zone where advertisers place their kiosks.
 * This is a machine that detects heat, sound and running activity of a person vending out an item on successful crossing of a threshold.
 * 
 */

/* External libraries for LCD */
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/* External libraries for Stepper motor */
#include <Stepper.h>

/* External libraries for Temperature sensor */
#include <DHT.h>

/*Button pins*/ 
const int buttonPin = 2; // Button pin for Temperature sensor
const int buttonPinSound=16; // Button pin for Sound sensor
const int buttonPinStep = A1; // Button pin for Accelerometer

/*Button states*/ 
int buttonState = 0;  // State of button pin for Temperature sensor
int buttonStateSound=0; // State of button pin for Sound sensor
int buttonStateStep=0;  // State of button pin for Accelerometer

/* Temperature sensor */
#define DHTPIN 7// what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

/* Sound sensor */
int sensorPin=A0;

/* Accelerometer */
const int x_out = A3; /* x-axis */
const int y_out = A4; /* y-axis */
const int z_out = A5; /* z-axis */

/* Stepper motor */
const int stepsPerRevolution = 90;  // change this to fit the number of steps per revolution for the motor
Stepper myStepper(stepsPerRevolution,3,5,6,13); // initialize the stepper library on pins 3,5,6,13

/* LCD varaibles */
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);

void setup(void)
{
  myStepper.setSpeed(60);  // set the speed at 60 rpm:
  pinMode(4,OUTPUT); // Backlight of LCD
  digitalWrite(4,HIGH); //Turn Backlight ON
  pinMode(buttonPin, INPUT); // Setting Temperautre sensor pin as input pin
  pinMode(buttonPinSound, INPUT); // Setting Sound sensor pin as input pin
  pinMode(buttonPinStep, INPUT); // Setting Accelerometer pin as input pin
  dht.begin();  // Initialize the temperature port:
  display.begin();   // Initialize the LCD
  display.setContrast(50);
  display.display(); // show splashscreen
  display.clearDisplay();   // clears the screen and buffer
  pinMode(sensorPin, INPUT); // sound sensor pin set as input pin
  Serial.begin(9600);   // Initialize the serial port:
}

void loop(void)
{   
    display.clearDisplay();
    display.println("Choose!");
    display.println();
    display.println("1. Hear");
    display.println("2. Feel");
    display.println("3. Run");
    display.display();
    buttonState = digitalRead(buttonPin);
    buttonStateSound = digitalRead(buttonPinSound);
    buttonStateStep= digitalRead(buttonPinStep);
    if (buttonState == HIGH) {
      TemperatureMonitor();
   }
   if (buttonStateSound == HIGH) {
      SoundMonitor();
   }
    if (buttonStateStep == HIGH) {
      AccelerometerMonitor();
   }
}

/* Customer has to reach the TARGET_TEMPERATURE in the TOTAL_TIME given to vend out an item. */ 
void TemperatureMonitor(){
  /* Constants */ 
  const int TOTAL_TIME= 30; // Total time in seconds to reach target temperature. Change as desired. 
  const int TARGET_TEMPERATURE = 28; // Target temperature in celsius. Change as desired. 
  /* Variables */ 
  int success=0;  // Intialize success to 0
  int currentTemperature=0; // Stores temperature value read in each second 
  /* 
   *  Loop which runs for TOTAL_TIME. 
   *  It senses the temperature and stores in variable (currentTemperature) in every iteration(i.e. second) and compares to TARGET_TEMPERATURE.
   *  If currentTemperature reaches or goes beyond TARGET_TEMPERATURE, then success is set to 1 and the loop is terminated.
  */ 
  for(int sec = TOTAL_TIME; sec >= 1; sec--) // 30 seconds to reach desired temp
  { 
      display.clearDisplay();
      display.print("Reach: ");
      display.print(TARGET_TEMPERATURE);
      display.println(" C!!");
      display.print("Time Left: ");
      display.println(sec);
      display.println("Temperature: ");
      currentTemperature = dht.readTemperature();
      if(currentTemperature >= TARGET_TEMPERATURE)
      {
        success = 1;
        sec = 0;
      }
      display.println(currentTemperature);
      display.display();
      delay(1000);
   }
   /* If TARGET_TEMPERATURE is reached */
   if(success==1)
   {
      display.clearDisplay();
      display.println("  You did it!");
      display.println(" Temperature:");
      display.print("   ");
      display.print(currentTemperature);
      display.println(" C");
      display.display();
      myStepper.step(stepsPerRevolution);  // Vends out item
   }
  /* If TARGET_TEMPERATURE is not reached */
  else{
     display.clearDisplay();
     display.println("Times up !!  ");
     display.println("Baaah!:(  ");
     display.println("Try Again!");
     display.display();
  }
  delay(5000);
}

/* 
 *  Customer has to reach the TARGET_SCORE in the TOTAL_TIME given to vend out an item by screaming out loud/making noise.  
 *  Note: In case no loud sound is being made sound sensor readings are around 45 - 50 where as if sound is being made readings are around 1000-1025. 
*/ 
void SoundMonitor(){
  /* Constants */ 
  const int TOTAL_TIME= 5; // Total time in seconds to reach target score. Change as desired. 
  const int TARGET_SCORE = 3000; // Target Score. Change as desired. 
  /* Variables */ 
  int currentSoundReading=0;
  int totalScore=0;
  /* Asks customer to get ready on LCD screen*/
  display.clearDisplay();
  display.println("    Scream !!");
  display.println("  Get ready!!");
  display.display();
  display.clearDisplay();
  delay(2000);
  /* 
   *  Loop which runs for TOTAL_TIME
   *  In every iteration(i.e. second) the following happens:
   *      1. It senses sound and stores readings in variable (currentSoundReading) 
   *      2. Adds currentSoundReading to totalScore
  */ 
  for(int sec=TOTAL_TIME;sec>=1;sec--)
  { 
      currentSoundReading =analogRead(sensorPin);
      totalScore = totalScore + currentSoundReading; 
      /* Serial Print currentSoundReading and totalScore for every iteration(i.e. second) */
      Serial.print("currentSoundReading:");
      Serial.print(currentSoundReading);
      Serial.print("      totalScore:");
      Serial.println(totalScore);
      /* Display time left on LCD */
      display.clearDisplay();
      display.println();
      display.println();
      display.print("       ");
      display.print(sec);
      display.println("!");
      display.display();
      delay(1000);
   }
  /* If customer is able to reach TARGET_SCORE by screaming */
  if(totalScore>=TARGET_SCORE)
  {
    display.clearDisplay();
    display.println("  You did it!");
    display.display();
    myStepper.step(stepsPerRevolution); // Vends out item
  }
  else{
     display.clearDisplay();
     display.println("Times up !!  ");
     display.println("Baaah!:(  ");
     display.println("Try Again!");
     display.display();
  }
  delay(5000);
}

/* 
 *  Measures the movement in the z direction (up and down).
 *  Note: if there is movement, then the zDifference value will be large. Sometimes the zDifference value could be negative, then it is converted to a positive value.
 *  Algorithm:
 *   1. Obtain intial z reading in previous second as zPrev
 *   2. For loop which runs for TOTAL_TIME
 *       i.   Obtain z reading in current second as zNew 
 *       ii.  Calculate zDifference [zDifference = zNEw - zPrev]
 *       iii. If zDifference is negative, then make it positive by [zDifference=-zDifference]
 *       iv.  TotalScore = TotalScore + zDifference
 *       v.   Set zPrev = zNEw
 *   3. If TotalScore reaches TARGET_SCORE, then vend item
 */
void AccelerometerMonitor(){
   /* Constants */ 
  const int TOTAL_TIME= 15; // Total time in seconds to reach target score. Change as desired. 
  const int TARGET_SCORE = 500; // Target Score. Change as desired. 
  /* Variables */ 
  int zPrev; //  value of z in previous second
  int zNew; // value of z in current second
  int zDifference=0;
  int totalScore=0;
  /* Asks customer to get ready on LCD screen*/
  display.clearDisplay();
  display.println("    Run !!");
  display.println("  Get ready!!");
  display.display();
  display.clearDisplay();
  delay(2000);
  /* Reads old value of z*/ 
  zPrev = analogRead(z_out); /* Digital value of voltage on z_out pin */ 
  for(int sec=TOTAL_TIME;sec>=1;sec--)
  {
      zNew = analogRead(z_out); /* Digital value of voltage on z_out pin */ 
      /* Serial print values for convenience */
      Serial.print("zPrev = ");
      Serial.print(zPrev);
      Serial.print("\t\t");
      Serial.print("zNew = ");
      Serial.print(zNew);
      Serial.print("\t\t");
      zDifference = zNew - zPrev;
      if(zDifference<0)
        zDifference=-zDifference;
      totalScore = totalScore + zDifference;
      zPrev=zNew;
      /* Serial print values for convenience */ 
      Serial.print("Diff = ");
      Serial.print(zDifference);
      Serial.print("\t\t");
      Serial.print("totalScore = ");
      Serial.print(totalScore);
      Serial.println("\t\t");
      /* Displays the following on the LCD:
       *   1. Target Score
       *   2. Time left
       *   3. Current Score
       */
      display.clearDisplay();
      display.println("Reach:");
      display.print(TARGET_SCORE);
      display.println(" points!");
      display.print("Time Left: ");
      display.println(sec);
      display.println("Current Score: ");
      display.println(totalScore);
      display.display();
      delay(1000);
  }
   /* Serial print values for convenience */
   Serial.print("totalScore=  ");
   Serial.println(totalScore);
   /* If Customer reaches target scoore */
   if(totalScore>=TARGET_SCORE)
    {
      display.clearDisplay();
      display.println("  You did it!");
      display.display();
      myStepper.step(stepsPerRevolution); //vends out item
    }
    else{
       display.clearDisplay();
       display.println("Times up !!  ");
       display.println("Baaah!:(  ");
       display.println("Try Again!");
       display.display();
    }
    delay(5000);
}

