#include <WiFi.h>
#include <PubSubClient.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

const char* ssid = "KostAl";
const char* password = "kost123";
const char* mqtt_server = "localhost";
const char* mqtt_username = "user123";
const char* mqtt_password = "pass123";
const int mqtt_port = 1883;
const char* topic = "sensor_data";

WiFiClient espClient;
PubSubClient client(espClient);

IPAddress server_addr(192, 168, 1, 100);  // IP address MySQL server
char user[] = "root";
char password[] = "";
char dbname[] = "db_motion";

void setup_wifi() {
  // Koneksi ke jaringan WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Proses pesan yang diterima dari broker MQTT (tidak digunakan dalam contoh ini)
}

void reconnect() {
  // Loop sampai terhubung ke server MQTT
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client", user123, pass123)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void saveToMySQL(String data) {
  Serial.println("Saving data to MySQL database...");
  MySQL_Connection conn((Client *)&espClient);

  if (conn.connect(server_addr, 3306, user, password, dbname)) {
    Serial.println("Connected to MySQL server");
    delay(1000);

    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    char query[128];
    sprintf(query, "INSERT INTO sensor_data (data) VALUES ('%s')", data.c_str());
    cur_mem->execute(query);
    delete cur_mem;
    conn.close();
    Serial.println("Data saved to MySQL");
  } else {
    Serial.println("Connection to MySQL failed");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Proses pesan yang diterima dari broker MQTT
  String data = "";
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
  }
  Serial.println("Data received from MQTT broker: " + data);

  // Simpan data ke MySQL
  saveToMySQL(data);
}
