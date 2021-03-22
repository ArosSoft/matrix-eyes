/*********
  Руи Сантос
  Более подробно о проекте на: http://randomnerdtutorials.com
*********/
// wikihandbk.com/wiki/ESP32:Примеры/Машинка-робот_с_дистанционным_WiFi-управлением#WiFi

// загружаем библиотеку для WiFi:
#include <ESP8266WiFi.h>
#define PIN_D1 5  // gpio5 = D1  PWM_A
#define PIN_D2 4  // gpio4 = D2  PWM_B
#define PIN_D3 0  // gpio0 = D3  DA (A- A+) подключается двигатель 
#define PIN_D4 2  // gpio2 = D4  DB (B- B+) подключается двигатель

// вставляем ниже SSID и пароль для своей WiFi-сети:
const char* ssid     = "KVANT";
const char* password = "Password123";
//char* ssid     = "KVANT";
//char* password = "Password123";

// создаем объект сервера и задаем ему порт «80»:
WiFiServer server(80);

// переменная для хранения HTTP-запроса:
String header;

// переменные для свойств широтно-импульсной модуляции (ШИМ):
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;

// переменные для расшифровки HTTP-запроса GET:
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;

void setup() {

  Serial.begin(115200);

  // переключаем контакты моторов в режим «OUTPUT»:
  pinMode(PIN_D1, OUTPUT); // инициализируем Pin как выход
  pinMode(PIN_D2, OUTPUT); // инициализируем Pin как выход
  pinMode(PIN_D3, OUTPUT); // инициализируем Pin как выход
  pinMode(PIN_D4, OUTPUT); // инициализируем Pin как выход

  /***************************************** надо разбираться
    // задаем настройки ШИМ-канала:
    ledcSetup(pwmChannel, freq, resolution);

    // подключаем ШИМ-канал 0 к контактам ENA и ENB,
    // т.е. к GPIO-контактам для управления скоростью вращения моторов:
    ledcAttachPin(enable1Pin, pwmChannel);
    ledcAttachPin(enable2Pin, pwmChannel);

    // подаем на контакты ENA и ENB
    // ШИМ-сигнал с коэффициентом заполнения «0»:
    ledcWrite(pwmChannel, dutyCycle);
  **************************************/
  // подключаемся к WiFi-сети при помощи заданных выше SSID и пароля:
  Serial.print("Connecting to ");  //  "Подключаемся к "
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // печатаем в мониторе порта
  // локальный IP-адрес и запускаем веб-сервер:
  Serial.println("");
  Serial.println("WiFi connected.");  //  "Подключились к WiFi-сети."
  Serial.println("IP address: ");  //  "IP-адрес: "
  Serial.println(WiFi.localIP());
  server.begin();
}

void right()
{
  digitalWrite(PIN_D1, HIGH);   // PWM_A HIGH
  digitalWrite(PIN_D3, HIGH);   // DA HIGH
  digitalWrite(PIN_D2, LOW);   // PWM_B HIGH, изменяется направление вращения двигателя на контактах (B- B+)
  digitalWrite(PIN_D4, LOW);   // DB HIGH
}

void left()
{
  digitalWrite(PIN_D2, HIGH);   // PWM_B HIGH, изменяется направление вращения двигателя на контактах (B- B+)
  digitalWrite(PIN_D4, LOW);   // DB HIGH
  digitalWrite(PIN_D1, LOW);   // PWM_A HIGH
  digitalWrite(PIN_D3, LOW);   // DA HIGH
}

void forward()
{
  digitalWrite(PIN_D1, HIGH);   // PWM_A HIGH
  digitalWrite(PIN_D3, HIGH);   // DA HIGH
  digitalWrite(PIN_D2, HIGH);   // PWM_B HIGH, изменяется направление вращения двигателя на контактах (B- B+)
  digitalWrite(PIN_D4, LOW);   // DB HIGH
}

void backward()
{
  digitalWrite(PIN_D1, HIGH);   // PWM_A HIGH
  digitalWrite(PIN_D3, LOW);   // DA HIGH
  digitalWrite(PIN_D2, HIGH);   // PWM_B HIGH, изменяется направление вращения двигателя на контактах (B- B+)
  digitalWrite(PIN_D4, HIGH);   // DB HIGH
}

void stop_motor()
{
  digitalWrite(PIN_D1, LOW);   // PWM_A HIGH
  digitalWrite(PIN_D3, LOW);   // DA HIGH
  digitalWrite(PIN_D2, LOW);   // PWM_B HIGH, изменяется направление вращения двигателя на контактах (B- B+)
  digitalWrite(PIN_D4, LOW);   // DB HIGH
}

// =================== MAIN LOOP ===============================

