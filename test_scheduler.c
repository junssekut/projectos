/*
 * test_scheduler.c - program buat test syscall scheduler
 * 
 * compile: gcc test_scheduler.c -o test_scheduler
 * run: ./test_scheduler
 * liat hasil: dmesg | tail -50
 */

#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

// ganti nomor ini sesuai yg ada di syscall_64.tbl
#define __NR_scheduler 548

int main() {
    // contoh data proses (sama kayak test case di scheduler.c)
    int arrivals[] = {0, 1, 2, 3};
    int bursts[]   = {8, 4, 9, 5};
    int n = 4;
    int quantum = 2;
    
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  TEST SCHEDULER SYSTEM CALL                                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Data proses:\n");
    printf("  P1: AT=0, BT=8\n");
    printf("  P2: AT=1, BT=4\n");
    printf("  P3: AT=2, BT=9\n");
    printf("  P4: AT=3, BT=5\n\n");
    
    printf("Testing FCFS (algo=1)...\n");
    syscall(__NR_scheduler, 1, n, arrivals, bursts, 0);
    printf("  ✓ Done\n\n");
    
    printf("Testing SJF (algo=2)...\n");
    syscall(__NR_scheduler, 2, n, arrivals, bursts, 0);
    printf("  ✓ Done\n\n");
    
    printf("Testing SRT (algo=3)...\n");
    syscall(__NR_scheduler, 3, n, arrivals, bursts, 0);
    printf("  ✓ Done\n\n");
    
    printf("Testing Round Robin (algo=4, quantum=%d)...\n", quantum);
    syscall(__NR_scheduler, 4, n, arrivals, bursts, quantum);
    printf("  ✓ Done\n\n");
    
    printf("════════════════════════════════════════════════════════════════\n");
    printf("Semua test selesai!\n");
    printf("Jalanin 'dmesg | tail -80' buat liat hasilnya di kernel log\n");
    printf("════════════════════════════════════════════════════════════════\n");
    
    return 0;
}
