#include <Arduino.h>
#include "font-5x7.hpp"
//#include <HardwareSerial.h>


const int DELAY_TIME = 200; // milliseconds to wait between redraws
constexpr int ROWS = 7; //number of rows
constexpr int COLS = 5; //number of columns

// Define the arduino pins for each column and row.
// This depends on the model used.
#if defined(ARDUINO_AVR_PRO)
const int col_pin[] = {9, 7, 12, 15, 14};
const int row_pin[] = {10, 11, 8, 13, 6, 5, 4};
const int button_pin = 3;

#elif defined(ARDUINO_AVR_PROMICRO)

const int col_pin[] = {9, 7, 14, 19, 18};
const int row_pin[] = {10, 16, 8, 15, 6, 5, 4};
const int button_pin = 3;

#else

#error please define pins for board

#endif

// represents the LED matrix state in memory.
bool matrix[ROWS*COLS];

boolean lastButton = LOW;    //Last Button State
boolean currentButton = LOW; //Current Button State

enum Mode {
    STRIPE_SCROLL,
    ROW_SCROLL,
    COL_SCROLL,
    RND_PIXEL,
    RND_PLAD,
    BOUNCE90,
    PONG,
    count
};

Mode mode = STRIPE_SCROLL;
int nc = 1; //number column
int nr = 1; //number row
int dr = 1; //direction column (move forward or backward
int dc = 1; //direction row (move forward or backward)
int pongball = 1;
int p1=0;
int p2=0;
unsigned long t2;
int t3;

void offMode()
{
  for (int i=0; i<COLS; i++)
    digitalWrite(col_pin[i], LOW);
  for (int i=0; i<ROWS; i++)
    digitalWrite(row_pin[i], HIGH);
}

void setMode(int nr, int nc)
{
  digitalWrite(col_pin[nc-1], HIGH);
  digitalWrite(row_pin[nr-1], LOW);
}

void offMatrix()
{
  for (int c=0; c<COLS; c++) {
    for (int r=0; r<ROWS; r++) {
      matrix[c*ROWS + r] = false;
    }
  }
}

void setMatrix(int nr, int nc)
{
  matrix[nc*ROWS + nr] = true;
}

void unsetMatrix(int nr, int nc)
{
  matrix[nc*ROWS + nr] = false;
}

void drawMatrix() {
  for (int c=0; c<COLS; c++) {
    digitalWrite(col_pin[c], HIGH);
    for (int r=0; r<ROWS; r++)
      digitalWrite(row_pin[r], matrix[c*ROWS+r] ? LOW : HIGH);
    delay(5); // with no delay, pixels "bleed over"

    // switch it off again
    digitalWrite(col_pin[c], LOW);
    for (int r=0; r<ROWS; r++)
      digitalWrite(row_pin[r], matrix[c*ROWS+r] ? LOW : HIGH);
  }
}

boolean debounce(boolean last)
{
  boolean current = digitalRead(button_pin);   //Read the button state
  if (last != current)                         //if it's different...
  {
    delay(5);                                  //wait 5ms
    current = digitalRead(button_pin);         //read it again
  }
  return current;                              //return the current value
}

void pongMode() {
  offMode();
  setMode(nr, nc);//ball determined by bounce90Mode
  delay(1);

  //paddle1
  offMode();
  if (nc + p1 <= 1){
    setMode(1, 1);
    delay(1);
    offMode();
    setMode(1,2);
    delay(1);
    offMode();
    setMode(1,3);
    delay(1);
    }
  else if (nc + p1 >= COLS) {
    setMode(1, COLS);
    delay(1);
    offMode();
    setMode(1, COLS-1);
    delay(1);
    offMode();
    setMode(1, COLS-2);
    delay(1);
  }
  else {
    setMode(1, nc+p1-1);
    delay(1);
    offMode();
    setMode(1, nc+p1);
    delay(1);
    offMode();
    setMode(1, nc+p1+1);
    delay(1);
  }
 
  //paddle2
  offMode();
  if (nc + p2 <= 1){
    setMode(ROWS, 1);
    delay(1);
    offMode();
    setMode(ROWS,2);
    delay(1);
    offMode();
    setMode(ROWS,3);
    delay(1);
    }
  else if (nc + p2 >= COLS) {
    setMode(ROWS, COLS);
    delay(1);
    offMode();
    setMode(ROWS, COLS-1);
    delay(1);
    offMode();
    setMode(ROWS, COLS-2);
    delay(1);
  }
  else {
    setMode(ROWS, nc+p2-1);
    delay(1);
    offMode();
    setMode(ROWS, nc+p2);
    delay(1);
    offMode();
    setMode(ROWS, nc+p2+1);
    delay(1);
  }
}

void columnBounceMode() {
  if (nr <= 1) {
    dr = 1;
  }
  if (nr >= ROWS) {
    dr = -1;
  }
  nr = nr + dr;
  offMode();
  setMode(nr, nc);
}

