#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
using namespace std;
#define SERVER_PORT 8243     /* define a server port number */

void sig_hand(int sig);
void *reader(void *);
struct clients
{
  int sd;
  string name;
};

int main(int argc, char* argv[]) {
  int sd;
  struct sockaddr_in server_addr = { AF_INET, htons(SERVER_PORT)};
  string test;
  char buf[512];
  struct hostent *hp;
  string hostname, nickname;
  pthread_t myclient;
  signal(SIGINT, sig_hand);
  clients client; 
  
  cout << "Enter hostname: ";
  cin >> hostname;
  
  // Get the host
  if((hp = gethostbyname(hostname.c_str())) == NULL) {
    cout << argv[0] << ": " << hostname << " unknown host" << endl;
    exit(1);
  }
  cout << "Enter username: ";
  cin.ignore();
  getline(cin,  client.name);
  

  bcopy(hp->h_addr_list[0], (char*)&server_addr.sin_addr, hp->h_length);

  // Create a socket
  if((client.sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    cout << "Client: Socket failed." << endl;
    exit(1);
  }

  if(connect(client.sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    cout << "Client: Connect failed." << endl;
    exit(1);
  }

  strcpy(buf, "/nick ");
  for(int i=6; i<client.name.length()+7; i++)
  {
    buf[i]=client.name[i-6];
  }
  write(client.sd, buf, sizeof(buf));
  read(client.sd, buf, sizeof(buf));
  printf("%s\n", buf);  
  
  pthread_create(&myclient, NULL, reader, &client);
  
  while(getline(cin, test)!=NULL)
  {
    for(int i=0; i<test.length()+1; i++)
    {
      buf[i]=test[i];
    }
    write(client.sd, buf , sizeof(buf));
    if(test == "/exit"
      || test == "/quit"
      || test == "/part") {
      break;
    }
  }
  
  close(client.sd);

  return 0;
}

void sig_hand(int sig)
{
  cout << "\nIf you wish to exit, please type /exit /quit/ or /part" << endl;
  return;
}

void *reader(void *passed_client)
{
  clients *client = (clients *)passed_client;
  char buf[512];
  
  while(read(client->sd, buf , sizeof(buf)) > 0)
  {
    printf("%s\n", buf);
  }
  pthread_exit(passed_client);
}
