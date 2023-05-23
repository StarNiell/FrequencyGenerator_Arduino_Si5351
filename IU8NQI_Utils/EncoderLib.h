#ifndef EncoderLib_h
#define EncoderLib_h

extern const int UP = 1;
extern const int DOWN = -1;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers

class EncoderLib {
  private:
  int ReadEncoder();
  int lastValue_Encoder;
  int _pinA;
  int _pinB;

  public:
  EncoderLib();
  EncoderLib(int pinA, int pinB);
  int Value();
};

EncoderLib::EncoderLib(int pinA, int pinB)
{
  _pinA = pinA;
  _pinB = pinB;
  pinMode(_pinA, INPUT_PULLUP);
  pinMode(_pinB, INPUT_PULLUP);
}

int EncoderLib::Value()
{
  return ReadEncoder();
}


int EncoderLib::ReadEncoder()
{
  int valore = 0;
  int n = digitalRead(_pinA);
  if ((lastValue_Encoder == HIGH) && (n == LOW) && ((millis() - lastDebounceTime) > debounceDelay))
  {
    if (digitalRead(_pinB))
      valore = DOWN;
    else
      valore = UP;
  
	lastDebounceTime = millis();
  }
  
  lastValue_Encoder = n;

  return valore;
}
#endif