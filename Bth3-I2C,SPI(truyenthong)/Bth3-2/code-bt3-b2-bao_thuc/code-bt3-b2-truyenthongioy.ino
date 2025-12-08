#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

// Khai bao chan
const int MODE = 3;
const int UP = 4;
const int DOWN = 5;
const int SET = 6;
const int BUZZER = 2;
const int tet = 7;

// Bien trang thai
enum State {
  xemngaygio,        
  chinhgio, 
  chinhphut
};

State tthientai = xemngaygio;

int giohen = 20;
int phuthen = 30;
bool batcoi = true;
bool ttcoi = false;

unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

void setup() {
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("RTC khong hoat dong!");
    while (1);
  }

  if (!rtc.isrunning()) {
    lcd.clear();
    lcd.print("RTC adjust...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(2000);
  }

  pinMode(MODE, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(SET, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(tet, OUTPUT);

}

void loop() {
  DateTime now = rtc.now();
  digitalWrite(BUZZER, HIGH);
  digitalWrite(tet, LOW);
  handleButtons();
  switch (tthientai) {
    case xemngaygio:
      hienThiNgayGio(now);
      kiemTraBaoThuc(now);
      break;
    case chinhgio:
      hienThiCaiDatBaoThuc(true);
      break;
    case chinhphut:
      hienThiCaiDatBaoThuc(false);
      break;
  }
  delay(100);
}

void handleButtons() {
  unsigned long ttthoigian = millis();
  if (ttthoigian - lastButtonPress < debounceDelay) {
    return;
  }
  
  if (digitalRead(MODE) == LOW) {
    lastButtonPress = ttthoigian;
    
    if (ttcoi) {
      ttcoi = false;
      tthientai = xemngaygio;
    } else {
      if (tthientai == xemngaygio) {
        tthientai = chinhgio;
        lcd.clear();
      }
    }
    delay(300);
  }

  if (digitalRead(UP) == LOW) {
    lastButtonPress = ttthoigian;
    if (tthientai == chinhgio) {
      giohen++;
      if (giohen > 23) giohen = 0;
    } else if (tthientai == chinhphut) {
      phuthen++;
      if (phuthen > 59) phuthen = 0;
    }
    delay(200);
  }
 
  if (digitalRead(DOWN) == LOW) {
    lastButtonPress = ttthoigian;
    if (tthientai == chinhgio) {
      giohen--;
      if (giohen < 0) giohen = 23;
    } else if (tthientai == chinhphut) {
      phuthen--;
      if (phuthen < 0) phuthen = 59;
    }
    delay(200);
  }

  if (digitalRead(SET) == LOW) {
    lastButtonPress = ttthoigian;
    
    if (tthientai == xemngaygio) {
      batcoi = !batcoi;
    } else if (tthientai == chinhgio) {
      tthientai = chinhphut;
      lcd.clear();
    } else if (tthientai == chinhphut) {
      tthientai = xemngaygio;
      lcd.clear();
    }
    delay(300);
  }
}

void hienThiNgayGio(DateTime now) {
  lcd.setCursor(0, 1);
  if (now.day() < 10) lcd.print('0');
  lcd.print(now.day());
  lcd.print('/');
  if (now.month() < 10) lcd.print('0');
  lcd.print(now.month());
  lcd.print('/');
  lcd.print(now.year());
  
  lcd.setCursor(11, 1);
  if (batcoi) {
    if (giohen < 10) lcd.print('0');
    lcd.print(giohen);
    lcd.print(':');
    if (phuthen < 10) lcd.print('0');
    lcd.print(phuthen);
  } else {
    lcd.print("      ");
  }

  lcd.setCursor(0, 0);
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());
  
  lcd.setCursor(11, 0);
  if (batcoi) {
    lcd.print("ON    ");
  } else {
    lcd.print("OFF   ");
  }
}

void hienThiCaiDatBaoThuc(bool isHour) { 
  lcd.setCursor(0, 1);
  lcd.print("Bao thuc:");
  if (isHour) {
    lcd.print('>');
  } else {
    lcd.print(' ');
  }
  
  if (giohen < 10) lcd.print('0');
  lcd.print(giohen);
  lcd.print(':');
  
  if (!isHour) {
    lcd.print('>');
  } else {
    lcd.print(' ');
  }
  if (phuthen < 10) lcd.print('0');
  lcd.print(phuthen);
  lcd.print("  ");
}

void kiemTraBaoThuc(DateTime now) {
  if (!batcoi) return;
  
  if (now.hour() == giohen && now.minute() == phuthen && now.second() == 0) {
    ttcoi = true;
  }
  
  if (ttcoi) {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(tet, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  WAKE UP!!!    ");
  }
}