const int BUTTON_PIN = 2;
const int LED_PIN = 3;

// Chế độ hệ thống
bool isConfigMode = true;

// Cấu hình baudrate
const long BAUD_RATES[] = {9600, 115200};
int selectedBaud = 0;  // 0: 9600, 1: 115200

// Xử lý nút nhấn
unsigned long pressStart = 0;
bool isPressed = false;
int pressCount = 0;

// LED
unsigned long ledTimer = 0;
bool ledOn = false;
int ledState = 0; // 0: tắt, 1: sáng, 2: nháy

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);//Khởi tạo ngay
 
  while (!Serial)   // ĐỢI SERIAL SẴN SÀNG (quan trọng!)
   {
    ; // Chờ kết nối Serial
  }
//Hiển thị thông báo
  Serial.println("=== CAU HINH UART BAUD ===");
  Serial.println("Nhan nut:");
  Serial.println("  1 lan -> 9600 bps");
  Serial.println("  2 lan -> 115200 bps");
  Serial.println("Giu =>3s de xac nhan");
  Serial.println(); // Dòng trống để dễ đọc
  
  digitalWrite(LED_PIN, LOW);//Led tắt(ban đầu)
  ledState = 0;
}

void loop() {
  handleButton();
  updateLED();
  
  // Nhận dữ liệu khi đang vận hành
  if (!isConfigMode && Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Serial.print("Nhan duoc: ");
    Serial.println(data);
  }
}

void handleButton() {
  bool btnState = (digitalRead(BUTTON_PIN) == LOW);
  
  // Bắt đầu nhấn
  if (btnState && !isPressed) {
    isPressed = true;
    pressStart = millis();
  }
  
  // Thả nút - nhấn ngắn
  if (!btnState && isPressed) {
    unsigned long duration = millis() - pressStart;
    
    if (duration < 3000 && isConfigMode) {
      pressCount++;
      
      if (pressCount == 1) {
        selectedBaud = 0;
        Serial.println(">>> Chon: 9600 bps");
        // LED nháy nhanh khi chọn cấu hình
        ledState = 2;
        ledTimer = millis(); // Reset timer
      } else if (pressCount >= 2) {
        selectedBaud = 1;
        Serial.println(">>> Chon: 115200 bps");
        pressCount = 0;
        // LED nháy nhanh khi chọn cấu hình
        ledState = 2;
        ledTimer = millis(); // Reset timer
      }
    }
    
    isPressed = false;
  }
  
  // Nhấn giữ > 3s
  if (isPressed && (millis() - pressStart >= 3000)) {
    isPressed = false;
    
    if (isConfigMode) {
      startRunningMode();
    } else {
      backToConfigMode();
    }
  }
}

void startRunningMode() {
  isConfigMode = false;
  
  // Đổi baudrate
  Serial.end();
  delay(100);
  Serial.begin(BAUD_RATES[selectedBaud]);
  
  // ĐỢI SERIAL MỚI SẴN SÀNG
  delay(100);
  
  Serial.println("\n=== CHE DO VAN HANH ===");
  
  // Hiển thị thông báo với tốc độ baud đang chạy
  if (selectedBaud == 0) {
    Serial.println(">>> Dang chay o Baud: 9600");
  } else {
    Serial.println(">>> Dang chay o Baud: 115200");
  }
  
  Serial.print("Baudrate: ");
  Serial.print(BAUD_RATES[selectedBaud]);
  Serial.println(" bps");
  Serial.println("Giu nut >3s de tro ve cau hinh");
  Serial.println(); // Dòng trống
  
  // LED sáng khi đã chọn xong cấu hình
  digitalWrite(LED_PIN, HIGH);
  ledState = 1;
  pressCount = 0;
}

void backToConfigMode() {
  isConfigMode = true;
  
  // QUAY LẠI BAUDRATE MẶC ĐỊNH ĐỂ HIỂN THỊ
  Serial.end();
  delay(100);
  Serial.begin(9600);
  delay(100);
  
  Serial.println("\n=== TRO VE CAU HINH ===");
  Serial.println("Nhan nut:");
  Serial.println("  1 lan => 9600 bps");
  Serial.println("  2 lan => 115200 bps");
  Serial.println("Giu =>3s de xac nhan");
  Serial.println(); // Dòng trống
  
  // LED tắt khi về chế độ cấu hình
  digitalWrite(LED_PIN, LOW);
  ledState = 0;
  pressCount = 0;
}

void updateLED() {
  if (!isConfigMode) return;
  
  // Xử lý các trạng thái LED
  switch(ledState) {
    case 0: // Tắt
      digitalWrite(LED_PIN, LOW);
      break;
      
    case 1: // Sáng
      digitalWrite(LED_PIN, HIGH);
      break;
      
    case 2: // Nháy nhanh khi đang chọn cấu hình
      if (millis() - ledTimer >= 200) { // Nhấp nháy nhanh 200ms
        ledTimer = millis();
        ledOn = !ledOn;
        digitalWrite(LED_PIN, ledOn);
      }
      break;
  }
}