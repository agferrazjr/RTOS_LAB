/*
MIT License

Copyright (c) 2020 agferrazjr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include<Arduino.h>

//#define _DEBUG_

#ifdef _DEBUG_
  #define _DEBUG(...) Serial.print(__VA_ARGS__)
  #define MAX_ADC_COUNT  8
  #define TASK1_T_MS 500
  #define TASK2_T_MS 100
  #define TASK3_T_MS  800
#else
  #define _DEBUG(...) 
  #define MAX_ADC_COUNT  8  
  #define TASK1_T_MS 500
  #define TASK2_T_MS 10
  #define TASK3_T_MS  80
#endif

static unsigned long current_time;
static int sensorValue=0, ADCcount=0;

void Task1_BlinkLed(unsigned long int);

void Task2_ReadADC(unsigned long int);

void Task3_ADC_FILTER(unsigned long int);


//put your setup code here, to run once:
void setup() {
//Inicialiazação pino 13 - acionamento LED
  pinMode(LED_BUILTIN, OUTPUT);
// initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
}

// put your main code here, to run repeatedly:
void loop() {
  
Task1_BlinkLed(TASK1_T_MS);

Task2_ReadADC(TASK2_T_MS);
  
Task3_ADC_FILTER(TASK3_T_MS);
  

}



void Task1_BlinkLed(unsigned long int timePeriod_ms){
  static unsigned long int initial_time, elapsed_time, last_time=0;
  
  initial_time = millis();
  elapsed_time = initial_time - last_time;

  if(elapsed_time >= timePeriod_ms){
      _DEBUG("Task1 elapsed_time="); 
      _DEBUG(elapsed_time, DEC);
      _DEBUG("\n");    
    if(digitalRead(LED_BUILTIN)==1){ //led aceso
        digitalWrite(LED_BUILTIN, 0); //apagar led 
        _DEBUG("LED APAGADO\n");     
     }
     else{
        digitalWrite(LED_BUILTIN, 1);//Se led apagado, acender
        _DEBUG("LED ACESO\n");
     }
    
    last_time = initial_time;
    } 
}

/*
 * WE CONNECTED LCD_KEYPAD SHIELD ON ARDUINO
 * WHEN SOME BUTTON IS PRESSED IT CHANGE ADC VALUE
 */
void Task2_ReadADC(unsigned long int timePeriod_ms){
  static unsigned long int initial_time, elapsed_time, last_time=0;
  initial_time = millis();
  elapsed_time = initial_time - last_time;

  if(elapsed_time >= timePeriod_ms){
        // read the input on analog pin 0:
        if(ADCcount < MAX_ADC_COUNT){
            _DEBUG("Task2 elapsed_time="); 
            _DEBUG(elapsed_time, DEC);
            _DEBUG("\n");
            sensorValue += analogRead(A0);
            _DEBUG("analogRead="); 
            _DEBUG(sensorValue, DEC);
            _DEBUG("\n");
            last_time = initial_time;
            ADCcount++;
        }
    else
    {
       _DEBUG("Task2 waiting [MAX_ADC_COUNT]\n");       
    }
    
  }
}


/*
 * ADC Filter calculation (MEDIAN FILTER)
 */
void Task3_ADC_FILTER(unsigned long int timePeriod_ms){
  static unsigned long int initial_time, elapsed_time, last_time=0;
  initial_time = millis();
  elapsed_time = initial_time - last_time;

  if(elapsed_time >= timePeriod_ms){
      _DEBUG("Task3 elapsed_time="); 
      _DEBUG(elapsed_time, DEC);
      _DEBUG("\n");
      if (ADCcount>=(MAX_ADC_COUNT-1))
      {        
        sensorValue = sensorValue / (int)MAX_ADC_COUNT; //Calculo damedia sensorValue = sensorValue / 8;
        Serial.print("ADC = ");
        Serial.println(sensorValue, DEC);
        sensorValue=0;
        ADCcount=0;
        last_time = initial_time;
      }
      else
      {
        _DEBUG("TASK3 waiting ADCcount\n");
      } 
   }  
}
