#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define NAME_LEN 80
#define BUFFER_LEN 256 // podría ser jugador_size

class Jugador: public Serializable
{
public:
    Jugador(const char* _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, NAME_LEN);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t jugador_size = 2 * sizeof(int16_t) + NAME_LEN * sizeof(char);

	alloc_data(jugador_size);

	char* tmp = data();
	memcpy(tmp, name, NAME_LEN * sizeof(char));
	//
	tmp += NAME_LEN * sizeof(char);
	memcpy(tmp, &x, sizeof(int16_t));
	//
	tmp += sizeof(int16_t);
	memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char* data)
    {
	memcpy(name, data, NAME_LEN * sizeof(char));
	//
	data += NAME_LEN * sizeof(char);
	memcpy(&x, data, sizeof(int16_t));
	//
	data += sizeof(int16_t);
	memcpy(&y, data, sizeof(int16_t));

	return 0;
    }


public:
    char name[NAME_LEN];

    int16_t x;
    int16_t y;
};

int main(int argc, char** argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int fd = open("./jugador.bin", O_CREAT | O_TRUNC | O_RDWR, 0666);
    write(fd, one_w.data(), one_w.size()); ////SEND
    close(fd);
    
    // 3. Leer el fichero
    char buffer[BUFFER_LEN];
    fd = open("./jugador.bin", O_RDONLY);
    read(fd, buffer, BUFFER_LEN); ////RECV
    close(fd);
    
    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);
    
    // 5. Mostrar el contenido de one_r
    std::cout << one_r.name << '\t' << one_r.x << '\t' << one_r.y << '\n';

    return 0;
}

/*
...
Observar el contenido del fichero,
usar el comando od con las opciones -sa (od -sa data)

¿qué hace el comando od?
	> Imprime por la salida estándar la "representación no ambigua"
	del fichero especificado. Permite ver el contenido de ficheros
	binarios como 'jugador.bin'.
	
¿qué relación hay entre la salida del comando y la serialización?
	
*/
