/*
 * ============================================================================
 *                   test_scheduler.c - CRUD test untuk scheduler syscall
 * ============================================================================
 * program buat test scheduler syscall dengan fitur CRUD:
 *   - Create: tambah proses baru
 *   - Read: liat daftar proses
 *   - Update: edit proses yang udah ada
 *   - Delete: hapus proses
 *   - Test: jalanin scheduler syscall
 * 
 * compile: gcc test_scheduler.c -o test_scheduler
 * run: ./test_scheduler
 * liat hasil: dmesg | tail -80
 * ============================================================================
 */

#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// (wajib sama dengan syscall_64.tbl)
#define __NR_scheduler 548

#define MAX_PROCESS 10

/* ============================================================================
 *                              struct data
 * ============================================================================ */

typedef struct {
    int id;               // id proses
    int arrival_time;     // kapan dateng
    int burst_time;       // butuh brp lama
    int is_used;          // slot dipake atau ga
} ProcessData;

// global storage buat proses
ProcessData processes[MAX_PROCESS];
int process_count = 0;

/* ============================================================================
 *                              fungsi tampilan
 * ============================================================================ */

void print_header(const char *title) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-60s║\n", title);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

void print_separator() {
    printf("────────────────────────────────────────────────────────────────\n");
}

void print_menu() {
    printf("\n  ┌─────────────────────────────────────────┐\n");
    printf("  │  MENU TEST SCHEDULER SYSCALL           │\n");
    printf("  ├─────────────────────────────────────────┤\n");
    printf("  │  1. [C] Create  - Tambah proses baru   │\n");
    printf("  │  2. [R] Read    - Lihat daftar proses  │\n");
    printf("  │  3. [U] Update  - Edit proses          │\n");
    printf("  │  4. [D] Delete  - Hapus proses         │\n");
    printf("  ├─────────────────────────────────────────┤\n");
    printf("  │  5. Test FCFS                          │\n");
    printf("  │  6. Test SJF                           │\n");
    printf("  │  7. Test SRT                           │\n");
    printf("  │  8. Test Round Robin                   │\n");
    printf("  │  9. Test Semua Algoritma               │\n");
    printf("  ├─────────────────────────────────────────┤\n");
    printf("  │  10. Load Sample Data                  │\n");
    printf("  │  11. Clear All                         │\n");
    printf("  │  0.  Keluar                            │\n");
    printf("  └─────────────────────────────────────────┘\n");
    printf("  Pilihan: ");
}

/* ============================================================================
 *                              CRUD Functions
 * ============================================================================ */

// [C] CREATE - tambah proses baru
void create_process() {
    print_header("CREATE - Tambah Proses Baru");
    
    if (process_count >= MAX_PROCESS) {
        printf("  ⚠ Sudah mencapai batas maksimal (%d proses)!\n", MAX_PROCESS);
        return;
    }
    
    // cari slot kosong
    int slot = -1;
    for (int i = 0; i < MAX_PROCESS; i++) {
        if (!processes[i].is_used) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        printf("  ⚠ Tidak ada slot tersedia!\n");
        return;
    }
    
    int at, bt;
    printf("\n  Masukkan data proses baru:\n");
    printf("    Arrival Time : ");
    scanf("%d", &at);
    printf("    Burst Time   : ");
    scanf("%d", &bt);
    
    if (at < 0 || bt <= 0) {
        printf("  ⚠ Data tidak valid! AT >= 0 dan BT > 0\n");
        return;
    }
    
    processes[slot].id = slot + 1;
    processes[slot].arrival_time = at;
    processes[slot].burst_time = bt;
    processes[slot].is_used = 1;
    process_count++;
    
    printf("\n  ✓ Proses P%d berhasil ditambahkan!\n", slot + 1);
    printf("    AT = %d, BT = %d\n", at, bt);
}

// [R] READ - lihat semua proses
void read_processes() {
    print_header("READ - Daftar Proses");
    
    if (process_count == 0) {
        printf("\n  ⚠ Belum ada proses! Gunakan Create atau Load Sample Data.\n");
        return;
    }
    
    printf("\n  ┌─────┬───────────────┬────────────┐\n");
    printf("  │ PID │ Arrival Time  │ Burst Time │\n");
    printf("  ├─────┼───────────────┼────────────┤\n");
    
    for (int i = 0; i < MAX_PROCESS; i++) {
        if (processes[i].is_used) {
            printf("  │ P%-2d │     %3d       │    %3d     │\n",
                   processes[i].id,
                   processes[i].arrival_time,
                   processes[i].burst_time);
        }
    }
    printf("  └─────┴───────────────┴────────────┘\n");
    printf("\n  Total proses aktif: %d\n", process_count);
}

