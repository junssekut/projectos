# Panduan Lengkap: Install Scheduler System Call di Linux VM (UTM)

Panduan ini khusus untuk **macOS** menggunakan **UTM** sebagai virtual machine.

---

## BAGIAN A: Setup Linux VM di Mac

### Step 1: Download UTM
1. Buka browser, pergi ke: https://mac.getutm.app
2. Klik tombol **"Download"** (gratis)
3. Buka file `.dmg` yang terdownload
4. Drag **UTM** ke folder **Applications**

### Step 2: Download Ubuntu ISO
Pilih sesuai Mac kamu:

**Untuk Mac M1/M2/M3/M4 (Apple Silicon):**
- Buka: https://ubuntu.com/download/server/arm
- Download file yang namanya ada **"arm64"**

**Untuk Mac Intel:**
- Buka: https://ubuntu.com/download/desktop
- Download file **"64-bit PC (AMD64)"**

### Step 3: Buat Virtual Machine
1. Buka **UTM** dari Applications
2. Klik **"Create a New Virtual Machine"**
3. Pilih **"Virtualize"**
4. Pilih **"Linux"**
5. Klik **"Browse"** → pilih file Ubuntu ISO yang tadi didownload
6. Klik **"Continue"**
7. Setting Memory: **4096 MB** (4 GB) atau lebih
8. Setting Storage: **64 GB** (butuh ruang buat compile kernel)
9. Klik **"Save"**

### Step 4: Install Ubuntu
1. Klik **Play** di VM yang baru dibuat
2. Pilih **"Install Ubuntu"** atau **"Try or Install Ubuntu"**
3. Ikuti wizard instalasi:
   - Bahasa: English
   - Keyboard: sesuai keyboard kamu
   - Installation type: **Erase disk and install Ubuntu** (aman, ini cuma VM)
   - Buat username dan password (INGAT INI!)
4. Tunggu sampai selesai (~10-15 menit)
5. Klik **"Restart Now"**
6. Kalau stuck, klik ikon ⚙ di UTM → **Drive** → **Eject** ISO nya, lalu restart

### Step 5: Login ke Ubuntu
1. Masukkan username dan password yang tadi dibuat
2. Sekarang kamu ada di dalam Linux! 🎉

---

## BAGIAN B: Setup Build Environment

Buka **Terminal** di Ubuntu (klik Activities → search "Terminal").

### Step 1: Update System
```bash
sudo apt update
sudo apt upgrade -y
```
Masukkan password kalau diminta. Tunggu sampai selesai.

### Step 2: Install Build Tools
```bash
sudo apt install build-essential libncurses-dev bison flex libssl-dev libelf-dev wget bc -y
```
Ini install semua tools yang dibutuhkan buat compile kernel.

### Step 3: Verifikasi Instalasi
```bash
gcc --version
make --version
```
Kalau muncul versi nya, berarti sukses.

---

## BAGIAN C: Download Linux Kernel Source

### Step 1: Pindah ke Home Directory
```bash
cd ~
```

### Step 2: Download Kernel
```bash
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.146.tar.xz
```
Tunggu sampai download selesai (~130 MB).

### Step 3: Extract Kernel
```bash
tar -xvf linux-5.15.146.tar.xz
```
Ini butuh beberapa menit.

### Step 4: Masuk ke Folder Kernel
```bash
cd linux-5.15.146
```

---

## BAGIAN D: Copy File dari Mac ke VM

Ada 2 cara:

**Cara 1: Shared Folder (Recommended)**
1. Di UTM, klik ikon ⚙ (Settings) pada VM
2. Pergi ke **Sharing**
3. Enable **Directory Sharing**
4. Pilih folder project kamu di Mac
5. Di dalam Ubuntu, folder akan muncul di `/media/` atau `/mnt/`

**Cara 2: Copy-Paste Manual**
1. Buka file `scheduler_syscall.c` di Mac (pakai TextEdit/VS Code)
2. Select All → Copy
3. Di Ubuntu, buka Terminal:
   ```bash
   nano kernel/scheduler_syscall.c
   ```
4. Paste isinya (Cmd+V di UTM biasanya jalan)
5. Save: `Ctrl+O`, Enter, `Ctrl+X`

**Cara 3: Transfer via USB/Cloud**
- Upload file ke Google Drive dari Mac
- Download di Ubuntu via Firefox

---

## BAGIAN E: Tambahkan System Call ke Kernel

### Step 1: Buat File scheduler_syscall.c
Kalau belum copy dari Mac, buat manual:
```bash
nano kernel/scheduler_syscall.c
```
Paste seluruh isi file `scheduler_syscall.c` dari project kamu.
Save: `Ctrl+O` → Enter → `Ctrl+X`

### Step 2: Edit kernel/Makefile
```bash
nano kernel/Makefile
```

Cari baris yang dimulai dengan `obj-y =` (biasanya di baris 10-15).
Baris itu panjang dan ada kelanjutannya dengan `\`.

Tambahin `scheduler_syscall.o` di akhir daftar. Contoh:

**SEBELUM:**
```
obj-y = fork.o exec_domain.o panic.o \
        cpu.o exit.o softirq.o ...
