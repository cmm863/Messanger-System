#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
using namespace std;

#define SERVER_PORT 8333        /* define a server port number */
#define MAX_CLIENT 10

void *handle_client( void * );

int main()
{
  int ns, sd, k;
  int FDarray[MAX_CLIENT];   // allocate file descriptors 
  pthread_t tid[MAX_CLIENT]   // for number of clients
  sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
  sockaddr_in client_addr = { AF_INET }; 
  unsigned int client_len = sizeof(client_addr);
  int counter = 0;
  string test;
  pthread_t  myThread;
  //mutex m;
  char buf[512];
  
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
  
  cout << "SERVER is listening for clients to establish a connection " << endl;
  
  
  /*while((ns=accept(sd, (sockaddr*)&client_addr, &client_len)) > 0)
  {
    FDarray[counter] = ns;
    pthread_create(&tid[counter], NULL, handle_client,  ) 
  }*/
  
  
  if((ns=accept(sd, (sockaddr*)&client_addr, &client_len)) == -1)
  {
    perror("server: accept failed");
    exit(1);
  }
  
  cout << "accept() successful.. a client has connected! waiting for a message" 
       << endl;
       
  
  while((k=read(ns,  buf, sizeof(buf))) != 0)
  {
    printf("SERVER RECEIVED: %s\n", buf);
    write(ns, buf , k);
  }  
  close(ns);
  close(sd);
  unlink((const char*)&server_addr.sin_addr);
  
  return 0;
}

void *handle_client( void * )
{


}
