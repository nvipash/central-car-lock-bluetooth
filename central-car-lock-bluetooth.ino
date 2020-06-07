//*****************************************************************************
// File Name:   central-car-lock-bluetooth.ino
// Author:      Pavlo Novitskyi
// Project:     Car central lock based on the Bluetooth technology
// Compiler:    Arduino IDE Ver 1.8.12 with ESP32 plugin
// Target:      ESP32 Dev Module
//*****************************************************************************

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define PERMISSION_RELAY        9       
#define POLARITY_CHANGE_RELAY_1 26
#define POLARITY_CHANGE_RELAY_2 27

#define ON              HIGH
#define OFF             LOW
#define PAUSE_DELAY     2000
/* 
 * оголошення затримки, яка знадобиться для паузи між змінами положень реле
 */
#define SWITCHING_DELAY 100

#define BLUETOOTH_DEVICE_NAME           "Car Lock"
#define BLUETOOTH_SERVICE_UUID          "641862ac-9bdc-43d1-a2ba-2dca6d9ac892"
#define BLUETOOTH_CHARACTERISTIC_UUID   "e220ff7f-5d58-4dc6-929b-367de4026d32"
/* 
 * оголошуємо команди, що будуть виконувати зміну полярності на релейних 
 * пристроях та закривати чи відкривати замковий механізм відповідно
 */
void plusMinus();
void minusPlus();
    
class CarLockCallbacks: public BLECharacteristicCallbacks {
    /* 
     * змінюємо полярність реле на (-) та закриваємо замок
     */
    void plusMinus() {
      digitalWrite(PERMISSION_RELAY, OFF);
      delay(SWITCHING_DELAY);
      digitalWrite(POLARITY_CHANGE_RELAY_1, OFF);
      digitalWrite(POLARITY_CHANGE_RELAY_2, OFF);
      delay(SWITCHING_DELAY);
      digitalWrite(PERMISSION_RELAY, ON);
    }
    /* 
     * змінюємо полярність реле на (+) та відкриваємо замок
     */
    void minusPlus() {
      digitalWrite(PERMISSION_RELAY, OFF);
      delay(SWITCHING_DELAY);
      digitalWrite(POLARITY_CHANGE_RELAY_1, ON);
      digitalWrite(POLARITY_CHANGE_RELAY_2, ON);
      delay(SWITCHING_DELAY);
      digitalWrite(PERMISSION_RELAY, ON);
    }
    
    void onWrite(BLECharacteristic *carLockBleChar) {
      std::string value = carLockBleChar->getValue();
      /* 
       * для перевірки роботи схеми перевіряємо за зміною 
       * отриманного значення на 0 чи 1, що буде виконуватись 
       * відкриття чи закриття замка
       */
      if (value.length() == 0) {
        delay(PAUSE_DELAY);
        plusMinus();
      }

      if (value.length() == 1) {
        delay(PAUSE_DELAY);
        minusPlus();
      }
    }
};


void setup() {
  /* 
   * оголошуємо виходи на піни трьох реле, двоє з яких відповідаються 
   * за зміну полярності на замковому приводі, а третє реагує на подання 
   * струму та пропускає його далі, щоб дозволити зреагувати цим двом реле
   * на цю зміну і змінити свій стан
   */
  pinMode(PERMISSION_RELAY, OUTPUT);
  pinMode(POLARITY_CHANGE_RELAY_1, OUTPUT);
  pinMode(POLARITY_CHANGE_RELAY_2, OUTPUT);
  /* 
   * оголошення Bluetooth Low Enegry (BLE) Server
   */
  BLEDevice::init(BLUETOOTH_DEVICE_NAME);
  BLEServer *carLockBleServer = BLEDevice::createServer();

  BLEService *carLockBleService = carLockBleServer->createService(BLUETOOTH_SERVICE_UUID);

  BLECharacteristic *carLockBleChar = carLockBleService->createCharacteristic(
                                        BLUETOOTH_CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
                                      
  carLockBleChar->setCallbacks(new CarLockCallbacks());

  carLockBleChar->setValue("0");
  carLockBleService->start();
  
  BLEAdvertising *carLockBleAd = carLockBleServer->getAdvertising();
  carLockBleAd->start();
}

void loop() {}
