#include "RYLR998_radio.h"
#include <Arduino.h>

#define DEFAULT_BAUD 115200

RYLR998_radio::RYLR998_radio(HardwareSerial *port)
{
  radio_serial = port;
}


void RYLR998_radio::transmit(uint16_t address, size_t payload_length, char* data)
{ // make sure that the data is terminated with a null character!!
  const char command[] = "AT+SEND=";

  radio_serial->print(command);
  radio_serial->print(address);
  radio_serial->write(',');
  radio_serial->print(payload_length);
  radio_serial->write(',');
  radio_serial->print(data);
  radio_serial->print("\r\n");

  Serial.print(command);
  Serial.print(address);
  Serial.write(',');
  Serial.print(payload_length);
  Serial.write(',');
  Serial.print(data);
  Serial.print("\r\n");

  wait_for_handshake();
}

// DOESNT WORK AS INTENDED
void RYLR998_radio::get_received(uint16_t* address, size_t* payload_length, char* data, uint8_t* rssi, uint8_t* snr)
{
  char command[] = "+RCV\r\n";
  radio_serial->print(command);
  Serial.print(command);

  parse_for_int(9, 9);
}


void RYLR998_radio::set_mode(device_mode_e mode)
{
  char message[] = "AT+MODE=0\r\n";
  char *numberPtr = message + 8;
  convert_int(mode, 1, numberPtr);

  Serial.print(message);
  radio_serial->print(message);

  wait_for_handshake();
}


void RYLR998_radio::set_frequency(int freq)
{
  char message[] = "AT+BAND=";

  Serial.print(message);
  Serial.print(freq);
  Serial.print("\r\n");
  radio_serial->print(message);
  radio_serial->print(freq);
  radio_serial->print("\r\n");

  wait_for_handshake();
} 


int RYLR998_radio::get_frequency(void)
{
  char message[] = "AT+BAND?\r\n";
  radio_serial->print(message);

  return parse_for_int(6, 9);
}


void RYLR998_radio::set_address(uint16_t address)
{
  radio_serial->print("AT+ADDRESS=");
  radio_serial->print(address);
  radio_serial->print("\r\n");
  
  wait_for_handshake();
}


uint16_t RYLR998_radio::get_address(void)
{
  char message[] = "AT+ADDRESS?\r\n";
  radio_serial->print(message);

  return parse_for_int(9, 3);
}


void RYLR998_radio::set_networkID(uint8_t networkID)
{
  radio_serial->print("AT+NETWORK=");
  radio_serial->print(networkID);
  radio_serial->print("\r\n");

  wait_for_handshake();
}

uint8_t RYLR998_radio::get_networkID(void)
{
  char message[] = "AT+NETWORKID?\r\n";
  radio_serial->print(message);

  return parse_for_int(11, 2);
}

void RYLR998_radio::set_radio_power(uint8_t power_level_dbm)
{
  radio_serial->print("AT+CRFOP=");
  radio_serial->print(power_level_dbm);
  radio_serial->print("\r\n");

  wait_for_handshake();
}


uint8_t RYLR998_radio::get_radio_power(void)
{
  char message[12] = "AT+CRFOP?\r\n";
  radio_serial->print(message);
  
  return parse_for_int(7, 2);
}

int RYLR998_radio::parse_for_int(int numStart, size_t alignmentSize)
{
  int i = 0;
  int number;
  char buffer[16];
  char* isolatedNums = buffer + numStart;
  do{
    while(!radio_serial->available());
    buffer[i] = radio_serial->read();
    Serial.write(buffer[i]);
    i++;
  } while(buffer[i - 1] != '\n');
  if ((*(isolatedNums + 1) < '0')||(*(isolatedNums + 1) > '9'))
    number = convert_text(1, isolatedNums);
  else
    number = convert_text(alignmentSize, isolatedNums); 
  return number;
}

void RYLR998_radio::wait_for_handshake(void) // need a better way to handle handshake not coming back correct
{
  const char ideal[6] = "+OK\r\n";
  char buffer[6];
  buffer[5] = '\0';
  do{
    int i = 0;
    do{
      while(!radio_serial->available());
      buffer[i] = radio_serial->read();
      i++; 
      // we are waiting for +OK\r\n
    } while(buffer[i - 1] != '\n');
  } while(!compare_string(buffer, ideal));
  Serial.println("Handshake recieved");
}


bool RYLR998_radio::compare_string(const char* str1, const char* str2)
{
  int i = 0;
  while(str1[i] != '\0'){
    if (str1[i] != str2[i]) return false;
    i++;
  }
  return true;
}


int RYLR998_radio::convert_text(size_t alignmentSize, char *inputStr)
{
  int number = 0;
  for (int i = 0; i < alignmentSize; i++)
    number += (inputStr[i] - '0') * pow(10, alignmentSize - i - 1);
  return number;
}


void RYLR998_radio::convert_int(int number, size_t alignmentSize, char *outputStr)
{
  int _n = 0; // how many digits does number take up

  while (number - pow(10, _n) >= 0)
    _n++;

  for (int i = 0; i < alignmentSize - _n; i++) outputStr[i] = '0';
  
  for (int i = _n; i > 0; i--){
    int power = pow(10, i - 1);
    int index = alignmentSize - i;
    outputStr[index] = number / power;
    number -= outputStr[index] * power; 
    outputStr[index] += '0';
  }
}
