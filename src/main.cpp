#include <Arduino.h>
#include "PCF8574.h"

#define SERIAL_BAUD 115200

#define PCF8_ADDR1 0x20
#define PCF8_ADDR2 0x21

const int numpair = 8 ;

// Set i2c address
PCF8574 pcf1(PCF8_ADDR1);
PCF8574 pcf2(PCF8_ADDR2);

// Tentukan pasangan kabel
const int pair0[][2] = // source - destination pada pcf1
{
    {P0, P0}, // P0 dari pcf1 ke P0 di pcf2
    {P1, P1}, // P1 dari pcf1 ke P1 di pcf2
    {P2, P2}, // P2 dari pcf1 ke P2 di pcf2
    {P3, P3}, // P3 dari pcf1 ke P3 di pcf2
    {P4, P4}, // P4 dari pcf1 ke P4 di pcf2
    {P5, P5}, // P5 dari pcf1 ke P5 di pcf2
    {P6, P6}, // P6 dari pcf1 ke P6 di pcf2
    {P7, P7}  // P7 dari pcf1 ke P7 di pcf2
};

void initPCF8574()
{
  if (pcf1.begin())
    Serial.println("ADDR1 OK");
  else
    Serial.println("ADDR1 KO");

  if (pcf2.begin())
    Serial.println("ADDR2 OK");
  else
    Serial.println("ADDR2 KO");
}

void setup()
{
  // Memulai I2C
  Serial.begin(SERIAL_BAUD);
  delay(1000);

  for (int j = 0; j < numpair; j++)
  {
    pcf1.pinMode(pair0[j][0], OUTPUT);       // TX dari pcf1
    pcf2.pinMode(pair0[j][1], INPUT_PULLUP); // RX dari pcf2
  }

  // Mulai PCF
  Serial.println("Mengirim Koneksi.......");
  initPCF8574();
}

void loop()
{
  Serial.println("Menguji kabel harness...");

  for (int i = 0; i < numpair; i++)
  {
    // Set semua output ke HIGH dulu sebelum pengujian
    for (int j = 0; j < numpair; j++)
    {
      pcf1.digitalWrite(pair0[j][0], HIGH);
    }
    delay(10); // Stabilkan sinyal

    // Kirim LOW ke satu pin output
    pcf1.digitalWrite(pair0[i][0], LOW);
    delay(10); // Stabilkan sinyal

    // Cek apakah hanya pasangan yang sesuai menerima LOW
    bool pasanganBenar = true;
    for (int j = 0; j < numpair; j++)
    {
      bool hasil0= (pcf2.digitalRead(pair0[j][1]) == LOW);

      if (j == i)
      {
        if (!hasil0)
          pasanganBenar = false; // Pasangan seharusnya dapat LOW tapi tidak
      }
      else
      {
        if (hasil0)
          pasanganBenar = false; // Ada pin lain yang ikut LOW (tertukar atau short)
      }
    }

    if (pasanganBenar)
    {
      Serial.print("Koneksi OK!:   ");
    }
    else
    {
      Serial.print("Kesalahan pasangan atau kabel putus:   ");
    }

    Serial.print("Koneksi dari ");
    Serial.print(pair0[i][0]);
    Serial.print(" ke ");
    Serial.println(pair0[i][1]);
  }

  Serial.println("Tes selesai!\n");
  delay(2000);
}
