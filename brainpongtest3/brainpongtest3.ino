// data on rows, clock on columns

// commands:
// set data: d01001000
// read status: r
// clock a column: c0
// test a column: t3
// test a row: T3

#define LED_PIN 13

// data on port D

//#define COLUMNS 16
//#define ROWS 11
//uint8_t clock_pins[COLUMNS] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
//uint8_t data_pins[ROWS] = {23, 25, 27, 29, 31, 33, 39, 41, 43, 45, 47};

//#define COLUMNS 8
//#define ROWS 6
//uint8_t clock_pins[COLUMNS] = {22, 24, 26, 28, 30, 32, 34, 36};
//uint8_t data_pins[ROWS] = {23, 25, 27, 29, 31, 33};

#define ALL_COLUMNS 16
#define ALL_ROWS 11
#define COLUMNS 16
#define ROWS 11
uint8_t clock_pins[ALL_COLUMNS] = {52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22};
uint8_t data_pins[ALL_ROWS] = {53, 51, 49, 47, 45, 43, 37, 35, 33, 31, 29};

#define BUFFERS 4
uint16_t buffers[BUFFERS][COLUMNS] = {
  { \
    0B111111,
    0B111111,
    0B000010,
    0B000010,
    0B000010,
    0B000010,
    0B000010,
    0B000010 \
  }, \
  { \
    0B111111,
    0B111111,
    0B111111,
    0B000100,
    0B000100,
    0B000100,
    0B111111,
    0B111111 \
  }, \
  { \
    0B000111,
    0B001111,
    0B011111,
    0B111101,
    0B111000,
    0B110000,
    0B110000,
    0B110000 \
  }, \
  { \
    0B111111,
    0B111111,
    0B111111,
    0B000111,
    0B001101,
    0B011000,
    0B110000,
    0B100000,
  } \
};

void draw(short buf) {
  for (int col = 0; col < COLUMNS; col++) {
    for (int row = 0; row < 9; row++) {
      digitalWrite(data_pins[row], buffers[buf][col] & 1<<row);
    }
    clock(col);
  }
}

void clock(int column) {
  digitalWrite(clock_pins[column], HIGH);
  delayMicroseconds(20);
  digitalWrite(clock_pins[column], LOW);
  delayMicroseconds(50);
}

void zero() {
  for (int row = 0; row < ALL_ROWS; row++) {
    digitalWrite(data_pins[row], LOW);
  }
  
  for (int col = 0; col < ALL_COLUMNS; col++) {
    clock(col);
  }
}

void testall() {
  for (int col = 0; col < ALL_COLUMNS; col++) {
    for (int row = 0; row < ALL_ROWS; row++) {
      Serial.print("column ");
      Serial.print(col);
      Serial.print(" row ");
      Serial.println(row);

      blink(col, row);
    }
  }
}

void blink(int column, int row) {
  for (int i = 0; i < ALL_ROWS; i++) {
    if (i == row)
      digitalWrite(data_pins[i], HIGH);
    else
      digitalWrite(data_pins[i], LOW);
  }
    
  clock(column);
    
  delay(150);
    
  digitalWrite(data_pins[row], LOW);
  clock(column);  
}

void setup() {
  int i, j;
  
  Serial.begin(115200);
  
  Serial.println("starting up");
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
 
  for (i = 0; i < ALL_COLUMNS; i++) {
    pinMode(clock_pins[i], OUTPUT);
    digitalWrite(clock_pins[i], LOW);
  }

  for (j = 0; j < ALL_ROWS; j++) {
    pinMode(data_pins[j], OUTPUT);
    digitalWrite(data_pins[j], LOW);
  }
  
  Serial.println("zeroing");
  for (i = 0; i < ALL_COLUMNS; i++) {
    clock(i);
  }
  
  Serial.println("ready");
}

