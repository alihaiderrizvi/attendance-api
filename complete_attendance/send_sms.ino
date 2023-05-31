void send_sms(const char* phone_number, String message){
  Serial.println(phone_number);
  bool res = sim_modem.sendSMS(phone_number, message);
  Serial.print("Res: ");
  Serial.println(res);
}