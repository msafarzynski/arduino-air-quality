HM330XErrorCode print_result(const char* str, uint16_t value) {
  if (NULL == str) {
    return ERROR_PARAM;
  }
  SERIAL_OUTPUT.print(str);
  SERIAL_OUTPUT.println(value);
  return NO_ERROR;
}

/*parse buf with 29 uint8_t-data*/
HM330XErrorCode parse_result(uint8_t* data) {
  uint16_t value = 0;
  if (NULL == data) {
    return ERROR_PARAM;
  }
  for (int i = 1; i < 8; i++) {
    value = (uint16_t)data[i * 2] << 8 | data[i * 2 + 1];
    print_result(str[i - 1], value);
  }

  return NO_ERROR;
}

uint16_t getPmReading(uint8_t* data, int pmReading) {
  if (NULL == data) {
    return ERROR_PARAM;
  }

  return (uint16_t)data[pmReading * 2] << 8 | data[pmReading * 2 + 1];
}

HM330XErrorCode parse_result_value(uint8_t* data) {
  if (NULL == data) {
    return ERROR_PARAM;
  }
  for (int i = 0; i < 28; i++) {
    SERIAL_OUTPUT.print(data[i], HEX);
    SERIAL_OUTPUT.print("  ");
    if ((0 == (i) % 5) || (0 == i)) {
      SERIAL_OUTPUT.println("");
    }
  }
  uint8_t sum = 0;
  for (int i = 0; i < 28; i++) {
    sum += data[i];
  }
  if (sum != data[28]) {
    SERIAL_OUTPUT.println("wrong checkSum!!");
  }
  SERIAL_OUTPUT.println("");
  return NO_ERROR;
}