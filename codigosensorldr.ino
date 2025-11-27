int pinLDR = 2;     
int pinLED = 20;    


int limiteEscuro = 1500;   
int limiteClaro  = 1800;   
bool luzLigada = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
}

void loop() {
  int valorLuz = analogRead(pinLDR);

  Serial.print("Luminosidade: ");
  Serial.println(valorLuz);

  
  if (!luzLigada && valorLuz > limiteEscuro) {
    digitalWrite(pinLED, HIGH);
    luzLigada = true;
    Serial.println("LED LIGADA");
  }

  if (luzLigada && valorLuz < limiteClaro) {
    digitalWrite(pinLED, LOW);
    luzLigada = false;
    Serial.println("LED DESLIGADA");
  }

  delay(200);
}