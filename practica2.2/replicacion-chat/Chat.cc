#include "Chat.h"

#include <stdio.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;
    memcpy(tmp, &type, sizeof(uint8_t));
    //
    tmp += sizeof(uint8_t);
    memcpy(tmp, &nick, sizeof(char) * 8);
    //
    tmp += sizeof(char) * 8;
    memcpy(tmp, &message, sizeof(char) * 80);
}

int ChatMessage::from_bin(char* bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    memcpy(&type, bobj, sizeof(uint8_t));
    //
    bobj += sizeof(uint8_t);
    memcpy(&nick, bobj, sizeof(char) * 8);
    //
    bobj += sizeof(char) * 8;
    memcpy(&message, bobj, sizeof(char) * 80);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{ //
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        char msg[sizeof(char) * 80];
        std::cin.getline(msg, sizeof(char) * 80);
        /////std::string msg;
        /////std::getline(std::cin, msg);
        
        // configurar mensaje de chat
        ChatMessage em(nick, msg);
    	em.type = ChatMessage::MESSAGE;
        
        // Enviar al servidor usando socket
        socket.send(em, socket);
        
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        // declarar mensaje de chat
    	ChatMessage em;
    
        //Recibir Mensajes de red
        socket.recv(em);
        
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << em.nick << ": " << em.message << '\n';
    }
}

