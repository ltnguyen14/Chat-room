/* Internet domain TCP server. 
   Usage: ./server <port> 
   Really only useful with client 
   Original code from Linux HowTos.
*/
#include "c-s-socket.h"

int main(int argc, char * argv[]) {
  fd_set master;
  fd_set read_fds;
  int fdmax;
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  char name_buffer[256][50];
  struct sockaddr_in serv_addr, cli_addr;
  int n, i, j, mes_len;

  FD_ZERO( & master);
  FD_ZERO( & read_fds);
  if (argc < 2) {
    fprintf(stderr, "usage %s port\n", argv[0]);
    exit(1);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  bzero((char * ) & serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  listen(sockfd, 5);
  FD_SET(sockfd, & master);
  fdmax = sockfd;

  while (1) {
    read_fds = master;
    char client_buffer[20];

    if (select(fdmax + 1, & read_fds, NULL, NULL, NULL) == -1) {
      perror("Select error!");
      exit(1);
    }
    for (i = 0; i <= fdmax; i++) {
      if (FD_ISSET(i, & read_fds)) {
        if (i == sockfd) {
          clilen = sizeof(cli_addr);
          newsockfd = accept(sockfd, (struct sockaddr * ) & cli_addr, & clilen);
          inet_ntop(AF_INET, & (cli_addr.sin_addr), client_buffer, 20);
          printf("%s connected\n", client_buffer);
          strcpy(name_buffer[newsockfd], client_buffer);
          int k;
          for (k = 0; k <= fdmax; k++) {
                  if (FD_ISSET(k, & master)) 
                    if ((k != sockfd) && (k != i)) {
                      char mess[256];
                      strcpy(mess, "SERVER: ");
                  	  strcat(mess, client_buffer);
                  	  strcat(mess, " connected\n");

                      send(k, mess, strlen(mess), 0);
                    }
                }
          if (newsockfd < 0)
            error("ERROR on accept");
          FD_SET(newsockfd, & master);
          if (newsockfd > fdmax)
            fdmax = newsockfd;
        } else {
          bzero((char * ) & buffer, sizeof(buffer));
          if ((mes_len = recv(i, buffer, sizeof(buffer), 0)) == 0) {
            int k;
            printf("%s hung up\n", client_buffer);
            for (k = 0; k <= fdmax; k++) {
              if (FD_ISSET(k, & master)) {
                if ((k != sockfd) && (k != i)) {
                  send(k, "SERVER: ", 8, 0);
                  send(k, name_buffer[i], strlen(name_buffer[i]), 0);
                  send(k, " hung up\n", 9, 0);
                } else if (k == i)
                  send(k, "SERVER: You are disconnected from the server!\n", 46, 0);
              }
            }
            name_buffer[i][0] = '\0';
            close(i);
            FD_CLR(i, & master);
          } else {
            if (buffer[0] == '/') {
              char request[10];
              sscanf(buffer, "%s", request);
              if (strcmp(request, "/quit") == 0) {
                int k;
                printf("%s disconnected\n", client_buffer);
                for (k = 0; k <= fdmax; k++) {
                  if (FD_ISSET(k, & master)) {
                    if ((k != sockfd) && (k != i)) {
                      char mess[256];
                      strcpy(mess, "SERVER: ");
                  	  strcat(mess, name_buffer[i]);
                  	  strcat(mess, " disconnected\n");
                    }
                    if (k == i)
                      send(k, "SERVER: You are disconnected from the server!\n", 46, 0);
                  }
                }
                name_buffer[i][0] = '\0';
                close(i);
                FD_CLR(i, & master);
              }
              else if (strcmp(request, "/user") == 0) {
                char * user_name = strchr(buffer, ' ');
                if (user_name != NULL) {
                  user_name++;
                  int k = 0;
                  do {
                    k++;
                    if ((user_name[k] == ' ') || (user_name[k] == '\n') || (user_name[k] == '\r')) {
                      user_name[k] = '\0';
                    }
                  } while (user_name[k] != '\0');
                  bzero((char * ) & name_buffer[i], sizeof(name_buffer[i]));
                  strcpy(name_buffer[i], user_name);
                } else {
                  send(i, "SYSTEM: Invalid name\n", 21, 0);
                }
              }
              else if (strcmp(request, "/list") == 0) {
                int k;
                for (k = 0; k <= fdmax; k++) {
                  if (strlen(name_buffer[k]) > 0) {
                    send(i, name_buffer[k], sizeof(name_buffer[k]), 0);
                    send(i, "\n", 1, 0);
                  }
                }
              }
              else if (strcmp(request, "/whoami") == 0) {
                send(i, name_buffer[i], sizeof(name_buffer[i]), 0);
                send(i, "\n", 1, 0);
              }
              else if (strcmp(request, "/help") == 0) {
                send(i, "\t----HELP MENU----\n", 19, 0);
                send(i, "\t/user USERNAME: set your username \n", 36, 0);
                send(i, "\t/list: list users connected to the server\n", 43, 0);
                send(i, "\t/whoami: get your name\n", 24, 0);
              }
              else{
              	send(i,"SERVER: invalid request - type /help to get list of request\n",60,0);
              }
            } else {
              for (j = 0; j <= fdmax; j++) {
                if (FD_ISSET(j, & master)) {
                  if ((j != sockfd) && (j != i)) {
                  	char mess[256];
                  	strcpy(mess, name_buffer[i]);
                  	strcat(mess, ": ");
                  	strcat(mess, buffer);
                    //send(j, name_buffer[i], strlen(name_buffer[i]), 0);
                    //send(j, ": ", 2, 0);
                    send(j, mess, strlen(mess), 0);
                  }
                  else if (j == i){
                  	char mess[256];
                  	strcpy(mess, "You: ");
                  	strcat(mess,buffer);
                  	send(j,mess,strlen(mess),0);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return (0);
}