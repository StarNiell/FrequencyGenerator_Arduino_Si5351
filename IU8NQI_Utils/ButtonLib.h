#ifndef ButtonLib_h
#define ButtonLib_h

extern const int SHORT_PRESS = 1;
extern const int LONG_PRESS = 2;

class ButtonLib {
  private:
  int ReadButton();
  int _lastValue;
  int _pin;
  int _lpTime;
  unsigned long _timer;
  bool _buttPressed;

  public:
  ButtonLib();
  ButtonLib(int pin);
  int Value();
};

ButtonLib::ButtonLib(int pin)
{
  _pin = pin;
  _buttPressed = false;
  _lastValue = HIGH;
  _timer = 0;
  _lpTime = 500;
  pinMode(_pin, INPUT_PULLUP);
}

int ButtonLib::Value()
{
  return ReadButton();
}


int ButtonLib::ReadButton()
{
  int valore = 0;
  int n = digitalRead(_pin);
  if (_lastValue != n || _buttPressed)
  {
    if (n == LOW)
    {
      if (!_buttPressed)
      {
        _buttPressed = true;
        _timer = millis();
      }
      else
      {
        if (millis() >= (_timer + _lpTime))
        {
          valore = LONG_PRESS;
          _buttPressed = false;
        }
      }
      
    }
    else
    {
      if (_buttPressed)
      {
        _buttPressed = false;
        if (millis() >= (_timer + _lpTime))
        {
          valore = LONG_PRESS;
        }
        else
          valore = SHORT_PRESS;
      }
    }
  }
  _lastValue = n;
  return valore;
}

#endif