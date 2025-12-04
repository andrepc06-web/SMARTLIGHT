#include <NimBLEDevice.h>
#include <Wire.h>
#include "DFRobot_RGBLCD1602.h"


DFRobot_RGBLCD1602 lcd(16, 2);


#define NOME_BLUETOOTH "ESP32-C6-SENSORES"


int pinLDR = 3;
int pinLED = 21;

#define TRIG 4
#define ECHO 7
#define BUZZER 15


int limiteEscuro = 1500;
int limiteClaro  = 1800;
bool luzLigada = false;


NimBLECharacteristic* pCharacteristicLED;
NimBLECharacteristic* pCharacteristicLuz;
NimBLECharacteristic* pCharacteristicDistancia;
NimBLECharacteristic* pCharacteristicAviso;


void setup() {
  Serial.begin(115200);

  
  Wire.begin(8, 9);        
  lcd.init();
  lcd.setRGB(255, 255, 255);

  lcd.setCursor(0, 0);
  lcd.print("SMARTLIGHT");
  lcd.setCursor(0, 1);
  lcd.print("A iniciar...");
  delay(1200);
  lcd.clear();

  
  pinMode(pinLED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);

  
  NimBLEDevice::init(NOME_BLUETOOTH);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  NimBLEServer* pServer = NimBLEDevice::createServer();
  NimBLEService* pService = pServer->createService("FFE0");

  pCharacteristicLED       = pService->createCharacteristic("LED",       NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristicLuz       = pService->createCharacteristic("LUZ",       NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristicDistancia = pService->createCharacteristic("DIST",      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristicAviso     = pService->createCharacteristic("AVISO",     NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  pCharacteristicLED->setValue("DESLIGADO");
  pCharacteristicLuz->setValue("0");
  pCharacteristicDistancia->setValue("0.0");
  pCharacteristicAviso->setValue("SEGURO");

  pService->start();

  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("FFE0");
  pAdvertising->start();

  Serial.println(" BLE ativo e LCD ligado!");
}


void loop() {
  
  int valorLuz = analogRead(pinLDR);

  if (!luzLigada && valorLuz > limiteEscuro) {
    digitalWrite(pinLED, HIGH);
    luzLigada = true;
  }
  if (luzLigada && valorLuz < limiteClaro) {
    digitalWrite(pinLED, LOW);
    luzLigada = false;
  }

  
  float distancia = medirDistancia();
  bool aviso = false;

  int delayBuzzer = 0;

  if (distancia <= 10 && distancia > 0) delayBuzzer = 100;
  else if (distancia <= 20)           delayBuzzer = 250;
  else if (distancia <= 30)           delayBuzzer = 500;
  else if (distancia <= 50)           delayBuzzer = 1000;
  else delayBuzzer = 0;

  if (delayBuzzer > 0) {
    tone(BUZZER, 2000);
    delay(delayBuzzer);
    noTone(BUZZER);
    delay(delayBuzzer);
    aviso = true;
  } else {
    noTone(BUZZER);
    aviso = false;
  }

  
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Luz: ");
  lcd.print(luzLigada ? "ON " : "OFF");

  if (aviso && distancia > 0 && distancia < 20) {
    lcd.setRGB(255, 0, 0); 
    lcd.setCursor(0, 1);
    lcd.print("  ! PERIGO ! ");
  }
  else {
    lcd.setRGB(255, 255, 255);  

    lcd.setCursor(0, 1);
    lcd.print("D:");
    lcd.print(distancia, 0);
    lcd.print("cm ");

    int barras = map(distancia, 0, 100, 1, 8);
    if (barras < 0) barras = 0;
    if (barras > 8) barras = 8;

    lcd.setCursor(9, 1);
    for (int i = 0; i < barras; i++)
      lcd.print((char)255);
  }

  
  pCharacteristicLED->setValue(luzLigada ? "LIGADO" : "DESLIGADO");
  pCharacteristicLuz->setValue(String(valorLuz).c_str());
  pCharacteristicDistancia->setValue(String(distancia, 1).c_str());
  pCharacteristicAviso->setValue(aviso ? "PERTO" : "SEGURO");

  pCharacteristicLED->notify();
  pCharacteristicLuz->notify();
  pCharacteristicDistancia->notify();
  pCharacteristicAviso->notify();

  
  Serial.print("LED: "); Serial.print(luzLigada ? "LIGADO" : "DESLIGADO");
  Serial.print(" | Luz: "); Serial.print(valorLuz);
  Serial.print(" | Dist: "); Serial.print(distancia, 1);
  Serial.print(" | Aviso: "); Serial.println(aviso ? "PERTO" : "SEGURO");

  delay(120);
}


float medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH, 40000);
  if (duracao == 0) return 0;
  return duracao * 0.0343 / 2;
}