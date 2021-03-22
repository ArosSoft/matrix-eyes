#include "ESP8266WiFi.h"
//#include <esp8266wifi.h>
//#include <esp8266webserver.h>
#include <ESP8266WebServer.h>


int val = 0; // По умолчанию диод не горит
const char *ssid = "ARSndKirZH"; // Название сети WiFi
const char *password = "123456789"; // Пароль для подключения
ESP8266WebServer server(80); // Создаем веб сервер на 80 порту

// Метод формирует стартовую страницу 192.168.4.1
// Выводит в браузер текущее состояние диода и две кнопки
void handleRoot() {
//String s = (val) ? "<h1>LED On</h1>" : "<h1>LED Off</h1>";
//s += "<h2>LED On ";
//s += "LED Off</h2> <hr> <div style=border:solid 1px> <a href=/led/on>test 1</a>  </div> <hr> <a href=/led/off>test 2</a>";
//String s = (val) ? "<h1>LED On</h1>" : "<h1>LED Off</h1>";
String s = " ";
s += " <head> <title>Wifi Server Custom Web Page</title> </head> <body style=background:#ffff00;>";
s += "<style> .controll{ background:#00ff00;font-size:54px;border: solid 1px;padding:10px;width:400px;cursor:pointer;margin:10px;}";
s += ".controll:hover{background:#ff0000;color:#fff;}";
s += "a{text-decoration:none; color:#777;}";
s += "</style>";

s += "<div align=center>";
s += "<h1 style=font-size:72px;> Remote Controll </h1>";
s += "<div class=controll><a href=led/on>button 1</a></div>";
s += "<div class=controll><a href=/led/off>button 2</a></div>";
s += "<div class=controll><a href=/go/left>LEFT</a></div>";
s += "</div> </body> ";

server.send(200, "text/html", s);
}

// Метод включения диода
void ledOn() {
val = 1; // Включаем диод
digitalWrite(BUILTIN_LED, val);
// Перенаправление обратно на стартовую страницу
server.sendHeader("Location", String("/"), true);
server.send ( 302, "text/plain", "");
}

void goLeft() {

digitalWrite(BUILTIN_LED, LOW);
server.sendHeader("Location", String("/"), true);
server.send ( 302, "text/plain", "");

}

// Метод выключения диода
void ledOff() {
val = 0; // Выключаем диод
digitalWrite(BUILTIN_LED, val);
// Перенаправление обратно на стартовую страницу
server.sendHeader("Location", String("/"), true);
server.send ( 302, "text/plain", "");
}

// Функция настройки. Выполняется один раз при запуске
void setup() {
delay(1000); // Ждём секунду
WiFi.softAP(ssid, password); // Создаём точку WiFi
// Указываем по каким роутам какие методы запускать
server.on("/", handleRoot);
server.on("/led/on", ledOn);
server.on("/led/off", ledOff);
server.on("/go/left",goLeft);
server.begin();
// Диод по умолчанию выключен
pinMode(BUILTIN_LED, OUTPUT);
digitalWrite(BUILTIN_LED, val);
}

// Основной цикл программы
void loop() {
// Ждём подключения
server.handleClient();
}
