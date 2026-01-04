/*
 * ============================================================================
 *                   scheduler system call untuk linux kernel
 * ============================================================================
 * ini versi kernel dari scheduler.c yang buat tugas os
 * ada 4 algoritma: fcfs, sjf, srt, sama round robin
 * bedanya sama yang user-space, ini jalan di dalam kernel
 * output nya ke dmesg, bukan printf
 * ============================================================================
 */

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

#define MAX_PROCESS 10
#define MAX_GANTT 100

#define ALGO_FCFS 1
#define ALGO_SJF  2
#define ALGO_SRT  3
#define ALGO_RR   4

/* ============================================================================
 *                              struct data
 * ============================================================================ */

struct sched_process {
    int id;               // id proses
    int arrival_time;     // kapan dateng
    int burst_time;       // butuh brp lama dieksekusi
    int completion_time;  // kapan selesai
    int turnaround_time;  // total waktu dari dateng sampe kelar (ct - at)
    int waiting_time;     // waktu nunggu (tat - bt)
    int response_time;    // waktu pertama kali dieksekusi - at
    int remaining_time;   // sisa waktu buat srt sama rr
    int is_started;       // udah pernah jalan atau belum
};

struct gantt_entry {
    int pid;              // process id, -1 kalo idle
    int start_time;       // mulai kapan
    int end_time;         // selesai kapan
};

struct gantt_chart {
    struct gantt_entry entries[MAX_GANTT];
    int count;
};

/* ============================================================================
 *                              fungsi utilitas
 * ============================================================================ */

// tambah entry ke gantt chart
// kalo pid sama kayak sebelumnya, gabungin aja biar ga kebanyakan entry
static void gantt_add(struct gantt_chart *chart, int pid, int start, int end)
{
    if (chart->count > 0 && 
        chart->entries[chart->count - 1].pid == pid &&
        chart->entries[chart->count - 1].end_time == start) {
        chart->entries[chart->count - 1].end_time = end;
    } else {
        if (chart->count < MAX_GANTT) {
            chart->entries[chart->count].pid = pid;
            chart->entries[chart->count].start_time = start;
            chart->entries[chart->count].end_time = end;
            chart->count++;
        }
    }
}

