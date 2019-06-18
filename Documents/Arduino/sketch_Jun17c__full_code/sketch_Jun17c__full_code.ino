#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <Time.h>
#include <string.h>
//Our Wi-Fi ssdid and password
//const char* ssid="RVCE_CISCO_IOT";
//const char* password = "Rvce@Cisco123";
//const char* ssid="Nammane-Hall-2.5Ghz";
//const char* password = "sui-tapak";
//const char* ssid="sam";
//const char* password = "samp4545";

const char* ssid="gls";
const char* password = "nammane1844";

//const char* ssid="ondhu";
//const char* password = "1234545123";

boolean pillDispensed = false;

boolean dispensedPillTaken = true;
float cutOffVoltageLDR = 0.80;
const int ledCount = 15;
String lastPillDispensedAt;
String lastPillTakenAt;
String missedDataSentFor="";
String pillNameAndItsTime = "";

String pillNotTakenPill_name;
int pillNotTakenPill_no;
String pillNotTakenAlarm_at;

boolean ledPinOne = true;

#define ledPin1  D4// the number of the LED pin
#define ledPin2  D2// the number of the LED pin

#define FIREBASE_HOST "smartmedicinedispenser-1a95d.firebaseio.com"
#define FIREBASE_AUTH "SUs3GNR0ze4LQXnwmRZVKR5ILiVwhX4BEVw7RJLu"

//#define FIREBASE_HOST "mysmartdispenser.firebaseio.com"
//#define FIREBASE_AUTH "tAKoBT0BeDIq9YL17DcAcxgdJhoc2SVZcc9rscTf"


//StepperMotorGlobalVariablesBegin
#define IN1  D5
#define IN2  D6
#define IN3  D7
#define IN4  D8
int Steps = 0;
boolean Direction = true;// gre
unsigned long last_timee;
unsigned long currentMillis ;
int steps_left=0;
long timee;
int rotatedAt;
//StepperMotorGlobalVariablesEnd


void setup() {
//  initializePin(); //call function
  //Making Connection With netword
  WiFi.begin(ssid, password);
  Serial.begin(9600);
  Serial.print("Searching Connection");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());

  initializeFireBase();

  //StepperMotor Setup PINOUT Begin
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT); 
  pinMode(IN4, OUTPUT); 
  //StepperMotor Setup PINOUT End

  pinMode(ledPin1, OUTPUT); 
  pinMode(ledPin2, OUTPUT); 

//  digitalWrite(ledPin1, LOW);
//  digitalWrite(ledPin2, LOW);

