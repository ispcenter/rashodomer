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
