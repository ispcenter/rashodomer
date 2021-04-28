int KeyOutPin = 2;
int KeyInPinOk = 3;
int KeyInPinDown = 4;
int KeyInPinUp = 5;
int KeyInPinBefore = 6;
int KeyInPinNext = 7;

void setup(){
  pinMode(KeyOutPin, OUTPUT);       //4-й пин сконфигурировать как выходной
    
  pinMode(KeyInPinOk, INPUT);        //6-й пин сконфигурировать как входной, кнопка "2"
  digitalWrite(KeyInPinOk, HIGH);    //Подтянуть к высокому уровню
  
  pinMode(KeyInPinBefore, INPUT);
  digitalWrite(KeyInPinBefore, HIGH);
  
  pinMode(KeyInPinUp, INPUT);
  digitalWrite(KeyInPinUp, HIGH);
  
  pinMode(KeyInPinDown, INPUT);
  digitalWrite(KeyInPinDown, HIGH);

  pinMode(KeyInPinNext, INPUT);
  digitalWrite(KeyInPinNext, HIGH);
  
  Serial.begin(9600);               //Открыть последовательный порт для передачи сообщений в монитор порта
}

void loop(){
  if(!digitalRead(KeyInPinOk)) {Serial.println("OK pressed");} //Считывание порта - если уровень подтянулся к низкому значит нажата кнопка, и так со всеми портами
  if(!digitalRead(KeyInPinNext)) {Serial.println("NEXT pressed");}
  if(!digitalRead(KeyInPinBefore)) {Serial.println("BEFORE pressed");}
  if(!digitalRead(KeyInPinUp)) {Serial.println("UP pressed");}
  if(!digitalRead(KeyInPinDown)) {Serial.println("DOWN pressed");}
}
