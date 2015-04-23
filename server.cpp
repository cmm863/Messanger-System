#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
#include <pthread.h>
#include <cstring>
#include <map>
using namespace std;

#define SERVER_PORT 9339        /* define a server port number */
#define MAX_CLIENT 10

void *task(void *arguments);
void timer_int(int sig);
void *Timer(void *arguments);

struct arg_struct {
  int ns;
};

static map<int, string> name_map;

static int connFd;
static int test_array[MAX_CLIENT];
pthread_mutex_t m;
int main()
{
  int ns, sd, k;
  int FDarray[MAX_CLIENT];   // allocate file descriptors 
  sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
  sockaddr_in client_addr = { AF_INET }; 
  unsigned int client_len = sizeof(client_addr);

  pthread_t threadA[MAX_CLIENT];
  signal(SIGINT, timer_int);
  
  
  for(int i = 0; i < MAX_CLIENT; i++) {
    test_array[i] = -1;
  }

  string test;
  pthread_t  myThread;
  
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
    pthread_mutex_lock(&m);
    test_array[noThread] = connFd;
    pthread_mutex_unlock(&m);
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
  

  if((ns=accept(sd, (sockaddr*)&client_addr, &client_len)) == -1)
  {
    perror("server: accept failed");
    exit(1);
  }
  
  cout << "accept() successful.. a client has connected! waiting for a message" 
       << endl;
       
  close(sd);
  unlink((const char*)&server_addr.sin_addr);
  
  return 0;
}

void *task(void *arguments) {
  struct arg_struct *args = (struct arg_struct *)arguments;
  int k, ns_l = args->ns;
  char buf[512];
  
  while((k=read(ns_l, buf, sizeof(buf))) != 0) {
    cout << ns_l << endl;
    if(buf[0] == '/') {
      if(strncmp(buf, "/part", 5) == 0
        || strncmp(buf, "/quit", 5) == 0
        || strncmp(buf, "/exit", 5) == 0) 
      {
        cout << "Quit recieved" << endl;
        strcpy(buf, name_map[ns_l].c_str());
        strcat(buf, " has quit.");
        for(int i = 0; i < MAX_CLIENT; i++) {
          if(test_array[i] != -1 && test_array[i] != ns_l) {
            write(test_array[i], buf, sizeof(buf));
          }
        }
        break;
      } else if (strncmp(buf, "/nick ", 6) == 0) {
        string name = "";
        for(int i = 6; i < 54; i++) {
          if(buf[i] != '\0'
            || buf[i] != '\n') {
            name += buf[i];
          } else {
            break;
          }
        }
        name_map.insert(std::pair<int, string>(ns_l, name));
        strcpy(buf, name_map[ns_l].c_str());
        strcat(buf, " has joined.");
        for(int i = 0; i < MAX_CLIENT; i++) {
          if(test_array[i] != -1 && test_array[i] != ns_l) {
            write(test_array[i], buf, sizeof(buf));
          }
        }
        continue;
      }
    }
    printf("SERVER RECEIVED: %s\n", buf);
    pthread_mutex_lock(&m);
    for(int i = 0; i < MAX_CLIENT; i++) {
      if(test_array[i] != -1 && test_array[i] != ns_l) {
        char message[512];
        strcpy(message, name_map[ns_l].c_str());
        strcat(message, ": ");
        strcat(message, buf);
        for(int j = strlen(message) - 1; j > 0; j--) {
          if(message[j] == '\n') {
            message[j] = '\0';
          }
        }
        write(test_array[i], message, sizeof(message));
        cout << "Sent to: " << test_array[i] << endl;
      }
    }
    pthread_mutex_unlock(&m);
  }
  close(ns_l);
  cout << "User thread ended." << endl;

  return NULL;
}

void timer_int(int sig)
{
  pthread_t mytimer;
  pthread_create(&mytimer, NULL,  Timer, NULL);
  Timer(NULL);
  return;
}

void *Timer(void *arguments)
{
  char shutdown[]="WARNING: SERVER WILL SHUT DOWN IN TEN SECONDS\0";
  for(int i = 0; i < MAX_CLIENT; i++) {
    if(test_array[i] != -1) {
      write(test_array[i], shutdown, sizeof(shutdown));
    }
  }
  sleep(10);
  for(int i = 0; i < MAX_CLIENT; i++) {
    if(test_array[i] != -1) {
      close(test_array[i]);
    }
  }
                  
  exit(1);
}

    
