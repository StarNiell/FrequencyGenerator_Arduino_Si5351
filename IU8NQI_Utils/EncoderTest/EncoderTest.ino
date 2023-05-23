#include "EncoderLib.h"

EncoderLib Enc1 = EncoderLib(A0, A1);


void setup() {
  Serial.begin(9600);

}

void loop() {
  int evalue = Enc1.Value();
  if (evalue != 0)
  {
    if (evalue == UP)
      Serial.println("UP");
    else if (evalue == DOWN)
      Serial.println("DOWN");
  }

}
