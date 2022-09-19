int L = 6, R = 8; // 左右馬達腳位
int trigPin = 13; // 超音波輸出及接收腳位
int echoPin = 12;
int condition = 0;
 /* 目前循線感光只有用到 L1 及 R1，因為經測試判斷如果四顆感光都用上會造成感測過於靈敏
  *  循線感光由左至右分別是：L2, L1, R1, R2
  *  其連接角為由左至右分別是：A4, A3, A1, A2
  */
int L1 = A3, L2 = A4;
int R1 = A1, R2 = A2;
int motor = 255; // 馬達供電
int dis = 100;

void setup(){
  pinMode(L, OUTPUT);
  pinMode(R, OUTPUT);
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
}

void distance(){
  /*
   * <Concept>
   * 超音波由 trig 發送並由 echo 進行接收
   * 首先把 trig 設定高電位表示發送超音波然後把他關掉
   * 
   */
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    pinMode(echoPin, INPUT);             // 讀取 echo 的電位
    int result = pulseIn(echoPin, HIGH);   // 音速與公分換算：1cm = 58.1 音波單位
    if(result!=0) dis = result; // 把零過濾掉(原因不明)
//    Serial.println(dis);
}

int Read(int n){     // 回傳路線狀態
  return analogRead(n);
}

void go(){ // 馬達供電
  analogWrite(L, motor);
  analogWrite(R, motor);
}

void stop(){ // 馬達斷電
  analogWrite(L, 0);
  analogWrite(R, 0);
}

void line(){ // 循線
  condition++; // 循線次數 +1
  go();
  while(Read(L1)<900||Read(R1)<900){ // 白線的讀取數值必大於900，因此若小於900表示偵測到黑線
    stop(); // 先把左右馬達斷電
    if(Read(L1)<900){ // 如果 L1 壓到黑線右馬達供電
      analogWrite(R, motor);
    }
    if(Read(R1)<900){ // 如果 R1 壓到黑線左馬達供電
      analogWrite(L, motor);
    }
    delay(15);
  }
  delay(18); // 馬達供電 n 毫秒後斷電避免車子一下子走太遠
  stop();
  delay(15);
}

void loop(){
  /* 經測試判斷若在循線實作更新dis的動作會導致循線任務遲緩
   * 因此利用 condition 紀錄循線任務執行次數
   * 每次執行 line() 就讓 condition +1
   * 若 condition = 指定次數就讓 condition 歸零並執行超音波感測
   * 若超音波回傳值達到指定距離變 stop() 直到此判斷式不成立
   */
  if(condition > 10){
    condition = 0;
    distance(); // 更新超音波距離
    while(dis <= 1220){ // 如果 dis 值小於等於 20 馬達斷電
      stop();
      delay(1000);
      distance();
    }
  }
  line(); // 開始循線
}
