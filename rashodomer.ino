/*
  Rashodomer
  By: Klykov Leonid
  Development started 03.02.2021
  Расчётный объём ёмкости 2.576 [л]
*/

//0 Подключение библиотек
#include <Arduino.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


//1 Назначение пинов

//1.1 Клавиатура
#define KeyOutPin 2
#define KeyInPinOk 3
#define KeyInPinDown 4
#define KeyInPinUp 5
#define KeyInPinBefore 6
#define KeyInPinNext 7

//1.2 Весы
#define SCK_PIN  8
#define DOUT_PIN  9

//1.3 Клапан заполнения и бузер
#define Fill_pin 10
#define buzzerPin 12

//2 объявление констант и переменных

const float Mavar = 900.0; //[г] - аварийный максимум
const float Mmax = 200.0; //[г] - рабочий максимум
const float Mmin = 52.0; //[г] - рабочий минимум
float mc = 889; //[] - наклон калибровки. смещение устанавливается тарировкой при полностью слитом топливе
float m;
float G;
const float Gzap = 0.05; //[л/с] расход заполнения, надо сделать хранение в EEPROM https://alexgyver.ru/lessons/eeprom/
const float ro = 0.85; //[г/мл] - средняя плотность дизельного топлива
const int Tizm = 5000; //[мс] - период измерений расхода
const int Nizm = 50; //[] - количество измерений за период Tizm


// const float Gminus = 0.01; //[л/с] для эмуляции расхода
float mContainer[2] = {0, 0}; // массив для сохранения данных с интервалом Tizm


//3 Активация библиотек
LiquidCrystal_I2C lcd(0x27, 20, 4);
HX711 scale;