void bounce90Mode() {
  if (nc <= 1) {
    dc = 1;
  }
  if (nc >= COLS) {
    dc = -1;
  }
  if (nr <= 1) {
    dr = 1;
  }
  if (nr >= ROWS) {
    dr = -1;
  }
  nc = nc + dc;
  nr = nr + dr;
  offMode();
  setMode(nr, nc);
}

void randomPladMode() {
#if defined(ARDUINO_AVR_PRO)
  digitalWrite(random(4, 15), HIGH);
  digitalWrite(random(4, 15), LOW);
#elif defined(ARDUINO_AVR_PROMICRO)
  digitalWrite(random(4, 19), HIGH);
  digitalWrite(random(4, 19), LOW);
#else
  #error undefined board variant
#endif
}


void randomPixelMode () {
  int nr = random(1, ROWS + 1);
  int nc = random (1, COLS + 1);
  offMode();
  setMode(nr, nc);
}

void stripeScrollMode () {
  nc = nc + 1;
  nr = nr + 1;
  if (nc > COLS ) {
    nc = 1;
  }
  if (nr > ROWS ) {
    nr = 1;
  }
  offMode();
  setMode(nr, nc);
}

void rowScrollMode() {
  nc = nc + 1;
  if (nc > COLS) {
    nc = 1;
    nr = nr + 1;
  }
  if (nr > ROWS ) {
    nr = 1;
  }
  offMode();
  setMode(nr, nc);
}

void columnScrollMode() {
  nr = nr + 1;
  if (nr > ROWS ) {
    nr = 1;
    nc = nc + 1;
  }
  if (nc > COLS) {
    nc = 1;
  }
  offMode();
  setMode(nr, nc);
}

void setup() {
  pinMode (button_pin, INPUT_PULLUP);  //Set button as input (not required)
  for (int i = 4; i < 21; i++) {
    pinMode (i, OUTPUT); //Set all pins to output
  }
  pinMode (20, HIGH);
  t2 = 0;
  t3 = 1;
  //Serial.begin(9600);
}

#define min(a,b) ( (a) < (b) ? (a) : (b) )
#define max(a,b) ( (a) > (b) ? (a) : (b) )

void drawLetter(char c, int shift=0) {
  c -= ' ';
  //offMatrix();
  for(int col = 0; col<COLS; col++){
    if (col-shift < 0 || col-shift >= COLS) continue;
    unsigned char fcol = font_5x7_data[5*c + (4-col+shift)];
    for(int row=0; row<ROWS; row++) {
      if (fcol & (1<<(6-row))) setMatrix(row, col);
      else unsetMatrix(row, col);
    }
  }
}

const unsigned LETTER_DELAY = 400;

void drawString(const char* str) {
  for (int i=0; i<strlen(str); i++) {
    int timer = millis();
    drawLetter(str[i], 0);
    while(millis() - timer < LETTER_DELAY) drawMatrix();
  }
  offMatrix();
  delay(2*LETTER_DELAY);
}

void scrollString(const char* str) {
  char lastLetter = ' ';
  for (int i=0; i<strlen(str); i++) {
    for(int shift=0; shift<(COLS+0); shift++) {
      // iterating to COLS+1, so there is a one-column gap between the letters
      drawLetter(lastLetter, shift);
      drawLetter(str[i], shift-(COLS+0));
      int timer = millis();
      while(millis() - timer < 100) drawMatrix();
    }
    lastLetter = str[i];
  }
  offMatrix();
  delay(2*LETTER_DELAY);
}

void loop() {
  // draw an "F"
  scrollString("1337 scrolltext");
}
void bloop() {
  currentButton = debounce(lastButton);           //read debounced state
  if (lastButton == LOW && currentButton == HIGH) //if it was pressed...
  {
    mode = (Mode)((int)mode + 1);                 //increment the LED value
  }
  lastButton = currentButton;
  //if you’ve cycled through the different options, reset the counter to 0
  if (mode == Mode::count + 1) mode = STRIPE_SCROLL;
  //  setMode(mode);                              //change the LED state

  //when clock reaches wakeup time call random pixle mode
  if (millis() > t2) {
    if (mode == STRIPE_SCROLL) {
      stripeScrollMode();
    }
    if (mode == ROW_SCROLL) {
      rowScrollMode();
    }
    if (mode == COL_SCROLL) {
      columnScrollMode();
    }
    if (mode == RND_PIXEL) {
      randomPixelMode ();
    }
    if (mode == RND_PLAD) {
      randomPladMode();
    }
    if (mode == BOUNCE90) {
      bounce90Mode();
    }
    if (mode == PONG) {
      if (nr == 1 || nr == ROWS ) {
        pongball = random(1, 5);
      }
      if (nr==1){p1=random(-1, 1);}
      if (nr==COLS){p2=random(-1, 1);}
      if (pongball == 1) {
        columnBounceMode();
      }
      else {bounce90Mode();}
    }

    //read in time
    //calculate time to wakeup
    t2 = millis() + DELAY_TIME;
/*
    Serial.print(nr);
    Serial.print(' ');
    Serial.print(nc);
    Serial.print(' ');
    Serial.print(pongball);
    Serial.print(' ');
    Serial.println(mode);
*/
  }
  if (mode == PONG) {
    pongMode();
  }
}
