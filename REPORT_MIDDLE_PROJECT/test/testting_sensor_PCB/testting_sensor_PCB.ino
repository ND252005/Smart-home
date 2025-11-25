
#include "DHT.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;


#define DHTTYPE DHT11   // DHT 11

#define DHT_PIN 4     // Digital pin connected to the DHT sensor
#define MQ2_PIN 35

const int GAS_THRESHOLD = 2000; 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  pinMode(MQ2_PIN, INPUT);

}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}

// Khởi tạo đối tượng Bluetooth

// Chọn chân Analog (Nên dùng GPIO 34, 35, 32, 33)

// Ngưỡng báo động (Bạn tự chỉnh sau khi test thực tế)
// Giá trị Analog của ESP32 từ 0 đến 4095
const int GAS_THRESHOLD = 2000; 

void setup() {
  Serial.begin(115200);

  // Đặt tên cho Bluetooth hiển thị trên điện thoại
    SerialBT.begin("ESP32_MQ2_Sensor"); 
    Serial.println("Bluetooth da san sang! Hay ket noi dien thoai.");


}

void loop() {
  // Đọc giá trị Analog từ cảm biến (0 - 4095)
  int sensorValue = analogRead(MQ2_PIN);

  // Đổi sang điện áp (tham khảo) để dễ hình dung (0 - 3.3V)
  float voltage = sensorValue * (3.3 / 4095.0);

  // Gửi dữ liệu qua Bluetooth
  SerialBT.print("Gia tri: ");
  SerialBT.print(sensorValue);
  SerialBT.print(" | Dien ap: ");
  SerialBT.print(voltage);
  SerialBT.println("V");

  // Cảnh báo nếu vượt ngưỡng
  if (sensorValue > GAS_THRESHOLD) {
    SerialBT.println("⚠️ CANH BAO: Phat hien KHOI/GAS! ⚠️");
    SerialBT.println("--------------------------------");
  }

  delay(1000); // Đọc 1 giây 1 lần
}