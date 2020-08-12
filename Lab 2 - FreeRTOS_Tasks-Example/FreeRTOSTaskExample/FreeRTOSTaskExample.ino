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

#include <Arduino_FreeRTOS.h>
#include<Arduino.h>

#define _DEBUG_
#define __USE_MILLIS__

#ifdef __USE_MILLIS__
#define GetTime_MS()  millis()
#else
#define GetTime_MS() (xTaskGetTickCount()*portTICK_PERIOD_MS)
#endif

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

/*
   Este é um exemplo inicial apresentando conceitos de tarefas.
   Adiante veremos como trocar informações entre Tasks através de Filas
   e também como proteger código contra reentrancia usando semáforos
*/
static int sensorValue = 0, ADCcount = 0;

/**
   Task handlers
   https://www.freertos.org/a00019.html#xTaskHandle
*/
TaskHandle_t Task1BlinkLedHandle;

TaskHandle_t Task2ReadADCHandle;

TaskHandle_t Task3ADCFilterHandle;

void Task1_BlinkLed(void *pvParameters);
void Task2_ReadADC(void *pvParameters);
void Task3_ADC_FILTER(void *pvParameters);


void setup() {
  //Inicializa Comunicação serial a 9600bps
  Serial.begin(115200);

  /**
     Task creation
  */
  xTaskCreate(Task1_BlinkLed, // Task function
              "Blink", // Task name
              192, // Stack size
              NULL,
              3, // Priority
              &Task1BlinkLedHandle); // Task handler

  xTaskCreate(Task2_ReadADC,
              "ReadADC",
              192,
              NULL,
              0,
              &Task2ReadADCHandle);

  xTaskCreate(Task3_ADC_FILTER,
              "ADC_FILTER",
              192,
              NULL,
              2,
              &Task3ADCFilterHandle);


}

void loop() {
  // put your main code here, to run repeatedly:
}


void Task1_BlinkLed(void *pvParameters) {
#ifdef _DEBUG_
  static unsigned long int initial_time, elapsed_time, last_time = 0;  
  initial_time = last_time = GetTime_MS();
#endif
  while (1) {
#ifdef _DEBUG_
    initial_time = GetTime_MS();
    elapsed_time = initial_time - last_time;    
#endif
    _DEBUG("Task1 elapsed_time=");
    _DEBUG(elapsed_time, DEC);
    _DEBUG("\n");

    if (digitalRead(LED_BUILTIN) == 1) { //led aceso
      digitalWrite(LED_BUILTIN, 0); //apagar led
      _DEBUG("LED APAGADO\n");
    }
    else {
      digitalWrite(LED_BUILTIN, 1);//Se led apagado, acender
      _DEBUG("LED ACESO\n");
    }
#ifdef _DEBUG_    
    last_time = GetTime_MS();
#endif    
    vTaskDelay(TASK1_T_MS / portTICK_PERIOD_MS);
  }
}


/*
   WE CONNECTED LCD_KEYPAD SHIELD ON ARDUINO
   WHEN SOME BUTTON IS PRESSED IT CHANGE ADC VALUE
   Tarefa de maior prioridade, premissa que as leituras devem ser feitas dentro dos tempos definidos (Frequencia de Amostragem)
*/
void Task2_ReadADC(void *pvParameters) {
#ifdef _DEBUG_
  static unsigned long int initial_time, elapsed_time, last_time = 0;
  initial_time = last_time = GetTime_MS();
#endif
  while (1) {
#ifdef _DEBUG_
    initial_time = GetTime_MS();
    elapsed_time = initial_time - last_time;    
#endif
    // read the input on analog pin 0:
    if (ADCcount < MAX_ADC_COUNT)
    {
      _DEBUG("Task2 elapsed_time=");
      _DEBUG(elapsed_time, DEC);
      _DEBUG("\n");
      sensorValue += analogRead(A0);
      _DEBUG("analogRead=");
      _DEBUG(sensorValue, DEC);
      _DEBUG("\n");
      ADCcount++;
    }
    else
    {
      _DEBUG("Task2 waiting [MAX_ADC_COUNT]\n");
    }
#ifdef _DEBUG_    
    last_time = GetTime_MS();
#endif        
    vTaskDelay(TASK2_T_MS / portTICK_PERIOD_MS);
  }
}

/*
   ADC Filter calculation (MEDIAN FILTER)
   Task de prioridade média. Se houver pequeno atraso nao compromete pois tem um bom tempo ate ter que calular novas amostras do ADC.
*/
void Task3_ADC_FILTER(void *pvParameters) {
#ifdef _DEBUG_
  static unsigned long int initial_time, elapsed_time, last_time = 0;
  initial_time = last_time = GetTime_MS();
#endif
  while (1) {
#ifdef _DEBUG_
    initial_time = GetTime_MS();
    elapsed_time = initial_time - last_time;
    last_time = initial_time;
#endif
    _DEBUG("Task3 elapsed_time=");
    _DEBUG(elapsed_time, DEC);
    _DEBUG("\n");
    if (ADCcount >= (MAX_ADC_COUNT - 1))
    {
      sensorValue = sensorValue / (int)MAX_ADC_COUNT; //Calculo damedia sensorValue = sensorValue / 8;
      Serial.print("ADC = ");
      Serial.println(sensorValue, DEC);
      sensorValue = 0;
      ADCcount = 0;
    }
    else
    {
      _DEBUG("TASK3 waiting ADCcount\n");
    }
#ifdef _DEBUG_    
    last_time = GetTime_MS();
#endif        
    vTaskDelay(TASK3_T_MS / portTICK_PERIOD_MS);
  }
}
