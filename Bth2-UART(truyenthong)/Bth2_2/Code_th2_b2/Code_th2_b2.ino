#include <ESP32Servo.h>
#include <WiFi.h>

Servo servo1;
Servo servo2;

// WiFi info
const char* ssid = "iPhone";
const char* password = "03092005";

// Web server
WiFiServer server(80);

// Door state
bool doorOpen = false;
int pos = 0;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(1000);

  servo1.attach(15);
  servo2.attach(13);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Web server started");
}

// ================= DOOR CONTROL =================
void openDoor() {
  Serial.println("Opening door...");
  for (pos = 0; pos <= 100; pos++) {
    servo1.write(pos);
    servo2.write(180 - pos);
    delay(10);
  }
  doorOpen = true;
}

void closeDoor() {
  Serial.println("Closing door...");
  for (pos = 100; pos >= 0; pos--) {
    servo1.write(pos);
    servo2.write(180 - pos);
    delay(10);
  }
  doorOpen = false;
}

// ================= LOOP =================
void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("\nNew client connected");

  String request = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      request += c;

      if (c == '\n') {
        break;   // đọc xong request line
      }
    }
  }

  Serial.println("Request:");
  Serial.println(request);

  // Handle request
  if (request.indexOf("GET /door/on") >= 0 && !doorOpen) {
    openDoor();
  }

  if (request.indexOf("GET /door/off") >= 0 && doorOpen) {
    closeDoor();
  }

  // ===== HTTP RESPONSE =====
    // ===== HTTP RESPONSE (UI đẹp hơn) =====
  String statusText = doorOpen ? "OPEN" : "CLOSED";
  String statusClass = doorOpen ? "st-open" : "st-closed";
  String btnHref = doorOpen ? "/door/off" : "/door/on";
  String btnText = doorOpen ? "CLOSE DOOR" : "OPEN DOOR";
  String btnClass = doorOpen ? "btn close" : "btn open";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();

  client.println(F(R"rawliteral(
<!DOCTYPE html><html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta name="theme-color" content="#0b1220" />
  <title>IoT Garage Door</title>
  <style>
    :root{
      --bg1:#0b1220; --bg2:#121a2b;
      --card: rgba(255,255,255,.08);
      --stroke: rgba(255,255,255,.14);
      --text:#e9eefc; --muted: rgba(233,238,252,.75);
      --shadow: 0 18px 50px rgba(0,0,0,.45);
      --r: 22px;
    }
    *{box-sizing:border-box}
    body{
      margin:0; min-height:100vh;
      font-family: system-ui, -apple-system, Segoe UI, Roboto, Arial;
      color:var(--text);
      background: radial-gradient(1200px 700px at 20% 10%, #2b5cff33, transparent 55%),
                  radial-gradient(900px 600px at 85% 20%, #00e5ff22, transparent 55%),
                  linear-gradient(160deg, var(--bg1), var(--bg2));
      display:flex; align-items:center; justify-content:center;
      padding:18px;
    }
    .wrap{width:min(560px, 100%);}
    .card{
      background: var(--card);
      border: 1px solid var(--stroke);
      border-radius: var(--r);
      box-shadow: var(--shadow);
      overflow:hidden;
      backdrop-filter: blur(10px);
    }
    header{
      padding:18px 18px 10px 18px;
      display:flex; gap:12px; align-items:center; justify-content:space-between;
    }
    .title{
      display:flex; flex-direction:column; gap:4px;
    }
    h1{margin:0; font-size:20px; letter-spacing:.2px}
    .sub{margin:0; color:var(--muted); font-size:13px}
    .badge{
      padding:8px 12px;
      border-radius: 999px;
      font-weight:700;
      font-size:12px;
      letter-spacing:.6px;
      border:1px solid var(--stroke);
      display:flex; align-items:center; gap:8px;
      user-select:none;
    }
    .dot{width:10px;height:10px;border-radius:50%;}
    .st-open{ background: rgba(0, 255, 170, .10); }
    .st-open .dot{ background: #00ffb3; box-shadow: 0 0 18px #00ffb3aa; }
    .st-closed{ background: rgba(255, 180, 0, .10); }
    .st-closed .dot{ background: #ffb300; box-shadow: 0 0 18px #ffb300aa; }

    .content{padding: 6px 18px 18px 18px;}
    .panel{
      margin-top:10px;
      background: rgba(0,0,0,.18);
      border:1px solid var(--stroke);
      border-radius: 18px;
      padding: 16px;
    }
    .row{display:flex; gap:12px; flex-wrap:wrap; align-items:center; justify-content:space-between;}
    .hint{color:var(--muted); font-size:13px; line-height:1.4; margin:0;}

    .btn{
      width:100%;
      margin-top:14px;
      display:flex; align-items:center; justify-content:center; gap:10px;
      padding:16px 18px;
      font-size:16px;
      font-weight:800;
      border-radius: 18px;
      border: 1px solid var(--stroke);
      color: var(--text);
      text-decoration:none;
      transition: transform .08s ease, filter .15s ease;
      user-select:none;
    }
    .btn:active{transform: scale(.99);}
    .open{
      background: linear-gradient(135deg, #2b8cff, #00e5ff);
    }
    .close{
      background: linear-gradient(135deg, #ff6b6b, #ffb300);
    }
    .btn:hover{filter: brightness(1.05);}

    .footer{
      padding: 14px 18px;
      border-top: 1px solid var(--stroke);
      color: var(--muted);
      font-size: 12px;
      display:flex; justify-content:space-between; gap:12px; flex-wrap:wrap;
    }
    .kbd{
      font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
      background: rgba(255,255,255,.08);
      border:1px solid var(--stroke);
      padding:3px 8px;
      border-radius: 999px;
      color: var(--text);
    }
    svg{width:20px;height:20px}
  </style>
</head>
<body>
  <div class="wrap">
    <div class="card">
      <header>
        <div class="title">
          <h1>IoT Garage Door</h1>
          <p class="sub">ESP32 Web Control Panel</p>
        </div>
)rawliteral"));

  // Badge trạng thái (có dynamic)
  client.print("<div class='badge ");
  client.print(statusClass);
  client.print("'><span class='dot'></span><span>");
  client.print(statusText);
  client.println("</span></div>");

  client.println(F(R"rawliteral(
      </header>

      <div class="content">
        <div class="panel">
          <div class="row">
            <p class="hint">
              Tip: Nhấn nút để điều khiển cửa. Trang sẽ phản hồi ngay sau khi ESP32 xử lý xong.
            </p>
            <span class="kbd">/door/on</span>
            <span class="kbd">/door/off</span>
          </div>
        )rawliteral"));

  // Nút dynamic + icon SVG
  client.print("<a class='");
  client.print(btnClass);
  client.print("' href='");
  client.print(btnHref);
  client.print("' onclick=\"this.innerHTML='Working...';\">");

  if (!doorOpen) {
    // icon "unlock"
    client.println(F(R"rawliteral(
      <svg viewBox="0 0 24 24" fill="none" aria-hidden="true">
        <path d="M7 11V8a5 5 0 0 1 9.6-2" stroke="white" stroke-width="2" stroke-linecap="round"/>
        <path d="M7 11h10a2 2 0 0 1 2 2v6a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2v-6a2 2 0 0 1 2-2Z" stroke="white" stroke-width="2"/>
      </svg>
    )rawliteral"));
  } else {
    // icon "lock"
    client.println(F(R"rawliteral(
      <svg viewBox="0 0 24 24" fill="none" aria-hidden="true">
        <path d="M7 11V8a5 5 0 0 1 10 0v3" stroke="white" stroke-width="2" stroke-linecap="round"/>
        <path d="M7 11h10a2 2 0 0 1 2 2v6a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2v-6a2 2 0 0 1 2-2Z" stroke="white" stroke-width="2"/>
      </svg>
    )rawliteral"));
  }

  client.print(btnText);
  client.println("</a>");

  client.println(F(R"rawliteral(
        </div>
      </div>

      <div class="footer">
        <span>Status updates on refresh</span>
        <span>Made with ESP32</span>
      </div>
    </div>
  </div>
</body>
</html>
)rawliteral"));

  client.println();
  delay(1);
  client.stop();
  Serial.println("Client disconnected");
}
