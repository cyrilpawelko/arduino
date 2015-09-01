/*
HIDEKI DV323 decoder

 Horloge = 819 Hz / 1220 µs 
 0: |---|_  : 839 µs - 362 µs
 1: |-|___  : 362 µs - 839 µs
 64 bits - 
 */

/* TODO:
 completely remove pinstatus
 */

volatile boolean pinStatus;

void setup() {
  Serial.begin(115200);
  pinMode(2,INPUT);
  digitalWrite(2,LOW);                                // init RF sensor
  attachInterrupt(0,interruptHandle,CHANGE);          // attach interrupt handler
  pinStatus=digitalRead(2);                           // try to correctly detect pin state
}

void printTempHum(float temperature, int humidity){
  Serial.print("Hideki Temperature: ");
  Serial.print(temperature);
  Serial.print(" C. Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}

void interruptHandle() {
  static unsigned long lastTime=0;
  static boolean isBitFirstPart = false;
  //  static byte bitsCount = 0;
  static byte receivedBits[70];
  static byte receivedBitsNumber=0;
  float temperature;
  int humidity;
  static unsigned long time;
  static unsigned long duration;
  time = micros();
  duration = time - lastTime;
  if (( duration > 790 && duration < 910 ) || ( duration > 300 && duration < 410 ) ) { // signal length is within boundaries (long or short)
    if (( isBitFirstPart == false ) && ( pinStatus == true))                           // First part of a bit
    {
      isBitFirstPart = true;
    }
    else                                                                               // Second part of a bit
    {
      isBitFirstPart = false;
      if (duration > 600) 
        receivedBits[receivedBitsNumber]=1;
      else
        receivedBits[receivedBitsNumber]=0;
      receivedBitsNumber++; 
      if (receivedBitsNumber == 64) {                                                  // Message complete
        if (( receivedBits[14] == 0) and ( receivedBits[15] == 1)) {                   // Dirty check of data validity
          temperature = (receivedBits[36]+receivedBits[37]*2+receivedBits[38]*4+receivedBits[39]*8)*10 
            + (receivedBits[31]+receivedBits[32]*2+receivedBits[33]*4+receivedBits[34]*8) 
            + (receivedBits[26]+receivedBits[27]*2+receivedBits[28]*4+receivedBits[29]*8) / 10;
          humidity = (receivedBits[51]+receivedBits[52]*2+receivedBits[53]*4+receivedBits[54]*8) * 10 
            + (receivedBits[46]+receivedBits[47]*2+receivedBits[48]*4+receivedBits[49]*8);
          printTempHum(temperature,humidity);
        }
        receivedBitsNumber = 0;
      }    
    }
  } 
  else  {
    isBitFirstPart=false;
    receivedBitsNumber=0;
  }
  lastTime = time; 
  pinStatus = !pinStatus;  
}

void loop() {
  static int heure;
  static int oldheure;
  heure=int(micros() / 30000000);
  if (heure != oldheure) Serial.println("+++++");
  oldheure = heure;
}


















