#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR   0x3C

const int B0_PIN = 7;
const int B1_PIN = 8;
const int S_PIN = 2;
const long interval = 1000;

unsigned long timer = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

String timerString = "";

byte b0 = 0;
byte b1 = 0;

byte toggle = 0;
byte state = 0;

bool active = true;

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT);

void setup() {
  pinMode(B0_PIN, INPUT_PULLUP);
  pinMode(B1_PIN, INPUT_PULLUP);
  pinMode(S_PIN, OUTPUT);
  
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setFont(&FreeSans9pt7b);
  
  display.clearDisplay();
  
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
}

void loop() {
  
  b0 = digitalRead(B0_PIN);
  b1 = digitalRead(B1_PIN);
  
  switch (state){
    
    case 0:  // idle state
      timer = 0;
      toggle = 1;
      
      digitalWrite(S_PIN, LOW);
      
      display.clearDisplay(); // Idle text
      display.setTextSize(2);
      display.setCursor(0, 26);
      display.println("Idle");
      
      display.setTextSize(1);  // Versioning text
      display.setCursor(0, 54);
      display.println("GrindCPU v0.5");
      
      display.display();
      
      if (b0 == LOW){  // goto selection state
        toggle = 1;
        delay(200);  
        state = 1;
      }
      
      if (b1 == LOW){ // goto pulse state
        state = 4;
        delay(25);
      }
      
      break;
      
    case 1:
      display.clearDisplay();  // Grind text
      display.setTextSize(2);
      display.setCursor(0, 26);
      display.println("Grind");
      
      if (timer > 0){  // Seconds text only appears after timer incremented
      display.setTextSize(2);
      display.setCursor(0, 60);
      timerString = String(timer);
      timerString.concat("s");
      display.println(timerString);
      }
      
      display.display();
      
      if (b0 == LOW){  // Add to timer
        toggle = 0;
        timer = timer + 1;
        delay(25);
      }
      
      if (b1 == LOW){  // Grind button pressed
        
        if (toggle == 1){  // If there is no timer... goto HOLD state
          delay(100);
          state = 3;
        }
        if (toggle == 0){  // If there is a timer... goto TIMER state
          delay(100);
          state = 2;
        }
        
      }
      
      break;
      
    case 2:  // timer state 
          
      digitalWrite(S_PIN, HIGH);  // turn on grinder relay
      previousMillis = 0;  // reset timer variables
      active = true;

      
      display.clearDisplay();
      
      display.setTextSize(2);  // Time remaining text
      display.setCursor(0, 30);
      timerString = String(timer);
      timerString.concat("s");
      display.println(timerString);
      
      display.setTextSize(1);  // "Grinding" text
      display.setCursor(0,54);
      display.println("Grinding...");
      
      display.display();
      
      delay(1000);
      
      while (active == true){  // timer loop
        b1 = digitalRead(B1_PIN);
        currentMillis = millis();
        
        if (b1 == LOW){  // cancel button
         digitalWrite(S_PIN, LOW);  // turn off grinder relay
         active = false;
         delay(150);
         break;
        }
        
        if ((currentMillis - previousMillis >= interval)){  // when time elapsed is greater than 1000ms
          if (timer > 0){  // only do this if the there is still time remaining
            previousMillis = currentMillis;
            timer = timer - 1;  // decrement timer by one
            
            digitalWrite(S_PIN, HIGH);  // turn on grinder relay
            
            display.clearDisplay();
            
            display.setTextSize(2); // time remaining text
            display.setCursor(0, 30);
            timerString = String(timer);
            timerString.concat("s");
            display.println(timerString);
            
            display.setTextSize(1);  // "Grinding" text
            display.setCursor(0,54);
            display.println("Grinding...");
            
            display.display();
           }
          }
          
          if (timer == 0){  // if the time has run out,
            digitalWrite(S_PIN, LOW);  // turn off grinder relay
            active = false;
            state = 0;  // return to IDLE state
            break;   
          }
        }
       
    state = 0;  // second failsafe
    break;
    
    case 3:
      display.clearDisplay();
      
      display.setTextSize(2);  // HOLD text
      display.setCursor(25, 40);
      display.println("Hold");
      
      display.display();
      
      digitalWrite(S_PIN, HIGH);  // turn on grinder relay
      
      if (b1 == LOW){  // cancel button
        digitalWrite(S_PIN, LOW);
        delay(200);
        state = 0;
      }
      
      else{
        delay(1);
      }
      break;
      
    case 4:
      display.clearDisplay();
      
      display.setTextSize(2);  // pulse text
      display.setCursor(0, 26);
      display.println("Pulse");
      
      display.display();
      
      digitalWrite(S_PIN, HIGH);
      
      if (b1 == LOW){  // button is pressed
        digitalWrite(S_PIN, HIGH);  // turn on grinder relay
      }
      
      if (b1 == HIGH){  // button is not pressed
        digitalWrite(S_PIN, LOW);  // turn off grinder relay
        state = 0;  // return to IDLE state
      }
      
  }
}
