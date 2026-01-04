# Oracle Cloud Free Tier - Kernel Compilation Guide

Panduan kompilasi kernel Linux di Oracle Cloud Free Tier (ARM64).
Cocok buat yang kehabisan storage di local VM!

## Kenapa Oracle Cloud?

- ✅ **Gratis selamanya** (Always Free tier)
- ✅ **ARM64** - sama kayak UTM VM di Mac M1/M2
- ✅ **4 CPU cores + 24GB RAM** - lebih cepat dari VM local
- ✅ **200GB storage** - ga bakal kehabisan space

---

## Step 1: Daftar Oracle Cloud

1. Buka [cloud.oracle.com](https://cloud.oracle.com)
2. Klik **"Start for Free"**
3. Isi data:
   - Email
   - Country: Indonesia
   - Name
4. Verifikasi email
5. Masukin kartu kredit/debit (TIDAK AKAN DI-CHARGE, cuma verifikasi)
6. Tunggu approval (biasanya instant)

---

## Step 2: Buat VM ARM64

### 2.1 Masuk Console
1. Login ke [cloud.oracle.com](https://cloud.oracle.com)
2. Klik **"Compute"** → **"Instances"**

### 2.2 Create Instance
1. Klik **"Create Instance"**
2. **Name**: `kernel-builder`
3. **Placement**: Biarkan default

### 2.3 Pilih Image
1. Klik **"Edit"** di bagian Image
2. Pilih **Ubuntu** → **Canonical Ubuntu 22.04** (atau 24.04)

### 2.4 Pilih Shape (PENTING!)
1. Klik **"Change Shape"**
2. Pilih tab **"Ampere"** (ARM64)
3. Pilih **VM.Standard.A1.Flex**
4. Set:
   - **Number of OCPUs**: 4
   - **Memory (GB)**: 24
5. Klik **"Select Shape"**

### 2.5 Networking
- Biarkan default (Create new VCN)

### 2.6 SSH Key (PENTING!)
1. Di Mac, buka Terminal:
   ```bash
   # Cek apakah sudah ada key
   cat ~/.ssh/id_rsa.pub
   
   # Kalau belum ada, generate:
   ssh-keygen -t rsa -b 4096
   # Tekan Enter untuk semua prompt
   
   # Copy public key
   cat ~/.ssh/id_rsa.pub
   ```
2. Copy output dari terminal
3. Di Oracle, pilih **"Paste public keys"**
4. Paste key yang tadi di-copy

### 2.7 Boot Volume
1. Klik **"Specify a custom boot volume size"**
2. Set **100 GB** (atau lebih, max 200GB gratis)

### 2.8 Create!
1. Klik **"Create"**
2. Tunggu status jadi **"RUNNING"** (1-2 menit)
3. Catat **Public IP Address**

---

## Step 3: Connect ke VM

```bash
# Dari Mac terminal
ssh ubuntu@<PUBLIC_IP>

# Contoh:
ssh ubuntu@152.70.123.45

# Kalau ada warning, ketik 'yes'
```

---

## Step 4: Setup Environment

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install dependencies
sudo apt install -y \
    build-essential \
    libncurses-dev \
    bison \
    flex \
    libssl-dev \
    libelf-dev \
    bc \
    git \
    wget \
    dwarves

# Cek arsitektur (harus aarch64)
uname -m
# Output: aarch64
```

---

## Step 5: Download Kernel Source

```bash
cd ~

# Download kernel 5.15.146
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.146.tar.xz

# Extract
tar xf linux-5.15.146.tar.xz
cd linux-5.15.146
```

---

## Step 6: Copy Scheduler Syscall

### Dari Mac (terminal baru):
```bash
# Copy scheduler_syscall.c ke cloud VM
scp /Users/arjunaandio/Projects/projectos/scheduler_syscall.c \
    ubuntu@<PUBLIC_IP>:~/linux-5.15.146/kernel/

# Copy test file juga
scp /Users/arjunaandio/Projects/projectos/test_scheduler.c \
    ubuntu@<PUBLIC_IP>:~/
```

### Di Cloud VM:
```bash
# Verify file ada
ls -la ~/linux-5.15.146/kernel/scheduler_syscall.c
```

---

## Step 7: Edit Kernel Files

### 7.1 Edit Makefile
```bash
nano ~/linux-5.15.146/kernel/Makefile

# Tambah di baris obj-y (sekitar line 10-15):
# obj-y += scheduler_syscall.o
```

### 7.2 Edit syscall_64.tbl (ARM64)
```bash
nano ~/linux-5.15.146/arch/arm64/include/asm/unistd32.h
# ATAU
nano ~/linux-5.15.146/include/uapi/asm-generic/unistd.h

# Tambah syscall entry (lihat KERNEL_GUIDE.md untuk detailnya)
```

---

## Step 8: Configure & Compile

```bash
cd ~/linux-5.15.146

# Generate default config
make defconfig

# Kompilasi (cepet! ~20-30 menit)
make -j4

# Kalau error BTF, disable:
scripts/config --disable CONFIG_DEBUG_INFO_BTF
make -j4
```

### Tanda sukses:
```
Kernel: arch/arm64/boot/Image is ready
```

---

## Step 9: Install di Cloud VM (Optional - buat test)

```bash
sudo make modules_install
sudo make install
sudo reboot
```

Setelah reboot:
```bash
uname -r
# Harus: 5.15.146

# Test syscall
gcc test_scheduler.c -o test_scheduler
./test_scheduler
dmesg | tail -50
```

---

## Step 10: Transfer ke Local VM (UTM)

### 10.1 Package files di Cloud:
```bash
cd ~
mkdir kernel-package
cp linux-5.15.146/arch/arm64/boot/Image kernel-package/vmlinuz-5.15.146
cp -r /lib/modules/5.15.146 kernel-package/modules-5.15.146
tar czf kernel-package.tar.gz kernel-package
```

### 10.2 Download ke Mac:
```bash
# Di Mac
scp ubuntu@<PUBLIC_IP>:~/kernel-package.tar.gz ~/Downloads/
```

### 10.3 Upload ke local UTM VM:
```bash
scp -P 2222 ~/Downloads/kernel-package.tar.gz ubuntu@localhost:~/
```

### 10.4 Install di local VM:
```bash
# Di UTM VM
tar xzf kernel-package.tar.gz
sudo cp kernel-package/vmlinuz-5.15.146 /boot/
sudo cp -r kernel-package/modules-5.15.146 /lib/modules/5.15.146
sudo update-grub
sudo reboot
```

---

## Troubleshooting

### SSH Connection Refused
```bash
# Cek security list di Oracle Console
# Buka port 22 di Ingress Rules
```

### Permission Denied (publickey)
```bash
# Pastikan pakai key yang benar
ssh -i ~/.ssh/id_rsa ubuntu@<PUBLIC_IP>
```

### Out of Memory saat compile
```bash
# Kurangi jobs
make -j2
# atau
make -j1
```

---

## Tips

1. **Jangan shutdown VM** - Always Free tier bisa di-reclaim kalau idle lama
2. **Pakai `screen` atau `tmux`** - Biar compile tetap jalan kalau koneksi putus
   ```bash
   screen -S kernel
   make -j4
   # Ctrl+A, D untuk detach
   # screen -r kernel untuk attach lagi
   ```
3. **Backup** - Download hasil compile ke Mac biar aman

---

## Quick Reference

| Item | Value |
|------|-------|
| Shape | VM.Standard.A1.Flex |
| CPU | 4 OCPU |
| RAM | 24 GB |
| Storage | 100-200 GB |
| OS | Ubuntu 22.04 ARM64 |
| SSH User | ubuntu |
| Cost | FREE! |

---

Happy compiling! 🚀
