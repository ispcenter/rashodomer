  //Проверка при включении: если вес меряется явно неверный, то необходимо калибровать прямо сейчас
  while (m < -1 || m > 5000){
    lcd.setCursor(0, 2); // 3) string. lcd.setCursor(col, row)
    lcd.print("m=");
    lcd.setCursor(2, 1);
    lcd.print(m);
    lcd.setCursor(17, 1);
    lcd.print("kg");
    lcd.setCursor(0, 3);
    lcd.print("Need calibration!"); // 3) string
  //  calibration();
    m = getM();
  
  }

   delay(2000);
   float mn = getM(); // новый вес топлива
   
   //  если заполнение не началось
   if(mn < 1.01*m){
     lcd.print("Filling problem");
     digitalWrite(Fill_pin, 0); // close input valve
     digitalWrite(Fill_pin, 1); // open input valve
     delay(2000);
     if(mn < 1.01*m){
       lcd.print("Filling ERROR!"); // Если вторая попытка заполнения не удалась, прога останавливается
     }
     // делаем ремонт, затем переходим в меню проблем и нажимаем исправлено, прога продолжит работу
   }

    boolean first = true;
    int cc = 0;
    // int fc = 0;
    // int fcg = 0;
    
         // первый раз информацию нужно показать на экране сразу. Второй и последующие разы экран обновлять 1 раз в 5с
      if (count == Tizm || first == true){
        cc += 1;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Filling:");
        lcd.print((m - Mmin)/(Mmax - Mmin)*100);
        lcd.print("%  ");
        lcd.setCursor(0, 1);
        lcd.print("m=");
        lcd.setCursor(2, 1);
        lcd.print(m);
        lcd.setCursor(0, 2);
        lcd.print("cc");
        lcd.print(cc);
        first = false;
      }

void menu_mc_cal() {
  lcd.clear();
  lcd.print("---mc calibration---");
  Serial.print("---mc calibration---");

  lcd.setCursor(0, 1); //cr
  lcd.print("mc=");
  lcd.print(mc);

  lcd.print("m=");
  lcd.print(m);
  lcd.print(" g");
}

float calibration() {
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  return mc;
}