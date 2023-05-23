#include "si5351.h"
#include <EEPROM.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_74HC595.h>
#include <Wire.h>
#include <IU8NQI_Utils.h>

 #define  IF         0       //Enter your IF frequency, ex: 455 = 455kHz, 10700 = 10.7MHz,  0 = to direct convert receiver or RF generator, + will add and - will subtract IF  offfset.
 #define  XT_CAL_F   84000     //Si5351 calibration factor, adjust to get exatcly 10MHz. Increasing  this value will decreases the frequency and vice versa.

 Si5351 si5351;

 long  interfreq = IF;
 long cal = XT_CAL_F;
 
 
//LiquidCrystal Standard****************************************
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//LiquidCrystal_74HC595 ****************************************
//shcp, stcp, ds: Arduino Pins
//rw, en, d4, d5, d6, d7: 74HC595 Pins
const int shcp = 13, stcp = 12, ds = 11, rw = 1, en = 3, d4 = 4,  d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal_74HC595 lcd(ds, shcp, stcp, rw, en, d4, d5, d6, d7);


const int W_CLK_PIN = 6;
const int FQ_UD_PIN = 7;
const int DATA_PIN = 8;
const int RESET_PIN = 9;

// Encoder 1 *************************************************
EncoderLib Enc1 = EncoderLib(A0, A1);

// Backlight LCD *********************************************
int backLight_Pin = 10;
int LumValue = -1;
int PwrValue = 0;
String sLum = "Off";

// Button 1 **************************************************
ButtonLib Btn1 = ButtonLib(A2);

unsigned long lastCursorShow = 0;
bool cursorShowed = false;

double frqstep = 0;
double freqstepMax = 100000000;

double dFreq = 0;
double freq = 0;
double freqStart = 5000000;
//const double trimFreq = 125007000;
double freqMin = 10000;
double freqMax = 125000000;
int phase = 0;
int FreqStateValue = 0;

const int MenuMax = 1;
const int SubMenuMax = 2;
const int LumMax = 3;
const int PwrMax = 4;
const int FreqStateMax = 1;
int buttonState = 0;
int currentMenu = 0;
int currentSubMenu = 1;
int currentLum = 3;
int currentFreqState = 0;
int si5351State = 0;
int currentPwr = 0;

// IDX Data[]
const int idx_Freq = 0;
const int idx_FreqStep = 1;
const int idx_Menu = 2;
const int idx_Lum = 3;
const int idx_Sub = 4;
const int idx_FreqState = 5;
const int idx_Pwr = 6;

bool PrintSerialEnabled = true;

double data[] {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};
byte noElem = 7;
unsigned int baseAddr = 0;
unsigned int n = 0;
unsigned long lastTimer;

