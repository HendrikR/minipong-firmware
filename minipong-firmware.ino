#if defined(ARDUINO_AVR_PRO)

const int col1 = 9 ; //column 1 is on pin 1...this is 1 in the data sheet
const int col2 = 7 ; //column 2 is on pin 3
const int col3 = 12; //column 3 is on pin 10
const int col4 = 15 ; //column 4 is on pin 7
const int col5 = 14; //column 5 is on pin 8
const int row1 = 10 ; //row 1 is on pin 12
const int row2 = 11 ; //row 2 is on pin 11
const int row3 = 8 ; //row 3 is on pin 2
const int row4 = 13 ; //row 4 is on pin 9
const int row5 = 6 ; //row 5 is on pin 4
const int row6 = 5 ; //row 6 is on pin 5
const int row7 = 4 ; //row 7 is on pin 6

#elif defined(ARDUINO_AVR_PROMICRO)

 //for simplicity the pins are given variable names so that it is easier to understand which light should turn on.
const int col1 = 9 ; //column 1 is on pin 1...this is 1 in the data sheet
const int col2 = 7 ; //column 2 is on pin 3
const int col3 = 14; //column 3 is on pin 10
const int col4 = 19 ; //column 4 is on pin 7
const int col5 = 18; //column 5 is on pin 8
const int row1 = 10 ; //row 1 is on pin 12
const int row2 = 16 ; //row 1 is on pin 11
const int row3 = 8 ; //row 1 is on pin 2
const int row4 = 15 ; //row 1 is on pin 9
const int row5 = 6 ; //row 1 is on pin 4
const int row6 = 5 ; //row 1 is on pin 5
const int row7 = 4 ; //row 1 is on pin 6

#else

#error please define pins for board

#endif

const int rows = 7; //number of rows
const int cols = 5; //number of columns

const int button = 3;
boolean lastButton = LOW;    //Last Button State
boolean currentButton = LOW; //Current Button State
int mode = 6;
int modes = 7; //number of led modes
int nc = 1; //number column
int nr = 1; //number row
int dr = 1; //direction column (move forward or backward
int dc = 1; //direction row (move forward or backward)
int pongball = 1;
int p1=0;
int p2=0;
unsigned long t2;
int t3;

void setup() {
  pinMode (button, INPUT_PULLUP);  //Set button as input (not required)
  for (int i = 4; i < 21; i++) {
    pinMode (i, OUTPUT); //Set all pins to output
  }
  pinMode (20, HIGH);
  t2 = 0;
  t3 = 1;
  Serial.begin(9600);
}


void loop() {
  currentButton = debounce(lastButton);           //read debounced state
  if (lastButton == LOW && currentButton == HIGH) //if it was pressed...
  {
    mode++;                                    //increment the LED value
  }
  lastButton = currentButton;
  //if you’ve cycled through the different options, reset the counter to 0
  if (mode == modes + 1) mode = 1;
  //  setMode(mode);                              //change the LED state

  //when clock reaches wakeup time call random pixle mode
  if (millis() > t2) {
    if (mode == 1) {
      stripeScrollMode();
    }
    if (mode == 2) {
      rowScrollMode();
    }
    if (mode == 3) {
      columnScrollMode();
    }
    if (mode == 4) {
      randomPixelMode ();
    }
    if (mode == 5) {
      randomPladMode();
    }
    if (mode == 6) {
      bounce90Mode();
    }
    if (mode == 7) {
      if (nr == 1 || nr == rows ) {
        pongball = random(1, 5);
      }
      if (nr==1){p1=random(-1, 1);}
      if (nr==cols){p2=random(-1, 1);}
      if (pongball == 1) {
        columnBounceMode();
      }
      else {bounce90Mode();}
    }

    //read in time
    //calculate time to wakeup
    t2 = millis() + 200;

    Serial.print(nr);
    Serial.print(' ');
    Serial.print(nc);
    Serial.print(' ');
    Serial.print(pongball);
    Serial.print(' ');
    Serial.println(mode);
  }
  if (mode == 7) {
    pongMode();
  }
}