//Time setup begin
  int timeZ = 5 * 3600;
  configTime(timeZ, 1800, "pool.ntp.org", "time.nist.gov");
 
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
//Time setup end

  
}
//Stepper Motor Logic Begin
void rotateStepperMotorToNextSlot(){
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  steps_left=512;
  Serial.print("micros() ---> ");
  Serial.println(micros());
  while(steps_left>0){
  currentMillis = micros();
 
  if(currentMillis-last_timee>=1000){
  stepper(1); 
  timee=timee+micros()-last_timee;
  last_timee=micros();
  steps_left--;
  }
  }
   Serial.println(timee);
  Serial.println("Wait...!");
  delay(2000);
//  Direction=!Direction;
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
}
void stepper(int xw){
  delay(10);
  for (int x=0;x<xw;x++){
    
switch(Steps){
   case 0:
     
//    digitalWrite(ledPin1, LOW);
//    digitalWrite(ledPin2, HIGH);   
    Serial.println("case 0");
     digitalWrite(IN1, LOW);  
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
   break; 
   case 1:
//   digitalWrite(ledPin1, HIGH);
//    digitalWrite(ledPin2, LOW);   
   Serial.println("case 1");
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, HIGH);
   break; 
   case 2:
//   digitalWrite(ledPin1, LOW);
//    digitalWrite(ledPin2, HIGH);   
   Serial.println("case 2");
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
   break; 
   case 3:
//   digitalWrite(ledPin1, HIGH);
//    digitalWrite(ledPin2, LOW);   
   Serial.println("case 3");
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
   break; 
   case 4:
//   digitalWrite(ledPin1, LOW);
//    digitalWrite(ledPin2, HIGH);   
   Serial.println("case 4");
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 5:
//   digitalWrite(ledPin1, HIGH);
//    digitalWrite(ledPin2, LOW);   
   Serial.println("case 5");
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
     case 6:
//     digitalWrite(ledPin1, LOW);
//     digitalWrite(ledPin2, HIGH);   
     Serial.println("case 6");
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 7:
//   digitalWrite(ledPin1, HIGH);
//    digitalWrite(ledPin2, LOW);   
   Serial.println("case 7");
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
   break; 
   default:
//   digitalWrite(ledPin1, LOW);
//   digitalWrite(ledPin2, LOW);
   Serial.println("case default");
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
}
SetDirection();
}
} 
void SetDirection(){
 
if(Direction==1){ Steps++;}
if(Direction==0){ Steps--; }
if(Steps>7){Steps=0;}
if(Steps<0){Steps=7; }
}
//Stepper Motor Logic End
void initializeFireBase(){
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int getNowTime(){
  time_t now = time(nullptr);
  String hour = getValue(getValue(ctime(&now), ':', 0), ' ', 3);
  String minutes = getValue(ctime(&now), ':', 1);

  int hourInt = hour.toInt();
  hourInt = hourInt*100;
  int minutesInt = minutes.toInt();
  int timeeeee = hourInt+minutesInt;
  return timeeeee;
  }
  void setPillTakenForLastPill(){
    String node = "/history";
    FirebaseObject object = Firebase.get(node);
    JsonObject& history = object.getJsonVariant();
    Serial.println("Now getting the history");
    for (auto kv : history) {
        Serial.println(kv.key);
        JsonObject& historyItems = history[kv.key];
        String alarm_at = historyItems["alarm_at"];
        int pill_no = historyItems["pill_no"];
        String pill_name = historyItems["pill_name"];

        
        String pill_taken_at = historyItems["pill_taken_at"];

  
        if(alarm_at==lastPillDispensedAt){
          Serial.print("Got the object for ");
          Serial.println(lastPillDispensedAt);
            time_t now = time(nullptr);
            String alarmAtFull = ctime(&now);
            Serial.print("alarm_at-> ");Serial.println(alarm_at);
            Serial.print("pill_no-> ");Serial.println(pill_no);
            Serial.print("pill_name-> ");Serial.println(pill_name);
            Serial.print("pill_taken_at-> ");Serial.println(alarmAtFull);
            
            StaticJsonBuffer<200> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["pill_no"] = pill_no;
            root["pill_name"] = pill_name;
            root["alarm_at"] = alarm_at;
            root["pill_taken_at"] = alarmAtFull;
            
            String atNode = node + "/" +kv.key;
            Serial.print("Altering the data at --> ");
            Serial.print(atNode);
            Firebase.set(atNode, root);
        }
        else Serial.println("Not this object, going to next object");

        
    }
    }
  void checkIfPillTaken(){
           
  int sensorValue = analogRead(A0);   // read the input on analog pin 0

  float voltage = sensorValue * (5.0 / 1023.0);   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)
  if(pillDispensed == true){
  if(voltage  > cutOffVoltageLDR){
    Serial.println("PILL TAKEN");
//    putDataToFirebaseHistory();
    pillDispensed = false;
    dispensedPillTaken = true;
    setPillTakenForLastPill();
  }
  else
    Serial.println("PILL NOT TAKEN");
  }

//  Serial.print("Voltage -> ");
//  Serial.println(voltage);   // print out the value you read
  
   }
void loop() {
  
  checkIfPillTaken();
  
  //Time begin
  
  delay(1000);
  //Time End
  
  getDataFromFireBase();
  delay(3000);
}
//Tue Apr 16 21:12:18 2019
void putDataToFirebaseHistory(int pill_no, String pill_name, String alarm_at, String missedOrFresh){
        if(missedOrFresh == "missed"){
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["pill_no"] = pill_no;
        root["pill_name"] = pill_name;
        root["alarm_at"] = alarm_at;
        root["pill_taken_at"] = missedOrFresh;
        // append a new value to /logDHT
        String name = Firebase.push("/history/", root);
          }
        else{
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["pill_no"] = pill_no;
        root["pill_name"] = pill_name;
        root["alarm_at"] = alarm_at;
        root["pill_taken_at"] = "-500";
        String name = Firebase.push("/history/", root);
        
        }
     }
