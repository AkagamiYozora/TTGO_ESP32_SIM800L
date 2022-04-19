// Your GPRS credentials (leave empty, if not needed)
// This apn work on By.U operator which will shown as Telkomsel
const char apn[]      = "apn"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

// TinyGSM Client for Internet connection
TinyGsmClient client(modem);

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  SerialMon.begin(115200);

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
//  modem.restart();
  modem.init();
  // use modem.init() if you don't need the complete restart
}

void loop() {
  // put your main code here, to run repeatedly:

  SerialMon.print("Connecting to APN: ");
  SerialMon.println(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
  } else{
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);
    bool res = modem.isGprsConnected();
    if(res){
      SerialMon.println("GPRS connected!");
    }
  
    String ccid = modem.getSimCCID();
    SerialMon.println("CCID:"+ccid);
  
    String imei = modem.getIMEI();
    SerialMon.println("IMEI:"+imei);
  
    String imsi = modem.getIMSI();
    SerialMon.println("IMSI:"+imsi);
  
    String cop = modem.getOperator();
    SerialMon.println("Operator:"+cop);
  
    IPAddress local = modem.localIP();
    SerialMon.print("Local IP:");
    SerialMon.println(local);
  
    int csq = modem.getSignalQuality();
    SerialMon.println("Signal quality:"+String(csq));

    float lat      = 0; float lon      = 0; float accuracy = 0;
    int   year     = 0; int   month    = 0; int   day      = 0;
    int   hour     = 0; int   min      = 0; int   sec      = 0;
    for (int8_t i = 15; i; i--) {
      SerialMon.println("Requesting current GSM location");
      if (modem.getGsmLocation(&lat, &lon, &accuracy, &year, &month, &day, &hour,
                               &min, &sec)) {
        SerialMon.println("Latitude:"+String(lat, 8)+"\tLongitude:"+String(lon, 8));
        SerialMon.println("Accuracy:"+String(accuracy));
        SerialMon.println("Year:"+String(year)+"\tMonth:"+String(month)+"\tDay:"+String(day));
        SerialMon.println("Hour:"+String(hour)+"\tMinute:"+String(min)+"\tSecond:"+String(sec));
        break;
      } else {
        SerialMon.println("Couldn't get GSM location, retrying in 15s.");
        delay(15000L);
      }
    }
    SerialMon.println("Retrieving GSM location again as a string");
    String location = modem.getGsmLocation();
    SerialMon.println("GSM Based Location String:"+location);
    //=============================================
    int   year3    = 0; int   month3   = 0; int   day3     = 0;
    int   hour3    = 0; int   min3     = 0; int   sec3     = 0;
    float timezone = 0;
    for (int8_t i = 5; i; i--) {
      SerialMon.println("Requesting current network time");
      if (modem.getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3,
                               &timezone)) {
        SerialMon.println("Year:"+String(year3)+"\tMonth:"+String(month3)+"\tDay:"+String(day3));
        SerialMon.println("Hour:"+String(hour3)+"\tMinute:"+String(min3)+"\tSecond:"+String(sec3));
        SerialMon.println("Timezone:"+String(timezone));
        break;
      } else {
        SerialMon.println("Couldn't get network time, retrying in 15s.");
        delay(15000L);
      }
    }
    SerialMon.println("Retrieving time again as a string");
    String time = modem.getGSMDateTime(DATE_FULL);
    SerialMon.println("Current Network Time:"+time);
    
    modem.gprsDisconnect();
    SerialMon.println(F("GPRS disconnected"));
  }
  delay(500);
}
