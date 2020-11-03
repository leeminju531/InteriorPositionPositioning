#include <SoftwareSerial.h>
#include <MsTimer2.h>
#include <AtQueue.h>

char str[3000];   //disi 명령어를 통해 읽어들인 문자열을 저장
int cnt = 0;//reading data 배열에 저장을 위한 변수
bool initialFlag; // 초기화시, 작동할 flag. 초기값 = true
bool atFlag;      //at 명령어를 bluetooth에 써줬을때, 작용할 flag. 초기값 = false
bool responseFlag; // at 명령어의 제대로된 응답이 들어왔을때, 작용할 flag. 초기값 = false
volatile int count = -1;
volatile boolean period = false; //스케줄링을 위한 변수.
int rssi[4] = {0,};

// 1,2,3,4 블루투스의 rssi값을 저장할 것
char* response; // AT응답 명령어 저장할 변수
bool extractDataAtStr; //scanning 을 완료했을시, str 문자열을 분석하기 위함
int numForAnalysis;
unsigned int cycle = 0; // 혹시모를 블루투스가 멈추는 상황을 대비한 초기화 주기 체크를 위한 변수

AtQueue instructionQueue,responseQueue;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  MsTimer2::set(300,scheduling);
  MsTimer2::start();
  initialFlag = true;
  atFlag = false;
  responseFlag = false;
}


void loop() {
 if( period == true ){
  period = false;

  switch(count){
    case 0:
      if( atFlag == true ){
        isResponseComplete();
      }
      break;
    case 1:
      if(initialFlag == true){
      initialFlag = false;
      // Queue 초기화
      instructionQueue.reset();
      responseQueue.reset();    

      //bluetooth Scanning Mode 설정 queue에 저장.
      instructionQueue.pushQueue("AT");
      responseQueue.pushQueue("OK");
      instructionQueue.pushQueue("AT+IMME1");
      responseQueue.pushQueue("OK+Set:1");
      instructionQueue.pushQueue("AT+ROLE1");
      responseQueue.pushQueue("OK+Set:1");
      
      atFlag = false;
      responseFlag = false;
      }else{

        
        if( responseFlag == true){
        responseFlag = false;
        atFlag = false;
        if(extractDataAtStr == true){
          extractDataAtStr = false;
          int check = Analysis(numForAnalysis, rssi);
          if(check == -1){
           //모든 타워 인식 실패
           Serial.println(-1);
          }else if( check == 0){            
            // 모든 타워 인식 성공
//            Serial.println();
//            Serial.println(" ㅇㅇ 모두 성공 ");
//            Serial.print("1번 타워 : ");
            Serial.print("/"); // 지정된 데이터를 확인하기위한 구분자로 사용될 것임.
            Serial.print(rssi[0]);
//            Serial.print("2번 타워 : ");
            Serial.print(rssi[1]);
//            Serial.print("3번 타워 : ");
            Serial.print(rssi[2]);
//            Serial.print("4번 타워 : ");
            Serial.println(rssi[3]);

         
            
            
          }
        }
        
        if(instructionQueue.isEmpty()){
          instructionQueue.pushQueue("AT+DISI?");
          responseQueue.pushQueue("OK+DISCE");
        }

        
       
     }
    }
     
     case 2: 
      break;
      
    case 3:  //at 명령어 (가능할 시) 수행
      if(atFlag == false) // atFlag =false -> at명령어 안쓴 상태(이미 썻는데 또 쓰기 방지)
        if(instructionQueue.isEmpty() == false){ // 수행할 명령어가 queue에 있을때만 써준다.
          char* instruction = instructionQueue.popQueue();
          Serial1.write(instruction);
//            Serial.print(instruction);
//            Serial.println(" commanded to bluetooth");
          response = responseQueue.popQueue();
//            Serial.print(response);
//            Serial.println(" 명령어 대기");
          atFlag = true; //atFlag를 통해 명령어 써줬음을 알린다.
          Serial.print(instruction);
          Serial.println("명령어 지시");
        }
      cycle++; // case 맨 마지막 cycle 증가 시키기(응답 x 경우 check를 위함)

      break;
      
    
   }
 }
}








