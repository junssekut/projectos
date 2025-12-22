/*
 * ============================================================================
 *                      simulasi penjadwalan proses cpu
 * ============================================================================
 * ini program buat simulasi 4 algoritma scheduling cpu
 * ada fcfs, sjf, srt, sama round robin
 * tinggal pilih aja mau yang mana terus liat hasilnya
 * ============================================================================
 */

#include <stdio.h>

#define MAX_PROCESS 100
#define MAX_GANTT 1000

/* ============================================================================
 *                              struct data
 * ============================================================================ */

typedef struct {
    int id;               // id proses
    int arrival_time;     // kapan dateng
    int burst_time;       // butuh brp lama dieksekusi
    int completion_time;  // kapan selesai
    int turnaround_time;  // total waktu dari dateng sampe kelar (ct - at)
    int waiting_time;     // waktu nunggu (tat - bt)
    int response_time;    // waktu pertama kali dieksekusi - at
    int remaining_time;   // sisa waktu buat srt sama rr
    int is_started;       // udah pernah jalan atau belum
} Process;

typedef struct {
    int pid;              // process id, -1 kalo idle
    int start_time;       // mulai kapan
    int end_time;         // selesai kapan
} GanttEntry;

typedef struct {
    GanttEntry entries[MAX_GANTT];
    int count;
} GanttChart;

/* ============================================================================
 *                              fungsi tampilan
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

void print_gantt_chart(GanttChart *chart) {
    if (chart->count == 0) return;
    
    printf("\n  Gantt Chart:\n");
    printf("  ");
    
    // bikin border atas
    for (int i = 0; i < chart->count; i++) {
        printf("┌────");
    }
    printf("┐\n");
    
    // tampilin process id nya
    printf("  ");
    for (int i = 0; i < chart->count; i++) {
        if (chart->entries[i].pid == -1) {
            printf("│ -- ");
        } else {
            printf("│ P%-2d", chart->entries[i].pid);
        }
    }
    printf("│\n");
    
    // border bawah
    printf("  ");
    for (int i = 0; i < chart->count; i++) {
        printf("└────");
    }
    printf("┘\n");
    
    // timeline nya
    printf("  ");
    for (int i = 0; i < chart->count; i++) {
        printf("%-5d", chart->entries[i].start_time);
    }
    printf("%d\n", chart->entries[chart->count - 1].end_time);
}

void gantt_add(GanttChart *chart, int pid, int start, int end) {
    // kalo pid sama kayak sebelumnya, gabungin aja biar ga kebanyakan entry
    if (chart->count > 0 && chart->entries[chart->count - 1].pid == pid &&
        chart->entries[chart->count - 1].end_time == start) {
        chart->entries[chart->count - 1].end_time = end;
    } else {
        chart->entries[chart->count].pid = pid;
        chart->entries[chart->count].start_time = start;
        chart->entries[chart->count].end_time = end;
        chart->count++;
    }
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
 *                              fungsi input
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
 *                              fungsi utilitas
 * ============================================================================ */

// copy data proses dari source ke destination
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

// ngitung rata rata statistik
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

// sort berdasarkan arrival time pake bubble sort
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
 *                     algoritma 1: fcfs (first come first served)
 * ============================================================================
 * yang dateng duluan dieksekusi duluan, simpel
 * non-preemptive jadi sekali jalan ga bisa diinterupsi
 * ============================================================================ */

void run_fcfs(Process original[], int n) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    sort_by_arrival(p, n);
    
    GanttChart chart = {.count = 0};
    int current_time = 0;
    
    for (int i = 0; i < n; i++) {
        // kalo cpu nganggur, skip ke waktu proses berikutnya dateng
        if (current_time < p[i].arrival_time) {
            gantt_add(&chart, -1, current_time, p[i].arrival_time);
            current_time = p[i].arrival_time;
        }
        
        p[i].response_time = current_time - p[i].arrival_time;
        int start = current_time;
        current_time += p[i].burst_time;
        p[i].completion_time = current_time;
        p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
        
        gantt_add(&chart, p[i].id, start, current_time);
    }
    
    double avg_rt, avg_tat, avg_wt;
    calculate_statistics(p, n, &avg_rt, &avg_tat, &avg_wt);
    
    print_table(p, n, "FCFS (First Come First Served)");
    print_gantt_chart(&chart);
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                     algoritma 2: sjf (shortest job first)
 * ============================================================================
 * yang burst time nya paling kecil dieksekusi duluan
 * non-preemptive juga, sekali jalan sampe kelar
 * ============================================================================ */

