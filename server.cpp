#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
#include <pthread.h>
using namespace std;

#define SERVER_PORT 8443        /* define a server port number */
#define MAX_CLIENT 10

void *task(void *arguments);

struct arg_struct {
  int ns;
};

static int connFd;
static int test_array[MAX_CLIENT];
int main()
{
  int ns, sd, k;
  int FDarray[MAX_CLIENT];   /* allocate file descriptors 
                                for number of clients*/
  sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
  sockaddr_in client_addr = { AF_INET }; 
  unsigned int client_len = sizeof(client_addr);
  int counter;

  pthread_t threadA[3];
  
  for(int i = 0; i < MAX_CLIENT; i++) {
    test_array[i] = -1;
  }
  
  //Create stream socket
  if((sd=socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("server: socket failed");
    exit(1);
  }
  
  //bind socket to an internet port
  if(bind(sd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("server: bind failed");
    exit(1);
  } 
  
  //listen for clients
  if(listen(sd, 10) == -1)
  {
    perror("server: listen failed");
    exit(1);
  }
  int noThread = 0;
  cout << "SERVER is listening for clients to establish a connection " << endl;

  struct arg_struct args;
  while(noThread < MAX_CLIENT) {
    cout << "Listening" << endl;
    struct arg_struct args;
    connFd = accept(sd, (sockaddr*)&client_addr, &client_len);
    args.ns = connFd;
    test_array[noThread] = connFd;

    if(connFd == -1) {
      cerr << "Cannot accept connection." << endl;
      exit(1);
    }
    else {
      cout << "Connection successful." << endl;
    }
    if(pthread_create(&threadA[noThread], NULL, task, (void *)&args) != 0) {
      cout << "Thread create failed." << endl;
      exit(1);
    }
    noThread++;
  }
  
  cout << "accept() successful.. a client has connected! waiting for a message" 
       << endl;
       
  close(sd);
  unlink((const char*)&server_addr.sin_addr);
  
  return 0;
}

void *task(void *arguments) {
  struct arg_struct *args = (struct arg_struct *)arguments;
  int k;
  char buf[512];
  while((k=read(args->ns, buf, sizeof(buf))) != 0) {
    cout << args->ns << endl;
    printf("SERVER RECEIVED: %s\n", buf);
    for(int i = 0; i < MAX_CLIENT; i++) {
      if(test_array[i] != -1) {
        write(test_array[i], buf, sizeof(buf));
        cout << "Sent to: " << test_array[i] << endl;
      }
    }
  }
  close(args->ns);

  return NULL;
}
