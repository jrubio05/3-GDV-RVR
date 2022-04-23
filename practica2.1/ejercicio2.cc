/// miudp modificado

/*
>./time_server 0.0.0.0 3000
> nc -u 127.0.0.1 3000
*/

#include <iostream>	// cout
#include <sys/types.h>	// getaddrinfo
#include <sys/socket.h>	// getaddrinfo
#include <netdb.h>	// getaddrinfo
#include <string.h>	// memset	strncmp
#include <stdio.h>	// perror
#include <time.h>	// localtime

#define BUFFSIZE 2

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
		close(sd); // cerrar socket
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	
	//// aqui ya hay servidor
	//// comprobar con 'ss -uan' y 'ss -uanp'
	
	// recibir del socket y leer resultados
	while (1) {
		char buffer[BUFFSIZE];
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		
		// socket cliente y su tamaño
		struct sockaddr_in client;
		socklen_t clientlen = sizeof(struct sockaddr_in);
		
		ssize_t bytes = recvfrom(sd, buffer, BUFFSIZE - 1, 0, (struct sockaddr*) &client, &clientlen);
		// ¡hay que detectar el cierre de la conexion!
		// se evita el error al hacer ctrl+C
		if (bytes == 0) { // cierre ordenado
			close(sd); // cerrar socket
			freeaddrinfo(result); // liberar memoria dinámica
			return 0; // terminar
		}
		else if (bytes == -1) { // error
			perror(NULL);
			close(sd); // cerrar socket
			freeaddrinfo(result); // liberar memoria dinámica
			return -1; // terminar
		}
		
		// ¡indicar finalización de cadena!
		// se evita 'la guarrería por pantalla'
		buffer[BUFFSIZE - 1]='\0';

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
		
		// estructuras para almacenar el tiempo
		// (se podría hacer esto ya dentro del 'if' de cada comando)
		/// https://www.tutorialspoint.com/c_standard_library/c_function_localtime.htm ///
		time_t timep;
		time(&timep);
		if (timep == ((time_t) -1)) {
			perror(NULL);
			close(sd); // cerrar socket
			freeaddrinfo(result); // liberar memoria dinámica
			return -1;
		}
		struct tm* myTime = localtime(&timep);
		if (myTime == NULL) {
			perror(NULL);
			close(sd); // cerrar socket
			freeaddrinfo(result); // liberar memoria dinámica
			return -1;
		}

		// impresión por pantalla y envío de la respuesta
		std::cout << bytes << " bytes de " << host << ":" << serv << "\n";
		if (strncmp(buffer, "t", BUFFSIZE) == 0) {
			std::cout << "Comando hora\n";
			char sendBuf[12];
			strftime(sendBuf, 12, "%I:%M:%S %p", myTime); /// %n para línea aparte
			// ^^^ supongo que no da error ^^^
			ssize_t sbytes = sendto(sd, sendBuf, 12, 0, (struct sockaddr*) &client, clientlen);
			if (sbytes == -1)
				perror(NULL);
		}
		else if (strncmp(buffer, "d", BUFFSIZE) == 0) {
			std::cout << "Comando fecha\n";
			char sendBuf[11];
			strftime(sendBuf, 11, "%Y-%m-%d", myTime); /// %n para línea aparte
			// ^^^ supongo que no da error ^^^
			ssize_t sbytes = sendto(sd, sendBuf, 11, 0, (struct sockaddr*) &client, clientlen);
			if (sbytes == -1)
				perror(NULL);
		}
		else if (strncmp(buffer, "q", BUFFSIZE) == 0) {
			std::cout << "Comando cierre\n";
			// salir:
			std::cout << "Saliendo...\n";
			close(sd); // cerrar socket
			freeaddrinfo(result); // liberar memoria dinámica
			return 0; // terminar
		}
		else {
			std::cout << "Comando desconocido\n";
		}
	}
	
	// cerrar socket
	close(sd);
	
	// liberar memoria dinámica
	freeaddrinfo(result);

	// éxito
	return 0;
}