void run_sjf(Process original[], int n) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    
    GanttChart chart = {.count = 0};
    int completed = 0;
    int current_time = 0;
    int idle_start = -1;
    
    while (completed < n) {
        // cari proses yg burst time paling kecil dan udah dateng
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
            if (idle_start != -1) {
                gantt_add(&chart, -1, idle_start, current_time);
                idle_start = -1;
            }
            p[shortest].response_time = current_time - p[shortest].arrival_time;
            int start = current_time;
            current_time += p[shortest].burst_time;
            p[shortest].completion_time = current_time;
            p[shortest].turnaround_time = p[shortest].completion_time - p[shortest].arrival_time;
            p[shortest].waiting_time = p[shortest].turnaround_time - p[shortest].burst_time;
            gantt_add(&chart, p[shortest].id, start, current_time);
            completed++;
        } else {
            // gaada proses siap, naikin waktu aja
            if (idle_start == -1) idle_start = current_time;
            current_time++;
        }
    }
    
    double avg_rt, avg_tat, avg_wt;
    calculate_statistics(p, n, &avg_rt, &avg_tat, &avg_wt);
    
    print_table(p, n, "SJF (Shortest Job First)");
    print_gantt_chart(&chart);
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                     algoritma 3: srt (shortest remaining time)
 * ============================================================================
 * mirip sjf tapi preemptive, jadi yang remaining time paling kecil jalan duluan
 * bisa diinterupsi kalo ada proses baru yg remaining nya lebih kecil
 * ============================================================================ */

void run_srt(Process original[], int n) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    
    GanttChart chart = {.count = 0};
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        // cari yg remaining time paling kecil
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
            // catat response time pas pertama kali jalan
            if (!p[shortest].is_started) {
                p[shortest].response_time = current_time - p[shortest].arrival_time;
                p[shortest].is_started = 1;
            }
            
            // jalan 1 unit waktu
            p[shortest].remaining_time--;
            gantt_add(&chart, p[shortest].id, current_time, current_time + 1);
            current_time++;
            
            // cek kalo udah kelar
            if (p[shortest].remaining_time == 0) {
                p[shortest].completion_time = current_time;
                p[shortest].turnaround_time = p[shortest].completion_time - p[shortest].arrival_time;
                p[shortest].waiting_time = p[shortest].turnaround_time - p[shortest].burst_time;
                completed++;
            }
        } else {
            gantt_add(&chart, -1, current_time, current_time + 1);
            current_time++;
        }
    }
    
    double avg_rt, avg_tat, avg_wt;
    calculate_statistics(p, n, &avg_rt, &avg_tat, &avg_wt);
    
    print_table(p, n, "SRT (Shortest Remaining Time)");
    print_gantt_chart(&chart);
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                     algoritma 4: rr (round robin)
 * ============================================================================
 * tiap proses dapet jatah waktu sama (quantum) gantian
 * kalo quantum habis, ganti proses lain dulu
 * ============================================================================ */

void run_round_robin(Process original[], int n, int quantum) {
    Process p[MAX_PROCESS];
    copy_processes(original, p, n);
    sort_by_arrival(p, n);
    
    GanttChart chart = {.count = 0};
    
    // queue pake array biasa aja
    int queue[MAX_PROCESS * 100];
    int front = 0, rear = 0;
    
    int completed = 0;
    int current_time = 0;
    int in_queue[MAX_PROCESS] = {0};
    
    // masukin proses yang udah dateng di waktu 0
    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time <= current_time) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }
    
    while (completed < n) {
        if (front < rear) {
            int i = queue[front++];
            
            // catat response time pas pertama jalan
            if (!p[i].is_started) {
                p[i].response_time = current_time - p[i].arrival_time;
                p[i].is_started = 1;
            }
            
            // jalan selama quantum atau sampe kelar
            int exec_time = (p[i].remaining_time <= quantum) ? p[i].remaining_time : quantum;
            int start = current_time;
            p[i].remaining_time -= exec_time;
            current_time += exec_time;
            gantt_add(&chart, p[i].id, start, current_time);
            
            // cek ada proses baru yang dateng ga
            for (int j = 0; j < n; j++) {
                int has_arrived = p[j].arrival_time <= current_time;
                int not_in_queue = !in_queue[j];
                int not_done = p[j].remaining_time > 0;
                
                if (has_arrived && not_in_queue && not_done) {
                    queue[rear++] = j;
                    in_queue[j] = 1;
                }
            }
            
            // cek udah kelar belom
            if (p[i].remaining_time == 0) {
                p[i].completion_time = current_time;
                p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
                p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
                completed++;
            } else {
                // belom kelar, masukin lagi ke queue
                queue[rear++] = i;
            }
        } else {
            // queue kosong, cari proses selanjutnya yg bakal dateng
            int next_arrival = 999999;
            int next_idx = -1;
            
            for (int i = 0; i < n; i++) {
                if (p[i].remaining_time > 0 && !in_queue[i] && p[i].arrival_time < next_arrival) {
                    next_arrival = p[i].arrival_time;
                    next_idx = i;
                }
            }
            
            if (next_idx != -1) {
                gantt_add(&chart, -1, current_time, next_arrival);
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
    print_gantt_chart(&chart);
    print_statistics(avg_rt, avg_tat, avg_wt);
}

/* ============================================================================
 *                              main function
 * ============================================================================ */

int main() {
    Process processes[MAX_PROCESS];
    int n, choice, quantum;
    
    print_header("SIMULASI PENJADWALAN PROSES CPU");
    
    // input data proses dulu
    n = input_processes(processes);
    printf("\n  ✓ Data %d proses berhasil disimpan!\n", n);
    
    // loop menu utama
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