```

**SESUDAH:**
```
obj-y = fork.o exec_domain.o panic.o \
        cpu.o exit.o softirq.o ... scheduler_syscall.o
```

Save: `Ctrl+O` → Enter → `Ctrl+X`

### Step 3: Tambah ke Syscall Table
```bash
nano arch/x86/entry/syscalls/syscall_64.tbl
```

Scroll ke bawah sampai ketemu baris terakhir yang ada nomornya.
Lihat nomor terakhir (misalnya 547), tambahin baris baru dengan nomor berikutnya:

```
548    common  scheduler               sys_scheduler
```

**PENTING:** Ingat nomor ini (548), nanti dipakai di test program!

Save: `Ctrl+O` → Enter → `Ctrl+X`

### Step 4: Tambah ke Header File
```bash
nano include/linux/syscalls.h
```

Scroll ke paling bawah file. Cari baris `#endif` yang terakhir.
Tambahin baris ini **SEBELUM** `#endif`:

```c
asmlinkage long sys_scheduler(int algo, int n, int __user *arrivals, int __user *bursts, int quantum);
```

Save: `Ctrl+O` → Enter → `Ctrl+X`

---

## BAGIAN F: Compile Kernel

### Step 1: Siapkan Config
```bash
cp /boot/config-$(uname -r) .config 2>/dev/null || make defconfig
```

### Step 2: Konfigurasi Kernel
```bash
make menuconfig
```
Muncul menu biru. Langsung:
1. Tekan `Tab` untuk pindah ke **< Save >**
2. Tekan `Enter`
3. Tekan `Enter` lagi
4. Tekan `Tab` ke **< Exit >**
5. Tekan `Enter`

### Step 3: Compile! (LAMA)
```bash
make -j$(nproc)
```

**⚠️ PERINGATAN:**
- Ini proses PALING LAMA: 30-90 menit
- Jangan tutup terminal
- Pastikan Mac tidak sleep (System Settings → Battery → Turn display off: Never)
- Kalau ada pertanyaan, tekan Enter aja (pakai default)

Kalau sukses, tidak ada pesan "Error" di akhir.

### Step 4: Install Modules
```bash
sudo make modules_install
```
Butuh ~5 menit.

### Step 5: Install Kernel
```bash
sudo make install
```
Butuh ~2 menit.

### Step 6: Update Bootloader
```bash
sudo update-grub
```

---

## BAGIAN G: Boot ke Kernel Baru

### Step 1: Restart Ubuntu
```bash
sudo reboot
```

### Step 2: Pilih Kernel di GRUB
Saat Ubuntu restart, tekan **Shift** atau **Esc** berkali-kali sampai muncul menu GRUB.

Pilih: **Advanced options for Ubuntu** → Pilih kernel **5.15.146**

### Step 3: Verifikasi
Setelah login, cek kernel:
```bash
uname -r
```
Harus muncul: `5.15.146`

---

## BAGIAN H: Test System Call

### Step 1: Buat Test Program
```bash
cd ~
nano test_scheduler.c
```

Paste kode ini:
```c
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_scheduler 548

int main() {
    int arrivals[] = {0, 1, 2, 3};
    int bursts[]   = {8, 4, 9, 5};
    int n = 4;
    int quantum = 2;
    
    printf("Testing FCFS...\n");
    syscall(__NR_scheduler, 1, n, arrivals, bursts, 0);
    
    printf("Testing SJF...\n");
    syscall(__NR_scheduler, 2, n, arrivals, bursts, 0);
    
    printf("Testing SRT...\n");
    syscall(__NR_scheduler, 3, n, arrivals, bursts, 0);
    
    printf("Testing Round Robin (quantum=%d)...\n", quantum);
    syscall(__NR_scheduler, 4, n, arrivals, bursts, quantum);
    
    printf("\nDone! Jalanin: dmesg | tail -80\n");
    return 0;
}
```

Save: `Ctrl+O` → Enter → `Ctrl+X`

### Step 2: Compile
```bash
gcc test_scheduler.c -o test_scheduler
```

### Step 3: Run
```bash
./test_scheduler
```

### Step 4: Lihat Hasil di Kernel Log
```bash
dmesg | tail -80
```

Kamu akan lihat output seperti ini:
```
============================================================
  FCFS (First Come First Served)
============================================================
  PID |  AT |  BT |  CT | TAT |  WT |  RT
------+-----+-----+-----+-----+-----+-----
  P1  |   0 |   8 |   8 |   8 |   0 |   0
  P2  |   1 |   4 |  12 |  11 |   7 |   7
  ...

  Gantt Chart:
  [P1][P2][P3][P4]
  0   8   12  21  26
============================================================
```

**🎉 SELAMAT! Kamu berhasil memodifikasi Linux Kernel!**

---

## Troubleshooting

### Kernel tidak muncul di GRUB
```bash
sudo update-grub
sudo reboot
```

### dmesg tidak menampilkan output
Pastikan nomor syscall di `test_scheduler.c` sama dengan yang di `syscall_64.tbl`

### Error saat compile: "scheduler_syscall.c: No such file"
Pastikan file ada di folder `kernel/`:
```bash
ls kernel/scheduler_syscall.c
```

### Error saat compile: implicit declaration
Pastikan sudah edit `include/linux/syscalls.h`
