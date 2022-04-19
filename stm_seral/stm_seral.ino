#include <ArduinoJson.h>
#include <MapleFreeRTOS900.h>

int a;
String err;
bool num2;

static void vJson(void *pvParameters) {
    for (;;) {
        DynamicJsonDocument  doc(768);
      
        a = random(1000,2000);
        doc["num"]=a;
        serializeJson(doc, Serial3);

        err="";
        DeserializationError error = deserializeJson(doc, Serial3);
        if (error) {
          err = error.c_str();
        }
        deserializeJson(doc, Serial3);
        num2 = doc["num2"];
        digitalWrite(PC13, !num2);
        vTaskDelay(500);
    }
}

static void vPrint(void *pvParameters){
  for (;;) {
    Serial.println("Random : " + String(a));
    Serial.println("Get :" + String(num2));
    if(err!=""){
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err);
    }

    vTaskDelay(500);
  }
}

void setup() {
  // Initialize Serial port
  Serial3.begin(9600);
  Serial.begin(115200);
  pinMode(PC13, OUTPUT);
  xTaskCreate(vPrint,
                "Task1",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 1,
                NULL);
  xTaskCreate(vJson,
                "Task2",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
  vTaskStartScheduler();
}

void loop() {}
