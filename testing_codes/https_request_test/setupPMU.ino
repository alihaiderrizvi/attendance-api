// Power configuration for SIM800L_IP5306_VERSION_20190610 (v1.3) board
bool setupPMU()
{
  bool en = true;
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en)
  {
    Wire.write(0x37);
  }
  else
  {
    Wire.write(0x35);
  }
  return Wire.endTransmission() == 0;
}