void loop() {
  WiFiClient client = server.available();  // Запускаем прослушку
  // входящих клиентов.

  if (client) {                            // Если подключился
    // новый клиент,
    Serial.println("New Client.");         // печатаем в монитор порта
    // сообщение об этом.
    String currentLine = "";               // Создаем строку
    // для хранения данных,
    // пришедших от клиента.
    while (client.connected()) {           // Запускаем цикл while(),
      // который будет работать,
      // пока клиент подключен.
      if (client.available()) {            // Если у клиента
        // есть байты, которые
        // можно прочесть,
        char c = client.read();            // считываем байт
        Serial.write(c);                   // и печатаем его
        // в мониторе порта.
        header += c;
        if (c == '\n') {                   // Если полученный байт –
          // это символ новой строки.
          // Если мы получили два символа новой строки подряд,
          // то это значит, что текущая строка пуста.
          // Это конец HTTP-запроса клиента, поэтому отправляем ответ:
          if (currentLine.length() == 0) {
            // HTTP-заголовки всегда начинаются с кода ответа
            // (например, с «HTTP/1.1 200 OK»),
            // а также с информации о типе контента,
            // чтобы клиент знал, что получает.
            // После этого пишем пустую строчку:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            //  "Соединение: отключено"
            client.println();

            // Этот код отвечает за управление контактами моторов
            // согласно тому, какие нажаты кнопки на веб-странице:
            if (header.indexOf("GET /forward") >= 0) {
              Serial.println("Forward");  //  "Вперед"
              forward();
            }  else if (header.indexOf("GET /left") >= 0) {
              Serial.println("Left");  //  "Влево"
              left();
            }  else if (header.indexOf("GET /stop") >= 0) {
              Serial.println("Stop");  //  "Стоп"
              stop_motor();
            } else if (header.indexOf("GET /right") >= 0) {
              Serial.println("Right");  //  "Вправо"
              right();
            } else if (header.indexOf("GET /reverse") >= 0) {
              Serial.println("Reverse");  //  "Назад"
              backward();
            }
            // Показываем веб-страницу:
            client.println("<!DOCTYPE HTML><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // При помощи CSS задаем стиль кнопок.
            // Попробуйте поэкспериментировать
            // с атрибутами «background-color» и «font-size»,
            // чтобы стилизовать кнопки согласно своим предпочтениям:
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: #4CAF50;");
            client.println("border: none; color: white; padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script></head>");

            // веб-страница:
            client.println("<p><button class=\"button\" onclick=\"moveForward()\">FORWARD</button></p>");
            client.println("<div style=\"clear: both;\"><p><button class=\"button\" onclick=\"moveLeft()\">LEFT </button>");
            client.println("<button class=\"button button2\" onclick=\"stopRobot()\">STOP</button>");
            client.println("<button class=\"button\" onclick=\"moveRight()\">RIGHT</button></p></div>");
            client.println("<p><button class=\"button\" onclick=\"moveReverse()\">REVERSE</button></p>");
            client.println("<p>Motor Speed: <span id=\"motorSpeed\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"100\" step=\"25\" id=\"motorSlider\" onchange=\"motorSpeed(this.value)\" value=\"" + valueString + "\"/>");

            client.println("<script>$.ajaxSetup({timeout:1000});");
            client.println("function moveForward() { $.get(\"/forward\"); {Connection: close};}");
            client.println("function moveLeft() { $.get(\"/left\"); {Connection: close};}");
            client.println("function stopRobot() {$.get(\"/stop\"); {Connection: close};}");
            client.println("function moveRight() { $.get(\"/right\"); {Connection: close};}");
            client.println("function moveReverse() { $.get(\"/reverse\"); {Connection: close};}");
            client.println("var slider = document.getElementById(\"motorSlider\");");
            client.println("var motorP = document.getElementById(\"motorSpeed\"); motorP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; motorP.innerHTML = this.value; }");
            client.println("function motorSpeed(pos) { $.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");

            client.println("</html>");

            // Пример HTTP-запроса: «GET /?value=100& HTTP/1.1»;
            // Он задает коэффициент заполнения ШИМ на 100% (255):
            if (header.indexOf("GET /?value=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1 + 1, pos2);
              // Задаем скорость мотора:
              /******************* нужно разбираться
                if (valueString == "0") {
                ledcWrite(pwmChannel, 0);
                digitalWrite(motor1Pin1, LOW);
                digitalWrite(motor1Pin2, LOW);
                digitalWrite(motor2Pin1, LOW);
                digitalWrite(motor2Pin2, LOW);
                }
                else {
                dutyCycle = map(valueString.toInt(), 25, 100, 200, 255);
                ledcWrite(pwmChannel, dutyCycle);
                Serial.println(valueString);
                }
              ***************************/
            }
            // HTTP-ответ заканчивается еще одной пустой строкой:
            client.println();
            // Выходим из цикла while():
            break;
          } else {  // Если получили символ новой строки,
            // то очищаем переменную «currentLine»:
            currentLine = "";
          }
        } else if (c != '\r') {  // Если получили что-либо,
          // кроме символа возврата каретки...
          currentLine += c;      // ...добавляем эти данные
          // в конец переменной «currentLine»
        }
      }
    }
    // Очищаем переменную «header»:
    header = "";
    // Отключаем соединение:
    client.stop();
    Serial.println("Client disconnected.");  // "Клиент отключен."
    Serial.println("");
  }
}