// sort berdasarkan arrival time pake bubble sort
static void sort_by_arrival(struct sched_process *p, int n)
{
    int i, j;
    struct sched_process temp;
    
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (p[j].arrival_time > p[j + 1].arrival_time) {
                temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
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

static void run_fcfs(struct sched_process *p, int n, struct gantt_chart *chart)
{
    int i, current_time = 0;
    
    sort_by_arrival(p, n);
    
    for (i = 0; i < n; i++) {
        // kalo cpu nganggur, skip ke waktu proses berikutnya dateng
        if (current_time < p[i].arrival_time) {
            gantt_add(chart, -1, current_time, p[i].arrival_time);
            current_time = p[i].arrival_time;
        }
        
        p[i].response_time = current_time - p[i].arrival_time;
        gantt_add(chart, p[i].id, current_time, current_time + p[i].burst_time);
        current_time += p[i].burst_time;
        p[i].completion_time = current_time;
        p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
    }
}

/* ============================================================================
 *                     algoritma 2: sjf (shortest job first)
 * ============================================================================
 * yang burst time nya paling kecil dieksekusi duluan
 * non-preemptive juga, sekali jalan sampe kelar
 * ============================================================================ */

static void run_sjf(struct sched_process *p, int n, struct gantt_chart *chart)
{
    int i, completed = 0, current_time = 0;
    int shortest, min_burst;
    int idle_start = -1;
    
    while (completed < n) {
        // cari proses yg burst time paling kecil dan udah dateng
        shortest = -1;
        min_burst = 999999;
        
        for (i = 0; i < n; i++) {
            int has_arrived = p[i].arrival_time <= current_time;
            int not_done = p[i].completion_time == 0;
            int is_shorter = p[i].burst_time < min_burst;
            
            if (has_arrived && not_done && is_shorter) {
                shortest = i;
                min_burst = p[i].burst_time;
            }
        }
        
        if (shortest != -1) {
            // kalo sebelumnya idle, tambahin ke gantt
            if (idle_start != -1) {
                gantt_add(chart, -1, idle_start, current_time);
                idle_start = -1;
            }
            
            p[shortest].response_time = current_time - p[shortest].arrival_time;
            gantt_add(chart, p[shortest].id, current_time, current_time + p[shortest].burst_time);
            current_time += p[shortest].burst_time;
            p[shortest].completion_time = current_time;
            p[shortest].turnaround_time = p[shortest].completion_time - p[shortest].arrival_time;
            p[shortest].waiting_time = p[shortest].turnaround_time - p[shortest].burst_time;
            completed++;
        } else {
            // gaada proses siap, naikin waktu aja
            if (idle_start == -1) idle_start = current_time;
            current_time++;
        }
    }
}

/* ============================================================================
 *                     algoritma 3: srt (shortest remaining time)
 * ============================================================================
 * mirip sjf tapi preemptive, jadi yang remaining time paling kecil jalan duluan
 * bisa diinterupsi kalo ada proses baru yg remaining nya lebih kecil
 * ============================================================================ */

static void run_srt(struct sched_process *p, int n, struct gantt_chart *chart)
{
    int i, completed = 0, current_time = 0;
    int shortest, min_remaining;
    
    while (completed < n) {
        // cari yg remaining time paling kecil
        shortest = -1;
        min_remaining = 999999;
        
        for (i = 0; i < n; i++) {
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
            gantt_add(chart, p[shortest].id, current_time, current_time + 1);
            current_time++;
            
            // cek kalo udah kelar
            if (p[shortest].remaining_time == 0) {
                p[shortest].completion_time = current_time;
                p[shortest].turnaround_time = p[shortest].completion_time - p[shortest].arrival_time;
                p[shortest].waiting_time = p[shortest].turnaround_time - p[shortest].burst_time;
                completed++;
            }
        } else {
            gantt_add(chart, -1, current_time, current_time + 1);
            current_time++;
        }
    }
}

/* ============================================================================
 *                     algoritma 4: rr (round robin)
 * ============================================================================
 * tiap proses dapet jatah waktu sama (quantum) gantian
 * kalo quantum habis, ganti proses lain dulu
 * ============================================================================ */

static void run_rr(struct sched_process *p, int n, struct gantt_chart *chart, int quantum)
{
    int i, j, completed = 0, current_time = 0;
    int queue[MAX_PROCESS * 10];
    int front = 0, rear = 0;
    int in_queue[MAX_PROCESS] = {0};
    int idx, exec_time;
    
    sort_by_arrival(p, n);
    
    // masukin proses yang udah dateng di waktu 0
    for (i = 0; i < n; i++) {
        if (p[i].arrival_time <= current_time) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }
    
    while (completed < n) {
        if (front < rear) {
            idx = queue[front++];
            
            // catat response time pas pertama jalan
            if (!p[idx].is_started) {
                p[idx].response_time = current_time - p[idx].arrival_time;
                p[idx].is_started = 1;
            }
            
            // jalan selama quantum atau sampe kelar
            exec_time = (p[idx].remaining_time <= quantum) ? p[idx].remaining_time : quantum;
            gantt_add(chart, p[idx].id, current_time, current_time + exec_time);
            p[idx].remaining_time -= exec_time;
            current_time += exec_time;
            
            // cek ada proses baru yang dateng ga
            for (j = 0; j < n; j++) {
                int has_arrived = p[j].arrival_time <= current_time;
                int not_in_queue = !in_queue[j];
                int not_done = p[j].remaining_time > 0;
                
                if (has_arrived && not_in_queue && not_done) {
                    queue[rear++] = j;
                    in_queue[j] = 1;
                }
            }
            
            // cek udah kelar belom
            if (p[idx].remaining_time == 0) {
                p[idx].completion_time = current_time;
                p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
                p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
                completed++;
            } else {
                // belom kelar, masukin lagi ke queue
                queue[rear++] = idx;
            }
        } else {
            // queue kosong, cari proses selanjutnya yg bakal dateng
            for (i = 0; i < n; i++) {
                if (p[i].remaining_time > 0 && !in_queue[i]) {
                    gantt_add(chart, -1, current_time, p[i].arrival_time);
                    current_time = p[i].arrival_time;
                    queue[rear++] = i;
                    in_queue[i] = 1;
                    break;
                }
            }
        }
    }
}

/* ============================================================================
 *                              fungsi output ke dmesg
 * ============================================================================ */

// print tabel hasil ke kernel log
static void print_results(struct sched_process *p, int n, const char *algo_name)
{
    int i;
    long total_wt = 0, total_tat = 0, total_rt = 0;
    
    printk(KERN_INFO "============================================================\n");
    printk(KERN_INFO "  %s\n", algo_name);
    printk(KERN_INFO "============================================================\n");
    printk(KERN_INFO "  PID |  AT |  BT |  CT | TAT |  WT |  RT\n");
    printk(KERN_INFO "------+-----+-----+-----+-----+-----+-----\n");
    
    for (i = 0; i < n; i++) {
        printk(KERN_INFO "  P%-2d | %3d | %3d | %3d | %3d | %3d | %3d\n",
               p[i].id, p[i].arrival_time, p[i].burst_time,
               p[i].completion_time, p[i].turnaround_time,
               p[i].waiting_time, p[i].response_time);
        total_wt += p[i].waiting_time;
        total_tat += p[i].turnaround_time;
        total_rt += p[i].response_time;
    }
    
    printk(KERN_INFO "------+-----+-----+-----+-----+-----+-----\n");
    // pake x100 karena kernel ga support float/double
    printk(KERN_INFO "  Avg WT  (x100): %ld\n", (total_wt * 100) / n);
    printk(KERN_INFO "  Avg TAT (x100): %ld\n", (total_tat * 100) / n);
    printk(KERN_INFO "  Avg RT  (x100): %ld\n", (total_rt * 100) / n);
}

// print gantt chart ke kernel log
static void print_gantt(struct gantt_chart *chart)
{
    int i;
    
    if (chart->count == 0) return;
    
    printk(KERN_INFO "\n  Gantt Chart:\n");
    printk(KERN_INFO "  ");
    
    // print process ids
    for (i = 0; i < chart->count; i++) {
        if (chart->entries[i].pid == -1) {
            printk(KERN_CONT "[--]");
        } else {
            printk(KERN_CONT "[P%d]", chart->entries[i].pid);
        }
    }
    printk(KERN_CONT "\n");
    
    // print timeline
    printk(KERN_INFO "  ");
    for (i = 0; i < chart->count; i++) {
        printk(KERN_CONT "%-4d", chart->entries[i].start_time);
    }
    printk(KERN_CONT "%d\n", chart->entries[chart->count - 1].end_time);
    printk(KERN_INFO "============================================================\n");
}

/* ============================================================================
 *                              main system call
 * ============================================================================
 * ini syscall utama yang dipanggil dari user space
 * parameter:
 *   algo     - algoritma mana yang mau dipake (1=fcfs, 2=sjf, 3=srt, 4=rr)
 *   n        - jumlah proses
 *   arrivals - pointer ke array arrival times dari user space
 *   bursts   - pointer ke array burst times dari user space
 *   quantum  - time quantum (cuma dipake buat round robin)
 * ============================================================================ */

SYSCALL_DEFINE5(scheduler, int, algo, int, n, int __user *, arrivals, int __user *, bursts, int, quantum)
{
    struct sched_process p[MAX_PROCESS];
    struct gantt_chart chart = {.count = 0};
    int arr[MAX_PROCESS], bur[MAX_PROCESS];
    int i;
    const char *algo_names[] = {"", "FCFS (First Come First Served)", 
                                "SJF (Shortest Job First)", 
                                "SRT (Shortest Remaining Time)", 
                                "Round Robin"};
    
    // validasi input
    if (n <= 0 || n > MAX_PROCESS) {
        printk(KERN_WARNING "scheduler: jumlah proses ga valid n=%d (max=%d)\n", n, MAX_PROCESS);
        return -EINVAL;
    }
    
    if (algo < 1 || algo > 4) {
        printk(KERN_WARNING "scheduler: algoritma ga valid algo=%d\n", algo);
        return -EINVAL;
    }
    
    // copy data dari user space ke kernel space
    if (copy_from_user(arr, arrivals, sizeof(int) * n))
        return -EFAULT;
    if (copy_from_user(bur, bursts, sizeof(int) * n))
        return -EFAULT;
    
    // inisialisasi struct process
    for (i = 0; i < n; i++) {
        p[i].id = i + 1;
        p[i].arrival_time = arr[i];
        p[i].burst_time = bur[i];
        p[i].remaining_time = bur[i];
        p[i].completion_time = 0;
        p[i].turnaround_time = 0;
        p[i].waiting_time = 0;
        p[i].response_time = 0;
        p[i].is_started = 0;
    }
    
    // jalanin algoritma yang dipilih
    switch (algo) {
        case ALGO_FCFS: run_fcfs(p, n, &chart); break;
        case ALGO_SJF:  run_sjf(p, n, &chart); break;
        case ALGO_SRT:  run_srt(p, n, &chart); break;
        case ALGO_RR:   run_rr(p, n, &chart, quantum > 0 ? quantum : 2); break;
    }
    
    // print hasil ke dmesg
    print_results(p, n, algo_names[algo]);
    print_gantt(&chart);
    
    return 0;
}
