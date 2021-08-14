#include <M5StickCPlus.h>
#include <Preferences.h>

Preferences prefs;
TFT_eSprite tftSprite = TFT_eSprite(&M5.Lcd); 

int last_value = 0;
int cur_value = 0;

int debug_mode = 0;

int x;
int ticker = 0;

unsigned long tick = 0;
unsigned long revol = 0;
unsigned long timer = 0;

float speed;
float wheel = 2170;
float odo;
float trip;

int gotinput=0;
int intimer;
int lighttimer;
int offtimer;
int lightlit = 0;

int totaltick = 0;

int displayseq = 0;
int max_displayseq = 3;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Serial.begin(115200);
  pinMode(33, INPUT);
//  pinMode(37,INPUT_PULLUP);

  M5.Lcd.setRotation(1);
  tftSprite.createSprite(240,135);
  tftSprite.setRotation(1);
  
  prefs.begin("odo",false);
  odo = prefs.getFloat("odo",0);
  prefs.begin("trip",false);
  trip = prefs.getFloat("trip",0);
  
  powerSave();
}

void powerSave() {
  btStop();
  //esp_bt_controller_disable();
  M5.Axp.ScreenBreath(7);
  setCpuFrequencyMhz(80);
}

void drawBat() {
  int maxVolts = 420;
  int minVolts = 327;
  int batt = map(M5.Axp.GetBatVoltage() * 100, minVolts, maxVolts, 0 , 10000) / 100.0;

  //x = 0-40, procentueel battery voltage
  x = 40*batt/100; 
  tftSprite.drawRoundRect(190,5,40,15,5,BLUE);
  tftSprite.fillRect(191,6,x,13,BLACK);  
}

void drawLines() {
  tftSprite.drawLine(0,95,240,95,BLUE);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Code voor grote button:
  //if(digitalRead(M5_BUTTON_HOME) == LOW){
  if(M5.BtnA.wasPressed()) {
    lighttimer = millis();
    offtimer = millis();
    M5.Axp.ScreenBreath(9);
    lightlit = 1;
    displayseq++;
  }
  if (displayseq == max_displayseq) 
    {displayseq = 0;}

  //if(digitalRead(M5_BUTTON_RST) == LOW){
  if(M5.BtnB.wasPressed()) {
    lighttimer = millis();
    offtimer = millis();    
    M5.Axp.ScreenBreath(9);
    lightlit = 1;
    if(displayseq==1){
      odo=0;
    }
    if(displayseq==2){
      trip=0;
    }

  }

// read the value of INPUT grove port
cur_value = digitalRead(33);
  
//Tick is binnen
//Magneet geeft een aan/uit tick bij binnenkomen en bij buitengaan
//dus een offset van ticker

  if(cur_value != last_value){
    ticker++; 
    last_value = cur_value;
    if (ticker==1){
    revol = millis()-tick;
    tick = millis();
    odo = odo + (wheel/1000000);
    trip = trip + (wheel/1000000);
    gotinput = 1;
    intimer = millis();
    offtimer = millis();    
    totaltick++;
    }

    //One for magnet on, one for magnet off
    if (ticker==2){
      ticker=0;
    }      
  }

  tftSprite.fillSprite(WHITE);
  tftSprite.setTextColor(BLACK);        

//Indicator linksboven voor moving
  //tftSprite.setTextSize(2);
  //tftSprite.setCursor(5,5); 
  if (gotinput == 1){  
    //tftSprite.print("*");
    if (lightlit == 0){
        M5.Axp.ScreenBreath(9);
        lightlit = 1;
        lighttimer = millis();
    }
  }
  //else{
    //tftSprite.print("-");    
  //}
  
  //Print speed
  //Speed = wheel/revol * 0.0036 (3600 seconden voor een uur, 1000000 millimeter voor een kilometer)
  if (revol>0){
    speed = wheel/revol*3.6;
  }
  else {
    speed = 0;
  }

//Display counters onderaan  

/*
  tftSprite.setTextSize(2);
  tftSprite.setCursor(120,110);
  tftSprite.print(displayseq);
*/

/*
  //Ticker in
  tftSprite.setTextSize(2);
  tftSprite.setCursor(80,115);
  tftSprite.print(ticker);
*/

/*
  //Total ticks
  tftSprite.setTextSize(2);
  tftSprite.setCursor(130,115);
  tftSprite.print(totaltick);
*/

/*  
  tftSprite.setCursor(180,115); 
  tftSprite.setTextSize(2);
  tftSprite.print(millis()/1000);
*/

  M5.update();
  drawBat();
  drawLines();

//Print hoofdgetal (speed, ODO, ...)
  if (millis()-timer>1000){
    prefs.putFloat("odo",odo);
    prefs.putFloat("trip",trip);

    switch (displayseq) {
      
      //SPEED is main display
      case 0:            
        //Type of main display
        tftSprite.setCursor(10,10); 
        tftSprite.setTextSize(3);
        tftSprite.setTextColor(BLUE);        
        tftSprite.print("KM/H");

        //SECONDARY display numbers
        tftSprite.setTextColor(BLACK);        
        tftSprite.setCursor(10,105);
        tftSprite.print(odo);
        tftSprite.setCursor(160,105);
        tftSprite.print(trip);

        //MAIN display number
        tftSprite.setCursor(40,50); 
        tftSprite.setTextSize(5);        
        tftSprite.print(speed);

        break;

      //ODO is main display
      case 1:
        //Type of main display
        tftSprite.setCursor(10,10); 
        tftSprite.setTextSize(3);
        tftSprite.setTextColor(BLUE);        
        tftSprite.print("ODO ");

        //SECONDARY display numbers
        tftSprite.setTextColor(BLACK);        
        tftSprite.setCursor(10,105);
        tftSprite.print(speed);
        tftSprite.setCursor(160,105);
        tftSprite.print(trip);

        //MAIN display number
        tftSprite.setCursor(40,50); 
        tftSprite.setTextSize(5);        
        tftSprite.print(odo);

        break;

      //TRIP is main display
      case 2:
        //Type of main display
        tftSprite.setCursor(10,10); 
        tftSprite.setTextSize(3);
        tftSprite.setTextColor(BLUE);        
        tftSprite.print("TRIP");

        //SECONDARY display numbers
        tftSprite.setTextColor(BLACK);        
        tftSprite.setCursor(10,105);
        tftSprite.print(speed);
        tftSprite.setCursor(160,105);
        tftSprite.print(odo);

        //MAIN display number
        tftSprite.setCursor(40,50); 
        tftSprite.setTextSize(5);        
        tftSprite.print(trip);
        
        break;
    }

    tftSprite.pushSprite(0, 0);
    timer = millis();
  }  

  //Timer to disable movement indicator
  if (millis()-intimer>2500) {
    gotinput=0;
    speed = 0;
    revol=0;
  }
  
  if(millis()-lighttimer>2500){
    M5.Axp.ScreenBreath(7);
    lightlit = 0;
  }
  
  if (millis()-offtimer>300000) {
    M5.Axp.PowerOff();    
  }
}
