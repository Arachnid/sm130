#ifndef sm130_h
#define sm130_h

#include <inttypes.h>
#include <SoftwareSerial.h>

enum nfc_command_t {
  NFC_NONE = 0,
  NFC_RESET = 0x80,
  NFC_GET_FIRMWARE = 0x81,
  NFC_SEEK = 0x82,
  NFC_SELECT = 0x83,
  NFC_AUTHENTICATE = 0x85,
  NFC_READ_BLOCK = 0x86,
  NFC_READ_VALUE = 0x87,
  NFC_WRITE_BLOCK = 0x89,
  NFC_WRITE_VALUE = 0x8A,
  NFC_WRITE_ULTRALIGHT = 0x8B,
  NFC_WRITE_KEY = 0x8C,
  NFC_INCREMENT = 0x8D,
  NFC_DECREMENT = 0x8E,
  NFC_ANTENNA = 0x90,
  NFC_READ_PORT = 0x91,
  NFC_WRITE_PORT = 0x92,
  NFC_HALT = 0x93,
  NFC_SET_BAUD_RATE = 0x94,
  NFC_SLEEP = 0x96,
};

class IInterfaceAdapter {
public:
  virtual void send(nfc_command_t command, uint8_t *data, int len) = 0;
  virtual uint8_t available() = 0;
  virtual int receive(nfc_command_t command, uint8_t *data) = 0;
};

class UARTInterfaceAdapter : public IInterfaceAdapter {
private:
public:
  SoftwareSerial _nfc;
  UARTInterfaceAdapter(int inpin, int outpin);
  void begin(int rate);
  virtual void send(nfc_command_t command, uint8_t *data, int len);
  virtual uint8_t available();
  virtual int receive(nfc_command_t command, uint8_t *data);
};

enum tag_type_t {
  TAG_TYPE_ULTRALIGHT = 0x01,
  TAG_TYPE_1K = 0x02,
  TAG_TYPE_4K = 0x03,
  TAG_TYPE_UNKNOWN = 0x04,
};

struct Tag {
  tag_type_t type;
  int serial_size;
  uint8_t serial[7];
};

enum status_code_t {
  STATUS_SUCCESS = 0x00,
  STATUS_INVALID_RESPONSE = 0x01,
  STATUS_INVALID_KEY = 0x45,
  STATUS_IN_PROGRESS = 0x4C,
  STATUS_LOGIN_SUCCESSFUL = 0x4C,
  STATUS_HALTED = 0x4C,
  STATUS_NO_TAG = 0x4E,
  STATUS_RF_OFF = 0x55,
  STATUS_LOGIN_FAILED = 0x55,
};

enum key_type_t {
  KEY_TYPE_A = 0xAA,
  KEY_TYPE_B = 0xBB,
  KEY_TRANSPORT = 0xFF,
  KEY_TYPE_A_STORED = 0x10,
  KEY_TYPE_B_STORED = 0x20,
};

struct Block {
  int block_num;
  uint8_t data[16];
};

class NFCReader {
private:
  IInterfaceAdapter *_adapter;
  nfc_command_t _last_command;
  void write_raw(nfc_command_t command, uint8_t *buf, int len);
public:
  NFCReader(IInterfaceAdapter *adapter);
  uint8_t available();
  
  void reset();
  void get_firmware_version();
  void select();
  void seek();
  void authenticate(int block_num, key_type_t type, uint8_t *key);
  void read_block(int block_num);
  void halt();
  
  int receive_raw(uint8_t *buf);
  status_code_t receive_status();
  status_code_t receive_tag(Tag *tag);
  status_code_t receive_block(Block *block);
};

#endif
