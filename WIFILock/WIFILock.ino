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
int doorPin = 7;



bool isDoor = false; //門狀態

void setup() {
  pinMode(ledPin,OUTPUT);
  pinMode(doorPin,INPUT);
  myServo.attach(servoPin);//門開關
  myServo.write(90);
  isDoor = false; //門狀態
  Serial.begin(9600);
  Serial1.begin(9600);
  InitWiFi();
}

void loop() {
  //Serial.println("Listening...");
  WiFiEspClient client = server.available();  // listen for incoming clients
  //Serial.println("checking switch ...");
  

  
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
      if (digitalRead(doorPin) == HIGH){
        Serial.print("關");
        isDoor = true; //傳訊
        client.println("L");
      }
      else{
        Serial.print("開");
        isDoor = false; //傳訊
        client.println("U");
      }
      delay(1000);
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
    if(cmd[index]=='A')//關參數A
    {
      Close();
      return;
    }
    else if(cmd[index]=='B')//開參數B
    {
      Open();
      return;
    }
    else if(cmd[index]=='Q') //傳訊
    {
      if(isDoor)
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

void Open()//鎖動作
{
  myServo.write(100);
  delay(300);
  
  //Serial.println("Close");
}

void Close()//開鎖動作
{
  myServo.write(0);
  delay(300);
  
  //Serial.println("Open");
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


//以下可以弄成.h檔
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
