#include <SoftwareSerial.h>
#include <sm130.h>

UARTInterfaceAdapter adapter(7, 8);
NFCReader reader(&adapter);

void setup() {
  Serial.begin(19200);
  adapter.begin(19200);
  Serial.println("Initialized");
  delay(1000);

  // Print firmware version
  char buf[10];
  int len = reader.receive_raw((uint8_t*)buf);
  buf[len] = 0;
  Serial.println(buf);
}

void loop() {
  Tag tag;
  status_code_t stat;

  reader.select();
  stat = reader.receive_tag(&tag);
  if(stat == STATUS_SUCCESS) {
    Serial.print("Detected tag with type: ");
    Serial.print(tag.type, HEX);
    Serial.print(", ID: ");
    for(int i = 0; i < tag.serial_size; i++)
      Serial.print(tag.serial[i], HEX);
    Serial.println();
    reader.halt();
    stat = reader.receive_status();
    if(stat != STATUS_HALTED) {
      Serial.print("Failed to halt tag, got status ");
      Serial.print(stat, HEX);
      Serial.println();
    }
  }
  delay(250);
}

