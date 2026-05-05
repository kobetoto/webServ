/*
       #include <sys/types.h>
       #include <sys/socket.h>
       #include <netdb.h>

       int getaddrinfo(const char *restrict node,
                       const char *restrict service,
                       const struct addrinfo *restrict hints,
                       struct addrinfo **restrict res);

       void freeaddrinfo(struct addrinfo *res);

*/


#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>




int main(){

  /********************************************************************************/

  //////////////////////////////////////////////////////////////////////////////
  //
  // addrinfo -> prepare la structure (protocol, port, le nom...) pour l'utilisation de bind() socket() 
  //

  int             check;
  struct addrinfo hints;
  struct addrinfo *rslt;

  //allocation papy
  memset(&hints, 0, sizeof(hints));

  //remplir la strouctour
  hints.ai_family       = AF_UNSPEC;
  hints.ai_socktype     = 0;
  hints.ai_protocol     = 0;
  hints.ai_flags        = AI_PASSIVE;
  hints.ai_canonname    = NULL;
  hints.ai_addr         = NULL;
  hints.ai_next         = NULL;

  //check call
  //check = getaddrinfo("127.0.0.1", "eerrerasdasdas", &hints, &rslt);
  check = getaddrinfo("127.0.0.1", "80", &hints, &rslt);
  if (check != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(check));
    exit (EXIT_FAILURE);
  }

  fprintf(stdout, "it's seem good (getaddrinfo: %s)\n", gai_strerror(check));

  //////////////////////////////////////////////////////////////////////////////
  //
  // bind + socket
  //
  //            int     socket(int domain, int type, int protocol);
  //            int     bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  //
  //                  The hints argument points to an addrinfo structure that specifies
  //                  criteria for selecting the socket address structures returned in
  //                  the list pointed to by res. 
  

  // variable tmp for check all the linked list
  struct addrinfo *rslt_tmp;
  int              socket_fd;
  for(rslt_tmp = rslt; rslt_tmp != NULL; rslt_tmp = rslt_tmp->ai_next){
    
    socket_fd = socket(rslt_tmp->ai_family, rslt_tmp->ai_socktype, rslt_tmp->ai_protocol);
    if (socket_fd == -1)
      continue; //socket() => ERROR // pass to the next element of the linked List
    
    if ( bind(socket_fd, rslt_tmp->ai_addr, rslt_tmp->ai_addrlen) == 0)
      break; // bind() == 0 => SUCESS // on garde le fd(socket_fd)

    close(socket_fd);
  }

  freeaddrinfo(rslt); // plus besoin pas *rslt juste rslt (he oui jamy cest deja une address)
  
  if (rslt_tmp == NULL){ // bind => ERROR
    fprintf(stderr, "bind() error \n");
    exit (EXIT_FAILURE);
  }

  /********************************************************************************/
  
  //////////////////////////////////////////////////////////////////////////////
  //
  // LOOOOOOOP
  //
  //        ssize_t recvfrom(int sockfd,  void buf[restrict .size], 
  //                                      size_t size,
  //                                      int flags,
  //                                      struct sockaddr *_Nullable restrict src_addr,
  //                                      socklen_t *_Nullable restrict addrlen);
  
  socklen_t                   peer_addr_len;
  struct sockaddr_storage   peer_addr;
  ssize_t                     nread;
  char                        buf[500];
  int                         s;
  for(;;){
    
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    
    peer_addr_len = sizeof(peer_addr);
    

  //       ssize_t recvfrom(int sockfd, void buf[restrict .size], size_t size,
  //                      int flags,
  //                      struct sockaddr *_Nullable restrict src_addr,
  //                      socklen_t *_Nullable restrict addrlen);





    nread = recvfrom(socket_fd, buf, 500, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
    if (nread == -1)
      continue; //ignore fail request
    


    //       int getnameinfo(const struct sockaddr *restrict addr, socklen_t addrlen,
    //                   char host[_Nullable restrict .hostlen],
    //                 socklen_t hostlen,
    //                   char serv[_Nullable restrict .servlen],
    //                   socklen_t servlen,
    //                   int flags);

    s = getnameinfo( (struct sockaddr *) &peer_addr, 
                                      peer_addr_len,
                                      host, 
                                      NI_MAXHOST,
                                      service,
                                      NI_MAXSERV,
                                      NI_NUMERICSERV);

    if (s == 0)
        printf("YEAH!!! receive %zd byte form %s:%s\n", nread, host, service);
    else
      fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
  
    //       ssize_t sendto(int sockfd, const void buf[.size], size_t size, int flags,
    //                  const struct sockaddr *dest_addr, socklen_t addrlen);
    if (sendto(socket_fd, buf, nread, 
               0, (struct sockaddr *) &peer_addr, peer_addr_len ) != nread){
      fprintf(stderr, "Error: sending reponse\n");
    }
  }

  exit (EXIT_SUCCESS);
}