void getDataFromFireBase(){
//  Serial.println("Am inside getDataFromFireBase()");
  String alarm_at;
  for(int i=1;i<8;i++){
  String index = "/test/pill_";
  index += i;
//  Serial.print("Getting data from path - > ");
//  Serial.println(index);
 
  FirebaseObject object = Firebase.get(index);
  String pill_name = object.getString("pill_name");
  alarm_at = object.getString("alarm_at");
//  pillNameAndItsTime+=pill_name;
//  pillNameAndItsTime+="=";
//  pillNameAndItsTime+=alarm_at;
//  pillNameAndItsTime += ",";
  int pill_no = object.getInt("pill_no");
  if(pill_no>0){
    Serial.print("Object Pill No: ");
    Serial.print(pill_no);
    Serial.print("\tObject Pill Name: ");
    Serial.print(pill_name);
    Serial.print("\tObject Alarm at: ");
    Serial.println(alarm_at);
    
    calculateAndRotateStepperMotor(pill_no,pill_name,  alarm_at);
  }
  else {
    Serial.print ("Pill obtained is ");
    Serial.println(pill_no);
    }
}

//  int nowTime = int(hour()*100 +minute());

  int nowTime = getNowTime();
  int fromTime = alarm_at.toInt();
  int nowPlusThreeMins = fromTime + 1;
  Serial.print("nowTime: ");
  Serial.println(nowTime);
//  Serial.println("fromTime");
//  Serial.println(fromTime);
  }
  
  void calculateAndRotateStepperMotor(int pill_no, String pill_name, String alarm_at){
//    int nowTime = int(hour()*100 +minute());
    time_t now = time(nullptr);
    String alarmAtFull = ctime(&now);
//    Serial.print("alarmAtFull--> ");
//    Serial.println(alarmAtFull);
    int nowTime = getNowTime();
    int fromTime = alarm_at.toInt();
    if(fromTime==nowTime && rotatedAt!=fromTime && dispensedPillTaken==true){
      rotatedAt = fromTime;
      pillDispensed = true;
      dispensedPillTaken = false;
      lastPillDispensedAt = alarmAtFull;
      pillNotTakenAlarm_at = alarmAtFull;
      Serial.print("2.  alarmAtFull-->");
      Serial.print(alarmAtFull);
      pillNotTakenPill_name = pill_name;
      pillNotTakenPill_no = pill_no;
      putDataToFirebaseHistory(pill_no, pill_name, alarmAtFull, "fresh");
      rotateStepperMotorToNextSlot();//Rotating Stepper Motor to next slot
  }
  else if(fromTime==nowTime && rotatedAt!=fromTime && dispensedPillTaken==false){
        Serial.println("Pill dispense missed since previous pill is not taken");
         if(!checkIfDataSentAlready(String(getNowTime()))){//false will satify condn
              if(missedDataSentFor==""){
                missedDataSentFor += String(getNowTime());
              }
              else  { 
                missedDataSentFor += "," + String(getNowTime());
                
              }
//                pillNameAndItsTime+= pill_name;
//                pillNameAndItsTime+="=";
//                pillNameAndItsTime+=String(getNowTime());
//                pillNameAndItsTime+=",";
              Serial.print("Missed data not sent for --> ");
              Serial.print(String(getNowTime()));
              Serial.println(" so sending it now");
              Serial.print(" 1. alarmAtFull-->");
              Serial.print(alarmAtFull);
              Serial.println(" 1. pill_name-->");
              Serial.print(pill_name);
              Serial.println(" Now sending for above^^");
              putDataToFirebaseHistory(pill_no, pill_name , alarmAtFull, "missed");
         }
         else{//true will satify condn
            Serial.print("Data already sent for ->  ");
            Serial.println(String(getNowTime()));
         }
    }
  }

  boolean checkIfDataSentAlready(String pillNotTakenAlarm_at){

    if(missedDataSentFor.indexOf(",") > 0){
    for(int i=1;i<10000;i++){
        String timeDate = getValue(missedDataSentFor, ',', i);
        if(timeDate=="") return false;
        else if(timeDate == pillNotTakenAlarm_at ) return true;
    }
  }
  else if(pillNotTakenAlarm_at==missedDataSentFor) return true;
        else return false;
    }
void getPillName(){
  
  }
void updateFirebaseForPillTaken(){
  
  }    
