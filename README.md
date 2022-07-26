Documentation will come later sorry. Just look at how the tests work for now.

If there are any problems tell me please.

I would suggest you to monitor your processes and to check your CPU usage while using this library.
I (have to) use new processes and not thread to run commands, so there is a chance that mine or your code has a bug and it could keep spawning new processes or not terminating old ones.

At the moment I'm working at the AsyncExecute() function (not commited yet).
