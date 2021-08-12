#include <ESP8266WiFi.h>
// ======================================================================
// --- Dados de Acesso do seu roteador ---
#define LED_NETWORK      2         // WIFI Module LED
#define BUTTON    D1        // Connectivity ReInitiate Button
#define WLAN_SSID   "ACCESS01"  // nome da sua rede
#define WLAN_PASS  "P@ssW0rd"   // senha de acesso do seu roteador
#define WLAN_SSID2   "ACCESS02"  // nome da sua rede
#define WLAN_PASS2  "P@ssW0rd"   // senha de acesso do seu roteador

const String ClientID = "SA01";   // Client Identification
bool ALARMED = false;
IPAddress server(192, 168, 1, 250);   // the fix IP address of the server
WiFiClient client;

//====================================================================================
void setup()
{
  pinMode(LED_NETWORK, OUTPUT); // Initialize the LED pin as an output
  Serial.begin(115200); // Inicia Serial com Baud Rate de 115200
  Serial.println();
  WiFi.mode(WIFI_STA); //Habilita o modo estação
  WiFi.begin(WLAN_SSID, WLAN_PASS); //Inicia WiFi com os dados preenchidos
  delay(1000); // aguarda 1 segundo
  Serial.print("Tentando conectar ...\n");
  checkNetwork();
  ESPRequest();  // Conecting The Device As A Client
}
//====================================================================================

void loop()
{
  checkNetwork();
  ESPRequest();
  ReadButton();
  checkAlarm();
}

//====================================================================================

void ReadButton()     // Reading Button Function
{

  int reading = digitalRead(BUTTON);

  if (reading == HIGH and !ALARMED)
  {
    ALARMED = true;
    delay(1500);
  }
  else if (reading == HIGH and ALARMED)
  {
    ALARMED = false;
  }
  delay(1);

}

void checkAlarm() {

  while (ALARMED) {

    int reading = digitalRead(BUTTON);

    client.stop();         // First Make Sure You Got Disconnected
    if (client.connect(server, 80))  // Connection to the server
    {

      Serial.println("Enviando <" + ClientID + "- ACIONADO> para o Server");
      client.println  ("<" + ClientID + "- ACIONADO>"); //If Sucessfully Connected Send Connection Message
      client.flush();
      Serial.println("Mensagem enviada com sucesso!");

      if (reading == HIGH)
      {
        ALARMED = false;
      }
      delay(250);

    } else {
      Serial.println("Falha de Conexão com o SERVER\n");
      digitalWrite(LED_NETWORK, LOW);
      delay(500);
      digitalWrite(LED_NETWORK, HIGH);
      checkAlarm();
    }

  }
}

//====================================================================================

void ESPRequest()
{
  client.stop();         // First Make Sure You Got Disconnected
  if (client.connect(server, 80))  // Connection to the server
  {
    Serial.println("Enviando <" + ClientID + "- CONNECTED> para o Server");
    client.println  ("<" + ClientID + "- CONNECTED>"); //If Sucessfully Connected Send Connection Message
    client.flush();
    Serial.println("Mensagem enviada com sucesso!");
  } else {
    Serial.println("Falha de Conexão com o SERVER\n");
    digitalWrite(LED_NETWORK, LOW);
    delay(500);
    digitalWrite(LED_NETWORK, HIGH);
    ESPRequest();
  }
}

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
            Serial.print("Falha de Conexão na ACCESS1\n");
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
            Serial.print("Falha de Conexão na ACCESS2\n");
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

      if (WiFi.status() == WL_CONNECTED) {
        ESPRequest();
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
