#include "sm130.h"

NFCReader::NFCReader(IInterfaceAdapter *adapter) : _adapter(adapter) {
  _last_command = NFC_GET_FIRMWARE;
}

void NFCReader::write_raw(nfc_command_t command, uint8_t *buf, int len) {
  _last_command = command;
  _adapter->send(command, buf, len);
}

void NFCReader::reset() {
  write_raw(NFC_RESET, 0, 0);
}

void NFCReader::get_firmware_version() {
  //read_raw
  write_raw(NFC_GET_FIRMWARE, 0, 0);
}

void NFCReader::select() {
  //read_tag
  write_raw(NFC_SELECT, 0, 0);
}

void NFCReader::seek() {
  //read_tag
  write_raw(NFC_SEEK, 0, 0);
}

void NFCReader::authenticate(int block_num, key_type_t type, uint8_t *key) {
  //read_status
  uint8_t buf[8];
  
  buf[0] = (uint8_t)block_num;
  buf[1] = (uint8_t)type;
  if(type == KEY_TYPE_A || type == KEY_TYPE_B) {
    memcpy(buf + 2, key, 6);
  } else {
    memset(buf + 2, 0, 6);
  }
  write_raw(NFC_AUTHENTICATE, buf, 8);
}

void NFCReader::read_block(int block_num) {
  uint8_t buf[1];
  buf[0] = (uint8_t)block_num;
  write_raw(NFC_READ_BLOCK, buf, 1);
}

void NFCReader::halt() {
  write_raw(NFC_HALT, 0, 0);
}

int NFCReader::receive_raw(uint8_t *buf) {
  return _adapter->receive(_last_command, buf);
}

status_code_t NFCReader::receive_status() {
  uint8_t status;
  int len = receive_raw(&status);
  if(len <= 0) {
    return STATUS_INVALID_RESPONSE;
  } else {
    return (status_code_t)status;
  }
}

status_code_t NFCReader::receive_tag(Tag *tag) {
  uint8_t buf[8];
  int len = receive_raw(buf);
  if(len < 0) {
    return STATUS_INVALID_RESPONSE;
  } else if(len == 1) {
    return (status_code_t)buf[0];
  } else {
    tag->type = (tag_type_t)buf[0];
    tag->serial_size = len - 1;
    memcpy(tag->serial, buf + 1, tag->serial_size);
    return STATUS_SUCCESS;
  }
}

status_code_t NFCReader::receive_block(Block *block) {
  uint8_t buf[17];
  int len = receive_raw(buf);
  if(len == 1) {
    return (status_code_t)buf[0];
  } else if(len == 17) {
    // TODO: Finish
  } else {
    return STATUS_INVALID_RESPONSE;
  }
}