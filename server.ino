#include <ESP8266WiFi.h>
#include <gpio.h>

// ======================================================================
// --- Dados de Acesso do seu roteador ---
#define       LED_NETWORK      2         // WIFI Module LED
//#define       BUTTON    D1        // Connectivity ReInitiate Button
#define WLAN_SSID   "ACCESS01"  // nome da sua rede
#define WLAN_PASS  "P@ssW0rd"   // senha de acesso do seu roteador
#define WLAN_SSID2   "ACCESS02"  // nome da sua rede
#define WLAN_PASS2  "P@ssW0rd"   // senha de acesso do seu roteador

#define LED_CONNECTED_1      D0          // Indicate Connectivity With Client "For All"
#define LED_CONNECTED_2      D1          // Connectivity With Cliente 01
#define LED_CONNECTED_3      D2          // Connectivity With Cliente 02
#define LED_CONNECTED_4      D3          // Connectivity With Cliente 03

#define LED_ALARMED_1      D4          // Indicate Connectivity With Client "For All"
#define LED_ALARMED_2      D5          // Connectivity With Cliente 01
#define LED_ALARMED_3      D6          // Connectivity With Cliente 02
#define LED_ALARMED_4      D7          // Connectivity With Cliente 03

#define ALARM_BUZZER      D8          // Connectivity With Cliente 03

#define LED_WIFI      RX          // Connectivity With Cliente 03
#define LED_ON      TX          // Connectivity With Cliente 03

#define MAXSC     4           // MAXIMUM NUMBER OF CLIENTS 

String      Message;                  // VARIABLE RECEIVE DATA FROM OTHER CLIENTS
bool ALARMED = false;
WiFiServer  server(80);               // THE SERVER AND THE PORT NUMBER
WiFiClient  client[MAXSC];           // THE SERVER client (Devices)

long TIMEOUT_LIFE_PROOF = 10000;
  
long LIFE_PROOF_1 = 0;
long LIFE_PROOF_2 = 0;
long LIFE_PROOF_3 = 0;
long LIFE_PROOF_4 = 0;

long ALARM_PROOF_1 = 0;
long ALARM_PROOF_2 = 0;
long ALARM_PROOF_3 = 0;
long ALARM_PROOF_4 = 0;

IPAddress ip(192, 168, 1, 250);      // IP address of the server
IPAddress gateway(192, 168, 1, 1);   // gateway of your network
IPAddress subnet(255, 255, 255, 0);   // subnet mask of your network

//====================================================================================
void setup()
{
    
  Serial.begin(115200); // Inicia Serial com Baud Rate de 115200
  WiFi.mode(WIFI_STA); //Habilita o modo estação
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(WLAN_SSID, WLAN_PASS); //Inicia WiFi com os dados preenchidos
  delay(1000); // aguarda 1 segundo
  Serial.print("Tentando conectar ...\n");
  checkNetwork();

  server.begin();

  pinMode(LED_NETWORK, OUTPUT); // Initialize the LED pin as an output

  pinMode(LED_CONNECTED_1, OUTPUT);          // Setting  LED_CONNECTED_1 Pin Mode Output
  pinMode(LED_CONNECTED_2, OUTPUT);          // Setting  LED_CONNECTED_2 Pin Mode Output
  pinMode(LED_CONNECTED_3, OUTPUT);          // Setting  LED_CONNECTED_3 Pin Mode Output
  pinMode(LED_CONNECTED_4, OUTPUT);          // Setting  LED_CONNECTED_4 Pin Mode Output

  pinMode(LED_ALARMED_1, OUTPUT);          // Setting  LED_CONNECTED_1 Pin Mode Output
  pinMode(LED_ALARMED_2, OUTPUT);          // Setting  LED_CONNECTED_2 Pin Mode Output
  pinMode(LED_ALARMED_3, OUTPUT);          // Setting  LED_CONNECTED_3 Pin Mode Output
  pinMode(LED_ALARMED_4, OUTPUT);          // Setting  LED_CONNECTED_4 Pin Mode Output

}

//====================================================================================
void loop()
{
  Availableclient();     // Checking For Available client
  //AvailableMessage();     // Checking For Available Client Messages
  //checkNetwork();
}

//====================================================================================

void Availableclient()
{
  if (server.hasClient())
  {
    for (uint8_t i = 0; i < MAXSC; i++)
    {
      if (!client[i] || !client[i].connected())           //find free/disconnected spot
      {
        if (client[i])                                     // Checks If Previously The Client Is Taken
        {
          client[i].stop();
        }
        if (client[i] = server.available())              // Checks If client Connected To The Server
        {
          Serial.print("Client " + String(i) + " => ");
          checkTerminals();
          AvailableMessage(i);
        }
        continue;                                          // Continue Scanning
      }
    }

    WiFiClient client = server.available();
    client.stop();

  }
}

// =================================================================================

void checkTerminals() {

  long current_time = millis();
  
  long time1 = current_time - LIFE_PROOF_1;
  long time2 = current_time - LIFE_PROOF_2;
  long time3 = current_time - LIFE_PROOF_3;
  long time4 = current_time - LIFE_PROOF_4;

  if(time1 > TIMEOUT_LIFE_PROOF){ digitalWrite(LED_CONNECTED_1, LOW); }
  if(time2 > TIMEOUT_LIFE_PROOF){ digitalWrite(LED_CONNECTED_2, LOW); }
  if(time3 > TIMEOUT_LIFE_PROOF){ digitalWrite(LED_CONNECTED_3, LOW); }
  if(time4 > TIMEOUT_LIFE_PROOF){ digitalWrite(LED_CONNECTED_4, LOW); }
  
  if (current_time > 2147483000) {
    ESP.restart();
  }

}

