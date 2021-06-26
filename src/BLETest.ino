#include <M5Core2.h>
#include "NimBLEDevice.h"

byte out_block[]={
  0x01, 0xFE, 0x00, 0x00,
  0x53, 0xFE, 0x1A, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xF0, 0x01, 0x24, 0x00,
  0x01, 0x38, 0x00, 0x00,
  0x02, 0xF7
};

const int out_len = 26;

const int BLKSIZE = 5000;
byte in_block[BLKSIZE];
int in_len;

//NimBLEDevice bt;
NimBLEAdvertisedDevice device;
NimBLEClient *pClient;
NimBLERemoteService *pService;
NimBLERemoteCharacteristic *pSender;
NimBLERemoteCharacteristic *pReceiver;


bool BLEconnected;
int pres;
int i;

void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify){
    if (length < BLKSIZE) memcpy(&in_block, pData, length);
    in_len = length;
    for (int i = 0; i < length; i++) {
        Serial.print(" ");
        Serial.print(in_block[i], HEX);
    }
    Serial.println();

}


void setup() {
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(2);
  
  BLEconnected = false;
  pres = 0;
  
  // Scan for devices and connect
  
  NimBLEDevice::init("");
  
  NimBLEScan *pScan = NimBLEDevice::getScan();
  NimBLEScanResults results = pScan->start(1);

  NimBLEUUID serviceUuid("ffc0");               // service ffc0 for Spark

  for(int i = 0; i < results.getCount() && !BLEconnected; i++) {
    device = results.getDevice(i);
    
    if (device.isAdvertisingService(serviceUuid)) {
      pClient = NimBLEDevice::createClient();
        
      if(pClient->connect(&device)) {
        BLEconnected = true;
        Serial.println("Connected");
      }
    }
  }

  // Get the services
  
  if (BLEconnected) {
    pService = pClient->getService(serviceUuid);
                
    if (pService != nullptr) {
      pSender   = pService->getCharacteristic("ffc1");
      pReceiver = pService->getCharacteristic("ffc2");
      if (pReceiver && pReceiver->canNotify()) {
        if (!pReceiver->subscribe(true, notifyCB, true)) {
          BLEconnected = false;
          pClient->disconnect();
        }
      }
    }
  }

  if (BLEconnected)
    Serial.println("Still connected");
}

void loop() {
  M5.update();

  /*
  pres++;
  if (pres > 3) pres = 0;
  out_block[24] = pres;
  
  // Send a preset change  
  if (BLEconnected) {
    if (pSender != nullptr) {
      pSender->writeValue(out_block, out_len, false);
      Serial.println("Sent");
    };
  };

  delay(1000);
  */

  /*
  if (BLEconnected) {
    if(pReceiver != nullptr) {
      std::string  val = pReceiver->readValue();
      byte *pData = (byte *) val.data();
      //delay(500);

      if (val.size() > 0) {
        for (i=0; i < val.size(); i++) {
          Serial.print(pData[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
    }
  }
  */
}
