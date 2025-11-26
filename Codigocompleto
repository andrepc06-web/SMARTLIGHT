int pinLDR = 2;     
int pinLED = 20;    
#define TRIG 5
#define ECHO 6
#define BUZZER 9



int limiteEscuro = 1500;   
int limiteClaro  = 1800;   
bool luzLigada = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
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

  float distancia = medirDistancia();

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  if (distancia > 0 && distancia < 30) {
    tone(BUZZER, 2000); // apitar a 2 kHz
  } else {
    noTone(BUZZER);     // parar o buzzer
  }

  delay(100);
}