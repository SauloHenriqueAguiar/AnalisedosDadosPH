
float  average = 0;
 float pH;

const float SensorPH = 36; 

#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 16;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);


//botão liga desliga e deep sleep

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

//#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define SCR_WD   120
#define SCR_HT   120
#define TEMP_X  10
#define TEMP_Y  45

// defined(ESP32)
#define TFT_DC 17 //A0
#define TFT_CS 21 //CS
#define TFT_MOSI 2 //SDA
#define TFT_CLK 23 //SCK
#define TFT_RST 0  
#define TFT_MISO 0 

// Color set
#define  BLACK           0x0000
#define RED             0xF800
#define GREEN           0x07E0
//#define BLUE            0x001F
#define BLUE            0x102E
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define ORANGE          0xFD20
#define GREENYELLOW     0xAFE5
#define DARKGREEN       0x03E0
#define WHITE           0xFFFF

// For 1.44" and 1.8" TFT with ST7735 use:
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);


//sensor umidade
# define UmidadeSensor 4 // usado para ESP32

/*
Method to print the reason by which ESP32
has been awaken from sleep
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


void setup() {
  pinMode(SensorPH, INPUT); //DEFINE O PINO COMO ENTRADA
  Serial.begin(115200); //INICIALIZA A SERIAL
  sensors.begin();

// Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
   tft.setRotation(2);
   tft.fillScreen(ST7735_BLACK);
 
pinMode (ledPin, OUTPUT);


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

  //ddp

 //botão e deep sleep

 touchValue = touchRead(touchPin)*100;
  Serial.print(touchValue);
  // check if the touchValue is below the threshold
  // if it is, set ledPin to HIGH
  if(touchValue > threshold){
     // turn LED ON
    digitalWrite(ledPin, HIGH);
    Serial.println(" Sistema Funcionando!");
    //Serial.println(touchValue);
    //Serial.println(threshold);


  }
  else{
   
    // turn LED OFF
    digitalWrite(ledPin, LOW);
    Serial.println("Sistema desligado!");
    //Serial.println(touchValue);
    //Serial.println(threshold);
 
    //VA dormir agora
  Serial.println("Modo Deep Sleep...");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
  }
  delay(500);


  

 float tensaoMedida = analogRead(SensorPH); //FAZ A LEITURA DO PINO ANALÓGICO E ARMAZENA NA VARIÁVEL O VALOR LIDO



  for (float i=0; i < 10000; i++) {
 average = (average + tensaoMedida);
 }
 average = average/1000;


  //temperatura
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
//Umidade
float sensorValue = analogRead(UmidadeSensor);

  Serial.print("Voltage(pH): ");
  Serial.print(average);
  Serial.println(" mV");
  Serial.print("Temperatura = ");
  Serial.print(temperatureC);
  Serial.println(" ºC");
  delay(2000);



/*
     tft.setCursor(10, 20);
     tft.setRotation(3);
     tft.setTextSize(2.5);
     tft.setTextColor(WHITE);
     tft.print(" pH: ");
     tft.print("11.41");
     
   

     tft.setCursor(20, 60);
     tft.setRotation(3);
     tft.setTextSize(2.0);
     tft.setTextColor(BLUE);
     tft.print(" T: ");
     tft.print(temperatureC);

     tft.setCursor(20, 80);
     tft.setRotation(3);
     tft.setTextSize(2.0);
     tft.setTextColor(DARKGREEN);
     tft.print(" U: ");
     tft.print("100 % ");

    delay(2000);
    tft.fillScreen(ST7735_BLACK);

    */
    
if (sensorValue > 3500)
{
   

Serial.print("Umidade: ");
Serial.println("0 %");

delay(2000);

  
}
else if ( sensorValue < 1000)
{
Serial.print("Umidade: ");
Serial.println("100 %");
  
delay(2000);

}
else 
{ 
 float Umid = -0.0664*sensorValue + 176.93;
 Serial.print("Umidade: ");
 Serial.print(Umid);
 Serial.println(" % ");

 delay(2000);
 





  
}








}
