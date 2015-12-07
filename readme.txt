## Git

## How to compile
	>> make clean
	>> make
## How to use
	>> ./CFS-emulator
## Initial Processes
	a set of 20 processes are stored in file "processes.txt"
	you can add or modify processes by editing this file
## Balancer
	the balancer are triggered every 2 seconds using sleep function
	you can change that by changing the b_freq macro
## Finish
	When there is no more process to execute, the balancer will stop the
	emulator by setting running = 0
## Slow down the print out
	You can slow down the printing by adjusting the macro "sleep_time"
## Note
- The process info table is showed at the beginning and when a process is finished.
- Cltr + C if you want to stop