void setup(){

  if (PrintSerialEnabled)
  {
    
    Serial.begin (9600);
    Serial.println("Start...");
    Serial.println("");
    Serial.println(">Setup<");
    Serial.println("");
    Serial.print("default freqStart: ");
    Serial.println(freqStart);
    Serial.println("");
    Serial.println("Read stored data...");
  }
 
  lcdSetup();
  delay(1000);
  printFixedParts();

  readStoredData();

  if (PrintSerialEnabled)
  {
    for (int i=0; i <= noElem-1; i++){
      Serial.print("data[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(data[i]);
    }
  }

  if (data[idx_Freq] >= freqMin && data[idx_Freq] <= freqMax)
    freqStart = data[idx_Freq];

  if (data[idx_FreqStep] >= 1 && data[idx_FreqStep] <= freqstepMax)
    frqstep = data[idx_FreqStep];  
  else
    frqstep = 100000;
  
  if (data[idx_Menu] >= 0 && data[idx_Menu] <= MenuMax)
    currentMenu = data[idx_Menu];  
  else
    currentMenu = 0;

  if (data[idx_Sub] >= 0 && data[idx_Sub] <= SubMenuMax)
    currentSubMenu = data[idx_Sub];  
  else
    currentSubMenu = 0;

  if (data[idx_Lum] >= 0 && data[idx_Lum] <= LumMax)
    currentLum = data[idx_Lum];  
  else
    currentLum = LumMax;

  if (data[idx_FreqState] >= 0 && data[idx_FreqState] <= 1)
    currentFreqState = data[idx_FreqState];  
  else
    currentFreqState = 0;

  if (data[idx_Pwr] >= 1 && data[idx_Pwr] <= PwrMax)
    currentPwr = data[idx_Pwr];  
  else
    currentPwr = PwrMax;

  if (PrintSerialEnabled)
  {
    Serial.print("freqStart: ");
    Serial.println(freqStart);
    Serial.print("frqstep: ");
    Serial.println(frqstep);
  }

  dFreq = freqStart;

  //si5351 initialization
  si5351Setup();

}

void si5351Setup()
{
  si5351.init(SI5351_CRYSTAL_LOAD_8PF,  0, 0);
  si5351.set_correction(cal, SI5351_PLL_INPUT_XO);
  si5351.drive_strength(SI5351_CLK0,  SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 0); 
  si5351.output_enable(SI5351_CLK1, 0);
  si5351.output_enable(SI5351_CLK2,  0);
}

void lcdSetup()
{
  Wire.setClock(10000);
  analogWrite(backLight_Pin, 100);
  lcd.begin(20, 2);
  lcdPrintClear(0, 0, "Si5351 10Khz-125Mhz ");
  lcdPrint(0, 1, "   DDS by IU8NQI    ");
  delay(3000);
  //lcd.clear();
}

void loop(){

 // Lettura Encoder1 ***************************************
  ManageEncoder1();
  // Lettura Button1 ***************************************
  ManageButton1();

  if (dFreq > freqMax)
    dFreq = freqMax;

  if (dFreq < freqMin)
    dFreq = freqMin;

  if ((dFreq != freq)
      || (FreqStateValue != currentFreqState)
  )
  {
    SetFrequencyDDS();
    String sFreq = formatNumber(freq);
    sFreq = RightAlign(sFreq, 11);
    lcdPrint(4, 0, sFreq);
    if (currentFreqState == 0)
      lcdPrint(0, 1, "OFF");
    else
      lcdPrint(0, 1, "ON ");
    setCursor();
  }

  if (LumValue != currentLum)
  {
    SetLum();
    lcdPrint(12, 1, sLum);
    setCursor();
  }

  if (PwrValue != currentPwr)
  {
    SetPwr();
    lcdPrint(18, 1, formatNumber(PwrValue));
    setCursor();
  }

  if (millis() > (lastTimer + 5000))
  {
    if (freq != data[idx_Freq] 
        || frqstep != data[idx_FreqStep] 
        || currentMenu != data[idx_Menu] 
        || currentLum != data[idx_Lum]
        || currentSubMenu != data[idx_Sub]
        || currentFreqState != data[idx_FreqState]
      )
    {
      if (PrintSerialEnabled)
      {
        Serial.println("Storing data...");
      }
      data[idx_Freq] = freq;
      data[idx_FreqStep] = frqstep;
      data[idx_Menu] = currentMenu;
      data[idx_Lum] = currentLum;
      data[idx_Sub] = currentSubMenu;
      Serial.print("Submenu: ");
      Serial.println(data[idx_Sub]);
      data[idx_FreqState] = currentFreqState;

      StoreData();
    }
    lastTimer = millis();

  }

  if (cursorShowed && (millis() > (lastCursorShow + 5000)))
  {
    lcd.noCursor();
    Serial.println("Hide cursor");
    cursorShowed = false;
  }
}

void SetLum()
{
  int realLum = 0;
  sLum = "Off";
  switch (currentLum)
  {
    case 1:
      realLum = 5;
      sLum = "Low";
      break;
    case 2:
      realLum = 100;
      sLum = "Mid";
      break;
    case 3:
      realLum = 254;
      sLum = "Hig";
      break;
  }
  analogWrite(backLight_Pin, realLum);
  LumValue = currentLum;
}

void SetPwr()
{
  switch(currentPwr)
  {
    case 1:
      si5351.drive_strength(SI5351_CLK0,  SI5351_DRIVE_2MA);
      break;
    case 2:
      si5351.drive_strength(SI5351_CLK0,  SI5351_DRIVE_4MA);
      break;
    case 3:
      si5351.drive_strength(SI5351_CLK0,  SI5351_DRIVE_6MA);
      break;
    default:
      si5351.drive_strength(SI5351_CLK0,  SI5351_DRIVE_8MA);
      currentPwr = 4;
      break;
  }
  PwrValue = currentPwr;
}

void SetFrequencyDDS()
{
  freq = dFreq;
  if (PrintSerialEnabled)
  {
    Serial.print("Set freq: ");
    Serial.println(freq);
  }
  SetFreqState();
  SetFrequency();
  FreqStateValue = currentFreqState;
}

 void SetFrequency() {
  if (currentFreqState == 1)   
  {
    si5351.set_freq((freq +  (interfreq * 1000ULL)) * 100ULL, SI5351_CLK0);
  }
}

void SetFreqState() 
{
  if (si5351State != currentFreqState)
  {
    si5351.output_enable(SI5351_CLK0, currentFreqState);
    si5351State = currentFreqState;
  }
}

String formatNumber(double num)
{
  String ret = "";

  String sNum = String(num);
  int dot = sNum.indexOf(".");
  sNum = sNum.substring(0,dot);
  int mil = 0;
  for (int x = sNum.length(); x>=1; x--)
  {
    mil++;
    ret = sNum.substring(x-1, x) + ret;
    if (mil == 3 && x > 1)
    {
      ret = "." + ret;
      mil = 0;
    }
  }
  return ret;
}

void lcdPrintClear(int p1, int p2, String text)
{
  lcd.clear();
  lcdPrint(p1, p2, text);
}

void lcdPrint(int p1, int p2, String text)
{
  lcd.setCursor(p1, p2);
  lcd.print(text);
}


String RightAlign(String str, int lenStr)
{
  String tmp = SpaceStr(lenStr) + str;
  return tmp.substring((tmp.length() - lenStr), tmp.length());
}

String SpaceStr(int num){
    String str;
    for(int i = 0; i < num; i++) {
        str += ' ';  
    }
    return str;
}

void printFixedParts() {
  lcdPrintClear(0, 0, "Hz ");
  lcdPrint(17, 0, "Pwr");
  lcdPrint(8, 1, "Lum ");
}

void SetButtonState()
{
  if (cursorShowed)
  {
    switch (currentMenu)
    {
      case 0: //Menu Frequency
        frqstep = frqstep * 10;
        if (frqstep > freqstepMax)
          frqstep = 1;
        break;
      
      case 1: //Other Function
        break;

    }
  }
  setCursor();
}

void setCursor()
{
  lcdPrint(3, 0, " ");
  lcdPrint(3, 1, " ");
  lcdPrint(11, 1, " ");
  lcdPrint(16, 1, " ");

  switch (currentMenu)
  {
    case 0:
      lcdPrint(3, 0, "~");
      if (frqstep == 1)
        lcd.setCursor(14, 0);
      if (frqstep == 10)
        lcd.setCursor(13, 0);
      if (frqstep == 100)
        lcd.setCursor(12, 0);
      if (frqstep == 1000)
        lcd.setCursor(10, 0);
      if (frqstep == 10000)
        lcd.setCursor(9, 0);
      if (frqstep == 100000)
        lcd.setCursor(8, 0);
      if (frqstep == 1000000)
        lcd.setCursor(6, 0);
      if (frqstep == 10000000)
        lcd.setCursor(5, 0);
      if (frqstep == 100000000)
        lcd.setCursor(4, 0);        
      break;

    case 1:
      switch (currentSubMenu)
      {
        case 0: //Freq ON/OFF
          lcdPrint(3, 1, "");
          lcd.setCursor(0, 1);
          break;

        case 1: // Luminosità
          lcdPrint(11, 1, "~");
          lcd.setCursor(14, 1);
          break;

        case 2: // Pwr
          lcdPrint(16, 1, "~");
          lcd.setCursor(18, 1);
          break;

      }
      break;
  }


  lcd.cursor();
  lastCursorShow = millis();
  cursorShowed = true;

  if (PrintSerialEnabled)
  {
    Serial.println("Show cursor");
  }
}

// Lettura Encoder1 ******************************************************************************************
void ManageEncoder1()
{

  int valore_Encoder1 = Enc1.Value();
  if (valore_Encoder1 != 0)
  {
    if (PrintSerialEnabled)
    {
      Serial.print("Encoder1: ");
      Serial.println(valore_Encoder1);
    }

    switch (currentMenu)
    {
      case 0: // Menu Frequency
        dFreq = freq;
        if (valore_Encoder1 == UP)
          dFreq = dFreq + frqstep;
        else
          dFreq = dFreq - frqstep;
        break;
      
      case 1: // Other
        switch (currentSubMenu)
        {
          case 0: //Freq ON/OFF
            if (valore_Encoder1 == UP)
            {
              currentFreqState ++;
              if (currentFreqState > FreqStateMax)
                currentFreqState = FreqStateMax;
            }
            else
            {
              currentFreqState --;
              if (currentFreqState < 0)
                currentFreqState = 0;
            }
            break;
          
          case 1: //Luminosità
            if (valore_Encoder1 == UP)
            {
              currentLum ++;
              if (currentLum > LumMax)
                currentLum = LumMax;
            }
            else
            {
              currentLum --;
              if (currentLum < 0)
                currentLum = 0;
            }
            break;
          case 2: //Pwr
            if (valore_Encoder1 == UP)
            {
              currentPwr ++;
              if (currentPwr > PwrMax)
                currentPwr = PwrMax;
            }
            else
            {
              currentPwr --;
              if (currentPwr < 1)
                currentPwr = 1;
            }
            break;
        }
        break;
    }
  }
}

// Lettura Button1 ******************************************************************************************
void ManageButton1()
{
  int valore_Button1 = Btn1.Value();
  if (valore_Button1 != 0)
  {
    if (valore_Button1 == LONG_PRESS)
    {
      currentMenu++;
      if (currentMenu > MenuMax)
        currentMenu = 0;
      cursorShowed = false;
    }
    else {
      currentSubMenu ++;
      if (currentSubMenu > SubMenuMax)
        currentSubMenu = 0;
    }

    SetButtonState();

    if (PrintSerialEnabled)
    {
      Serial.print("Button1: ");
      Serial.print(valore_Button1);
      Serial.print(" - ");
      Serial.print("Menu: ");
      Serial.print(currentMenu);
      Serial.print(" - ");
      Serial.println(frqstep);
    }
  }
}

// EEPROM FUNCTIONS ******************************************************************************************

void StoreData()
{
  for (int i=0; i <= noElem-1; i++){
    n = EEPROM_writeAnything( (i*4)+baseAddr, data[i]);
  }
}

void readStoredData()
{
  //read data back
  for (int i=0; i <= noElem-1; i++){
    double val;
    int addr = (i*4)+baseAddr;
    n = EEPROM_readAnything( addr, val);
    data[i] = val;
  }
}


template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
}
