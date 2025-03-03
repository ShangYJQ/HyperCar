#include <ps5Controller.h>

void handleAction(int fb, int lr) {
  // String act = (String)fb + (String)lr;
  char act[7];
  sprintf(act,"%03d%03d\n",fb,lr);
  Serial.print(act);
}

int to_bf(int push) {
  return 50 + int((double)push / 5.1);
}

int to_lr(int turn) {
  return 50 + int(double(turn) / 2.55);
}


void setup() {
  Serial.begin(115200);
  ps5.begin("58:10:31:D1:4C:03");  //replace with MAC address of your controller
  Serial.println("Ready.");
}

void loop() {
  while (ps5.isConnected() == true) {

    int bf = to_bf(ps5.R2Value());
    int lr = to_lr(ps5.LStickX());

    handleAction(bf, lr);
    delay(15);
  }
}
