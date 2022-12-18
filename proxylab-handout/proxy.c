#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void doit(int fd);
int parse_uri(char *uri, char *host, char *port, char *path);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void *thread(void *vargp);
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

    //doit(connfd);                                             //doit
    //Close(connfd);                                            //close
    }


	printf("%s", user_agent_hdr);
    return 0;
}

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

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum,
         char *shortmsg, char *longmsg)
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */




