# Process Scheduling Simulator

A comprehensive C program that implements four CPU scheduling algorithms for educational purposes.

## Features

- **Four Scheduling Policies:**
  - FCFS (First Come First Served)
  - SJF (Shortest Job First - Non-preemptive)
  - SRT (Shortest Remaining Time - Preemptive)
  - RR (Round Robin)

- **Statistics Calculated:**
  - Average Response Time
  - Average Turnaround Time
  - Average Waiting Time

- **User-Friendly Interface:**
  - Simple input prompts
  - Formatted output tables
  - Menu-driven navigation

## Compilation

```bash
make
```

Or compile directly:
```bash
gcc -Wall -std=c99 -o scheduler scheduler.c
```

## Running the Program

```bash
make run
```

Or:
```bash
./scheduler
```

## Usage

1. Enter the number of processes (1-100)
2. Enter arrival time and burst time for each process
3. Select a scheduling policy from the menu
4. View the calculated statistics

## Example

```
Number of processes: 3

Process 1:
  Arrival Time: 0
  Burst Time: 8

Process 2:
  Arrival Time: 1
  Burst Time: 4

Process 3:
  Arrival Time: 2
  Burst Time: 2
```

## Clean Up

```bash
make clean
```

## Scheduling Algorithms Explained

### FCFS (First Come First Served)
- Processes are executed in the order they arrive
- Non-preemptive: Once a process starts, it runs to completion

### SJF (Shortest Job First)
- The process with the shortest burst time is executed first
- Non-preemptive: A shorter process won't interrupt a running process

### SRT (Shortest Remaining Time)
- The process with the shortest remaining time is executed next
- Preemptive: A shorter process can interrupt a running process

### Round Robin
- Each process gets a fixed time quantum to execute
- If not completed within the quantum, it goes to the back of the queue
- Fair scheduling with configurable time quantum
