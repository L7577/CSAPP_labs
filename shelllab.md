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

在eval函数中，首先要判断读入的命令是不是内置命令，

一些注意事项

在fork子进程之前需要阻塞信号，fork之后，execve命令之前则要恢复。

```c
/* block SIGCHLD SIGINT SIGTSTP ,and save current signal mask */  
if(sigprocmask(SIG_BLOCK,&mask_one,&prev_one) < 0)
    unix_error("SIG_BLOCK error"); 

if ((pid = fork()) == 0) {   /* Child runs user job */
    /* reset signal mask which  unblock signal */
    if (sigprocmask(SIG_SETMASK,&prev_one,NULL) < 0)
        unix_error("SIG_SETMASK error");  

    if(setpgid(0,0) < 0){
        unix_error("setpgid error");
    } 

    if (execve(argv[0], argv, environ) < 0) {
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
    }
}
```



同样在addjob 之前需要阻塞信号，添加完成以后恢复。若是前台作业，还需要在添加完之后，等待作业完成。





```c
// builtin_cmd  25 lines
/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
```

builtin_cmd则需要判断并处理 `quit` `jobs` `bg` `fg` 这四个内置命令

可以使用`strcmp`



```c
//do_bgfg 50 lines
/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
```



do_bgfg就是分析输入的内置命令，是否正确，如果正确则正常执行

注意如果是前台作业，需要等待任务执行结束。





```c
// waitfg 20 lines
/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
```

waitfg 主要用来等待前台任务执行结束，

需要明白一个进程执行结束时，kernel会发出一个SIGCHLD信号，如何保证前台只有一个任务进行，并且不会影响后台进程运行。值得思考

实验报告中，推荐使用简单粗暴的忙等待方式，也就是sleep()，还能如何改进呢

使用`sigsuspend`





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

sigchld_handler 在收到SIGCHID信号时进行工作，主要负责删除执行完成以及被终止的作业，

需要注意在delete_job时，也要阻塞各种信号。保证命令不可中断。



```c
// sigint_handler 15 lines
/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */

```

sigint_handler 函数在捕捉到SIGINT信号时，发送SIGINT信号到当前前台任务，使其终止。



```c
// sigtstp_handler 15 lines
/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */

```

sigtstp_handler 函数在捕捉到SIGTSTP信号时，发送给当前前台任务，使其中止，然后挂起该任务。









