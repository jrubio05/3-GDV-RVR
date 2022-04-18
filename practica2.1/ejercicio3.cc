/// cliente UDP

/*
>./time_client 127.0.0.1 3000 t
*/

#include <iostream>	// cout
#include <sys/types.h>	// getaddrinfo
#include <sys/socket.h>	// getaddrinfo
#include <netdb.h>	// getaddrinfo
#include <string.h>	// memset
#include <stdio.h>	// perror
#include <time.h>	// localtime

#define BUFFSIZE 128

int main(int argc, char** argv){
	if (argc != 4) { // formato malo
		printf("ERROR_FORMATO\n");
		return -1;
	}
	
	// criterios
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; // Direcciones tipo IPv4
	hints.ai_socktype = SOCK_DGRAM; // Datagrama (socket tipo UDP)
	// lista de resultados
	struct addrinfo* result;
	
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
		perror("ERROR_EN_SOCKET"); /// o strerror(errno)
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	
	////BIND////
	
	//// aqui ya hay servidor
	//// comprobar con 'ss -uan' y 'ss -uanp'
	
	// enviar por el socket
	ssize_t bytesSent = sendto(sd, argv[3], 1, 0, result->ai_addr, result->ai_addrlen);
	if (bytesSent == -1) {
		perror("ERROR_EN_SENDTO");
		freeaddrinfo(result); // liberar memoria dinámica
		return -1; // terminar
	}
	
	// recibir respuesta (si el servidor va a generarla) y mostrarla
	if (strncmp(argv[3], "t", 1) == 0 || strncmp(argv[3], "d", 1) == 0) {
		char buffer[BUFFSIZE];
		ssize_t bytesReceived = recvfrom(sd, buffer, BUFFSIZE - 1, 0, result->ai_addr, &result->ai_addrlen);
		if (bytesReceived == -1) {
			perror("ERROR_EN_RECVFROM");
			freeaddrinfo(result); // liberar memoria dinámica
			return -1; // terminar
		}
		// ¡indicar finalización de cadena!
		buffer[BUFFSIZE - 1]='\0';
		
		// impresión por pantalla
		std::cout << buffer << "\n";
	}
	
	// liberar memoria dinámica
	freeaddrinfo(result);

	// éxito
	return 0;
}
