void mark_attendance(int fingerprintID){

  String endpoint = "/mark_attendance/";
  String arg = "?id=";
  String fingerID = String(fingerprintID);
  String fullURL = endpoint + arg + fingerID;
  Serial.println(fullURL);

  if (sim_modem.isGprsConnected()){
    Serial.println("Making GET request");
    http_client.get(fullURL);

    int status_code = http_client.responseStatusCode();
    String response = http_client.responseBody();

    Serial.print("Status code: ");
    Serial.println(status_code);
    Serial.print("Response: ");
    Serial.println(response);

    JsonObject& json_response = jsonBuffer.parseObject(response);
    // deserializeJson(json_response, response);

    if(!json_response.success()){
      Serial.println("parseObject() failed");
    }
    else{
      String message = json_response["message"];
      Serial.print("Json Response message: ");
      Serial.println(message);

      // JsonArray& num_array = jsonBuffer.parseArray(json_response["numbers"]);
      JsonArray& num_array = json_response["numbers"];

      if (num_array.success()) {
        for (JsonArray::iterator it = num_array.begin(); it != num_array.end(); ++it) {
          const char* phone_number = (*it).as<const char*>();
          send_sms(phone_number, message);
        }
      } else {
        Serial.println("Failed to Parse array");
      }
    }
  }
}
