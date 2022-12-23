#include "BluetoothSerial.h"
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11 // DHT 11
const int ledPin = 2;
bool ledState = LOW;
DHT dht(DHTPIN, DHTTYPE);
int flag;

int ishumid = 1;
int iscel = 0; // 0 for F, 1 for C
int isheat_index = 1;
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
void readDHT();
void setup()
{
    pinMode(14, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);
    Serial.begin(115200);
    SerialBT.begin("AA"); // Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
    Serial.println(F("DHT test!"));

    dht.begin();
}

void loop()
{

    delay(200);

    if (SerialBT.available())
    {
        char message;
        message = SerialBT.read();
        if (message == '1')
        {
            Serial.println("LED ON");
            SerialBT.println("LED ON");
            ledState = HIGH;
            digitalWrite(ledPin, ledState);
        }
        else if (message == '0')
        {
            Serial.println("LED OFF");
            SerialBT.println("LED OFF");
            ledState = LOW;
            digitalWrite(ledPin, ledState);
        }
        else if (message == '2')

        {
            Serial.println("LED TOGGLE");
            SerialBT.println("LED TOGGLED");
            ledState = !ledState;
            digitalWrite(ledPin, ledState);
        }
        else if (message == '3')
        {
            Serial.println("LED READ");
            if (ledState == HIGH)
            {
                SerialBT.println("LED IS ON");
            }
            else
            {
                SerialBT.println("LED IS OFF");
            }
        }

        else if (message == 's')
        {
            Serial.println("Read DHT");
            readDHT();
        }
        else if (message == 'r')
        {
            int iterat = 5;
            while (iterat--)
            {
                Serial.println("Read DHT");
                readDHT();
                delay(1000);
            }
            SerialBT.println("DHT READ 5 TIMES");
        }
        else if (message == 'u')
        {

            while (1)
            {
                flag = touchRead(14);
                if (flag < 80)
                {
                    SerialBT.println("KEY PRESSED");
                    break;
                }
                readDHT();
                delay(2000);
            }
        }
        else if (message == 'H')
        {
            if (ishumid == 1)
            {
                ishumid = 0;
                SerialBT.println("HUMIDITY OFF");
            }
            else
            {
                ishumid = 1;
                SerialBT.println("HUMIDITY ON");
            }
        }
        else if (message == 'I')
        {
            if (isheat_index == 1)
            {
                isheat_index = 0;
                SerialBT.println("HEAT INDEX OFF");
            }
            else
            {
                isheat_index = 1;
                SerialBT.println("HEAT INDEX ON");
            }
        }

        else if (message == 'T')
        {
            if (iscel == 1)
            {
                iscel = 0;
                SerialBT.println("FAHRENHEIT ON");
            }
            else
            {
                iscel = 1;
                SerialBT.println("CELSIUS ON");
            }
        }

        else if (message == 'h')
        {

            delay(50);
            Serial.println("HELP");
            delay(50);
            SerialBT.println("*************");
            delay(50);
            SerialBT.println("HELP : ");
            delay(50);
            SerialBT.println("0 -   LED OFF");
            delay(50);
            SerialBT.println("1 -   LED ON");
            delay(50);
            SerialBT.println("2 -   LED TOGGLE");
            delay(50);
            SerialBT.println("3 -   LED READ");
            delay(50);
            SerialBT.println("-----------------------------------");
            delay(50);
            SerialBT.println("s -   Read DHT");
            delay(50);
            SerialBT.println("r -   Read DHT 10 times");
            delay(50);
            SerialBT.println("u -   Read DHT till keypress");
            delay(50);
            SerialBT.println("H -   Toggle Humidity");
            delay(50);
            SerialBT.println("I -   Toggle Heat Index");
            delay(50);
            SerialBT.println("T -   Toggle Temperature Units");
            delay(50);
            SerialBT.println("h -   HELP");
            delay(50);
            SerialBT.println("*************");
        }

        else
        {
            if (message == '\0' or message == ' ' or message == '\n' or message == 13)
            {
            }
            else
            {
                SerialBT.println("Invalid Command");
            }
        }
    }
}

void readDHT()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        SerialBT.println(F("Failed to read from DHT sensor!"));
        return;
    }

    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
    delay(50);
    SerialBT.println("*************");
    if (ishumid)
    {
        delay(50);
        String s = "";
        s += "Humidity          : ";
        s += String(h);
        s += " %";
        SerialBT.println(s);
        Serial.println(s);
    }
    if (iscel)
    {
        delay(50);
        String s = "";
        s += "Temperature   : ";
        s += String(t);
        s += " °C ";
        SerialBT.println(s);
        Serial.println(s);
        if (isheat_index)
        {
            delay(50);
            String s = "";
            s += " Heat index     : ";
            s += String(hic);
            s += " °C ";
            SerialBT.println(s);
            Serial.println(s);
        }
    }
    else
    {
        delay(50);
        String s = "";
        s += "Temperature   : ";
        s += String(f);
        s += " °F ";
        SerialBT.println(s);
        Serial.println(s);
        if (isheat_index)
        {
            delay(50);
            String s = "";
            s += "Heat index      : ";
            s += String(hif);
            s += " °F ";
            SerialBT.println(s);
            Serial.println(s);
        }
    }
    delay(50);
    SerialBT.println("*************");
}