/*
 * ============================================================================
 *                      SIMULASI PENJADWALAN PROSES CPU
 * ============================================================================
 * Program ini mensimulasikan 4 algoritma penjadwalan CPU:
 *   1. FCFS - First Come First Served
 *   2. SJF  - Shortest Job First (Non-preemptive)
 *   3. SRT  - Shortest Remaining Time (Preemptive)
 *   4. RR   - Round Robin
 * ============================================================================
 */

#include <stdio.h>

#define MAX_PROCESS 100

/* ============================================================================
 *                              STRUKTUR DATA
 * ============================================================================ */

typedef struct {
    int id;               // Nomor proses
    int arrival_time;     // Waktu tiba
    int burst_time;       // Waktu eksekusi
    int completion_time;  // Waktu selesai
    int turnaround_time;  // Waktu putar (CT - AT)
    int waiting_time;     // Waktu tunggu (TAT - BT)
    int response_time;    // Waktu respon (pertama kali dieksekusi - AT)
    int remaining_time;   // Sisa waktu (untuk SRT dan RR)
    int is_started;       // Penanda: sudah pernah dieksekusi atau belum
} Process;

/* ============================================================================
 *                              FUNGSI TAMPILAN
 * ============================================================================ */

void print_header(char *title) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-60s║\n", title);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

void print_table(Process list[], int n, char *algorithm_name) {
    printf("\n  %s\n", algorithm_name);
    printf("  ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┐\n");
    printf("  │ PID │  AT │  BT │  CT │ TAT │  WT │  RT │\n");
    printf("  ├─────┼─────┼─────┼─────┼─────┼─────┼─────┤\n");
    
    for (int i = 0; i < n; i++) {
        printf("  │ P%-2d │ %3d │ %3d │ %3d │ %3d │ %3d │ %3d │\n",
               list[i].id,
               list[i].arrival_time,
               list[i].burst_time,
               list[i].completion_time,
               list[i].turnaround_time,
               list[i].waiting_time,
               list[i].response_time);
    }
    printf("  └─────┴─────┴─────┴─────┴─────┴─────┴─────┘\n");
}

void print_statistics(double avg_response, double avg_turnaround, double avg_waiting) {
    printf("\n  ┌───────────────────────────────────────────┐\n");
    printf("  │  Avg Response Time:   %8.2f            │\n", avg_response);
    printf("  │  Avg Turnaround Time: %8.2f            │\n", avg_turnaround);
    printf("  │  Avg Waiting Time:    %8.2f            │\n", avg_waiting);
    printf("  └───────────────────────────────────────────┘\n");
}

void print_menu() {
    printf("\n  ┌─────────────────────────────────────────┐\n");
    printf("  │  PILIH ALGORITMA PENJADWALAN           │\n");
    printf("  ├─────────────────────────────────────────┤\n");
    printf("  │  1. FCFS (First Come First Served)     │\n");
    printf("  │  2. SJF  (Shortest Job First)          │\n");
    printf("  │  3. SRT  (Shortest Remaining Time)     │\n");
    printf("  │  4. RR   (Round Robin)                 │\n");
    printf("  │  5. Jalankan Semua Algoritma           │\n");
    printf("  │  0. Keluar                             │\n");
    printf("  └─────────────────────────────────────────┘\n");
    printf("  Pilihan: ");
}

/* ============================================================================
 *                              FUNGSI INPUT
 * ============================================================================ */

int input_processes(Process list[]) {
    int n;
    
    printf("\n  Masukkan jumlah proses (1-%d): ", MAX_PROCESS);
    scanf("%d", &n);
    
    if (n <= 0 || n > MAX_PROCESS) {
        printf("  Jumlah tidak valid. Menggunakan 3 proses.\n");
        n = 3;
    }
    
    printf("\n  Masukkan data setiap proses:\n");
    printf("  ─────────────────────────────\n");
    
    for (int i = 0; i < n; i++) {
        list[i].id = i + 1;
        printf("\n  Proses %d:\n", i + 1);
        printf("    Arrival Time : ");
        scanf("%d", &list[i].arrival_time);
        printf("    Burst Time   : ");
        scanf("%d", &list[i].burst_time);
    }
    
    return n;
}

/* ============================================================================
 *                              FUNGSI UTILITAS
 * ============================================================================ */

// Menyalin data proses dari array sumber ke array tujuan
void copy_processes(Process src[], Process dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i].id = src[i].id;
        dest[i].arrival_time = src[i].arrival_time;
        dest[i].burst_time = src[i].burst_time;
        dest[i].remaining_time = src[i].burst_time;
        dest[i].completion_time = 0;
        dest[i].turnaround_time = 0;
        dest[i].waiting_time = 0;
        dest[i].response_time = 0;
        dest[i].is_started = 0;
    }
}

