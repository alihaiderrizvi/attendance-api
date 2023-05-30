void setupSim()
{
  if (!setupPMU())
  {
    Serial.println("Setting board power management error");
  }

  // Set SIM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  //Add CA Certificate
  secure_presentation_layer.setCACert(root_ca);

  // SIM modem initial setup
  setupModem();
}