void AvailableMessage(int i)
{

  Message = client[i].readStringUntil('\r');

  if (Message.length() > 0 && Message.indexOf('-') > 0) {
    client[i].flush();
    Serial.println(Message);
    ClientNumber(Message);
    Message = "";
  }

}

// =================================================================================

void ClientNumber(String Message) {

  long current_time = millis();
  
  if (Message == "<EN01- CONNECTED>" || Message == "<EN01- ACIONADO>") {
    LIFE_PROOF_1 = current_time;
    digitalWrite(LED_CONNECTED_1, HIGH);
  } else if (Message == "<EN02- CONNECTED>" || Message == "<EN02- ACIONADO>") {
    LIFE_PROOF_2 = current_time;
    digitalWrite(LED_CONNECTED_2, HIGH);
  } else if (Message == "<SA01- CONNECTED>" || Message == "<SA01- ACIONADO>") {
    LIFE_PROOF_3 = current_time;
    digitalWrite(LED_CONNECTED_3, HIGH);
  } else if (Message == "<SA02- CONNECTED>" || Message == "<SA02- ACIONADO>") {
    LIFE_PROOF_4 = current_time;
    digitalWrite(LED_CONNECTED_4, HIGH);
  }

  if (Message == "<EN01- ACIONADO>") {
    //LIFE_PROOF_1 = current_time;
    digitalWrite(LED_ALARMED_1, HIGH);
  } else if (Message == "<EN02- ACIONADO>") {
    //LIFE_PROOF_2 = current_time;
    digitalWrite(LED_ALARMED_2, HIGH);
  } else if (Message == "<SA01- ACIONADO>") {
    //LIFE_PROOF_3 = current_time;
    digitalWrite(LED_ALARMED_3, HIGH);
  } else if (Message == "<SA02- ACIONADO>") {
    //LIFE_PROOF_4 = current_time;
    digitalWrite(LED_ALARMED_4, HIGH);
  }

  if (Message == "<EN01- CONNECTED>") {
    //LIFE_PROOF_1 = current_time;
    digitalWrite(LED_ALARMED_1, LOW);
  } else if (Message == "<EN02- CONNECTED>") {
    //LIFE_PROOF_2 = current_time;
    digitalWrite(LED_ALARMED_2, LOW);
  } else if (Message == "<SA01- CONNECTED>") {
    //LIFE_PROOF_3 = current_time;
    digitalWrite(LED_ALARMED_3, LOW);
  } else if (Message == "<SA02- CONNECTED>") {
    //LIFE_PROOF_4 = current_time;
    digitalWrite(LED_ALARMED_4, LOW);
  }

}

// =================================================================================

void checkNetwork() {

  if (!ALARMED) {

    bool Net2 = false;

    while (WiFi.status() != WL_CONNECTED) // Tentando conectar na Rede WiFi
    {

      digitalWrite(LED_NETWORK, HIGH);
      delay(500);
      digitalWrite(LED_NETWORK, LOW);

      if (!Net2) {
        for (int i = 0; i < 5; i++) {
          if (WiFi.status() != WL_CONNECTED) {
            Serial.print("Falha de Conexão na Rede 1\n");
            digitalWrite(LED_NETWORK, HIGH);
            delay(1500);
          } else {
            digitalWrite(LED_NETWORK, LOW);
          }
        }

        if (WiFi.status() != WL_CONNECTED) {
          Net2 = true;
          WiFi.begin(WLAN_SSID2, WLAN_PASS2); //Inicia WiFi com os dados preenchidos
          digitalWrite(LED_NETWORK, HIGH);
        } else {
          digitalWrite(LED_NETWORK, LOW);
        }

      } else {
        for (int i = 0; i < 5; i++) {
          if (WiFi.status() != WL_CONNECTED) {
            Serial.print("Falha de Conexão na Rede 2\n");
            digitalWrite(LED_NETWORK, HIGH);
            delay(1500);
          } else {
            digitalWrite(LED_NETWORK, LOW);
          }
        }

        if (WiFi.status() != WL_CONNECTED) {
          Net2 = false;
          WiFi.begin(WLAN_SSID, WLAN_PASS); //Inicia WiFi com os dados preenchidos
          digitalWrite(LED_NETWORK, HIGH);
        } else {
          digitalWrite(LED_NETWORK, LOW);
        }

      }

    }

    if (!Net2) {
      Serial.print("ACCESS1 => Conectado | IP: "); // Imprime na Serial
    } else {
      Serial.print("ACCESS2 => Conectado | IP: "); // Imprime na Serial
    }

    Serial.print(WiFi.localIP()); //Imprime o Endereço IP Local do ESP8266
    Serial.print(" | MAC: ");
    Serial.println(WiFi.macAddress());

    digitalWrite(LED_NETWORK, HIGH);
    delay(250);
    digitalWrite(LED_NETWORK, LOW);
    delay(1);

  }

}
