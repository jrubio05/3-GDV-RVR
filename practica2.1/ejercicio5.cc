/// cliente TCP

/*
>./echo_client 127.0.0.1 2222
*/

#include <iostream>	// cout
#include <sys/types.h>	// getaddrinfo
#include <sys/socket.h>	// getaddrinfo
#include <netdb.h>	// getaddrinfo
#include <string.h>	// memset
#include <stdio.h>	// perror
#include <unistd.h>	// sleep

int main(int argc, char** argv){
	// criterios
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
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
	
	// apertura y conexión
	int sd = socket(result->ai_family, result->ai_socktype, 0);
	if (sd == -1) {
		perror("ERROR_EN_SOCKET"); /// o strerror(errno)
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	int juan = connect(sd, result->ai_addr, result->ai_addrlen);
	if (juan == -1) {
		perror("ERROR_EN_CONNECT");
		freeaddrinfo(result); // liberar memoria dinámica
		return -1;
	}
	
	// comunicación
	int salir = 0;
	while (!salir) {
		///////////////////////////char bufferUsuario[128];
		char bufferRecepcion[128];
		
		/*////////////////////////////////////
		// socket servidor y su tamaño
		struct sockaddr_in client;
		socklen_t clientlen = sizeof(struct sockaddr_in);
		*/
		
		char bufferUsuario[] = "HelloThere\n";//std::cin >> bufferUsuario;
		
		//int scn = sscanf(bufferUsuario, "q");
		
		// envío del mensaje
		ssize_t sbytes = send(sd, bufferUsuario, 11, 0);
		if (sbytes == -1) {
			perror(NULL);
			printf("\n__-POLLAS~1\n");///////////////////////////////////
			salir = 1;///////////////////////////////////////////////////
			return -1;
		}
		
		// recepción de la réplica
		ssize_t c = 1;
		int rbytes = recv(sd, bufferRecepcion, 11, 0);
		if (rbytes == -1) {
			perror(NULL);
			printf("\n__-POLLAS~2\n");//////////////////////////////////
			salir = 1;//////////////////////////////////////////////////
			return -1;
		}
		// ¡indicar finalización de cadena!
		bufferRecepcion[rbytes]='\0';
		
		// impresión por pantalla
		std::cout << bufferRecepcion;
		
		sleep(1);
	}
	
	// liberar memoria dinámica
	freeaddrinfo(result);

	// éxito
	return 0;
}

