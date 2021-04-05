# hanin.4

This project was going really well until I was about 3/4ths of the way done, when I hit a wall that took about 10 hours and was never accomplished.

Most of the code works, it will run *correctly* for a few times, and then as soon as something is blocked my code hits a wall.

I removed the block condition to see if it was an error in my logic, and even without the block condition, it happens still.

It seems that, at some point in time, my code will go to send a message to a process in readyQueue that has already been completed and -1'd out.

I've tested this multiple times, checking that specific index's PID before and after turning it to -1 to see if the initialization is sticking, and it is. But after a certain number of times through the program, the code will get to that index, and resurrect an already dead PID from the grave, and attempt to send a message to a process that has already finished, and get stuck until timeout.

