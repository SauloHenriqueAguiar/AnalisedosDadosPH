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
# define UmidadeSensor 2 // usado para ESP32

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
#define Qtd_Amostras 1000    // ***Quantas Amostras o filtro terá para filtrar, mude para testar outros filtros***
#define Intervalo_Amostragem 1 // ***definindo o intervalo de amostragem em ms. Mude para testar novos filtros***
int Leitura_analogica = 0; // Variável global que salva o dado bruto lido da porta serial.
// A estratégia usada aqui é porque o comando analogRead possui um custo alto para o Arduino.
// Com isso salvamos na variável para essa leitura ser feita apenas uma vez a cada interação de loop. 
unsigned long timer1=0; // A variável que irá contar o útimo 


float filtroMediaMovel(bool atualiza_saida){ // A função de media movel trabalha com variavel estática, que salva as variaveis sem perder
  static  int Leituras_anteriores[Qtd_Amostras]; // Esse é o vetor que servirá como buffer circular
  static  int Posicao = 0; // A posicao atual de leitura, que deverá ficar salva
  static long Soma=0; // A soma total do buffer circular
  static float Media = 0; // A media, que é a saída da função quando é chamada
  static bool zera_vetor = 1;  // A variavel para saber se é a primeira execução. Se for, ele zera todo o buffer circular.
  
  if (zera_vetor){ // Zerando todo o buffer circular, para que as subtrações das sobrescrição não atrapalhe o filtro
    for(int i=0; i<= Qtd_Amostras; i++){
      Leituras_anteriores[i] = 0;
    }
    zera_vetor = 0;
//    Serial.println("Não entra mais no laço");
  }
  if(atualiza_saida == 0) return((double)Media); // Se o parametro recebido na funcao for zero, ele retorna somente o valor de media calculado anteriormente
  
  else{ // Caso seja 1, signfica que está no tempo de amostragem, e ai atualiza a variável média
  Soma = Leitura_analogica - Leituras_anteriores[Posicao%Qtd_Amostras] + Soma;
  Leituras_anteriores[Posicao%Qtd_Amostras] = Leitura_analogica;
  Media = (float)Soma/(float)(Qtd_Amostras);
  Posicao = (Posicao+1)%Qtd_Amostras;
  return((double)Media);
  }
}


/*metodo para imprimir na ESP32
entrada no modo deep sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Sistema Dormindo"); break;
    
  }
}

/*
Method to print the touchpad by which ESP32
has been awaken from sleep
*/
void print_wakeup_touchpad(){
  touchP = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
   
    case 2  : Serial.println("Despertar causado pelo touchpad"); break;
   
  }
}

void callback(){
  //placeholder callback function
}


void liga() {
   
     // turn LED ON
    digitalWrite(ledPin, HIGH);
    Serial.println("sistema ligado!");
}

void desliga(){
  
    // turn LED OFF
    
    
    digitalWrite(ledPin, LOW);
    Serial.println("modo deep sleep!");
    esp_deep_sleep_start();
    Serial.println("Isso não deve aparecer!");
  
}


 //metodos filtro MM
 void Amostragem(){ // Essa função verifica se o tempo de amostragem  selecionado ocorreu
  if(millis() - timer1>Intervalo_Amostragem){ // Caso o tempo de amostragem tenha ocorrido, ele envia 1 para a função de filtro de media movel
    //Dessa forma a função sabe que é para atualizar o valor de saída para um novo valor filtrado
    filtroMediaMovel(1);
    timer1 = millis(); // atualiza para contar o tempo mais uma vez
  }
}

void setup() {
  //pinMode(SensorPH, INPUT); //DEFINE O PINO COMO ENTRADA
  //Serial.begin(115200); //INICIALIZA A SERIAL
  sensors.begin();

  pinMode(LED,OUTPUT); //inicializa o LED
  

  Heltec.begin(true /*Habilita o Display*/, false /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/);
 
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


void loop(){

  //botão touch
  touchValue = touchRead(touchPin)*100;
   if(touchValue >= 100){
    liga();
  }else if(touchValue <100){
    desliga();
  }

 //sensor pH
  Leitura_analogica = analogRead(36);  // Leitura_analogica aqui é o valor bruto
  Amostragem(); // Essa é a função que fará a amostragem no tempo que determinamos no intervalo de amostragem que definimos na segunda linha de código.
  Serial.print(Leitura_analogica); // Imprime o dado bruto
  Serial.print(",");
  Serial.println(filtroMediaMovel(0)); // Imprime o dado filtrado
  float  FMM = filtroMediaMovel(0);
  
  // A função recebe o valor 0 que é para saber que não é para alterar o seu valor, somente imprimir o que está lá.
  if (FMM >= 300 && FMM <= 950){
    int pH = -0.01535*FMM + 17.1679;
    Heltec.display->drawString(28, 42,"pH:" + (String)pH);
    Heltec.display->display();
    
    }else{
    Heltec.display->drawString(28, 42,"pH: 0");
    Heltec.display->display();
   
  
 
 

  //temperatura
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  //Umidade
  float sensorValue = analogRead(UmidadeSensor);

  
  float currentTemp = temperatureC;
  float currentHum = sensorValue;
  
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);

  //temperatura display
  Heltec.display->drawString(5, 5,"Temp:" + (String)currentTemp + "°C");
  Heltec.display->display();


   

  // Humidade 
  if (sensorValue >= 3500){

 
  Heltec.display->drawString(20, 25,"Umi: 0%");
  Heltec.display->display();
 // delay(5000);
 

  
 }else if ( sensorValue < 1000){

  
  Heltec.display->drawString(20, 25,"Umi: 100%" );
  Heltec.display->display();
  //delay(5000);

  }else { 
  float umi = -0.0664*currentHum + 176.93;
 
 
  Heltec.display->drawString(20, 25,"Umi:" + (String)umi + "%");
  Heltec.display->display();
  //delay(5000);
  }
    }
}









 

 