// Menghitung rata-rata statistik
void calculate_statistics(Process list[], int n, double *avg_rt, double *avg_tat, double *avg_wt) {
    double total_rt = 0, total_tat = 0, total_wt = 0;
    
    for (int i = 0; i < n; i++) {
        total_rt += list[i].response_time;
        total_tat += list[i].turnaround_time;
        total_wt += list[i].waiting_time;
    }
    
    *avg_rt = total_rt / n;
    *avg_tat = total_tat / n;
    *avg_wt = total_wt / n;
}

// Mengurutkan proses berdasarkan arrival time (bubble sort)
void sort_by_arrival(Process list[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (list[j].arrival_time > list[j + 1].arrival_time) {
                Process temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

/* ============================================================================
 *                     ALGORITMA 1: FCFS (First Come First Served)
 * ============================================================================
 * Proses dieksekusi sesuai urutan kedatangan.
 * Non-preemptive: proses berjalan sampai selesai.
 * ============================================================================ */

void run_fcfs(Process original[], int n) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    sort_by_arrival(p, n);
    
    int current_time = 0;
    
    for (int i = 0; i < n; i++) {
        // Jika CPU idle, lompat ke waktu tiba proses berikutnya
        if (current_time < p[i].arrival_time) {
            current_time = p[i].arrival_time;
        }
        
        p[i].response_time = current_time - p[i].arrival_time;
        current_time += p[i].burst_time;
        p[i].completion_time = current_time;
        p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
    }
    
    double avg_rt, avg_tat, avg_wt;
    calculate_statistics(p, n, &avg_rt, &avg_tat, &avg_wt);
    
    print_table(p, n, "FCFS (First Come First Served)");
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                     ALGORITMA 2: SJF (Shortest Job First)
 * ============================================================================
 * Proses dengan burst time terpendek dieksekusi duluan.
 * Non-preemptive: proses berjalan sampai selesai.
 * ============================================================================ */

void run_sjf(Process original[], int n) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        // Cari proses dengan burst time terpendek yang sudah tiba
        int shortest = -1;
        int min_burst = 999999;
        
        for (int i = 0; i < n; i++) {
            int has_arrived = p[i].arrival_time <= current_time;
            int not_done = p[i].completion_time == 0;
            int is_shorter = p[i].burst_time < min_burst;
            
            if (has_arrived && not_done && is_shorter) {
                shortest = i;
                min_burst = p[i].burst_time;
            }
        }
        
        if (shortest != -1) {
            p[shortest].response_time = current_time - p[shortest].arrival_time;
            current_time += p[shortest].burst_time;
            p[shortest].completion_time = current_time;
            p[shortest].turnaround_time = p[shortest].completion_time - p[shortest].arrival_time;
            p[shortest].waiting_time = p[shortest].turnaround_time - p[shortest].burst_time;
            completed++;
        } else {
            // Tidak ada proses yang siap, maju waktu
            current_time++;
        }
    }
    
    double avg_rt, avg_tat, avg_wt;
    calculate_statistics(p, n, &avg_rt, &avg_tat, &avg_wt);
    
    print_table(p, n, "SJF (Shortest Job First)");
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                     ALGORITMA 3: SRT (Shortest Remaining Time)
 * ============================================================================
 * Proses dengan sisa waktu terpendek dieksekusi duluan.
 * Preemptive: proses bisa diinterupsi jika ada proses lain dengan sisa waktu lebih pendek.
 * ============================================================================ */

void run_srt(Process original[], int n) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        // Cari proses dengan remaining time terpendek yang sudah tiba
        int shortest = -1;
        int min_remaining = 999999;
        
        for (int i = 0; i < n; i++) {
            int has_arrived = p[i].arrival_time <= current_time;
            int not_done = p[i].remaining_time > 0;
            int is_shorter = p[i].remaining_time < min_remaining;
            
            if (has_arrived && not_done && is_shorter) {
                shortest = i;
                min_remaining = p[i].remaining_time;
            }
        }
        
        if (shortest != -1) {
            // Catat response time saat pertama kali dieksekusi
            if (!p[shortest].is_started) {
                p[shortest].response_time = current_time - p[shortest].arrival_time;
                p[shortest].is_started = 1;
            }
            
            // Eksekusi 1 unit waktu
            p[shortest].remaining_time--;
            current_time++;
            
            // Cek apakah proses selesai
            if (p[shortest].remaining_time == 0) {
                p[shortest].completion_time = current_time;
                p[shortest].turnaround_time = p[shortest].completion_time - p[shortest].arrival_time;
                p[shortest].waiting_time = p[shortest].turnaround_time - p[shortest].burst_time;
                completed++;
            }
        } else {
            current_time++;
        }
    }
    
    double avg_rt, avg_tat, avg_wt;
    calculate_statistics(p, n, &avg_rt, &avg_tat, &avg_wt);
    
    print_table(p, n, "SRT (Shortest Remaining Time)");
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                     ALGORITMA 4: RR (Round Robin)
 * ============================================================================
 * Setiap proses mendapat jatah waktu (quantum) yang sama secara bergiliran.
 * Preemptive: proses diinterupsi setelah quantum habis.
 * ============================================================================ */

void run_round_robin(Process original[], int n, int quantum) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    sort_by_arrival(p, n);
    
    // Antrian sederhana menggunakan array
    int queue[MAX_PROCESS * 100];
    int front = 0, rear = 0;
    
    int completed = 0;
    int current_time = 0;
    int in_queue[MAX_PROCESS] = {0};
    
    // Masukkan proses yang sudah tiba di waktu 0
    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time <= current_time) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }
    
    while (completed < n) {
        if (front < rear) {
            int i = queue[front++];
            
            // Catat response time saat pertama kali dieksekusi
            if (!p[i].is_started) {
                p[i].response_time = current_time - p[i].arrival_time;
                p[i].is_started = 1;
            }
            
            // Eksekusi selama quantum atau sampai selesai
            int exec_time = (p[i].remaining_time <= quantum) ? p[i].remaining_time : quantum;
            p[i].remaining_time -= exec_time;
            current_time += exec_time;
            
            // Masukkan proses baru yang tiba selama eksekusi
            for (int j = 0; j < n; j++) {
                int has_arrived = p[j].arrival_time <= current_time;
                int not_in_queue = !in_queue[j];
                int not_done = p[j].remaining_time > 0;
                
                if (has_arrived && not_in_queue && not_done) {
                    queue[rear++] = j;
                    in_queue[j] = 1;
                }
            }
            
            // Cek apakah proses selesai
            if (p[i].remaining_time == 0) {
                p[i].completion_time = current_time;
                p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
                p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
                completed++;
            } else {
                // Belum selesai, masukkan kembali ke antrian
                queue[rear++] = i;
            }
        } else {
            // Antrian kosong, cari proses berikutnya yang akan tiba
            int next_arrival = 999999;
            int next_idx = -1;
            
            for (int i = 0; i < n; i++) {
                if (p[i].remaining_time > 0 && !in_queue[i] && p[i].arrival_time < next_arrival) {
                    next_arrival = p[i].arrival_time;
                    next_idx = i;
                }
            }
            
            if (next_idx != -1) {
                current_time = next_arrival;
                queue[rear++] = next_idx;
                in_queue[next_idx] = 1;
            }
        }
    }
    
    double avg_rt, avg_tat, avg_wt;
    calculate_statistics(p, n, &avg_rt, &avg_tat, &avg_wt);
    
    char title[50];
    sprintf(title, "Round Robin (Quantum = %d)", quantum);
    print_table(p, n, title);
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                              FUNGSI UTAMA
 * ============================================================================ */

int main() {
    Process processes[MAX_PROCESS];
    int n, choice, quantum;
    
    print_header("SIMULASI PENJADWALAN PROSES CPU");
    
    // Input data proses
    n = input_processes(processes);
    printf("\n  ✓ Data %d proses berhasil disimpan!\n", n);
    
    // Menu utama
    while (1) {
        print_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                run_fcfs(processes, n);
                break;
            case 2:
                run_sjf(processes, n);
                break;
            case 3:
                run_srt(processes, n);
                break;
            case 4:
                printf("\n  Masukkan Time Quantum: ");
                scanf("%d", &quantum);
                if (quantum <= 0) quantum = 2;
                run_round_robin(processes, n, quantum);
                break;
            case 5:
                run_fcfs(processes, n);
                run_sjf(processes, n);
                run_srt(processes, n);
                printf("\n  Masukkan Time Quantum untuk Round Robin: ");
                scanf("%d", &quantum);
                if (quantum <= 0) quantum = 2;
                run_round_robin(processes, n, quantum);
                break;
            case 0:
                printf("\n  Terima kasih!\n\n");
                return 0;
            default:
                printf("\n  ⚠ Pilihan tidak valid!\n");
        }
    }
    
    return 0;
}
