/// mitcp --- ahora se trata una conexión en lugar de mensajes

/*
>./echo_server 0.0.0.0 2222
> nc 127.0.0.1 2222
*/

/*
las 4 secciones que SIEMPRE van a estar:
	* INICIALIZAR SOCKET Y ADDR
	* PONER EL SOCKET EN LISTEN (ACEPTAR CONEXIONES)
	* ACEPTAR UNA NUEVA CONEXIÓN (PRIMER WHILE)
	* TRATAR LA CONEXIÓN (SEGUNDO WHILE)
*/

#include <iostream>	// cout
#include <sys/types.h>	// getaddrinfo
#include <sys/socket.h>	// getaddrinfo
#include <netdb.h>	// getaddrinfo
#include <string.h>	// memset
#include <stdio.h>	// perror

int main(int argc, char** argv){
	// criterios
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE; // Se puede escuchar en 0.0.0.0
	hints.ai_family = AF_INET; // Direcciones tipo IPv4
	hints.ai_socktype = SOCK_STREAM; // Flujo (socket tipo TCP)
	// lista de resultados
	struct addrinfo *result;
	
	// obtener resultados
	int luis = getaddrinfo(argv[1], argv[2], &hints, &result);
	// error?
	if (luis != 0) {
		std::cerr << "Error getaddrinfo: " << gai_strerror(luis) << "\n";
		return -1;
	}
	
	// apertura, asociación y escucha del socket
	int sd = socket(result->ai_family, result->ai_socktype, 0);
	if (sd == -1) {
		perror(NULL); /// o strerror(errno)
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	int juan = bind(sd, result->ai_addr, result->ai_addrlen);
	// ¡es MUY IMPORTANTE tratar los errores que de bind(...)!
	if (juan == -1) {
		perror(NULL);
		close(sd); // cerrar socket
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	//// ejecuto listen(...), luego aparece en el comando 'ss ...'
	int paco = listen(sd, 16); // backlog (ha de ser 2^n) de 16, ya que es el mínimo para el kernel linux
	if (paco == -1) {
		perror(NULL);
		close(sd); // cerrar socket
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	
	// recibir del socket y leer resultados
	while (1) {
		char buffer[128];
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		
		// socket cliente y su tamaño
		struct sockaddr_in client;
		socklen_t clientlen = sizeof(struct sockaddr_in);
		
		// aceptar conexión
		int client_sd = accept(sd, (struct sockaddr*) &client, &clientlen); // se emplean las tres cosas que representan al cliente (en UDP no hay identificación del cliente)
		if (client_sd == -1) {
			perror(NULL);
			close(sd); // cerrar socket
			freeaddrinfo(result); // liberar memoria dinámica
			return -1;
		}
		
		// ...
		int pepe = getnameinfo(
			(struct sockaddr*) &client, clientlen,
			host, NI_MAXHOST,
			serv, NI_MAXSERV,
			NI_NUMERICHOST | NI_NUMERICSERV);
		// error?
		if (pepe != 0) {
			std::cerr << "Error getnameinfo: " << gai_strerror(pepe) << "\n";
			close(sd); // cerrar socket
			freeaddrinfo(result); // liberar memoria dinámica
			return -1;
		}
		// impresión por pantalla
		printf("Conexión desde %s:%s\n",host, serv);
		
		// tratar conexión
		int conn = 1;
		while (conn) {
			ssize_t c = 1;
			int bytes = 0;
			int finCadena = 0;
			// se reciben hasta 127 bytes y luego se termina el búffer con \n
			while (c > 0 && bytes < 127 && !finCadena) {
				c = recv(client_sd, &(buffer[bytes]), 1, 0); // ¡se usa client_sd, no del propio descriptor sd!
				if (buffer[bytes] == '\n')
					finCadena = 1;
				bytes++;
			}
			if (c < 0) { // error
				perror(NULL);
				conn = 0;
				continue;
			}
			else if (c == 0) { // cierre ordenado
				conn = 0;
				continue;
			}
		
			// ¡indicar finalización de cadena!
			buffer[bytes]='\0';
		
			// reenvío del mensaje
			ssize_t sbytes = send(client_sd, buffer, bytes, 0); // ¡se usa client_sd, no del propio descriptor sd!
			if (sbytes == -1) {
				perror(NULL);
				conn = 0;
			}
		}
		// impresión por pantalla
		printf("Conexión %s:%s terminada\n",host, serv);
	}
	
	// cerrar socket
	close(sd);
	
	// liberar memoria dinámica
	freeaddrinfo(result);

	// éxito
	return 0;
}

