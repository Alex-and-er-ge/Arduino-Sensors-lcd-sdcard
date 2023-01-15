#include <SD.h> // библиотека для SD Card.
#include <DS1302.h> // это скачанная библиотека для DS1302 Real Time Clock Module.

#include <LiquidCrystal.h>   // библиотека для lcd 1602.
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);  // пины на Arduino uno
// pin 9 - Serial clock out (SCLK)
// pin 8 - Serial data out (DIN)
// pin 7 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 5 - LCD reset (RST)

#include <Wire.h>   // библиотека для подключения датчиков по 1 проводу

#include "SHT21.h"  // датчик температуры и влажности
SHT21 SHT21; 

DS1302 rtc(3, 2, 17); 
// пины на DS1302 Real Time Clock Module
// ** RST - pin 2
// ** DAT - pin 3
// ** CLK - pin 17
// ! +5v не подключаем достаточно только "-" / землю

File myFile; // SD card

int SaveTemperature; // Для фиксации температуры.



void setup()
{
  Serial.begin(9600);  // Инициализируем Serial 
  lcd.begin(16, 2);    // Инициализируем lcd
  SHT21.begin();       // Инициализируем sht

  SaveTemperature = SHT21.getTemperature(); // Фиксируем текущую температуру.
  Serial.println("CLEARDATA");
  Serial.println("LABEL,time,date,h,t");
  
   //rtc.halt(false);
   //rtc.writeProtect(false);
  
  // для первоначальной установки даты и времени расскоментировать строки ниже и ввести нужное время
  //rtc.setDOW(WEDNESDAY);        // Set Day-of-Week to WEDNESDAY
  //rtc.setTime(10, 1, 0);       // Set the time to 10:01:00 (24hr format)
  //rtc.setDate(15, 5, 2019);   // Set the date to May 15th, 2019

  // Тестируем SD card на работоспособность.
  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) // Здесь можно изменить № pin  ** CS - pin 10.
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
// ----------------  

  SDcardWrite(); // Запускаем функцию записи на SD card.
}


void SDcardWrite()  // Делаем функцию, для записи на SD card.

{

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // Создаём новый, или открываем существующий файл на SD card,
  // например с именем dimasens.txt
  // максимум "8" символов может иметь имя файла 12345678.123 

  myFile = SD.open("dimasens.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to dimasens.txt...");
    // начинаем записывать на SD card, строку в файл dimasens.txt
    myFile.print(rtc.getDateStr()); // Текущую дату.
    myFile.print(" -- ");
    myFile.print(rtc.getTimeStr()); // текущие время.
    myFile.print(" -- ");
    myFile.print("Humidity: "); 
    myFile.print(SHT21.getHumidity()); // Значения влажности.
    myFile.print(" %\t");
    myFile.print("Temperature: "); 
    myFile.print(SHT21.getTemperature()); // Значения температуры.
    myFile.println(" *C"); 
    myFile.close();  // close the file:
    
    Serial.println("done.");
  } 
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening dimasens.txt");
  }
}

// Делаем функцию для чтения с SD card и вывода в Serial монитор.

void SDcardRead()
{
  // re-open the file for reading:
  myFile = SD.open("dimasens.txt");
  if (myFile) {
    Serial.println("dimasens.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } 
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening dimasens.txt");
  }
}



void loop()
{

  //  Для того чтоб бессмысленно не записывать на SD card данные с датчиков,
  // сделаем проверку. 
  // Только если прошлое значение температуры не равно текущему значению, 
  // то есть произошло изменение температуры.
  if (SaveTemperature != SHT21.getTemperature())

    // Можно и по-другому, увеличивая порог, обнаружения изменения температуры.
    // if (SaveTemperature >= (dht.readTemperature()+2) || SaveTemperature <= (dht.readTemperature()-2))
  {

    //   Serial.println(rtc.getDateStr());
    //  Serial.println(dht.readHumidity());
    //  Serial.println(dht.readTemperature());

    SDcardWrite(); // Запускаем функцию записи на SD card.

    // Снова фиксируем текущее значение температуры.
    SaveTemperature = SHT21.getTemperature(); 
  }


  // Для вывода в  Serial монитор данных с  SD card. 

  if (Serial.available() > 0) {   
    // Если отправляем через Serial монитор цифру 1 
    // запускаем функцию SDcardRead.
    // То есть выводим в Serial монитор всё содержимое файла dimasens.txt  
    if (Serial.read()-48 == 1)SDcardRead(); 
  }
  
// Выводим время и все измерения в serial:
  Serial.print("DATA,TIME,");
  Serial.print(rtc.getDateStr());
  Serial.print(",");

  Serial.print(SHT21.getHumidity());
  Serial.print(",");
  Serial.println(SHT21.getTemperature());

// Выводим время и все измерения на экран:
  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.print(SHT21.getHumidity());
  lcd.print("  T:");
  lcd.print(SHT21.getTemperature());

  lcd.setCursor(0, 2);
  lcd.print(rtc.getTimeStr());
  lcd.setCursor(11, 1);
  lcd.print(rtc.getDateStr());
 
  delay(10000);
}





