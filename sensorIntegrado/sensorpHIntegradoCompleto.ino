//inclusão de bibliotecas
#include "heltec.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 17;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);


//sensor umidade
//# define UmidadeSensor 2 // usado para ESP32
// digital pin 2 has a pushbutton attached to it. Give it a name:
int umidadesensor = 2;


//sensor pH

float  average = 0;
float pH;

//const float SensorPH = 36;

//sensor touch
// set pin numbers
const int touchPin = 13;
const int ledPin = 13;

#define Threshold 60
RTC_DATA_ATTR int bootCount = 0;
touch_pad_t touchP;
// change with your threshold value
const int threshold = 50;
// variable for storing the touch pin value
int touchValue;

//filtro MM

// --- Constantes Auxiliares ---
#define     num    100       //número de iterações da média móvel


// ========================================================================================================
// --- Protótipo das Funções ---
long moving_average(int sig);


// ========================================================================================================
// --- Variáveis Globais ---
int values[num];              //vetor com num posições, armazena os valores para cálculo da média móvel

int adc_noise1,               //armazena a leitura sem filtro da entrada analógica
    adc_noise2;               //armazena a leitura filtrada da entrada analógica


/*metodo para imprimir na ESP32
  entrada no modo deep sleep
*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {

    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Sistema Dormindo"); break;

  }
}

/*
  Method to print the touchpad by which ESP32
  has been awaken from sleep
*/
void print_wakeup_touchpad() {
  touchP = esp_sleep_get_touchpad_wakeup_status();

  switch (touchPin)
  {

    case 2  : Serial.println("Despertar causado pelo touchpad"); break;

  }
}

void callback() {
  //placeholder callback function
}


void liga() {

  // turn LED ON
  digitalWrite(ledPin, HIGH);
  Serial.println("sistema ligado!");
}

void desliga() {

  // turn LED OFF


  digitalWrite(ledPin, LOW);
  Serial.println("modo deep sleep!");
  esp_deep_sleep_start();
  Serial.println("Isso não deve aparecer!");

}


//metodos filtro MM


void setup() {
  //pinMode(SensorPH, INPUT); //DEFINE O PINO COMO ENTRADA
  //Serial.begin(115200); //INICIALIZA A SERIAL

  sensors.begin();

  pinMode(LED, OUTPUT); //inicializa o LED
  pinMode(umidadesensor, INPUT);

  Heltec.begin(true /*Habilita o Display*/, false /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/);

  /*
    Heltec.display->clear();
    Heltec.display->init();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_16);
  */

  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Ligando");
  Heltec.display->drawString(10, 30, "o Sensor!");
  Heltec.display->display();





  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();

  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T4, callback, Threshold);

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();



}


void loop() {

  //botão touch
  touchValue = touchRead(touchPin) * 100;
  if (touchValue >= 100) {
    liga();

  } else {

    desliga();

  }


  



  //temperatura
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  

  float currentTemp = temperatureC;
  

  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);

  //temperatura display
  Heltec.display->drawString(5, 5, "Temp:" + (String)currentTemp + "°C");
  Heltec.display->display();


 //sensor pH
   adc_noise1 = analogRead(36);
   adc_noise2 = moving_average(adc_noise1);
   float  FMM = adc_noise2;
  // Heltec.display->drawString(28, 42, "pH:" + (String)FMM);
  // Heltec.display->display();
    float pH1 = -0.01535*FMM + 17.1679; // regressão linear
     
    float pH2 = (3e-08)*pow(FMM,3) - (8e-05)*pow(FMM,2) + 0.0527*FMM - 1.4208;// polinomial 3 ordem   
    Serial.println("pH linear: "+ (String)pH1);
    Serial.println("pH poli: " + (String)pH2);
    
    Serial.println(FMM);
    // Serial.print(adc_noise1);
   // Heltec.display->drawString(28, 42, "dd:" + (String)FMM);
    // Heltec.display->display();


  // A função recebe o valor 0 que é para saber que não é para alterar o seu valor, somente imprimir o que está lá.
    if (FMM >= 200 && FMM <= 1200){
   
    Heltec.display->drawString(28, 42,"pH:" + (String)pH2);
    Heltec.display->display();
    
    }else{
    
    Heltec.display->drawString(28, 42,"pH: 0");
    Heltec.display->display();
    }
    

  // Humidade
  //Umidade
 
/* // read the input pin:
  int umidadeState = digitalRead(umidadesensor);
   //Serial.println(umidadeState);
   
   if(umidadeState == 0){
  // print out the state of the button:
  
    Heltec.display->drawString(20, 25, "Umi: 100%");
    Heltec.display->display();
 
   } else if(umidadeState == 1){ 
     Heltec.display->drawString(20, 25, "Umi: 0%");
    Heltec.display->display();

   }
*/
  
  int sensorValue = analogRead(2);
   int currentHum = sensorValue;
  if (sensorValue >= 3500) {


    Heltec.display->drawString(20, 25, "Umi: 0%");
    Heltec.display->display();
    // delay(5000);



  } else if ( sensorValue < 1000) {


    Heltec.display->drawString(20, 25, "Umi: 100%" );
    Heltec.display->display();
    //delay(5000);

  } else {
    int umi = -0.0664 * currentHum + 176.93;


    Heltec.display->drawString(20, 25, "Umi:" + (String)umi + "%");
    Heltec.display->display();
    //delay(5000);
  }


  
}
// --- Desenvolvimento das Funções ---
long moving_average(int sig)
{
  int i;               //variável auxiliar para iterações
  long acc = 0;        //acumulador

  //Desloca o vetor completamente eliminando o valor mais antigo
  for (i = num; i > 0; i--) values[i] = values[i - 1];

  values[0] = sig;           //carrega o sinal no primeiro elemento do vetor

  // long sum = 0;            //Variável para somatório

  for (i = 0; i < num; i++) acc += values[i];


  return acc / num;         //Retorna a média móvel

} //end moving_average
