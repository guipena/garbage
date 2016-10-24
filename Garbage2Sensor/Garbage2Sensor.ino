// Distanzmessung 10-100cm mit Sprachausgabe   K.Hirschelmann    22.11.2013 

#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

#define trigger 6  // Arduino Pin an HC-SR04 Trig
#define echo 7     // Arduino Pin an HC-SR04 Echo
#define trigger_2 8
#define echo_2 9
#define trigger_3 A2
#define echo_3 A1

long distance, duration, sonar1, sonar2, sonar3;

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

// 0-9, 10-19, 20-21
//----------------------------
char * wavFiles[] = {
"brief.wav","genius.wav", "giveitup.wav", "gold.wav", "price.wav", "gross.wav", "lion.wav", "offended.wav", "proud.wav", "promo.wav", "stinker.wav", "theworst.wav", "titanium.wav"};

////////////////////////////////////////////////////////////////
// this handy function will return the number of bytes currently 
// free in RAM, great for debugging!   
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

/////////////////////////////////////////////////////
void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}

/////////////////////////////////////////////////////
void setup() {

  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trigger_2, OUTPUT);
  pinMode(echo_2, INPUT);
  pinMode(echo_3, INPUT);
  pinMode(trigger_3, OUTPUT);

   
  // set up serial port
  Serial.begin(9600);
  putstring_nl("WaveHC");
  
  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
  
  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  // pin13 LED
  pinMode(13, OUTPUT);

  randomSeed(analogRead(A0));
 
  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while(1);                            // then 'halt' - do nothing!
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
 
// Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while(1);                            // then 'halt' - do nothing!
  }
  
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while(1);                             // then 'halt' - do nothing!
  }
  
   playcomplete(wavFiles[12]);            // Startmessage 
   delay(2000);
 
  // Whew! We got past the tough parts.
  putstring_nl("Ready!");
}

void SonarSensor(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
 
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); 
  distance = duration / 48.2; // value 148 for distances in inch
}

////////////////////////////////////////////////////////////
void loop() {

  SonarSensor(trigger, echo);
  sonar1 = distance;
  
  Serial.print("Distance of Sonar 1: ");  
  Serial.print(sonar1);
  Serial.println("cm");

  SonarSensor(trigger_2, echo_2);
  sonar2 = distance;

  Serial.print("distance of Sonar 2: ");
  Serial.print(sonar2);
  Serial.println("cm");

  SonarSensor(trigger_3, echo_3);
  sonar3 = distance;

  Serial.print("distance of Sonar 3: ");
  Serial.print(sonar3);
  Serial.println("cm");

  /*SonarSensor(trigger_4, echo_4);
  sonar4 = distance;

  Serial.print("distance of Sonar 4: ");
  Serial.print(sonar4);
  Serial.println("cm");*/
  
  int randomNumber;

// playing if distances are more than 1Meter  
  if (sonar1 < 48 || sonar2 < 48 || sonar3 < 48)   {
    Serial.println("detected!");
    randomNumber = random(0,12);    
    playcomplete(wavFiles[randomNumber]); 
  }

}

/////////////////////////////////////////////////////////
// Plays a full file from beginning to end with no pause.
void playcomplete(char *name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
}

//////////////////////////////////////////////////////////
void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
   // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
   // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
   // ok time to play! start playback
  wave.play();
}
