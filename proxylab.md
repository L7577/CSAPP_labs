# Proxy Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Proxy Lab*](http://csapp.cs.cmu.edu/im/labs/proxylab.tar) ***[Updated 11/14/19]*** ([README](http://csapp.cs.cmu.edu/3e/README-proxylab), [Writeup](http://csapp.cs.cmu.edu/3e/proxylab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/proxylab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/proxylab-handout.tar)) ![[NEW]](proxylab_images/new.gif)





需要阅读原书：10、11、12章节



实验说明：http://csapp.cs.cmu.edu/3e/proxylab.pdf





Part I 实验顺序的web代理

```c
int main(int argc, char** argv)
{
	int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    int *connfdp;
 
    pthread_t tid;
    /* Check command line args */
    if (argc != 2) { 
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //accept
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
    
    connfdp  = (int*)Malloc(sizeof(int));
    *connfdp = connfd;
    Pthread_create(&tid, NULL, thread, connfdp);

    //doit(connfd);   
    //Close(connfd);  
    }


	printf("%s", user_agent_hdr);
    return 0;
}
/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
void doit(int client_fd)
{
    int  serverfd;
    char c_buf[MAXLINE], s_buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char host[MAXLINE], path[MAXLINE], port[MAXLINE];
	rio_t rio_client, rio_server;

    /* Read request line and headers */
    Rio_readinitb(&rio_client, client_fd);
    if (!Rio_readlineb(&rio_client, c_buf, MAXLINE))  //readrequest
        return;
    printf("%s", c_buf);
    sscanf(c_buf, "%s %s %s", method, uri, version);       //parserequest
    if (strcasecmp(method, "GET")) {                      //beginrequesterr
        clienterror(client_fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    }                                                    //endrequesterr


   int tag = parse_uri(uri, host, port, path);
   printf("tag = %d\n", tag);
   if (tag == -1 || tag == 0) {
      return ;
   }

   /* connect server  */
   serverfd = Open_clientfd(host, port);
   Rio_readinitb(&rio_server, serverfd);
   
   sprintf(c_buf, "%s %s %s\n", method, path, version);
   Rio_writen(serverfd, c_buf, strlen(c_buf));
   printf("%s",c_buf);
   size_t sn, cn;
   do {
     cn = Rio_readlineb(&rio_client, c_buf, MAXLINE);
     printf("%s",c_buf);
     Rio_writen(serverfd, c_buf, cn);
  } while(strcmp(c_buf,"\r\n"));
  
 
  while ((sn  = Rio_readlineb(&rio_server, s_buf, MAXLINE)) != 0)
    Rio_writen(client_fd, s_buf, sn);
  Close(serverfd);

}
/* $end doit */

/*
 * parse_uri - parse URI 
 * return 0 if uri : /
 *
 * return 1 if uri : http://localhost:88/filepath 
				   : http://localhost/filepath  (port default is 80)
 *
 * return -1 if uri: https  ftp ,it`s not http protocal.
 *
 */
/* $begin parse_uri */
int parse_uri(char *uri, char *host, char *port,  char *path)
{

    if (uri[0] == '/')
      return 0;
    char *ptr = "http://";
    int ptrlen = strlen(ptr);
    if (strncmp(uri, ptr, ptrlen))
      return -1;

    char *start, *end;
    start = uri + ptrlen;
    end =  start;


    while (*end != ':' && *end != '/') {
      end++;
	}
    strncpy(host, start, end-start);

    if (*end == ':') {
      ++end;
      start = end;
      while (*end != '/')
        end++;
      strncpy(port, start, end-start);
	}
    else{ 
      strncpy(port, "80",2);
	}

    strcpy(path,end);
    return 1;
}
/* $end parse_uri */

```

ParT II 多线程并发处理

```c
/* Thread routine */
void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd);
    Close(connfd);
    return NULL;
}



```


评测

```sh

> ./driver.sh 
*** Basic ***
Starting tiny on 3624
Starting proxy on 20102
1: home.html
   Fetching ./tiny/home.html into ./.proxy using the proxy
   Fetching ./tiny/home.html into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
2: csapp.c
   Fetching ./tiny/csapp.c into ./.proxy using the proxy
   Fetching ./tiny/csapp.c into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
3: tiny.c
   Fetching ./tiny/tiny.c into ./.proxy using the proxy
   Fetching ./tiny/tiny.c into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
4: godzilla.jpg
   Fetching ./tiny/godzilla.jpg into ./.proxy using the proxy
   Fetching ./tiny/godzilla.jpg into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
5: tiny
   Fetching ./tiny/tiny into ./.proxy using the proxy
   Fetching ./tiny/tiny into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
Killing tiny and proxy
basicScore: 40/40

*** Concurrency ***
Starting tiny on port 1886
Starting proxy on port 28373
Starting the blocking NOP server on port 5617
Trying to fetch a file from the blocking nop-server
Fetching ./tiny/home.html into ./.noproxy directly from Tiny
Fetching ./tiny/home.html into ./.proxy using the proxy
Checking whether the proxy fetch succeeded
Success: Was able to fetch tiny/home.html from the proxy.
Killing tiny, proxy, and nop-server
concurrencyScore: 15/15

*** Cache ***
Starting tiny on port 7448
Starting proxy on port 8967
Fetching ./tiny/tiny.c into ./.proxy using the proxy
Fetching ./tiny/home.html into ./.proxy using the proxy
Fetching ./tiny/csapp.c into ./.proxy using the proxy
Killing tiny
Fetching a cached copy of ./tiny/home.html into ./.noproxy
Failure: Was not able to fetch tiny/home.html from the proxy cache.
Killing proxy
cacheScore: 0/15

totalScore: 55/70

```