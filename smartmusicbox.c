#include "Nextion.h"
#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h>
#include <SFEMP3Shield.h>

SdFat sd;

SFEMP3Shield MP3player;

const int LED = 23;
const int LED1 = 22;
const int LED2 = 24;

int seconds = 0;
int minutes = 55;
int hours = 23;

int val = 0;
int val1 = 0;
int val2 = 0;
int val3 = 0;
int val4 = 0;
int val5 = 0;
char msg[100];

NexButton b0 = NexButton(1,5,"b0"); // 취침모드 실행
NexButton b1 = NexButton(3,20,"b8"); // 취침모드 종료 후 홈
NexButton b2 = NexButton(3,10,"b4"); // 취침 타이머 종료후 알람 시작
NexButton b3 = NexButton(6,3,"b5"); // 알람 종료 버튼
NexButton b4 = NexButton(1,6,"b1");
NexButton b5 = NexButton(2,14,"b9");
NexButton bt0 = NexButton(2,3,"bt0"); //무드등 ON/OFF 버튼
NexButton bt1 = NexButton(2,4,"bt1"); //MP3 시작// 정지
NexTouch *nex_listen_list[] =
{
  &b0, &b1, &b2, &b3, &bt0, &bt1, &b4, &b5,
  NULL
};

void b0PopCallback(void *ptr){
  val++;
  Serial.println(val);
  if(val == 1){
    MP3player.playTrack(2);
    val =0;
  }
}

void b1PopCallback(void *ptr){
    MP3player.stopTrack();
}

void b2PopCallback(void *ptr){
  val4++;
  if(val4==1){
    MP3player.stopTrack();
    MP3player.setVolume(0,0);
    delay(1000);
    val4++;
  }
  if(val4==2){
    MP3player.playTrack(3);
    val4=0;
  }
}

void b3PopCallback(void *ptr){
  MP3player.stopTrack();
  MP3player.setVolume(20,20);
}

void b4PopCallback(void *ptr){
  MP3player.playTrack(1);
  digitalWrite(LED,HIGH);
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
}

void b5PopCallback(void *ptr){
    MP3player.stopTrack();
    digitalWrite(LED, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
}

void bt0PopCallback(void *ptr){
  val2++;
  if(val2==1){
    digitalWrite(LED, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
  }
  if(val2==2){
    digitalWrite(LED, HIGH);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    val2=0;
  }
}

void bt1PopCallback(void *ptr){
  val3++;
  if(val3==1){
    MP3player.pauseMusic();
  }
  if(val3==2){
    MP3player.resumeMusic();
    val3=0;
  }
}

void setup(){
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);//블루투스
  Serial2.begin(9600);//Nextion

  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();
  MP3player.setVolume(20,20);//볼륨 설정

  b0.attachPop(b0PopCallback);
  b1.attachPop(b1PopCallback);
  b2.attachPop(b2PopCallback);
  b3.attachPop(b3PopCallback);
  b4.attachPop(b4PopCallback);
  b5.attachPop(b5PopCallback);
  bt0.attachPop(bt0PopCallback);
  bt1.attachPop(bt1PopCallback);
}

void loop(){

  #if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )

  MP3player.available();
#endif

  
  if(hours >=0){
  delay(1000);
  seconds++;
  Serial2.print("n1.val=");
  Serial2.print(minutes);
  Serial2.write(0xff); //0xff *3 = 아두이노 >> 넥션
  Serial2.write(0xff);
  Serial2.write(0xff);
  
  
  Serial2.print("n2.val=");
  Serial2.print(hours);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  }
  if(seconds==59){
    minutes++;
    seconds = 0;
  }
  if(minutes==60){
    hours++;
    minutes = 0;
  }
  delay(30);

  if(Serial1.available()){   // 블루투스 통신 받기
    Serial.write(Serial1.read()); // 앱 인벤터에서 날씨정보 받기
  }
  if(Serial1.available()){
    Serial1.write(Serial.read());
  }
  val5 = Serial.read();
  sprintf (msg, "t0.txt=\"%d \"%c%c%c",val5,0xff,0xff,0xff); // 날씨정보 >> Nextion
  Serial1.print(msg);
  
  
  nexLoop(nex_listen_list);
}