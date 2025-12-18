# Panduan Lengkap: Menggunakan WSL untuk Menjalankan Program Scheduler

---

## Daftar Isi
1. [Apa itu WSL?](#apa-itu-wsl)
2. [Persyaratan Sistem](#persyaratan-sistem)
3. [Instalasi WSL Step-by-Step](#instalasi-wsl-step-by-step)
4. [Setup Environment di WSL](#setup-environment-di-wsl)
5. [Menjalankan Program Scheduler](#menjalankan-program-scheduler)
6. [Tips & Troubleshooting](#tips--troubleshooting)

---

## Apa itu WSL?

**WSL (Windows Subsystem for Linux)** adalah fitur Windows yang memungkinkan Anda menjalankan Linux langsung di Windows tanpa perlu virtual machine atau dual boot.

### Keuntungan WSL:
- ✅ Akses command-line tools Linux di Windows
- ✅ Lebih ringan dari virtual machine
- ✅ Terintegrasi dengan Windows Explorer
- ✅ Gratis dan built-in di Windows 10/11
- ✅ Ideal untuk development C/C++

### Versi WSL:
- **WSL 1**: Lebih ringan, kompatibilitas kurang
- **WSL 2**: Lebih berat, kompatibilitas lebih baik, recommended

---

## Persyaratan Sistem

- **Windows 10** (Build 2004 atau lebih tinggi) atau **Windows 11**
- **Administrator access** untuk instalasi
- **Virtualization enabled** di BIOS (untuk WSL 2)
- Minimal **2-4 GB RAM** yang tersisa
- Minimal **5 GB disk space**

### Cara Cek Versi Windows:
1. Tekan `Windows + R`
2. Ketik: `winver`
3. Lihat Build number-nya

---

## Instalasi WSL Step-by-Step

### Langkah 1: Buka PowerShell sebagai Administrator

1. Klik kanan tombol **Start** di Windows
2. Pilih **Terminal (Admin)** atau **Windows PowerShell (Admin)**
3. Jika diminta UAC, klik **Yes**

### Langkah 2: Install WSL dengan Command Sederhana

Jalankan command ini di PowerShell:

```powershell
wsl --install
```

Ini akan:
- Install WSL 2 (versi terbaru)
- Download dan install Ubuntu (default)
- Enable virtual machine platform

### Langkah 3: Tunggu Proses Selesai

Proses instalasi bisa memakan waktu 5-15 menit tergantung internet speed.

Output yang Anda lihat:
```
Installing: Virtual Machine Platform
Installing: Windows Subsystem for Linux
Installing: Ubuntu
The requested operation was successful.
```

### Langkah 4: Restart Komputer

Setelah instalasi selesai, **restart komputer Anda**:

```powershell
Restart-Computer
```

### Langkah 5: Setup Ubuntu User

Setelah restart, Ubuntu terminal akan otomatis terbuka. Anda akan diminta untuk:

1. **Membuat username** (bukan Administrator):
   ```
   Enter new UNIX username: your_username
   ```
   Contoh: `arjun`

2. **Membuat password**:
   ```
   New password: ••••••••
   Retype new password: ••••••••
   ```
   Password tidak akan terlihat saat diketik (normal).

3. Selesai! Anda sekarang memiliki Linux environment.

---

## Verifikasi Instalasi WSL

Buka **PowerShell** (regular, tidak perlu admin) dan jalankan:

```powershell
wsl --list --verbose
```

Output yang diharapkan:
```
NAME            STATE           VERSION
Ubuntu          Running         2
```

Jika belum running, jalankan:

```powershell
wsl --set-default-version 2
```

---

## Setup Environment di WSL

### Langkah 1: Buka WSL Terminal

Pilih salah satu cara:

**Cara 1:** Dari PowerShell
```powershell
wsl
```

**Cara 2:** Dari Start Menu
- Cari "Ubuntu" di Windows Search
- Klik "Ubuntu"

**Cara 3:** Dari File Explorer
- Tekan `Windows + R`
- Ketik `\\wsl$`
- Klik kanan > Open Linux shell here

### Langkah 2: Update Package Manager

Jalankan commands ini satu per satu:

```bash
sudo apt update
```

```bash
sudo apt upgrade -y
```

`sudo` = run as administrator di Linux
`apt` = package manager (seperti app store)
`-y` = automatically answer yes

### Langkah 3: Install Compiler dan Build Tools

Untuk compile program C, Anda perlu `gcc` dan `make`:

```bash
sudo apt install gcc make -y
```

Tunggu sampai selesai (biasanya 2-5 menit).

### Langkah 4: Verifikasi Instalasi

Cek apakah gcc dan make sudah terinstall:

```bash
gcc --version
```

Output seharusnya:
```
gcc (Ubuntu 11.4.0-1ubuntu1~22.04.1) 11.4.0
```

```bash
make --version
```

Output seharusnya:
```
GNU Make 4.3
```

---

## Menjalankan Program Scheduler

### Langkah 1: Navigate ke Folder Project

Di WSL, drive Windows bisa diakses melalui `/mnt/`:

```bash
cd /mnt/c/Users/YourUsername/Projects/projectos
```

**Penting:** Ganti `YourUsername` dengan username Windows Anda.

Untuk cek username Windows:
- Buka File Explorer
- Lihat "Users" di Drive C
- Atau buka "Settings" > "System" > lihat nama account

### Langkah 2: List File di Folder

Pastikan file project sudah ada:

```bash
ls -la
```

Output seharusnya menunjukkan:
```
total 96
drwxr-xr-x  8 user group  4096 Dec 18 10:30 .
drwxr-xr-x  3 user group  4096 Dec 18 10:20 ..
-rw-r--r--  1 user group  1024 Dec 18 10:30 Makefile
-rw-r--r--  1 user group 15234 Dec 18 10:30 scheduler.c
-rw-r--r--  1 user group  2048 Dec 18 10:30 README.md
-rw-r--r--  1 user group  5120 Dec 18 10:30 PRESENTASI.md
```

### Langkah 3: Compile Program

```bash
make clean
make
```

Output yang diharapkan:
```
rm -f scheduler
gcc -Wall -std=c99 -o scheduler scheduler.c
```

Jika ada error, lihat [section Troubleshooting](#tips--troubleshooting).

### Langkah 4: Jalankan Program

```bash
./scheduler
```

Program akan menampilkan:
```
╔══════════════════════════════════════════════════════════════╗
║  SIMULASI PENJADWALAN PROSES CPU                            ║
╚══════════════════════════════════════════════════════════════╝

  Masukkan jumlah proses (1-100): 
```

### Langkah 5: Input Data

Masukkan jumlah proses (contoh: 4):

```
4
```

Kemudian masukkan data untuk setiap proses:

```
  Proses 1:
    Arrival Time : 0
    Burst Time   : 8

  Proses 2:
    Arrival Time : 1
    Burst Time   : 4

  Proses 3:
    Arrival Time : 2
    Burst Time   : 9

  Proses 4:
    Arrival Time : 3
    Burst Time   : 5
```

### Langkah 6: Pilih Algoritma

Menu akan muncul:
```
  ┌─────────────────────────────────────────┐
  │  PILIH ALGORITMA PENJADWALAN           │
  ├─────────────────────────────────────────┤
  │  1. FCFS (First Come First Served)     │
  │  2. SJF  (Shortest Job First)          │
  │  3. SRT  (Shortest Remaining Time)     │
  │  4. RR   (Round Robin)                 │
  │  5. Jalankan Semua Algoritma           │
  │  0. Keluar                             │
  └─────────────────────────────────────────┘
  Pilihan: 
```

Pilih **5** untuk menjalankan semua algoritma sekaligus.

### Langkah 7: Lihat Hasil

Tabel hasil akan ditampilkan untuk setiap algoritma:

```
  FCFS (First Come First Served)
  ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┐
  │ PID │  AT │  BT │  CT │ TAT │  WT │  RT │
  ├─────┼─────┼─────┼─────┼─────┼─────┼─────┤
  │ P1  │   0 │   8 │   8 │   8 │   0 │   0 │
  │ P2  │   1 │   4 │  12 │  11 │   7 │   7 │
  │ P3  │   2 │   9 │  21 │  19 │  10 │  10 │
  │ P4  │   3 │   5 │  26 │  23 │  18 │  18 │
  └─────┴─────┴─────┴─────┴─────┴─────┴─────┘

  ┌───────────────────────────────────────────┐
  │  Avg Response Time:     8.75              │
  │  Avg Turnaround Time:   15.25             │
  │  Avg Waiting Time:      8.75              │
  └───────────────────────────────────────────┘
```

---

## Quick Commands Reference

Setelah familiar, gunakan shortcut ini:

```bash
# Navigate ke project
cd /mnt/c/Users/YourUsername/Projects/projectos

# Compile
make clean && make

# Run
./scheduler

# Run langsung (compile + run)
make run

# Clean compiled files
make clean
```

---

## Tips & Troubleshooting

### Issue 1: "WSL command not found"

**Masalah:** Command `wsl` tidak dikenali di PowerShell.

**Solusi:**
1. Pastikan Anda menggunakan **Windows 10 Build 2004+** atau **Windows 11**
2. Restart PowerShell
3. Jalankan: `wsl --install` lagi

---

### Issue 2: "Cannot find Ubuntu installation"

**Masalah:** Ubuntu tidak terinstall setelah `wsl --install`.

**Solusi:**
1. Buka **Microsoft Store**
2. Search "Ubuntu"
3. Download dan install versi terbaru
4. Launch Ubuntu dari Start Menu

---

### Issue 3: "gcc: command not found"

**Masalah:** gcc tidak terinstall di WSL.

**Solusi:**
```bash
sudo apt update
sudo apt install gcc make -y
```

---

### Issue 4: "Permission denied" saat akses file

**Masalah:** Tidak bisa baca/tulis file di folder Windows.

**Solusi:**
```bash
# Dari WSL, ganti permission
chmod 755 /mnt/c/Users/YourUsername/Projects/projectos
```

---

### Issue 5: Program berjalan lambat di WSL 1

**Masalah:** Performance kurang baik.

**Solusi:** Upgrade ke WSL 2
```powershell
wsl --set-version Ubuntu 2
```

---

### Issue 6: "Module not found" atau error kompilasi

**Masalah:** File scheduler.c tidak ditemukan.

**Solusi:**
1. Pastikan Anda di folder yang benar: `pwd` (print working directory)
2. List file: `ls -la`
3. Jika scheduler.c tidak ada, copy dari Windows Explorer

---

### Issue 7: Keyboard layout berbeda

**Masalah:** Ketika mengetik, tombol tidak sesuai layout.

**Solusi:** Di WSL terminal, setting keyboard layout:
```bash
loadkeys us  # untuk layout US
# atau
dpkg-reconfigure keyboard-configuration
```

---

## File System Navigation

### Akses Windows dari WSL:
```bash
# Drive C
cd /mnt/c

# Drive D (jika ada)
cd /mnt/d

# User folder
cd /mnt/c/Users/YourUsername
```

### Akses WSL dari Windows:
1. Buka File Explorer
2. Masukkan di address bar: `\\wsl$`
3. Pilih distro (Ubuntu)
4. Browse file seperti normal

---

## Backup & Export WSL

### Backup distro:
```powershell
wsl --export Ubuntu D:\backup\ubuntu.tar
```

### Restore distro:
```powershell
wsl --import Ubuntu D:\wsl\ubuntu D:\backup\ubuntu.tar
```

---

## Uninstall WSL (Jika Diperlukan)

```powershell
# List installed distros
wsl --list

# Uninstall Ubuntu
wsl --uninstall Ubuntu

# Atau uninstall WSL seluruhnya
wsl --uninstall --all
```

---

## Resources Tambahan

- [Microsoft WSL Documentation](https://docs.microsoft.com/en-us/windows/wsl/)
- [Ubuntu in WSL](https://ubuntu.com/wsl)
- [GCC Compiler Guide](https://gcc.gnu.org/)

---

## Checklist Sebelum Presentasi

- ✅ WSL terinstall dan berjalan
- ✅ gcc dan make terinstall
- ✅ Project folder terkopas dengan benar
- ✅ Program compile tanpa error: `make clean && make`
- ✅ Program berjalan: `./scheduler`
- ✅ Test dengan input sample
- ✅ Terminal fullscreen untuk rekam video
- ✅ Font cukup besar untuk dibaca

---

*Selamat! Anda siap menjalankan scheduler di WSL. Good luck! 🚀*
