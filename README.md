# Documentation
## Description
This program implements the Banker's Algorithm, a deadlock-avoidance algorithm used in operating systems to determine whether a given allocation of resources to process is safe.

This code

- Reads an input file describing the Available, Max, and Allocation, matrices for a system with multiple processes and resource types.
- Computes the Need matrix and runs the safety algorithm to determine whether the system is currently in a safe or unsafe state
- If the system is safe, it prints a valid safe sequence of process execution.
- If unsafe, it prints an error message stating that no safe sequence exists.
- After the safety check, thte user may optionally enter a resource request for a specific process.
- The algorithm evaluates whether granting the request would leave the system in a safe state:
  - If request <= need and request <= available and safety holds then the request is granted
  - Otherwise then the request is denied

### Banker's Algorithm Overview

```Need[i][j] = Max[i][j] - Allocation[i][j]```

Safety steps:

1. Intialize ```Work = Available, Finish[i] = false.```
2. Find a process ```i``` such that ```Finish[i] == false``` and ```Need[i] <= Work```.
3. Pretend process ```[i]``` completes:
- Release its resources: ```Work += Allocation[i]```
- Mark ```Finish[i] = true```
- Append to safe sequence
4. Repeat until:
- If all processes finish then the system is in a safe state
- If no process can finish then the system is in an unsafe state
Request Algorithm Steps:

1. Check if ```Request[p] <= Need[p]```
2. Check if ```Request[p] <= Available```
3. Temporarily allocate resources
4. Run the safety algorithm
5. If safe then we grant
   If unsafe then we rollback

### Usage Instructions
Linux/WSL Required Make sure g++ is installed

```sudo apt update```
```sudo apt install build-essential```
Compile

```g++ -std=c++17 -Wall -Wextra -O2 banker.cpp -o banker```
Run with an input file

```./banker input.txt```
You will see:

1. Whether the system is safe or unsafe
2. The safe sequence if it exists
3. A prompt asking if you want to test a resource request
To test a request
```
Do you want to test a resource request? (y/n): y
Enter process id: 
Enter request vector:
```
To skip
```
Do you want to test a resource request? (y/n): n
```
Input file format
```
<num_processes> <num_resources>
<Available vector>
# Max matrix (n lines)
# Allocation matrix (n lines)
```
Example:

```
5 3
3 3 2
7 5 3
3 2 2
9 0 2
2 2 2
4 3 3
0 1 0
2 0 0
3 0 2
2 1 1
0 0 2
```

Examples and Results
Safe State Example

<img width="899" height="85" alt="Screenshot 2025-11-23 155157" src="https://github.com/user-attachments/assets/4eaebaac-39f0-4d17-b709-bd6cc7fdd9c1" />

Request Example (Denied Due to Not Enough Available)

<img width="693" height="149" alt="Screenshot 2025-11-24 130708" src="https://github.com/user-attachments/assets/24357340-489d-456f-ba8c-b6e980c492fe" />
