/* Internet domain TCP client. 
   Usage: ./client <hostname> <port> 
   Really only useful with server
   Original code from Linux HowTos.
*/
#include "c-s-socket.h"

WINDOW *top;
WINDOW *bottom;
int done=0;
int maxx, maxy;
int quit = 0;

int output_line = 1;
int input_line = 1; 

void * receive(void * sockfd) {
  int n;
  char recv_buffer[256];
  int sock_fd = * ((int * ) sockfd);

  while (1) {
	bzero(recv_buffer, 256);
    n = recv(sock_fd, recv_buffer, sizeof(recv_buffer), 0);
    if (n < 0)
      error("ERROR reading from socket");
    mvwprintw(top,output_line,3,recv_buffer);
    
    if (output_line != maxy/2-2)
    	output_line ++;
    else{ 
    	scroll(top);}
    wrefresh(top);
    wrefresh(bottom);
  }
  pthread_exit(NULL);
}

void * sending(void * sockfd) {
  int n;
  char send_buffer[256];
  int sock_fd = * ((int * ) sockfd);
  
  while (1) {
    bzero(send_buffer, 256);
    wrefresh(top);
    wrefresh(bottom);
    
    mvwgetstr(bottom,input_line,2,send_buffer);
    n = send(sock_fd, send_buffer, 255, 0);
    if (n < 0)
      error("ERROR writing to socket");
    if (strcmp(send_buffer, "/quit") == 0) {
    	done = 1;
    	endwin();
    	pthread_exit(NULL);
    	break;
    }
    if (input_line != maxy/2 -2)
    	input_line ++;
    else{
    	scroll(bottom);}

  }
  pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent * server;
  pthread_t threads[2];

  if (argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(1);
  }

  portno = atoi(argv[2]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  server = gethostbyname(argv[1]);
  if (server == NULL)
    error("ERROR no such host");

  bzero((char * ) & serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char * ) server -> h_addr, (char * ) & serv_addr.sin_addr.s_addr, server -> h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");
  
  
  initscr();
  getmaxyx(stdscr,maxy,maxx);
  
  top = newwin(maxy/2,maxx,0,0);
  bottom = newwin(maxy/2,maxx,maxy/2,0);
  
  scrollok(top,TRUE);
  scrollok(bottom,TRUE);
  
  box(top,'|','-');
  box(bottom,'|','=');
  
  wsetscrreg(top,1,maxy/2-2);
  wsetscrreg(bottom,1,maxy/2-2);
  
  pthread_create( & threads[0], NULL, receive, & sockfd);
  pthread_create( & threads[1], NULL, sending, & sockfd);
	while (!done);
  close(sockfd);

  return (0);
}