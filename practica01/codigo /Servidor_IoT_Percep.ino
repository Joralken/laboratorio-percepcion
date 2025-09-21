#include <WiFi.h>

const char* ssid = "ssid";
const char* password = "password";

WiFiServer server(80);

String header; //variable para almacenar el encabezado de la solicitud HTTP


#define LED_BUILTIN 2 // pin de salida
String outState = "apagado"; //variables auxiliar para almacenar el estado actual de la salida


unsigned long lastTime, timeout = 2000;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  

  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  

  Serial.println("");
  Serial.println("Dispositivo conectado.");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
}

void loop(){
  
  WiFiClient client = server.available();   //El ESP32 siempre está escuchando a los clientes entrantes

  if (client) {
                               
    lastTime = millis();
    
    Serial.println("Nuevo cliente.");      
    String currentLine = ""; // Cadena para contener los datos entrantes del cliente          
    while (client.connected() && millis() - lastTime <= timeout) 
    {
      
      if (client.available()) {        
        char c = client.read();            
        Serial.write(c);                  
        header += c;
        // Si recibe salto de linea y la currentLine esta vacia,
        // la peticion http finalizó, se responde:
        if (c == '\n') {   
                        
          if (currentLine.length() == 0) {

            // Los encabezados HTTP siempre comienzan con un código de respuesta (por ejemplo, HTTP/1.1 200 OK) 
            // y un tipo de contenido para que el cliente sepa lo que viene, luego una línea en blanco:
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            
            if (header.indexOf("GET /LED_BUILTIN/on") >= 0) {
              Serial.println("GPIO LED_BUILTIN encendido");
              outState = "encendido";
              digitalWrite(LED_BUILTIN, HIGH);
            } 
            else if (header.indexOf("GET /LED_BUILTIN/off") >= 0) {
              Serial.println("GPIO LED_BUILTIN apagado");
              outState = "apagado";
              digitalWrite(LED_BUILTIN, LOW);
            } 
            
            // Pagina web
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonOff {background-color: #555555;}</style></head>");
            
         
            client.println("<body><h1>Control Remoto</h1>");  // Titulo
            
  
            client.println("<p>Dispositivo: " + outState + "</p>"); // Estado del dispositivo
                  
            if (outState=="apagado") {
              client.println("<p><a href=\"/LED_BUILTIN/on\"><button class=\"button\">Encender</button></a></p>");
            } else {
              client.println("<p><a href=\"/LED_BUILTIN/off\"><button class=\"button buttonOff\">Apagar</button></a></p>");
            } 
               
            client.println();
           
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {// si tiene algo diferente a un carácter de retorno de carro 
          currentLine += c;      
        }
      }
    }
    
    header = "";  
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }
}