// [U] UPDATE - edit proses
void update_process() {
    print_header("UPDATE - Edit Proses");
    
    if (process_count == 0) {
        printf("\n  ⚠ Belum ada proses untuk diedit!\n");
        return;
    }
    
    // tampilkan daftar dulu
    read_processes();
    
    int pid;
    printf("\n  Masukkan PID yang mau diedit (1-%d): P", MAX_PROCESS);
    scanf("%d", &pid);
    
    if (pid < 1 || pid > MAX_PROCESS) {
        printf("  ⚠ PID tidak valid!\n");
        return;
    }
    
    int idx = pid - 1;
    if (!processes[idx].is_used) {
        printf("  ⚠ Proses P%d tidak ditemukan!\n", pid);
        return;
    }
    
    printf("\n  Data saat ini: AT = %d, BT = %d\n",
           processes[idx].arrival_time, processes[idx].burst_time);
    
    int at, bt;
    printf("\n  Masukkan data baru:\n");
    printf("    Arrival Time baru : ");
    scanf("%d", &at);
    printf("    Burst Time baru   : ");
    scanf("%d", &bt);
    
    if (at < 0 || bt <= 0) {
        printf("  ⚠ Data tidak valid! AT >= 0 dan BT > 0\n");
        return;
    }
    
    processes[idx].arrival_time = at;
    processes[idx].burst_time = bt;
    
    printf("\n  ✓ Proses P%d berhasil diupdate!\n", pid);
    printf("    AT = %d, BT = %d\n", at, bt);
}

// [D] DELETE - hapus proses
void delete_process() {
    print_header("DELETE - Hapus Proses");
    
    if (process_count == 0) {
        printf("\n  ⚠ Belum ada proses untuk dihapus!\n");
        return;
    }
    
    // tampilkan daftar dulu
    read_processes();
    
    int pid;
    printf("\n  Masukkan PID yang mau dihapus (1-%d): P", MAX_PROCESS);
    scanf("%d", &pid);
    
    if (pid < 1 || pid > MAX_PROCESS) {
        printf("  ⚠ PID tidak valid!\n");
        return;
    }
    
    int idx = pid - 1;
    if (!processes[idx].is_used) {
        printf("  ⚠ Proses P%d tidak ditemukan!\n", pid);
        return;
    }
    
    char confirm;
    printf("\n  Yakin hapus P%d (AT=%d, BT=%d)? (y/n): ",
           pid, processes[idx].arrival_time, processes[idx].burst_time);
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        processes[idx].is_used = 0;
        processes[idx].arrival_time = 0;
        processes[idx].burst_time = 0;
        process_count--;
        printf("\n  ✓ Proses P%d berhasil dihapus!\n", pid);
    } else {
        printf("\n  ✗ Penghapusan dibatalkan.\n");
    }
}

/* ============================================================================
 *                              Test Functions
 * ============================================================================ */

// helper buat prepare data test
int prepare_test_data(int arrivals[], int bursts[]) {
    int n = 0;
    for (int i = 0; i < MAX_PROCESS; i++) {
        if (processes[i].is_used) {
            arrivals[n] = processes[i].arrival_time;
            bursts[n] = processes[i].burst_time;
            n++;
        }
    }
    return n;
}

void test_fcfs() {
    print_header("TEST FCFS (First Come First Served)");
    
    if (process_count == 0) {
        printf("\n  ⚠ Tidak ada proses untuk ditest!\n");
        return;
    }
    
    int arrivals[MAX_PROCESS], bursts[MAX_PROCESS];
    int n = prepare_test_data(arrivals, bursts);
    
    printf("\n  Menjalankan syscall scheduler (FCFS)...\n");
    read_processes();
    
    errno = 0;
    long ret = syscall(__NR_scheduler, 1, n, arrivals, bursts, 0);
    
    if (ret == 0) {
        printf("\n  ✓ FCFS test selesai!\n");
        printf("  → Jalankan 'dmesg | tail -30' untuk lihat hasil\n");
    } else {
        printf("\n  ✗ Error! Return code: %ld, errno: %d (%s)\n", ret, errno, strerror(errno));
        if (errno == ENOSYS) printf("     → Syscall %d tidak ada di kernel ini!\n", __NR_scheduler);
        else if (errno == EFAULT) printf("     → Bad address - masalah pointer\n");
        else if (errno == EINVAL) printf("     → Invalid argument\n");
    }
}

