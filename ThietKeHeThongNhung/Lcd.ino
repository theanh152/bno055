void LcdDisplay(float t, float h){
  DateTime now = rtc.now();
  lcd.setCursor(1, 0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(",");
  if(now.day()<=9)
  {
    lcd.print("0");
    lcd.print(now.day());
  }
  else {
    lcd.print(now.day()); 
  }
  lcd.print('/');
  if(now.month()<=9)
  {
    lcd.print("0");
    lcd.print(now.month());
  }
  else {
    lcd.print(now.month()); 
  }
  lcd.print('/');
  lcd.print(now.year());
  lcd.setCursor(15, 0);
  lcd.print(" "); 

  // line 2
  lcd.setCursor(0, 1);  // display position
  lcd.print("T:");
  lcd.print(t);     // display the temperature
  //lcd.print("°C");
  lcd.print(" H:");
  lcd.print(h);
  //lcd.print("%");
}