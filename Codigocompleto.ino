#include <NimBLEDevice.h>

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

  
  pinMode(pinLED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);

  
  NimBLEDevice::init(NOME_BLUETOOTH);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  NimBLEServer* pServer = NimBLEDevice::createServer();
  NimBLEService* pService = pServer->createService("FFE0");

  
  pCharacteristicLED = pService->createCharacteristic("LED", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristicLuz = pService->createCharacteristic("LUZ", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristicDistancia = pService->createCharacteristic("DIST", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristicAviso = pService->createCharacteristic("AVISO", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  
  pCharacteristicLED->setValue("DESLIGADO");
  pCharacteristicLuz->setValue("0");
  pCharacteristicDistancia->setValue("0.0");
  pCharacteristicAviso->setValue("SEGURO");

  pService->start();

  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("FFE0");
  pAdvertising->start();

  Serial.println("✅ BLE ATIVO - 4 characteristics NOTIFY DISPONÍVEIS!");
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
  else if (distancia <= 20) delayBuzzer = 250;
  else if (distancia <= 30) delayBuzzer = 500;
  else if (distancia <= 50) delayBuzzer = 1000;
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
  Serial.print(" | Distancia: "); Serial.print(distancia, 1); Serial.print(" cm");
  Serial.print(" | Aviso: "); Serial.println(aviso ? "PERTO" : "SEGURO");

  delay(100); 
}


float medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH);
  float distancia = duracao * 0.0343 / 2;
  return distancia;
}
