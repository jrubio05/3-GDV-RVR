/// $ miaddr www.ucm.es http -> 147.96.1.15:80; ...

/*
>./gai www.google.com
66.102.1.147	2	1
66.102.1.147	2	2
66.102.1.147	2	3
2a00:1450:400c:c06::67	10	1
2a00:1450:400c:c06::67	10	2
2a00:1450:400c:c06::67	10	3
*/

#include <iostream>	// cout
#include <sys/types.h>	// getaddrinfo
#include <sys/socket.h>	// getaddrinfo
#include <netdb.h>	// getaddrinfo
#include <string.h>	// memset

int main(int argc, char** argv){
	// criterios
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	// lista de resultados
	struct addrinfo *result;
	
	// obtener resultados
	int luis = getaddrinfo(argv[1], argv[2], &hints, &result);
	// error?
	if (luis != 0) {
		std::cerr << "Error getaddrinfo: " << gai_strerror(luis) << "\n";
		return -1;
	}
	
	// leer resultados
	for (struct addrinfo* i = result; i != nullptr; i = i->ai_next) {
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		// obtener dirección
		int pepe = getnameinfo(
			i->ai_addr, i->ai_addrlen,
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
		std::cout << "HOST: " << host
			<< "\tFAMILIA: " << i->ai_family
			<< "\tTIPO: " << i->ai_socktype
			<< "\n";
	}
	
	// liberar memoria dinámica
	freeaddrinfo(result);

	// éxito
	return 0;
}
