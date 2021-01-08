#include <WiFiEsp.h>
#include <Servo.h>

#define LAN_SSID "FredMyisYEEFAN"
#define LAN_PASS "YEE810643"

WiFiEspServer server(80);
int status = WL_IDLE_STATUS;     // the Wifi radio's status
RingBuffer buf(32);
char cmd[32];
Servo myServo;

int ledPin = 12;
int servoPin = 8;

bool isLock = false; //傳訊

void setup() {
  pinMode(ledPin, OUTPUT);
  myServo.attach(servoPin);
  myServo.write(0);
  isLock = false; //傳訊
  Serial.begin(9600);
  Serial1.begin(9600);
  InitWiFi();
}

void loop() {
  WiFiEspClient client = server.available();  // listen for incoming clients
 
  if(client) {
    Serial.println("New client");
    buf.init();
    int index = 0;
    while(client.connected()) {
      if(client.available()) {
        char c = client.read();
        buf.push(c);
        cmd[index++] = c;
        if (buf.endsWith(";")) {
          cmd[index]=0;
          execCmd(client);
          index = 0;
        }
      }
      delay(10);
    }
 
    // close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}

void execCmd(WiFiEspClient client)
{
  Serial.println(cmd);
  int index = 0;
  while(cmd[index]!='@')
    index++;
  while(cmd[index]!=';')
  {
    if(cmd[index]=='A')//鎖參數A
    {
      lock();
      return;
    }
    else if(cmd[index]=='B')//開鎖參數B
    {
      unlock();
      return;
    }
    else if(cmd[index]=='Q') //傳訊
    {
      if(isLock)
        client.println("L");
      else
        client.println("U");
      return;
    }
    else if(cmd[index]=='C')//馬達的部分
    {
      int v = getInt(cmd, index+1);
      Serial.println(v);
      myServo.write(v%181);
    }
    index++;
    delay(10);
  }
  Serial.println("Job done");
}

void lock()//鎖動作
{
  myServo.write(90);
  delay(300);
  isLock = true; //傳訊
  Serial.println("Close");
}

void unlock()//開鎖動作
{
  myServo.write(0);
  delay(300);
  isLock = false; //傳訊
  Serial.println("Open");
}

void turnOnLed(int ledpin)
{
  digitalWrite(ledpin, HIGH);  
}

void turnOffLed(int ledpin)
{
  digitalWrite(ledpin, LOW);  
}

int getInt(char *cmd, int index)
{
  int v = 0;
  while(cmd[index]<'0'&&cmd[index]>'9')
    index++;
  while(cmd[index]>='0'&&cmd[index]<='9')
  {
    v = v*10+(cmd[index]-'0');
    index++;
  }
  return v;
}



void InitWiFi()
{
  WiFi.init(&Serial1);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // don't continue
  }

  Serial.print("Attempting to start AP ");
  Serial.println(LAN_SSID);

  // start access point
  status = WiFi.beginAP(LAN_SSID, 10, LAN_PASS, ENC_TYPE_WPA2_PSK);

  Serial.println("Access point started");
  printWifiStatus();

  // start the web server on port 80
  server.begin();
  Serial.println("Server started");
}

void printWifiStatus()
{
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address of This AP: ");
  Serial.println(ip);
 }