void test_sjf() {
    print_header("TEST SJF (Shortest Job First)");
    
    if (process_count == 0) {
        printf("\n  ⚠ Tidak ada proses untuk ditest!\n");
        return;
    }
    
    int arrivals[MAX_PROCESS], bursts[MAX_PROCESS];
    int n = prepare_test_data(arrivals, bursts);
    
    printf("\n  Menjalankan syscall scheduler (SJF)...\n");
    read_processes();
    
    errno = 0;
    long ret = syscall(__NR_scheduler, 2, n, arrivals, bursts, 0);
    
    if (ret == 0) {
        printf("\n  ✓ SJF test selesai!\n");
        printf("  → Jalankan 'dmesg | tail -30' untuk lihat hasil\n");
    } else {
        printf("\n  ✗ Error! Return code: %ld, errno: %d (%s)\n", ret, errno, strerror(errno));
        if (errno == ENOSYS) printf("     → Syscall %d tidak ada di kernel ini!\n", __NR_scheduler);
        else if (errno == EFAULT) printf("     → Bad address - masalah pointer\n");
        else if (errno == EINVAL) printf("     → Invalid argument\n");
    }
}

void test_srt() {
    print_header("TEST SRT (Shortest Remaining Time)");
    
    if (process_count == 0) {
        printf("\n  ⚠ Tidak ada proses untuk ditest!\n");
        return;
    }
    
    int arrivals[MAX_PROCESS], bursts[MAX_PROCESS];
    int n = prepare_test_data(arrivals, bursts);
    
    printf("\n  Menjalankan syscall scheduler (SRT)...\n");
    read_processes();
    
    errno = 0;
    long ret = syscall(__NR_scheduler, 3, n, arrivals, bursts, 0);
    
    if (ret == 0) {
        printf("\n  ✓ SRT test selesai!\n");
        printf("  → Jalankan 'dmesg | tail -30' untuk lihat hasil\n");
    } else {
        printf("\n  ✗ Error! Return code: %ld, errno: %d (%s)\n", ret, errno, strerror(errno));
        if (errno == ENOSYS) printf("     → Syscall %d tidak ada di kernel ini!\n", __NR_scheduler);
        else if (errno == EFAULT) printf("     → Bad address - masalah pointer\n");
        else if (errno == EINVAL) printf("     → Invalid argument\n");
    }
}

void test_round_robin() {
    print_header("TEST Round Robin");
    
    if (process_count == 0) {
        printf("\n  ⚠ Tidak ada proses untuk ditest!\n");
        return;
    }
    
    int arrivals[MAX_PROCESS], bursts[MAX_PROCESS];
    int n = prepare_test_data(arrivals, bursts);
    
    int quantum;
    printf("\n  Masukkan Time Quantum: ");
    scanf("%d", &quantum);
    
    if (quantum <= 0) {
        printf("  → Quantum tidak valid, menggunakan default (2)\n");
        quantum = 2;
    }
    
    printf("\n  Menjalankan syscall scheduler (Round Robin, Q=%d)...\n", quantum);
    read_processes();
    
    errno = 0;
    long ret = syscall(__NR_scheduler, 4, n, arrivals, bursts, quantum);
    
    if (ret == 0) {
        printf("\n  ✓ Round Robin test selesai!\n");
        printf("  → Jalankan 'dmesg | tail -30' untuk lihat hasil\n");
    } else {
        printf("\n  ✗ Error! Return code: %ld, errno: %d (%s)\n", ret, errno, strerror(errno));
        if (errno == ENOSYS) printf("     → Syscall %d tidak ada di kernel ini!\n", __NR_scheduler);
        else if (errno == EFAULT) printf("     → Bad address - masalah pointer\n");
        else if (errno == EINVAL) printf("     → Invalid argument\n");
    }
}

