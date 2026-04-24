# Dual-Axis Solar Tracker dengan Arduino

Proyek ini adalah sistem pelacak matahari dua sumbu (Dual-Axis) otomatis berbasis Arduino. Sistem ini dirancang untuk memaksimalkan penyerapan energi pada panel surya dengan memastikan panel selalu menghadap tegak lurus ke arah datangnya cahaya matahari secara real-time.

---

## Penjelasan Sistem
Sistem ini menggunakan dua buah sensor cahaya (LDR) sebagai input utama untuk menggerakkan dua sumbu:
1. **Sumbu Horizontal (Base):** Mengikuti pergerakan matahari dari Timur ke Barat (Azimuth).
2. **Sumbu Vertikal (Tail/Tilt):** Menyesuaikan kemiringan panel terhadap ketinggian matahari di langit (Elevasi).

Sistem ini memiliki fitur **Hybrid Control**, di mana pengguna dapat berpindah dari mode otomatis (sensor LDR) ke mode manual (potensiometer & tombol).

## Fitur Utama
- **Auto Tracking:** Pelacakan otomatis menggunakan algoritma diferensial LDR.
- **Manual Override:** Kontrol penuh via Potensiometer (Base) dan Tombol (Tail).
- **Smooth Movement:** Pergerakan servo halus untuk mencegah kerusakan mekanik.
- **LCD Dashboard:** Menampilkan mode aktif dan sudut derajat servo secara real-time.
- **Serial Interface:** Monitoring dan kontrol mode melalui Serial Monitor.

## Library yang Digunakan
Pastikan library berikut sudah terinstal di Arduino IDE Anda sebelum melakukan upload kode:

1. **Servo Library (Built-in)** Digunakan untuk mengontrol motor servo SG90/MG995.  
    [GitHub: Arduino-Libraries/Servo](https://github.com/arduino-libraries/Servo)

2. **LiquidCrystal I2C** Digunakan untuk menampilkan data pada layar LCD 16x2 melalui komunikasi I2C.  
    [GitHub: johnrickman/LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)

3. **Wire Library (Built-in)** Library standar untuk komunikasi protokol I2C.  
    [GitHub: esp8266/Arduino (Wire)](https://github.com/esp8266/Arduino/tree/master/libraries/Wire)

---

## Konfigurasi Pin

| Komponen | Pin Arduino | Deskripsi |
| :--- | :--- | :--- |
| **Servo Base** | D9 | Rotasi Horizontal (Azimuth) |
| **Servo Tail** | D10 | Kemiringan Vertikal (Elevasi) |
| **LDR Left** | A0 | Sensor Cahaya Sisi Kiri |
| **LDR Right** | A1 | Sensor Cahaya Sisi Kanan |
| **Potensiometer** | A2 | Kontrol Manual Base |
| **Button Mode** | D5 | Switch Auto/Manual |
| **Button Inc** | D7 | Tambah Sudut Tail (Manual) |
| **Button Dec** | D6 | Kurangi Sudut Tail (Manual) |
| **LCD I2C** | A4 (SDA), A5 (SCL) | Antarmuka I2C |

---

## Instalasi & Penggunaan

### 1. Persiapan Software
- Unduh dan instal [Arduino IDE](https://www.arduino.cc/en/software).
- Buka **Library Manager** (Ctrl+Shift+I), cari dan instal **"LiquidCrystal I2C"** oleh Frank de Brabander.
- Pastikan library **Servo** sudah aktif.

### 2. Cara Upload
- Hubungkan Arduino ke PC menggunakan kabel USB.
- Pilih board yang sesuai (Arduino Uno/Nano) dan pilih Port-nya.
- Klik tombol **Upload** (panah kanan).

### 3. Operasional
- **Mode Default:** Manual. Putar potensiometer untuk menggerakkan Base.
- **Pindah Mode:** Tekan tombol pada Pin D5 atau kirim karakter `'A'` melalui Serial Monitor untuk mode Auto.
- **Monitoring:** Buka Serial Monitor pada baudrate `9600` untuk melihat data sensor.

---

## Tips Troubleshooting
- **LCD Tidak Tampil:** Cek alamat I2C (default: `0x27`). Jika teks tidak terlihat, putar trimpot kontras di belakang modul I2C.
- **Servo Bergetar:** Pastikan tegangan stabil. Disarankan menggunakan power supply eksternal 5V-6V untuk servo, jangan hanya mengandalkan pin 5V Arduino.
- **Gerak Terbalik:** Jika servo bergerak menjauhi cahaya, tukar posisi kabel LDR A0 dan A1.

---
*[Akses Program](Solar_tracker.ino) , Mau lihat hasil Simulasii? *[klik disini](https://youtu.be/bYPxUNTSIR0?si=929z6YjS58x-HAaG) , sudah lihat videonya? pengen nyoba? nih *[skemanya](Schematic Diagram(2).png)
---
*Dibuat oleh Muhammad Rafi dan untuk keperluan dokumentasi proyek Solar Tracker.* 

