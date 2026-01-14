# Presentasi Video: Simulasi Penjadwalan Proses CPU

### Durasi Total: 10 Menit

---

## 1. Pembukaan & Pengenalan WSL (0:00 - 1:00)

**Script:**
> "Halo semuanya! Di video ini saya akan mendemonstrasikan program simulasi penjadwalan proses CPU menggunakan bahasa C."

### Brief tentang WSL

**Script:**
> "Program ini saya jalankan di WSL atau Windows Subsystem for Linux. Untuk yang belum tahu, WSL adalah fitur Windows yang memungkinkan kita menjalankan Linux langsung di Windows tanpa virtual machine."
>
> "Instalasi WSL cukup mudah:"
> 1. "Buka PowerShell sebagai Administrator"
> 2. "Jalankan: `wsl --install`"
> 3. "Tunggu selesai dan restart komputer"
> 4. "Set username dan password"
> 5. "Install gcc dan make dengan: `sudo apt install gcc make -y`"

> "Setelah itu, kita perlu pindahkan file .c dan Makefile ke folder yang bisa diakses WSL."
> "File kita ada di Windows, tapi WSL bisa akses folder Windows melalui path `/mnt/c/Users/...`"
> "Kita tinggal navigate ke folder project kita dan compile program: `make` dan `./scheduler`"

> "Program ini mengimplementasikan 4 algoritma: FCFS, SJF, SRT, dan Round Robin."

[Tampilkan judul video / intro]

---

## 2. Penjelasan Singkat Algoritma (1:00 - 3:00)

**Script:**
> "Sebelum demo, saya jelaskan singkat keempat algoritma ini."

**FCFS (First Come First Served):**
> "FCFS: proses yang datang duluan, dieksekusi duluan. Simpel seperti antrian."

**SJF (Shortest Job First):**
> "SJF: proses dengan waktu eksekusi terpendek dijalankan duluan. Non-preemptive."

**SRT (Shortest Remaining Time):**
> "SRT: versi preemptive dari SJF. Proses bisa diinterupsi jika ada proses lebih pendek."

**Round Robin:**
> "Round Robin: setiap proses dapat jatah waktu sama, bergantian. Adil untuk semua."

[Tampilkan diagram sederhana tiap algoritma]

---

## 3. Demo Program (3:00 - 8:00)

**Script:**
> "Sekarang langsung ke demo program."

[Buka terminal, jalankan program]

### 3.1 Input Data (3:00 - 4:00)

**Script:**
> "Saya masukkan 4 proses dengan data berikut:"

```
Jumlah proses: 4

P1: AT = 0, BT = 8
P2: AT = 1, BT = 4  
P3: AT = 2, BT = 9
P4: AT = 3, BT = 5
```

[Ketik input di terminal]

---

### 3.2 Jalankan Semua Algoritma (4:00 - 7:30)

**Script:**
> "Saya pilih opsi 5 untuk menjalankan semua algoritma sekaligus."

[Pilih opsi 5, tampilkan hasil]

**Saat hasil FCFS muncul:**
> "Ini hasil FCFS. Lihat tabelnya: PID, Arrival Time, Burst Time, Completion Time, Turnaround Time, Waiting Time, dan Response Time."

**Saat hasil SJF muncul:**
> "Ini SJF. Perhatikan waiting time-nya lebih rendah dari FCFS."

**Saat hasil SRT muncul:**
> "SRT memberikan hasil paling optimal karena preemptive."

**Saat hasil RR muncul:**
> "Round Robin dengan quantum 2. Response time-nya lebih merata."

[Scroll perlahan agar semua tabel terlihat]

---

### 3.3 Tunjukkan Kode Singkat (7:30 - 8:00)

**Script:**
> "Sekilas struktur kode yang saya gunakan."

[Buka file scheduler.c, tunjukkan struct Process]

```c
typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
    ...
} Process;
```

> "Struct ini menyimpan semua data dan hasil perhitungan setiap proses."

---

## 4. Perbandingan Hasil (8:00 - 9:00)

**Script:**
> "Mari kita bandingkan hasil keempat algoritma."

[Tampilkan tabel perbandingan - bisa screenshot atau graphic overlay]

| Algoritma | Avg RT | Avg TAT | Avg WT |
|-----------|--------|---------|--------|
| FCFS      | ...    | ...     | ...    |
| SJF       | ...    | ...     | ...    |
| SRT       | ...    | ...     | ...    |
| RR (Q=2)  | ...    | ...     | ...    |

**Script:**
> "Dari tabel ini terlihat:"
> - "SRT punya waiting time terendah"
> - "Round Robin punya response time paling merata"
> - "FCFS paling simpel tapi tidak selalu optimal"

---

## 5. Penutup (9:00 - 10:00)

**Script:**
> "Kesimpulannya, setiap algoritma punya kelebihan dan kekurangan masing-masing. Tidak ada yang sempurna untuk semua situasi."
>
> "FCFS simpel, SJF dan SRT optimal untuk waiting time, Round Robin adil untuk semua proses."
>
> "Sekian demo dari saya. Terima kasih sudah menonton!"

[Tampilkan outro / credit]

---

## Tips Rekam Video

1. **Persiapan:**
   - Compile program dulu: `make clean && make`
   - Buka terminal fullscreen
   - Perbesar font terminal agar mudah dibaca

2. **Saat Rekam:**
   - Bicara dengan tempo sedang, jelas
   - Pause sebentar saat hasil muncul agar penonton bisa baca
   - Gunakan mouse/cursor untuk menunjuk bagian penting

3. **Software Rekam:**
   - macOS: QuickTime (⌘ + Shift + 5)
   - OBS Studio (gratis, cross-platform)

4. **Input Siap Pakai:**
   ```
   4
   0
   8
   1
   4
   2
   9
   3
   5
   5
   2
   0
   ```
   (Simpan di notepad, bisa copy-paste saat demo)

---

*Durasi total: ~10 menit. Semoga lancar! 🎬*