boolean debounce(boolean last)
{
  boolean current = digitalRead(button);       //Read the button state
  if (last != current)                         //if it's different...
  {
    delay(5);                                  //wait 5ms
    current = digitalRead(button);             //read it again
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
  else if (nc + p1 >= cols) {
    setMode(1, cols);
    delay(1);
    offMode();
    setMode(1, cols-1);
    delay(1);
    offMode();
    setMode(1, cols-2);
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
    setMode(rows, 1);
    delay(1);
    offMode();
    setMode(rows,2);
    delay(1);
    offMode();
    setMode(rows,3);
    delay(1);
    }
  else if (nc + p2 >= cols) {
    setMode(rows, cols);
    delay(1);
    offMode();
    setMode(rows, cols-1);
    delay(1);
    offMode();
    setMode(rows, cols-2);
    delay(1);
  }
  else {
    setMode(rows, nc+p2-1);
    delay(1);
    offMode();
    setMode(rows, nc+p2);
    delay(1);
    offMode();
    setMode(rows, nc+p2+1);
    delay(1);
  }
}

void columnBounceMode() {
  if (nr <= 1) {
    dr = 1;
  }
  if (nr >= rows) {
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
  if (nc >= cols) {
    dc = -1;
  }
  if (nr <= 1) {
    dr = 1;
  }
  if (nr >= rows) {
    dr = -1;
  }
  nc = nc + dc;
  nr = nr + dr;
  offMode();
  setMode(nr, nc);
}

void randomPladMode() {
  digitalWrite(random(4, 15), HIGH);
  digitalWrite(random(4, 15), LOW);
}


void randomPixelMode () {
  int nr = random(1, rows + 1);
  int nc = random (1, cols + 1);
  offMode();
  setMode(nr, nc);
}

void stripeScrollMode () {
  nc = nc + 1;
  nr = nr + 1;
  if (nc > cols ) {
    nc = 1;
  }
  if (nr > rows ) {
    nr = 1;
  }
  offMode();
  setMode(nr, nc);
}

void rowScrollMode() {
  nc = nc + 1;
  if (nc > cols) {
    nc = 1;
    nr = nr + 1;
  }
  if (nr > rows ) {
    nr = 1;
  }
  offMode();
  setMode(nr, nc);
}

void columnScrollMode() {
  nr = nr + 1;
  if (nr > rows ) {
    nr = 1;
    nc = nc + 1;
  }
  if (nc > cols) {
    nc = 1;
  }
  offMode();
  setMode(nr, nc);
}

void offMode()
{
  digitalWrite(col1, LOW);
  digitalWrite(col2, LOW);
  digitalWrite(col3, LOW);
  digitalWrite(col4, LOW);
  digitalWrite(col5, LOW);
  digitalWrite(row1, HIGH);
  digitalWrite(row2, HIGH);
  digitalWrite(row3, HIGH);
  digitalWrite(row4, HIGH);
  digitalWrite(row5, HIGH);
  digitalWrite(row6, HIGH);
  digitalWrite(row7, HIGH);
}

void setMode(int nr, int nc)
{
  if (nc == 1)
  {
    digitalWrite(col1, HIGH);
  }
  if (nc == 2)
  {
    digitalWrite(col2, HIGH);
  }
  if (nc == 3)
  {
    digitalWrite(col3, HIGH);
  }
  if (nc == 4)
  {
    digitalWrite(col4, HIGH);
  }
  if (nc == 5)
  {
    digitalWrite(col5, HIGH);
  }


  if ( nr == 1)
  {
    digitalWrite(row1, LOW);
  }
  if ( nr == 2)
  {
    digitalWrite(row2, LOW);
  }
  if ( nr == 3)
  {
    digitalWrite(row3, LOW);
  }
  if ( nr == 4)
  {
    digitalWrite(row4, LOW);
  }
  if ( nr == 5)
  {
    digitalWrite(row5, LOW);
  }
  if ( nr == 6)
  {
    digitalWrite(row6, LOW);
  }
  if ( nr == 7)
  {
    digitalWrite(row7, LOW);
  }
}
