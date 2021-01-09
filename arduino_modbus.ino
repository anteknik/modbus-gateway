#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
#include "DHT.h"
/*
Cara Mudah Menjadikan arduino menjadi alat modbus
Alamat device ID 5 pada 9600 baud.
*/


#define LED 13
//#define TOMBOL 3
#define dhtPin 7
#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);

unsigned long previousMillis = 0;
const long interval = 5000; 

//Setup register bank
//semua data ditulis dan disimpan disini
modbusDevice regBank;
//mode slave
modbusSlave slave;

void setup()
{   
  pinMode(LED, OUTPUT);
  //pinMode(TOMBOL, INPUT);
//menentukan device ID.  
  regBank.setId(5);
  dht.begin();
/*
modbus registers format seperti berikut
00001-09999  Digital Outputs, master dapat menulis dan membaca data
10001-19999  Digital Inputs, master hanya bisa baca data
30001-39999  Analog Inputs,  master hanya bisa membaca nilai pada register
40001-49999  Analog Outputs, master dapat tulis baca register

*/

//tambahkan alamat LED pada register bank
  regBank.add(1); //LED
  

//Tombol sebagai Input registers 10001
  regBank.add(10001);  //TOMBOL
 

//Analog Input registers 30001-30003 untuk DHT11
  regBank.add(30001);  //DHT11 - suhu puluhan
  regBank.add(30002);  //DHT11 - suhu desimal
  regBank.add(30003);  //DHT11 - kelembaban
  
//Coba simpan register umum 40001-40015 
  regBank.add(40001);  // w
  regBank.add(40002);  // w
  regBank.add(40003);  // w
  regBank.add(40004);  // .
  regBank.add(40005);  // a
  regBank.add(40006);  // n
  regBank.add(40007);  // t
  regBank.add(40008);  // o
  regBank.add(40009);  // n
  regBank.add(40010);  // 5
  regBank.add(40011);  // 5
  regBank.add(40012);  // .
  regBank.add(40013);  // c
  regBank.add(40014);  // o
  regBank.add(40015);  // m


/*
protocol handler yang akan menulis dan membaca
register data.  
*/
  slave._device = &regBank;  

// Serial pada 9600
  slave.setBaud(9600);   
//isi register umum diawal
  regBank.set(40001,'w');
  regBank.set(40002,'w');
  regBank.set(40003,'w');
  regBank.set(40004,'.');
  regBank.set(40005,'a');
  regBank.set(40006,'n');
  regBank.set(40007,'t');
  regBank.set(40008,'o');
  regBank.set(40009,'n');
  regBank.set(40010,'5');
  regBank.set(40011,'5');
  regBank.set(40012,'.');
  regBank.set(40013,'c');
  regBank.set(40014,'o');
  regBank.set(40015,'m');
}

//fungsi memecah angka dibelakang koma dht11
int ExtractDecimalPart(float Value, int numberOfDecimals)
{
  float temp = Value - (long)(Value); 
  long p = 1;
  for (int i=0; i< numberOfDecimals; i++) p*=10;
  long DecimalPart = p * temp;
  return DecimalPart;
}

void loop()
{
//Melihat data register 1 dan lempar ke LED
  digitalWrite(LED,regBank.get(1));  

//Membaca input tombol dan lempar ke register 10001
//  regBank.set(10001, digitalRead(TOMBOL));

//membaca dht11 tiap 5 detik
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor 
    previousMillis = currentMillis;   
 
    int h = dht.readHumidity();
    float t= dht.readTemperature();
    int t1 = t;
    int t2 = ExtractDecimalPart(t,2); //pecah desimal

    regBank.set(30001,t1); // tulis suhu depan koma 
    regBank.set(30002,t2); // tulis suhu belakang koma
    regBank.set(30003,h); // tulis kelembaban


    }

   
// loop terus sebagai slave    
  slave.run();  

 
}
