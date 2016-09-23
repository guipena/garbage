// Distanzmessung 10-100cm mit Sprachausgabe   K.Hirschelmann    22.11.2013 

#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

#define trigger 7  // Arduino Pin an HC-SR04 Trig
#define echo 6     // Arduino Pin an HC-SR04 Echo

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

// 0-9, 10-19, 20-21
//----------------------------
char * wavFiles[] = {
"brief.WAV","genius.WAV", "giveitup.WAV", "gold.WAV", "gross.WAV", "lion.WAV", "offended.WAV", "priceless.WAV", "promotion.WAV", "proud.WAV", "stinker.WAV", "theworst.WAV", "titanium.WAV"};

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

long duration=0;
long distance=0;

////////////////////////////////////////////////////////////
void loop() {
 
  digitalWrite(trigger, LOW);  
  delayMicroseconds(2); 
 
  digitalWrite(trigger, HIGH);  
  delayMicroseconds(10);
  
  digitalWrite(trigger, LOW);
  duration = pulseIn(echo, HIGH); 
  distance = duration / 48.2; // value 148 for distances in inch
  
  Serial.print("Distance ");  
  Serial.print(distance);
  Serial.println("cm");

  int randomNumber;

// playing if distances are more than 1Meter  
  if (distance<102)   {
    Serial.println("detected!");
    randomNumber = random(0,12);    
    playcomplete(wavFiles[randomNumber]); 
  }

// select for playing distances between 1m and 10cm  
//if (distance>=97&&distance<102)   {
//   Serial.println("100");    
//   playcomplete(wavFiles[18]); }
//if (distance>=92&&distance<97)   {
//   Serial.println("95");   
//   playcomplete(wavFiles[17]); }
//if (distance>=87&&distance<92)   {
//   Serial.println("90");    
//   playcomplete(wavFiles[16]); }
//if (distance>=82&&distance<87)   {
//   Serial.println("85");    
//   playcomplete(wavFiles[15]); }
//if (distance>=77&&distance<82)   {
//   Serial.println("80");    
//   playcomplete(wavFiles[14]); }
//if (distance>=72&&distance<77)   {
//   Serial.println("75");    
//   playcomplete(wavFiles[13]); }
//if (distance>=67&&distance<72)   {
//   Serial.println("70");    
//   playcomplete(wavFiles[12]); }
//if (distance>=62&&distance<67)   {
//   Serial.println("65");    
//   playcomplete(wavFiles[11]); }
//if (distance>=57&&distance<62)   {
//   Serial.println("60");    
//   playcomplete(wavFiles[10]); }
//if (distance>=52&&distance<57)   {
//   Serial.println("55");    
//   playcomplete(wavFiles[9]); }
//if (distance>=47&&distance<52)   {
//   Serial.println("50");    
//   playcomplete(wavFiles[8]); }
//if (distance>=42&&distance<47)   {
//   Serial.println("45");    
//   playcomplete(wavFiles[7]); }
//if (distance>=37&&distance<42)   {
//   Serial.println("40");    
//   playcomplete(wavFiles[6]); }
//if (distance>=32&&distance<37)   {
//   Serial.println("35");    
//   playcomplete(wavFiles[5]); }
//if (distance>=27&&distance<32)   {
//   Serial.println("30");    
//   playcomplete(wavFiles[4]); }
//if (distance>=22&&distance<27)   {
//   Serial.println("25"); 
//   playcomplete(wavFiles[3]); }
//if (distance>=17&&distance<22)   {
//   Serial.println("20");    
//   playcomplete(wavFiles[2]); }
//if (distance>=12&distance<17)   {
//   Serial.println("15");    
//   playcomplete(wavFiles[1]); }
//if (distance>=7&&distance<12)   {
//   Serial.println("10");    
//   playcomplete(wavFiles[0]); }

// playing if distances are less than 10cm  
//if (distance<7)   {
//   Serial.println("less than 10cm");    
//   playcomplete(wavFiles[20]); }
//   delay(1000);
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