// 블루투스의 명령어가 응답을 완료하였는지 확인하는 함수
// 블루투스 명령어 응답 -> responseFlag = true 
// 블루투스 명령어 일정시간 무응답 -> initialFlag = ture
void isResponseComplete(){
  int responseSize = strlen(response);
  int count = 0;

  if( cnt >= responseSize )
    for (int i = 0; i < responseSize ; i++)
      if(response[i] == str[cnt - responseSize + i])
        count++;
      else
        count = 0;

  if(count == responseSize){

    
    if(response == "OK+DISCE"){
      extractDataAtStr = true;
      numForAnalysis = cnt;
      }
    cycle = 0;
    count = 0;
    cnt = 0;
    responseFlag = true; 

  }

  if( cycle >= 6 && responseFlag == false ){ //10은 임의의 값. 위 시간동안 정상응답 없을 시, 초기화.
    cycle = 0;
    initialFlag = true;
   } 
}

void scheduling(){
  count = (count+1)%4; 
  period = true;
}

void serialEvent1(){
  str[cnt] = Serial1.read();
//  Serial.print(str[cnt]);
  cnt++;
}



// rssi배열에 각각 고정 블루투스들의 rssi 값을 저장한다.
//rssi[0] = 1번 블루투스의 rssi 값 .... rssi[3] = 4번 블루투스의 rssi 값

// 인식 실패한 블루투스가 존재할 시, -1을 반환한다.

int Analysis(int num,int* rs){

  int rssi[4] = {-1,-1,-1,-1};
  String trans = "   ";
  
  for( int i = 0; i<num ; i++ ){ // 각 tower uuid 에 대응하는 rssi 값을 추출한다.
    if(str[i] == '5' && str[i-1] == 'D' && str[i-2] == 'D' && str[i-3] == '1' && str[i-4] =='C'){
      trans.setCharAt(0,str[i+4]);
      trans.setCharAt(1,str[i+5]);
      rssi[0] = trans.toInt();
//      Serial.println("Tower1 Recognition Success ");
    }   
    else if(str[i] == 'D' && str[i-1] == '1' && str[i-2] == '0' && str[i-3] == '1' && str[i-4] =='D'){
      trans.setCharAt(0,str[i+4]);
      trans.setCharAt(1,str[i+5]);
      rssi[1] = trans.toInt();
//      Serial.println("Tower2 Recognition Success ");
    }
    else if(str[i] == '4' && str[i-1] == 'C' && str[i-2] == '3' && str[i-3] == '1' && str[i-4] =='D'){
      trans.setCharAt(0,str[i+4]);
      trans.setCharAt(1,str[i+5]);
      rssi[2] = trans.toInt();
//      Serial.println("Tower3 Recognition Success ");
    } 
    else if(str[i] == '4' && str[i-1] == 'C' && str[i-2] == 'B' && str[i-3] == 'F' && str[i-4] =='B'){
      trans.setCharAt(0,str[i+4]);
      trans.setCharAt(1,str[i+5]);
      rssi[3] = trans.toInt();
//      Serial.println("Tower4 Recognition Success ");

    } 
  }

  for(int i=0 ; i<4 ; i++) // 만약, rssi[5] 값 중 여전히 -1인 것이 존재한다면, 모든 tower 를 scanning 하지 못함을 의미한다.
    if(rssi[i] == -1)
      return -1; 

  for(int i=0 ; i<4 ; i++) // 만약, rssi[5] 값 중 여전히 -1인 것이 존재한다면, 모든 tower 를 scanning 하지 못함을 의미한다.
    rs[i] = rssi[i];

  return 0;

}
