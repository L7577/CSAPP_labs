# Shell Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Shell Lab*](http://csapp.cs.cmu.edu/im/labs/shlab.tar) *[Updated 7/28/03]* ([README](http://csapp.cs.cmu.edu/3e/README-shlab), [Writeup](http://csapp.cs.cmu.edu/3e/shlab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/shlab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/shlab-handout.tar))



写一个支持作业控制的Unix shell程序。需阅读原书（CSAPP）第八章

实验给出了一个参考程序 `tshref`，对于一些输出内容，可以对比。



主要完善 `tsh.c`文件

补充主要函数

`eval` ：读取命令行，`fork`子进程，执行命令等

`builtin_cmd`：内置命令的处理

`do_bgfg`：处理`fg` 和`bg`操作

`waitfg`：等待前台程序结束

`sigchld_handler`：捕捉 `SIGCHILD`信号

`sigint_handler`：捕捉`SIGINT(ctrl-c)`信号

`sigtstp_handler`: 捕捉 `SIGTSTP (ctrl-z)`信号



书上第八章提供了一个有缺陷的[eval](http://csapp.cs.cmu.edu/3e/ics3/code/ecf/shellex.c)函数，我们可以基于这个函数继续完善，当然也可以自己重新写

另外还一些有用（安全可靠）的函数：http://csapp.cs.cmu.edu/3e/ics3/code/src/csapp.c



```c
// eval   70 lines
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/

```







```c
// builtin_cmd  25 lines
/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
```







```c
//do_bgfg 50 lines
/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
```









```c
// waitfg 20 lines
/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
```





```c
// sigchld_handler  80 lines
/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
```





```c
// sigint_handler 15 lines
/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */

```





```c
// sigtstp_handler 15 lines
/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */

```











