#include "cs537.h"
#include "request.h"
#include <pthread.h>

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too
pthread_cond_t  fill = PTHREAD_COND_INITIALIZER;
pthread_cond_t  empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int count = 0;
int *buffer;

int fillnum = 0;
int usenum = 0;
int numb = 0;

void *child();
int get();
void put(int request);
void getargs(int *port, int argc, char *argv[], int *threads, int *buffer)
{
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }
  *port = atoi(argv[1]);
  //get the number of threads and the number of buffers in the command line 
  *threads = atoi(argv[2]);
  *buffer = atoi(argv[3]);
}

//child thread
void *child(){
  //we check if the buffer is empty
  int temp = -1;
  while(1){
    pthread_mutex_lock(&m);
    while(count == 0){
      pthread_cond_wait(&fill, &m);
    }
    temp = get();
    printf("child get %d\n", temp);
   
    pthread_cond_signal(&empty);
  requestHandle(temp);
    Close(temp);
    pthread_mutex_unlock(&m);
  
  }
}

void put(int request){
  buffer[fillnum] = request;
  fillnum = (fillnum+1)%numb;
  count++;
}

int get(){
  int temp = buffer[usenum];
  usenum = (usenum+1)%numb;
  count--;
  return temp;
}

int main(int argc, char *argv[])
{
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;
  int numt;
  // int numb;
  getargs(&port, argc, argv, &numt, &numb);
  //  printf("%d %d \n", numt, numb);
  // 
  // CS537: Create some threads...
  //
  //create buffer
 buffer = malloc(numb*sizeof(int));
 pthread_t threads[numt]; //= malloc(numt*sizeof(pthread_t));
  int t;
  for (t = 0; t < numt; t++) {
    pthread_create(&threads[t], NULL, child, NULL);
  }

  listenfd = Open_listenfd(port);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    //put in the buffer
    pthread_mutex_lock(&m);
    while(count == numb){
      pthread_cond_wait(&empty, &m);
    }
    // printf("before\n");
    // printf("we try to put %d\n", connfd);
    put(connfd);
    // printf("after\n");
    pthread_cond_signal(&fill);
    pthread_mutex_unlock(&m);
    
    // 
    // CS537: In general, don't handle the request in the main thread.
    // Save the relevant info in a buffer and have one of the worker threads 
    // do the work.
  
    //	requestHandle(connfd);
	
   
  }

}


    


 