//4 Логика
void setup() {
  Serial.begin(9600);

  pinMode(SCK_PIN, INPUT);
  pinMode(DOUT_PIN, INPUT);
  pinMode(Fill_pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(Fill_pin, 0); // close input valve

  // Включим измерение веса
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(mc); //Adjust to this calibration factor

  m = scale.get_units(); //[г]

  // Активируем экран. Приветствие. Проверка адекватности весов
  lcd.begin(); lcd.backlight(); Z_signal(3);
  lcd.setCursor(0, 0); lcd.print("Hello!");
  lcd.setCursor(0, 1); lcd.print(m);
  delay(3000); lcd.clear();
  weightCheck();
}

void loop() {
  Serial.println("new loop");

  m = scale.get_units();
  float count = 0.0;
  

  // Случай А - [опасно] топливо выше аварийного уровня. Расход тоже надо мерить
  while (m >= Mavar) {
    digitalWrite(Fill_pin, 0); // close input valve (вдруг он залип и со второго раза закроется)
    lcd.print("avary! "); lcd.print("m= "); lcd.print(m);
    Serial.print("avary! "); Serial.print("m= "); Serial.println(m);
    delay(Tizm / Nizm);
    lcd.clear();
  }

    
  // Случай Б - топливо выше рабочего минимума: тратим и меряем расход.
  Serial.print("rabo4 kol topl. tratim"); Serial.println(count);
  while (m > Mmin && m < Mavar) {
    count += Tizm / Nizm;
    m = scale.get_units();
    Serial.print(", m= "); Serial.println(m);
    

    if (count == Tizm) {
      
      Z_signal(1);
      m = scale.get_units();
      mContainer[1] = m;
      lcd.clear();
      Serial.print(mContainer[0]); Serial.print(", "); Serial.println(mContainer[1]);
      G = (mContainer[0] - mContainer[1]) * ro * 60 / Tizm; // [л/мин]
      mContainer[0] = m;
      lcd.print(G, 3); lcd.setCursor(6, 0); lcd.print(" l/min"); // первая строка дисплея
      Serial.print("G="); Serial.println(G, 3);
      
      count = 0.0;
    }
    delay(Tizm / Nizm);
  }

  // Случай В - уровень опустился до рабочего минимума или ниже: заполняем ёмкость (мерить тоже надо) Управление клапаном нужно только здесь
  if (m <= Mmin) {
    digitalWrite(Fill_pin, 1); // open input valve

    // заполняем, пока уровень меньше рабочего максимума
    count = 0.0;
    boolean first = true;
    int cc = 0; //дополнительный счётчик сколко раз выполнилось условие показа расхода на экран
    // int fc = 0;
    // int fcg = 0;

    while (m < Mmax) {
      count = round((count + Tizm / Nizm) * 10.0) / 10.0;

      m = scale.get_units();
      Serial.print("zapolnRem"); Serial.print(", m= "); Serial.println(m);

      // первый раз информацию нужно показать на экране сразу. Второй и последующие разы экран обновлять 1 раз в 5с
      if (count == Tizm || first == true) {
        cc += 1;
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Filling:"); lcd.print((m - Mmin) / (Mmax - Mmin) * 100); lcd.print("%  ");
        lcd.setCursor(0, 1); lcd.print("m="); lcd.print(m);
        lcd.setCursor(0, 2); lcd.print("cc"); lcd.print(cc);
        first = false;
      }

      delay(Tizm / Nizm); // задержка между измерениями m
    }
    digitalWrite(Fill_pin, 0); // close input valve
    lcd.clear();
    Z_signal(2);
  }
}


//5 functions

void Z_signal(int n) { // аргумент: сколько раз пикнуть
  for (int i=0; i<n-1; i++){
    tone(buzzerPin, 1500, 50); //tone(pin, frequency [hertz], duration [milliseconds])
    delay(100); 
  }
  tone(buzzerPin, 1500, 50);
}



// функции, которые ещё не применены!


float getAvgG(float arr[10], float Gzap) {
  int arrlen = sizeof(arr)/sizeof(arr[0]);
  float Dsum = 0;
  
  for(int i=1; i<=arrlen; i++) {
    Dsum += arr[i-1] - arr[i];
  }
  float Davg = Dsum / (arrlen - 1);
  return Davg * ro * 60 / Tizm - Gzap;
}



float calibration() {
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  return mc;
}


//калибровка множителя весов
void menu_mc_cal() {
  lcd.clear();
  lcd.print("---mc calibration---");
  Serial.print("---mc calibration---");

  lcd.setCursor(0, 1); //col, row
  lcd.print("mc=");
  lcd.print(mc);

  lcd.print("m=");
  lcd.print(m);
  lcd.print(" g");
}



//Проверка при включении: если вес меряется явно неверный, то необходимо калибровать прямо сейчас
void weightCheck() {
  while (m < -1 || m > 5000){
    lcd.setCursor(0, 2); // 3) string. lcd.setCursor(col, row)
    lcd.print("m=");
    lcd.setCursor(2, 1);
    lcd.print(m);
    lcd.setCursor(17, 1);
    lcd.print("kg");
    lcd.setCursor(0, 3);
    lcd.print("Need calibration!"); // 3) string
    //  calibration(); асинхронная функция, ждать её выполнения
    m = scale.get_units();
  }
}



// проверка работы клапана
void valveCheck() {
  delay(2000);
  float mn = scale.get_units(); // новый вес топлива
  
  //  если заполнение не началось
  if(mn < 1.01*m) {
    lcd.print("Filling problem");
    // antiZalip();
    delay(2000);
    if(mn < 1.01*m) {
    lcd.print("Filling ERROR!"); // Если вторая попытка заполнения не удалась, прога останавливается
    }
   // делаем ремонт, затем переходим в меню проблем и нажимаем исправлено, прога продолжит работу
  }
}

// void antiZalip(bool target, int count) { // если клапан залип, пробует открывать-закрывать его 3 раза. Надо сделать в зависимости от направления отслеживать изменение и прерывать дергание если клапан заработал чтобы не ждать лишнее время
//   digitalWrite(Fill_pin, 0); // close input valve
//   delay(1000);
//   digitalWrite(Fill_pin, 1); // open input valve
//   delay(1000);
//   digitalWrite(Fill_pin, 0); // close input valve
//   delay(1000);
//   digitalWrite(Fill_pin, 1); // open input valve
//   delay(1000);
//   digitalWrite(Fill_pin, 0); // close input valve
//   delay(1000);
//   digitalWrite(Fill_pin, 1); // open input valve
//   for (int i = 0; i < count; i++)
//   {
//     /* code */
//   }
  
// }