void loop() {
  String serialBuffer = String();
  short data[ALL_ROWS];
  int col, row, buf;
  char cmd;
  
  while (Serial.available()) {
    delay(10);
    
    char c = Serial.read();
    if (c == '\n' || c == '\r') break;
    serialBuffer += c;
  }
  
  // A command should be ready
  if (serialBuffer.length() == 0) {
    // no command
    return;
  }

//  Serial.print("received: ");
//  Serial.println(serialBuffer);
  
  cmd = serialBuffer.charAt(0);
//  Serial.print("command: ");
//  Serial.println(cmd);
  
  switch (cmd) {
    case 'h':
    case '?':
      Serial.print("we have ");
      Serial.print(COLUMNS);
      Serial.print(" columns ");
      Serial.print(" and ");
      Serial.print(ROWS);
      Serial.println(" rows");
      Serial.println("set data: d100101");
      Serial.println("clock a column: d5");
      Serial.println("test column: t5");
      Serial.println("test row: T3");
      Serial.println("read pins: r");
      Serial.println("test all: a");
      Serial.println("zero all: z");
      Serial.println("row tests: b");
      Serial.println("column tests: B");
      Serial.println("draw buffer 1: D1");
      Serial.println("cycle through all buffers (reset to stop): C");
      
     
      break;
      
    case 'z':
      Serial.println("zeroing all LEDs");
      zero();
      break;
    
    case 'a':
      Serial.println("testing all LEDs");
      testall();
      break;
      
    case 'd':
      Serial.println("asserting data lines");
      if (serialBuffer.length() != ALL_ROWS + 1) {
        Serial.println("you must specify on or off for all data lines");
        break;
      }
      
      for (int i = 0; i < ALL_ROWS; i++) {
        byte val = serialBuffer.charAt(i + 1);
        if (val != '0' && val != '1') {
          Serial.print("all values must be 0 or 1, error at row ");
          Serial.println(i);
          break;
        }
        
        if (val == '0') {
          digitalWrite(data_pins[i], LOW);
        } else {
          digitalWrite(data_pins[i], HIGH);
        }
      }
      break;
    case 'r':
      Serial.println("reading status");
      
      Serial.print("data lines: ");
      for (int i = 0; i < ALL_ROWS; i++) {
        Serial.print(digitalRead(data_pins[i]));
      }
      Serial.println();
      
      Serial.print("clock lines: ");
      for (int i = 0; i < ALL_COLUMNS; i++) {
        Serial.print(digitalRead(clock_pins[i]));
      }
      Serial.println();
      break;
    case 'c':
    case 't':
      char col_str[30];
      serialBuffer.substring(1).toCharArray(col_str, 30);
      col = atoi(col_str);
      
      if (col < 0 || col > ALL_COLUMNS-1) {
        Serial.print("column ");
        Serial.print(col);
        Serial.println(" out of bounds");
        break;
      }

      if (cmd == 'c') {      
        Serial.print("clocking column ");
        Serial.println(col);
        clock(col);
      } else {
        Serial.print("testing column ");
        Serial.println(col);
        
        for (int j = 0; j < ALL_ROWS; j++) {
          Serial.print("column ");
          Serial.print(col);
          Serial.print(" row ");
          Serial.println(j);
          blink(col, j);
        }
      }
      break;
    case 'D':
      char buf_str[30];
      serialBuffer.substring(1).toCharArray(buf_str, 30);
      buf = atoi(buf_str);
      
      if (buf < 0 || col > BUFFERS-1) {
        Serial.print(F("buffer "));
        Serial.print(buf);
        Serial.println(F(" out of bonds"));
        return;
      }
      
      Serial.print(F("drawing buffer "));
      Serial.println(buf);
      draw(buf);
      break;
    case 'C':
      for (;;) {
        for (int i = 0; i < BUFFERS; i++) {
          draw(i);
          delay(800);
        }
      }
      break;      
    case 'T':
      char row_str[30];
      serialBuffer.substring(1).toCharArray(row_str, 30);
      row = atoi(row_str);
      
      if (row < 0 || row > ALL_ROWS-1) {
        Serial.print("row ");
        Serial.print(row);
        Serial.println(" out of bounds");
        break;
      }
      
      for (int i = 0; i < ALL_COLUMNS; i++) {
        Serial.print("column ");
        Serial.print(i);
        Serial.print(" row ");
        Serial.println(row);
        blink(i, row);
      }
      break;
    default:
      Serial.println("unknown command");
      break;
  }
}

