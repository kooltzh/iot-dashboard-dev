#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "ds3231.h"
dht DHT;
#define BUFF_MAX 128
#define DHT11_PIN 13

int potPin = 0;
uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 1000;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
void setup(){
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial.println("Time Stamp,Tep(from LM35_analog),Temperature,Humidity");// output and display characters beginning with Tep
  //Serial1.println("Time Stamp,Tep(from LM35_analog),Temperature,Humidity");// output and display characters beginning with Tep  
  Wire.begin();
  DS3231_init(DS3231_CONTROL_INTCN);
  memset(recv, 0, BUFF_MAX);
  Serial.println(recv);
//  Serial.println("GET time");
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("SPIRIT LAB");
}

void parse_cmd(char *cmd, int cmdsize)
{
    uint8_t i;
    uint8_t reg_val;
    char buff[BUFF_MAX];
    struct ts t;

    //snprintf(buff, BUFF_MAX, "cmd was '%s' %d\n", cmd, cmdsize);
    //Serial.print(buff);

    // TssmmhhWDDMMYYYY aka set time
    if (cmd[0] == 84 && cmdsize == 16) {
        //T355720619112011
        t.sec = inp2toi(cmd, 1);
        t.min = inp2toi(cmd, 3);
        t.hour = inp2toi(cmd, 5);
        t.wday = cmd[7] - 48;
        t.mday = inp2toi(cmd, 8);
        t.mon = inp2toi(cmd, 10);
        t.year = inp2toi(cmd, 12) * 100 + inp2toi(cmd, 14);
        DS3231_set(t);
        Serial.println("OK");
    } else if (cmd[0] == 49 && cmdsize == 1) {  // "1" get alarm 1
        DS3231_get_a1(&buff[0], 59);
        Serial.println(buff);
        Serial.println("OK1");
    } else if (cmd[0] == 50 && cmdsize == 1) {  // "2" get alarm 1
        DS3231_get_a2(&buff[0], 59);
        Serial.println(buff);
        Serial.println("OK2");
    } else if (cmd[0] == 51 && cmdsize == 1) {  // "3" get aging register
        Serial.print("aging reg is ");
        Serial.println(DS3231_get_aging(), DEC);
        Serial.println("OK3");
    } else if (cmd[0] == 65 && cmdsize == 9) {  // "A" set alarm 1
        DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A1IE);
        //ASSMMHHDD
        Serial.println("OK4");
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // ss, mm, hh, dd
        }
        uint8_t flags[5] = { 0, 0, 0, 0, 0 };
        DS3231_set_a1(time[0], time[1], time[2], time[3], flags);
        DS3231_get_a1(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 66 && cmdsize == 7) {  // "B" Set Alarm 2
        DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A2IE);
        //BMMHHDD
        Serial.println("OK5");
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // mm, hh, dd
        }
        uint8_t flags[5] = { 0, 0, 0, 0 };
        DS3231_set_a2(time[0], time[1], time[2], flags);
        DS3231_get_a2(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 67 && cmdsize == 1) {  // "C" - get temperature register
        Serial.print("temperature reg is ");
        Serial.println(DS3231_get_treg(), DEC);
        Serial.println("OK6");
    } else if (cmd[0] == 68 && cmdsize == 1) {  // "D" - reset status register alarm flags
        reg_val = DS3231_get_sreg();
        reg_val &= B11111100;
        DS3231_set_sreg(reg_val);
        Serial.println("OK7");
    } else if (cmd[0] == 70 && cmdsize == 1) {  // "F" - custom fct
        reg_val = DS3231_get_addr(0x5);
        Serial.print("orig ");
        Serial.print(reg_val,DEC);
        Serial.print("month is ");
        Serial.println(bcdtodec(reg_val & 0x1F),DEC);
        Serial.println("OK8");
    } else if (cmd[0] == 71 && cmdsize == 1) {  // "G" - set aging status register
        DS3231_set_aging(0);
        Serial.println("OK9");
    } else if (cmd[0] == 83 && cmdsize == 1) {  // "S" - get status register
        Serial.print("status reg is ");
        Serial.println(DS3231_get_sreg(), DEC);
        Serial.println("OK10");
    } else {
        Serial.print("unknown command prefix ");
        Serial.println(cmd[0]);
        Serial.println(cmd[0], DEC);
        Serial.println("OK11");
    }
}

void loop()
{
  
    char in;
    char buff[BUFF_MAX];
    unsigned long epoc;
    unsigned long now = millis();
    struct ts t;

    // show time once in a while
    if ((now - prev > interval) && (Serial.available() <= 0)) {
        DS3231_get(&t);

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
#ifdef __AVR__
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d %ld", t.year,
#error AVR
#else
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d %d", t.year,
#endif
             t.mon, t.mday, t.hour, t.min, t.sec, t.unixtime);
#else
        snprintf(buff, BUFF_MAX, "%02d.%02d %02d:%02d:%02d",
             t.mon, t.mday, t.hour, t.min, t.sec);
#endif
        epoc = ((t.year - 1970) * 365 * 24 * 3600) + ((t.mon -1 )* 30 * 24 * 3600) + (t.mday * 24 * 3600) + (t.hour * 3600) + (t.min * 60) + t.sec;
        //Serial.println(get_unixtime(t));

        //Serial.println(epoc);
        //Serial.print(t);
        Serial.print(buff);
        //Serial1.print(buff);//not sending time to wifi mod
        lcd.setCursor(0,0); //col=0, line=0
        lcd.print(buff);
        Serial.print(",");
        //Serial1.print(",");
        prev = now;
    }

    if (Serial.available() > 0) {
        in = Serial.read();

        if ((in == 10 || in == 13) && (recv_size > 0)) {
            parse_cmd(recv, recv_size);
            recv_size = 0;
            recv[0] = 0;
        } else if (in < 48 || in > 122) {;       // ignore ~[0-9A-Za-z]
        } else if (recv_size > BUFF_MAX - 2) {   // drop lines that are too long
            // drop
            recv_size = 0;
            recv[0] = 0;
        } else if (recv_size < BUFF_MAX - 2) {
            recv[recv_size] = in;
            recv[recv_size + 1] = 0;
            recv_size += 1;
        }
    }
delay(1000);
  int val;// define variable
  int dat;// define variable
  val=analogRead(0);// read the analog value of the sensor and assign it to val
  dat=(125*val)>>8;// temperature calculation formula
  lcd.setCursor(0,1);
  Serial.print(dat);// output and display value of dat
  Serial.print(",");
  //Serial1.print(dat);// output and display value of dat from analog device
  //Serial1.print(",");
  lcd.print(dat);
  lcd.print(",");
  
  int chk = DHT.read11(DHT11_PIN);
//  Serial.print("Temperature = ");
  Serial.print(DHT.temperature);
  Serial.print(",");
  Serial1.print(DHT.temperature);
  Serial1.print(",");
  lcd.print(DHT.temperature);
  lcd.print(",");
  
//  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  Serial1.println(DHT.humidity);
  lcd.print(DHT.humidity);
  delay(1000);
}

