#include "WProgram.h"
#include "sm130.h"

uint8_t sm130_checksum(uint8_t *data, int len) {
  uint8_t checksum;
  
  for(int i = 0; i < len; i++) {
    checksum += data[i];
  }
  return checksum;
}

UARTInterfaceAdapter::UARTInterfaceAdapter(int inpin, int outpin) : _nfc(inpin, outpin) {}

void UARTInterfaceAdapter::begin(int rate) {
  _nfc.begin(rate);
}

void UARTInterfaceAdapter::send(nfc_command_t command, uint8_t *data, int len) {
  uint8_t checksum = len + 1 + command;
  _nfc.write(0xFF);
  _nfc.write(0x00);
  _nfc.write(len + 1);
  _nfc.write(command);
  for(int i = 0; i < len; i++) {
    checksum += data[i];
    _nfc.write(data[i]);
  }
  _nfc.write(checksum);
}

uint8_t UARTInterfaceAdapter::available() {
  return _nfc.available();
}

int UARTInterfaceAdapter::receive(nfc_command_t command, uint8_t *data) {
  uint8_t checksum = 0;
  while(_nfc.read() != 0xFF);
  if(_nfc.read() != 0x00)
    return -1;
  int len = _nfc.read();
  checksum += len;
  int command_in = _nfc.read();
  checksum += command_in;
  if(command_in != command)
    return -1;
  for(int i = 0; i < len - 1; i++) {
    data[i] = _nfc.read();
    checksum += data[i];
  }
  int checksum_in = _nfc.read();
  if(checksum_in != checksum)
    return -1;
  return len - 1;
}

