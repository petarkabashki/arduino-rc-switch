/* 
This is just a naive trial for decoding streams for 433MHz remote controls
The aim is just to have differents results for differents remote buttons
Image of the remote control (Jolly Open) : http://mgrigaut.free.fr/arduino/Plip.jpg
Output signal from the 433MHz receiver : http://mgrigaut.free.fr/arduino/Jolly2.gif (probably NRZ)
*/

// Led on Arduino hardware (I use Arduino Nano)
const int led = 13;  // Blink when code found
// debug=1 for debugging
const int debug=0;
// input pin that receive signal
const int sig = 2;
// Code timing :
// Short bit is 600 us ; Long bit is 1.2 ms so 900 us is a good threshold to compare.
const int shortbit=9; // 0.9 ms (9 times 100 microseconds)
int maxlength; // 5 time shortbit (computed in Setup)
const int headerlength=25; // 25ms minimum (different units than shortbit)
int last = 0;
int cur = 0;
int nochange = 0; // Number of times (of 1 ms) the signal does not change
char resu[32] = ""; // Contains binary code as as string of '0' and '1'
int dur=0; // The measured duration between two changes
int bit=0; // the decoded bit (0 or 1)
int nb=0;
unsigned long HexCode=0;  // The found code

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(sig, INPUT);
  maxlength=5 * shortbit;
  Serial.print("\nInit : Start decoding\n");
}

void loop() {
  // Header detection :
  cur=digitalRead(sig);
  // Wait for a long stable signal :
  if (last!=cur) {
    // Not stable yet
    last=cur;
    nochange=0;
  } else {
    nochange++;
    if (nochange>headerlength) {
        // Header found
        // pri() will print only if debug=1
        pri("Found Header : ");
        // Wait end of header (at 0.1 ms precision)
        while(cur==last) {
          cur=digitalRead(sig);
          delayMicroseconds(100);
        }
        // Start decode whatever comes after this end of header :
        nb=my_decode();
        // nb is the number of bits detected
        // If the expected number of bits is known, then the following
        //    condition can be more precise to have a better filtering.
        if (nb>0) {
          // All the following stuff is mainly for debugging
          pri((String)nb);
          pri(" bits word\n");
          pri(resu);
          pri(" => ");
          // Build a filter to print out only known codes :
          // Now, this is the only useful info (printed even if debug=0) :
          Serial.print(HexCode,HEX);
          if (HexCode==0xCCCCCD) {
            Serial.print(" DOOR SECTIO");
          }
          if (HexCode==0xCCCCD3) {
            Serial.print(" DOOR PORTAL");
          }
          Serial.println();
          // Blink led :
          // Not useful since Serial.print makes another led blink on Arduino Nano
          digitalWrite(led, HIGH);
          delay(5);
          digitalWrite(led, LOW);
        } else {
          pri("Noise...\n");
        }
        // Done. Wait now for a new header
        nochange=0;
    }
  }
  // Wait every ms for the header
  delay(1);
}

int my_decode() {
  int nbits=0;
  cur=last;
  resu[0]='\0';
  // Duration (in 0.1 ms):
  dur=0;
  // Result :
  HexCode=0;
  while(dur<maxlength) {
    dur=0;
    // Wait for a change
    while(cur==last) {
      cur=digitalRead(sig);
      delayMicroseconds(100);
      dur++;
    }
    // So, state has changed now.
    last=cur;
    
    // this coding does not follow any specs.
    // See if duration is greater than 900 us :
    // This If then else is there to spend the same time in each branch
    if(dur>shortbit) {
      bit=1;
    } else {
      bit=0;
   }
    // Add bit to string resu :
    sprintf(resu,"%s%d",resu,bit);
    // Shift HexCode 1 bit to multiply it by 2 :
    HexCode=HexCode<<1;
    // Then add bit to it :
    HexCode+=bit;
    // Increment number of bits
    nbits++;
    // Avoid overflow :
    if (nbits>31) {
      nbits=0;
      // exit loop :
      dur=maxlength+1;
    }
  }
  return(nbits);
}


void pri(String truc) {
  if (debug==1) {
    Serial.print(truc);
  }
}
