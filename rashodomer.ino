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

const float ro = 0.85; //[г/мл] - средняя плотность дизельного топлива
const double Tizm = 2.000; //[с] - период измерений расхода
const double Nizm = 10.000; //[] - количество измерений за период Tizm

float mc = 889; //[] - наклон калибровки. смещение устанавливается тарировкой при снятой с датчика ёмкости
const float Gzap = 0.05; //[л/с] расход заполнения

const float Gplus = 0.05; //[л/с] для эмуляции заполнения
const float Gminus = 0.01; //[л/с] для эмуляции расхода


//3 Активация библиотек
LiquidCrystal_I2C lcd(0x27, 20, 4);
HX711 scale;

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

  float m = getM(); //[г]

  // Активируем экран
  lcd.begin(); lcd.backlight();
  tone(buzzerPin, 1500, 50); //tone(pin, frequency [hertz], duration [milliseconds])
  delay(100);
  tone(buzzerPin, 1500, 50);
  lcd.setCursor(0, 0); lcd.print("Hello!");
  lcd.setCursor(0, 1); lcd.print(m);
  delay(3000);
  lcd.clear();


}

void loop() {
  Serial.println("new loop");

  float m = getM();
  float count = 0.0;
  float G = 0.0;

  // Случай А - топливо выше аварийного уровня. Расход тоже надо мерить
  if(m >= Mavar){
    digitalWrite(Fill_pin, 0); // close input valve

    while (m >= Mavar) {
      lcd.print("avary! "); lcd.print("m= "); lcd.print(m);
      Serial.print("avary! "); Serial.print("m= "); Serial.println(m);
      delay(1000);
      lcd.clear();
    }
  }
  
  float massContainer[2] = {m, m}; // массив для сохранения данных с интервалом Tizm
  
  // Случай Б - топливо выше рабочего минимума: тратим и меряем расход
  while (m > Mmin) {
    digitalWrite(Fill_pin, 0); // close input valve
    count += Tizm / Nizm;
    m = getM();
    boolean qq = (count==0.2 || count==0.4 || count==0.6 || count==0.8 || count==1.0 || count==1.2 || count==1.4 || count==1.6 || count==1.8 || count==2.0);
    Serial.print("tratim "); Serial.print("count="); Serial.print(count);
    if (qq){Serial.print(" true ");}
    else{Serial.print(" false ");};
    Serial.print(count==0.2 || count==0.4 || count==0.6 || count==0.8 || count==1.0);
    Serial.print(", m= "); Serial.println(m);
    

    if (count == Tizm) {
      Serial.print("newRashod"); Serial.println(count);
      tone(buzzerPin, 1500, 50);
      m = getM();
      massContainer[1] = m;
      lcd.clear();
      G = (massContainer[0] - massContainer[1]) * ro * 60 / Tizm / 1000; // [л/мин]
      massContainer[0] = m;
      lcd.print("G=");
      lcd.setCursor(2, 0);
      lcd.print(G);
      lcd.setCursor(6, 0);
      lcd.print(" l/min");
      count = 0.0;
    }
    delay(1000);
  }

  // Случай В - уровень опустился до рабочего минимума или ниже: заполняем ёмкость (мерить тоже надо)
  if (m <= Mmin) {
    digitalWrite(Fill_pin, 1); // open input valve



    // заполняем, пока уровень меньше рабочего максимума
    count = 0.0;
    boolean first = true;
    int cc = 0;
    // int fc = 0;
    // int fcg = 0;

    while (m < Mmax) {
      count = round((count + Tizm / Nizm) * 10.0) / 10.0;

      m = getM();
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

      delay(Tizm / Nizm * 1000 * 5);
    }
    tone(buzzerPin, 1500, 50);
    delay(100);
    tone(buzzerPin, 1500, 50);
    delay(100);
    tone(buzzerPin, 1500, 50);
  }
}

//functions


float getM() {
  float m = scale.get_units();
  return m;
}