#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


 class clienteInternoAlambrado{
	
public:
	 void error(const char *msg)
	{
    		perror(msg);
    		exit(0);
	}
	 void enviarMensaje(char* buffer, int portno, char* host){
		struct sockaddr_in serv_addr;
    		struct hostent *server;
	
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) 
        		error("ERROR opening socket");
    		server = gethostbyname(host);
    		if (server == NULL) {
        		fprintf(stderr,"ERROR, no such host\n");
        		exit(0);
    		}

		//Conexion servidor
		bzero((char *) &serv_addr, sizeof(serv_addr));
    		serv_addr.sin_family = AF_INET;
    		bcopy((char *)server->h_addr, 
         		(char *)&serv_addr.sin_addr.s_addr,
         		server->h_length);
    		serv_addr.sin_port = htons(portno);
    		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        		error("ERROR connecting");
		


        	int n = write(sockfd,buffer,strlen(buffer));
        	if (n < 0) 
             		error("ERROR writing to socket");
        	//bzero(buffer,256);
		close(sockfd);
    	}
	clienteInternoAlambrado(){
	}
};
