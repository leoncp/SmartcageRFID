// Jan2014 - leon@itactixsoftware.com

#include <SoftwareSerial.h>

#define DEBUG

SoftwareSerial SerialX = SoftwareSerial(10, 11);

int ant1 = 22;  //reader1
int ant2 = 24;
int ant3 = 26;
int bnt1 = 28;  //reader2
int bnt2 = 30;
int bnt3 = 32;
int cnt1 = 34;  //reader3
int cnt2 = 36;
int cnt3 = 38;
int dnt1 = 40;  //reader4
int dnt2 = 42;
int dnt3 = 44;

String arrayTags[35];
long arraySeen[35];
long arraySent[35];

int active = 1; //the active antennae

long lastmillis = 0; //track antennae switching
int interval = 400; //antennae switching interval
int threshholdXB = 7; //every # antennae switches clear the XBeeQ
int countXB = 1;

String lastTagRead, msg, tag0, tag1, tag2, tag3 = "";
char c;

void setup() {      

  //Configure Array (up to 30 tags)
  for (int i = 1; i < 31; i++) {
    arrayTags[i] = "";
    arraySent[i] = 0;
    arraySeen[i] = 0;
  }

  // initialize the digital pin as an output.
  pinMode(ant1, OUTPUT);
  pinMode(ant2, OUTPUT);
  pinMode(ant3, OUTPUT);
  pinMode(bnt1, OUTPUT);
  pinMode(bnt2, OUTPUT);
  pinMode(bnt3, OUTPUT);
  pinMode(cnt1, OUTPUT);
  pinMode(cnt2, OUTPUT);
  pinMode(cnt3, OUTPUT);
  pinMode(dnt1, OUTPUT);
  pinMode(dnt2, OUTPUT);
  pinMode(dnt3, OUTPUT);

  //Init Serial Ports including Xbee Serial Port for Output
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  SerialX.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {

  //Check of the Readers for RFID and Process
  //CheckReader0();
  CheckReader1();
  CheckReader2();
  CheckReader3();

  if (millis() > (lastmillis+interval))
  {
    lastmillis = millis();
    SwitchAntennae();

    if (countXB >= threshholdXB)
    {
      countXB = 1;
      ProcessXBQ();
    } 
    else 
    {
      countXB++;
    }
  } 
}

void CheckReader0()
{
  while(Serial.available()>0)
  {   
    c=Serial.read();
    if (c==0x02)
    {
      tag0 = "";
    }
    else
    {
      if (c!=0x03) //this is the last character in the tag 
      { //Only Add AlphaNumberic Characters to the Actual Tag  
        if (isAlphaNumeric(c)) //isDigit, isAscii can also be used
        {
          tag0 += c;
        }
      }
    }
    if (tag0.length()==12) //Tag Should be completed - 12 chars
    {
      ListTagInArray(tag0);
    }
  }
}

void CheckReader1()
{
  while(Serial1.available()>0)
  {   
    c=Serial1.read();
    //ProcessReadChar();
    if (c==0x02)
    {
      tag1 = "";
    }
    else
    {
      if (c!=0x03) //this is the last character in the tag 
      { //Only Add AlphaNumberic Characters to the Actual Tag  
        if (isAlphaNumeric(c)) //isDigit, isAscii can also be used
        {
          tag1 += c;
        }
      }
    }
    if (tag1.length()==12) //Tag Should be completed - 12 chars
    {
      ListTagInArray(tag1);
    }
  }
}

void CheckReader2()
{
  while(Serial2.available()>0)
  {   
    c=Serial2.read();
    //ProcessReadChar();
    if (c==0x02)
    {
      tag2 = "";
    }
    else
    {
      if (c!=0x03) //this is the last character in the tag 
      { //Only Add AlphaNumberic Characters to the Actual Tag  
        if (isAlphaNumeric(c)) //isDigit, isAscii can also be used   
        {
          tag2 += c;
        }
      }
    }
    if (tag2.length()==12) //Tag Should be completed - 12 chars
    {
      ListTagInArray(tag2);
    }
  }
}

void CheckReader3()
{
  while(Serial3.available()>0)
  {   
    c=Serial3.read();
    if (c==0x02)
    {
      tag3 = "";
    }
    else
    {
      if (c!=0x03) //this is the last character in the tag 
      { //Only Add AlphaNumberic Characters to the Actual Tag  
        if (isAlphaNumeric(c)) //isDigit, isAscii can also be used
        {
          tag3 += c;
        }
      }
    }
    if (tag3.length()==12) //Tag Should be completed - 12 chars
    {
      ListTagInArray(tag3);
    }
  }
}

void ProcessXBQ()
{
  long m = millis();
  for (int i = 1; i < 31; i++) {
    if (arrayTags[i] != "")
    {
      if (arraySent[i] < arraySeen[i])  //Check if its been sent since it was last seen
      {
        //Candidate for a Send
        if ((m - arraySent[i]) > 5000)  //If not sent within the last 5 secs then send, otherwise don't
        {
          String msgtoxbee = "!" + arrayTags[i] + "#";
          SerialX.print(msgtoxbee);
          arraySent[i] = m;
          delay(1); 
        }
      }
    }
  }
}

void ListTagInArray(String tag)
{
  //Check if Tag Found
  boolean found = false;
  for (int i = 1; i < 31; i++) {
    if (arrayTags[i] == tag)
    {
      found = true;
      arraySeen[i] = millis();
      break;
    }
  }

  if (!found)
  {
    for (int i = 1; i < 31; i++) {
      if (arrayTags[i] == "")
      {
        found = true;
        arrayTags[i] = tag;
        arraySeen[i] = millis();
        break;
      }
    } 
  }
}

void SwitchAntennae()
{
  active++;
  if (active == 4) active = 1;
  Serial.print("Switch Ant: ");
  Serial.println(active);
  switch (active){
  case 1:
    digitalWrite(ant1, HIGH);
    digitalWrite(ant2, LOW);
    digitalWrite(ant3, LOW);
    digitalWrite(bnt1, HIGH);
    digitalWrite(bnt2, LOW);
    digitalWrite(bnt3, LOW);
    digitalWrite(cnt1, HIGH);
    digitalWrite(cnt2, LOW);
    digitalWrite(cnt3, LOW);
    digitalWrite(dnt1, HIGH);
    digitalWrite(dnt2, LOW);
    digitalWrite(dnt3, LOW);
    break;
  case 2:
    digitalWrite(ant1, LOW);
    digitalWrite(ant2, HIGH);
    digitalWrite(ant3, LOW);
    digitalWrite(bnt1, LOW);
    digitalWrite(bnt2, HIGH);
    digitalWrite(bnt3, LOW);
    digitalWrite(cnt1, LOW);
    digitalWrite(cnt2, HIGH);
    digitalWrite(cnt3, LOW);
    digitalWrite(dnt1, LOW);
    digitalWrite(dnt2, HIGH);
    digitalWrite(dnt3, LOW);
    break;
  case 3:
    digitalWrite(ant1, LOW);
    digitalWrite(ant2, LOW);
    digitalWrite(ant3, HIGH);
    digitalWrite(bnt1, LOW);
    digitalWrite(bnt2, LOW);
    digitalWrite(bnt3, HIGH);
    digitalWrite(cnt1, LOW);
    digitalWrite(cnt2, LOW);
    digitalWrite(cnt3, HIGH);
    digitalWrite(dnt1, LOW);
    digitalWrite(dnt2, LOW);
    digitalWrite(dnt3, HIGH);
    break;
  }
}




