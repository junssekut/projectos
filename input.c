/*
 * ============================================================================
 *                   input.c - Test Program untuk Scheduler Syscall
 * ============================================================================
 * Program untuk test custom scheduler syscall dari userspace
 * 
 * Compile:  gcc input.c -o input
 * Run:      sudo ./input
 * 
 * Lihat hasil di kernel log:  dmesg | tail -100
 * ============================================================================
 */

#define _GNU_SOURCE

#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/* ============================================================================
 *                              Syscall Definition
 * ============================================================================ */

/* Syscall number (harus sama dengan yang di syscall_64.tbl) */
#define __NR_scheduler 548

/* ============================================================================
 *                              Test Data
 * ============================================================================ */

int main(void) {
    int algo = 1;  /* Algorithm:  1=FCFS, 2=SJF, 3=SRT, 4=Round Robin */
    int n = 4;     /* Jumlah proses */
    int quantum = 2; /* Time quantum untuk Round Robin */
    
    /* Sample data (dari TEST_CASES. txt) */
    int arrivals[] = {0, 1, 2, 3};
    int bursts[] = {8, 4, 9, 5};
    
    long ret;
    
    printf("=====================================\n");
    printf("  TEST SCHEDULER SYSCALL\n");
    printf("=====================================\n\n");
    
    printf("Input Data:\n");
    printf("-----------\n");
    for (int i = 0; i < n; i++) {
        printf("  P%d: AT=%d, BT=%d\n", i+1, arrivals[i], bursts[i]);
    }
    printf("\n");
    
    /* ========== TEST 1: FCFS ========== */
    printf("========== TEST 1: FCFS ==========\n");
    printf("Calling syscall:  algo=1, n=%d\n", n);
    
    ret = syscall(__NR_scheduler, 1, n, arrivals, bursts, 0);
    
    if (ret == 0) {
        printf("✓ FCFS test SUCCESS!\n");
        printf("  Check kernel log:  dmesg | tail -50\n");
    } else {
        printf("✗ FCFS test FAILED!\n");
        printf("  Return code: %ld\n", ret);
        printf("  Error: %s (errno=%d)\n", strerror(errno), errno);
    }
    printf("\n");
    
    /* ========== TEST 2: SJF ========== */
    printf("========== TEST 2: SJF ==========\n");
    printf("Calling syscall:  algo=2, n=%d\n", n);
    
    ret = syscall(__NR_scheduler, 2, n, arrivals, bursts, 0);
    
    if (ret == 0) {
        printf("✓ SJF test SUCCESS!\n");
        printf("  Check kernel log: dmesg | tail -50\n");
    } else {
        printf("✗ SJF test FAILED!\n");
        printf("  Return code: %ld\n", ret);
        printf("  Error: %s (errno=%d)\n", strerror(errno), errno);
    }
    printf("\n");
    
    /* ========== TEST 3: SRT ========== */
    printf("========== TEST 3: SRT ==========\n");
    printf("Calling syscall:  algo=3, n=%d\n", n);
    
    ret = syscall(__NR_scheduler, 3, n, arrivals, bursts, 0);
    
    if (ret == 0) {
        printf("✓ SRT test SUCCESS!\n");
        printf("  Check kernel log: dmesg | tail -50\n");
    } else {
        printf("✗ SRT test FAILED!\n");
        printf("  Return code: %ld\n", ret);
        printf("  Error:  %s (errno=%d)\n", strerror(errno), errno);
    }
    printf("\n");
    
    /* ========== TEST 4: Round Robin ========== */
    printf("========== TEST 4: Round Robin ==========\n");
    printf("Calling syscall: algo=4, n=%d, quantum=%d\n", n, quantum);
    
    ret = syscall(__NR_scheduler, 4, n, arrivals, bursts, quantum);
    
    if (ret == 0) {
        printf("✓ Round Robin test SUCCESS!\n");
        printf("  Check kernel log:  dmesg | tail -50\n");
    } else {
        printf("✗ Round Robin test FAILED!\n");
        printf("  Return code: %ld\n", ret);
        printf("  Error: %s (errno=%d)\n", strerror(errno), errno);
    }
    printf("\n");
    
    printf("=====================================\n");
    printf("  ALL TESTS COMPLETED\n");
    printf("=====================================\n");
    printf("\nUntuk lihat hasil lengkap:\n");
    printf("  sudo dmesg | tail -100\n");
    printf("  atau\n");
    printf("  sudo dmesg | grep scheduler\n\n");
    
    return 0;
}