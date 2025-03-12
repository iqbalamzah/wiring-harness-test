#include <Arduino.h>
#include "PCF8574.h"
#include "PCF8575.h"

#define SERIAL_BAUD 115200

#define PCF8_ADDR0 0x20
#define PCF8_ADDR1 0x21
#define PCF8_ADDR2 0x22

const int numpair0 = 16; // jumlah pasangan list array
const int numpair1 = 8;

// Set i2c address
PCF8574 pcf1(PCF8_ADDR1);
PCF8574 pcf2(PCF8_ADDR2);

PCF8575 pcf0(PCF8_ADDR0);

// Tentukan pasangan kabel
// Pengujian Pertama Front Harness
const int pair0[][2] = // source - destination pada pcf0  || FRONT HARNESS
    {
        // sementara menggunakan 16 dgn 8 Bit
        {P0, P0}, // dari TX 0 ke RX 0 connector J104 pin 1 ke pin 1 connector J105
        {P1, P1}, // dari TX 1 ke RX 1 connector J104 pin 2 ke pin 1 connector J114
        {P2, P2}, // dari TX 2 ke RX 2 connector J113 pin 1 ke pin 1 connector J108
        {P3, P3}, // dari TX 3 ke RX 3 connector J113 pin 2 ke pin 1 connector J111
        {P4, P4}, // dari TX 4 ke RX 4 connector J113 pin 3 ke pin 2 connector J111
        {P5, P5}, // dari TX 5 ke RX 5 connector J113 pin 4 ke pin 1 connector J91
        {P6, P6}, // dari TX 6 ke RX 6 connector J113 pin 5 ke pin 1 connector J140
        {P7, P7}, // dari TX 7 ke RX 7 connector J113 pin 6 ke pin 1 connector J85

        // sementara menggunakan 16 dgn 8 Bit
        {P7, P0},  // dari TX 8 ke RX 8 connector J113 pin 6 ke pin 1 connector J134
        {P8, P1},  // dari TX 9 ke RX 9 connector J113 pin 7 ke pin 1 connector J87
        {P8, P2},  // dari TX 10 ke RX 10 connector J113 pin 7 ke pin 1 connector J136
        {P9, P3},  // dari TX 11 ke RX 11 connector J113 pin 8 ke pin 1 connector J119
        {P10, P4}, // dari TX 12 ke RX 12 connector J113 pin 9 ke pin 1 connector J93
        {P10, P5}, // dari TX 13 ke RX 13 connector J113 pin 10 ke pin 1 connector J124
        {P11, P6}, // dari TX 14 ke RX 14 connector J113 pin 11 ke pin 1 connector J97
        {P11, P7}, // dari TX 15 ke RX 15 connector J113 pin 12 ke pin 1 connector J132

        /*
        // Belum terdefinisikan
        {P14, P16}, // dari TX 16 ke RX 16 connector J116 pin 1 ke pin 1 connector J99
        {P14, P17}, // dari TX 17 ke RX 17 connector J116 pin 2 ke pin 1 connector J117
        {P14, P18}, // dari TX 18 ke RX 18 connector J116 pin 3 ke pin 1 connector J128
        {P14, P19}  // dari TX 19 ke RX 19 connector J116 pin 5 ke pin 2 connector J128
        */
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

void initPCF8575()
{
  if (pcf0.begin())
    Serial.println("ADDR0 OK");
  else
    Serial.println("ADDR0 KO");
}

void setup()
{
  // Memulai I2C
  Serial.begin(SERIAL_BAUD);
  delay(1000);

  // definis pin IO
  for (int j = 0; j < numpair0; j++)
  {
    pcf0.pinMode(pair0[j][0], OUTPUT); // TX dari pcf0

    for (int j = 0; j < 8; j++)
    {
      pcf1.pinMode(pair0[j][1], INPUT_PULLUP); // RX dari pcf1
    }
    for (int j = 8; j < 16; j++)
    {
      pcf2.pinMode(pair0[j][1], INPUT_PULLUP); // RX dari pcf2
    }
  }

  // Mulai PCF
  Serial.println("Mengirim Koneksi.......");
  initPCF8574();
  initPCF8575();
}

void loop()
{
  Serial.println("Menguji kabel harness...");


  // Uji Sinyal dari PCF0 as TX
  for (int i = 0; i < numpair0; i++)
  {
    // Set semua output ke HIGH dulu sebelum pengujian
    for (int j = 0; j < numpair0; j++)
    {
      pcf0.digitalWrite(pair0[j][0], HIGH);
    }
    delay(50); // Stabilkan sinyal

    // Kirim LOW ke satu pin output
    pcf0.digitalWrite(pair0[i][0], LOW);
    delay(50); // Stabilkan sinyal

    // Cek apakah hanya pasangan yang sesuai menerima LOW
    bool pasanganBenar = true;

    //pcf1 RX
    for (int j = 0; j < 8; j++)
    {
      bool hasil0 = (pcf1.digitalRead(pair0[j][1]) == LOW);

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

    //pcf2 RX
    for (int j = 8; j < 16; j++)
    {
      bool hasil0 = (pcf2.digitalRead(pair0[j][1]) == LOW);

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