void test_all() {
    print_header("TEST SEMUA ALGORITMA");
    
    if (process_count == 0) {
        printf("\n  ⚠ Tidak ada proses untuk ditest!\n");
        return;
    }
    
    int arrivals[MAX_PROCESS], bursts[MAX_PROCESS];
    int n = prepare_test_data(arrivals, bursts);
    
    int quantum;
    printf("\n  Masukkan Time Quantum untuk Round Robin: ");
    scanf("%d", &quantum);
    if (quantum <= 0) quantum = 2;
    
    printf("\n  Menjalankan semua test...\n");
    read_processes();
    
    printf("\n  [1/4] Testing FCFS...\n");
    syscall(__NR_scheduler, 1, n, arrivals, bursts, 0);
    printf("        ✓ Done\n");
    
    printf("  [2/4] Testing SJF...\n");
    syscall(__NR_scheduler, 2, n, arrivals, bursts, 0);
    printf("        ✓ Done\n");
    
    printf("  [3/4] Testing SRT...\n");
    syscall(__NR_scheduler, 3, n, arrivals, bursts, 0);
    printf("        ✓ Done\n");
    
    printf("  [4/4] Testing Round Robin (Q=%d)...\n", quantum);
    syscall(__NR_scheduler, 4, n, arrivals, bursts, quantum);
    printf("        ✓ Done\n");
    
    printf("\n  ════════════════════════════════════════════════════════\n");
    printf("  ✓ Semua test selesai!\n");
    printf("  → Jalankan 'dmesg | tail -100' untuk lihat semua hasil\n");
    printf("  ════════════════════════════════════════════════════════\n");
}

/* ============================================================================
 *                              Utility Functions
 * ============================================================================ */

void load_sample_data() {
    print_header("LOAD SAMPLE DATA");
    
    // clear dulu
    for (int i = 0; i < MAX_PROCESS; i++) {
        processes[i].is_used = 0;
        processes[i].arrival_time = 0;
        processes[i].burst_time = 0;
    }
    process_count = 0;
    
    // sample data (sama kayak di TEST_CASES.txt)
    int sample_at[] = {0, 1, 2, 3};
    int sample_bt[] = {8, 4, 9, 5};
    int sample_count = 4;
    
    for (int i = 0; i < sample_count; i++) {
        processes[i].id = i + 1;
        processes[i].arrival_time = sample_at[i];
        processes[i].burst_time = sample_bt[i];
        processes[i].is_used = 1;
        process_count++;
    }
    
    printf("\n  ✓ Sample data berhasil dimuat!\n");
    printf("\n  Data yang dimuat:\n");
    printf("    P1: AT=0, BT=8\n");
    printf("    P2: AT=1, BT=4\n");
    printf("    P3: AT=2, BT=9\n");
    printf("    P4: AT=3, BT=5\n");
}

void clear_all() {
    print_header("CLEAR ALL");
    
    if (process_count == 0) {
        printf("\n  ⚠ Tidak ada data untuk dihapus!\n");
        return;
    }
    
    char confirm;
    printf("\n  Yakin hapus semua %d proses? (y/n): ", process_count);
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        for (int i = 0; i < MAX_PROCESS; i++) {
            processes[i].is_used = 0;
            processes[i].arrival_time = 0;
            processes[i].burst_time = 0;
        }
        process_count = 0;
        printf("\n  ✓ Semua data berhasil dihapus!\n");
    } else {
        printf("\n  ✗ Penghapusan dibatalkan.\n");
    }
}

/* ============================================================================
 *                              Main Function
 * ============================================================================ */

int main() {
    // inisialisasi
    memset(processes, 0, sizeof(processes));
    
    print_header("TEST SCHEDULER SYSCALL - CRUD Interface");
    printf("\n  Selamat datang di Test Scheduler!\n");
    printf("  Gunakan menu CRUD untuk kelola proses dan test syscall.\n");
    printf("\n  TIPS: Mulai dengan 'Load Sample Data' untuk quick test.\n");
    
    int choice;
    
    while (1) {
        print_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:  create_process();    break;
            case 2:  read_processes();    break;
            case 3:  update_process();    break;
            case 4:  delete_process();    break;
            case 5:  test_fcfs();         break;
            case 6:  test_sjf();          break;
            case 7:  test_srt();          break;
            case 8:  test_round_robin();  break;
            case 9:  test_all();          break;
            case 10: load_sample_data();  break;
            case 11: clear_all();         break;
            case 0:
                printf("\n  Terima kasih! Sampai jumpa!\n\n");
                return 0;
            default:
                printf("\n  ⚠ Pilihan tidak valid!\n");
        }
    }
    
    return 0;
}
