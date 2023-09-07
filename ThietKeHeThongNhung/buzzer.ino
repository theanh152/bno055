void buzzerAlarm(){

  ledcWriteNote(BUZZER_CHANNEL, (note_t)NOTE_D, 6);
  delay(200);
  ledcWrite(0, 0); 
}