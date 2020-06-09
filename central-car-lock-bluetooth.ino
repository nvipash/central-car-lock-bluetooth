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

class CarLockSystemCallbacks: public BLECharacteristicCallbacks {
  
    private: 
      const int ON = LOW;
      const int OFF = HIGH;
      /*
        адреси виходів, що будуть контролювати процес роботи
      */
      const int PERMISSION_RELAY = 25;
      const int POLARITY_CHANGE_RELAY_1 = 26;
      const int POLARITY_CHANGE_RELAY_2 = 27;
      /*
        оголошення затримки, яка знадобиться для паузи між змінами положень реле
      */
      const int SWITCHING_DELAY = 1000;
      /*
        оголошуємо конфігураційні ідентифікатори для функціонування Bluetooth
      */
      const std::string BLUETOOTH_DEVICE_NAME = "Car Lock";
      const std::string BLUETOOTH_SERVICE_UUID = "641862ac-9bdc-43d1-a2ba-2dca6d9ac892";
      const std::string BLUETOOTH_CHARACTERISTIC_UUID = "e220ff7f-5d58-4dc6-929b-367de4026d32";
      /*
        дані-тригери, на які відгукнеться контролер, щоб закрити чи 
        відкрити замок, якщо отримує ці символи за Bluetooth з'єднанням 
      */
      const std::string TURN_ON_TRIGGER = "ON";
      const std::string TURN_OFF_TRIGGER = "OFF";
  
    private: void permissionRelayOff() {
      digitalWrite(PERMISSION_RELAY, OFF);
      delay(SWITCHING_DELAY);
    }
    
    private: void permissionRelayOn() {
      digitalWrite(PERMISSION_RELAY, OFF);
      delay(SWITCHING_DELAY);
    }
    /*
      оголошуємо виходи на піни трьох реле, двоє з яких 
      відповідаються за зміну полярності на замковому приводі,
      а третє реагує на подання струму та пропускає його далі, щоб 
      дозволити зреагувати цим двом реле на цю зміну і змінити свій стан
    */
    public: void initRelayPins() {
      pinMode(PERMISSION_RELAY, OUTPUT);
      pinMode(POLARITY_CHANGE_RELAY_1, OUTPUT);
      pinMode(POLARITY_CHANGE_RELAY_2, OUTPUT);
    }
    /*
      оголошення Bluetooth Low Enegry (BLE) Server
    */
    public: void initBluetoothServer() {
      BLEDevice::init(BLUETOOTH_DEVICE_NAME);
      BLEServer *carLockBleServer = BLEDevice::createServer();

      BLEService *carLockBleService = carLockBleServer -> createService(BLUETOOTH_SERVICE_UUID);

      BLECharacteristic *carLockBleChar = carLockBleService -> createCharacteristic(
        BLUETOOTH_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
      );

      carLockBleChar -> setCallbacks(new CarLockSystemCallbacks());
    
      carLockBleChar -> setValue("0");
      carLockBleService -> start();
    
      BLEAdvertising *carLockBleAd = carLockBleServer -> getAdvertising();
      carLockBleAd -> start();
    }
    /*
      оголошуємо команди, що будуть виконувати зміну полярності на релейних
      пристроях та закривати чи відкривати замковий механізм відповідно
    */
    public: void carLock() {
      /*
        змінюємо полярність реле на (-) та закриваємо замок
      */
      permissionRelayOff();
      digitalWrite(POLARITY_CHANGE_RELAY_1, OFF);
      digitalWrite(POLARITY_CHANGE_RELAY_2, OFF);
      permissionRelayOn();
    }
    
    public: void carUnlock() {
      /*
        змінюємо полярність реле на (+) та відкриваємо замок
      */
      permissionRelayOff();
      digitalWrite(POLARITY_CHANGE_RELAY_1, ON);
      digitalWrite(POLARITY_CHANGE_RELAY_2, ON);
      permissionRelayOn();
    }

    void onWrite(BLECharacteristic *carLockBleChar) {
      std::string value = carLockBleChar -> getValue();
      /*
         для перевірки роботи схеми перевіряємо за зміною
         отриманного значення на  чи 1, що буде виконуватись
         відкриття чи закриття замка
      */
      if (value == TURN_ON_TRIGGER) {
        carLock();
      }

      if (value == TURN_OFF_TRIGGER) {
        carUnlock();
      }
    }
};

void loop() {}

void setup() {
  CarLockSystemCallbacks carLockSystem;
  /*
    виконати команди на визначення керуючих виводів, 
    зачинення дверей на початку роботи системи та
    ініціалізації Bluetooth LE Server
  */
  carLockSystem.initRelayPins();
  carLockSystem.initBluetoothServer();
  carLockSystem.carLock();
}
