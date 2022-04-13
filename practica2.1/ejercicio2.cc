/// miudp

/*
>./time_server 0.0.0.0 3000
> nc -u 127.0.0.1 3000
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
	hints.ai_socktype = SOCK_DGRAM; // Datagrama (socket tipo UDP)
	// lista de resultados
	struct addrinfo *result;
	
	// obtener resultados
	int luis = getaddrinfo(argv[1], argv[2], &hints, &result);
	// error?
	if (luis != 0) {
		std::cerr << "Error getaddrinfo: " << gai_strerror(luis) << "\n";
		return -1;
	}
	
	// apertura y asociación del socket
	int sd = socket(result->ai_family, result->ai_socktype, 0);
	if (sd == -1) {
		perror(NULL); /// o strerror(errno)
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	int juan = bind(sd, /*(struct sockaddr*)*/ result->ai_addr, result->ai_addrlen);
	// ¡es MUY IMPORTANTE tratar los errores que de bind(...)!
	if (juan == -1) {
		perror(NULL);
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	
	//// aqui ya hay servidor
	//// comprobar con 'ss -uan' y 'ss -uanp'
	
	// recibir del socket y leer resultados
	while (1) {
		char buffer[80];
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		
		// socket cliente y su tamaño
		struct sockaddr_in client;
		socklen_t clientlen = sizeof(struct sockaddr_in);
		
		ssize_t bytes = recvfrom(sd, buffer, 79, 0, (struct sockaddr*) &client, &clientlen);
		// ¡hay que detectar el cierre de la conexion!
		// se evita el error al hacer ctrl+C
		if (bytes == 0) { // cierre ordenado
			freeaddrinfo(result); // liberar memoria dinámica
			return 0; // terminar
		}
		else if (bytes == -1) { // error
			perror(NULL);
			freeaddrinfo(result); // liberar memoria dinámica
			return -1; // terminar
		}
		
		// ¡indicar finalización de cadena!
		// se evita 'la guarrería por pantalla'
		buffer[bytes]='\0';

		int pepe = getnameinfo(
			(struct sockaddr*) &client, clientlen,
			host, NI_MAXHOST,
			serv, NI_MAXSERV,
			NI_NUMERICHOST | NI_NUMERICSERV);
		// error?
		if (pepe != 0) {
			std::cerr << "Error getnameinfo: " << gai_strerror(pepe) << "\n";
			freeaddrinfo(result); // liberar memoria dinámica
			return -1;
		}

		// impresión por pantalla
		std::cout << "HOST: " << host << "\t PORT: " << serv << "\n";
		std::cout << "\tMSG: " << buffer;
		
		// (reenvío del mensaje)
		ssize_t sbytes = sendto(sd, buffer, bytes, 0, (struct sockaddr*) &client, clientlen);
		if (sbytes == -1)
			perror(NULL);
	}
	
	// liberar memoria dinámica
	freeaddrinfo(result);

	// éxito
	return 0;
}
