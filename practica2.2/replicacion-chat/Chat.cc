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
    memcpy(tmp, /*&*/nick.c_str(), sizeof(char) * 8);
    //
    tmp += sizeof(char) * 8;
    memcpy(tmp, /*&*/message.c_str(), sizeof(char) * 80);
}

int ChatMessage::from_bin(char* bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    memcpy(&type, bobj, sizeof(uint8_t));
    //
    bobj += sizeof(uint8_t);
    char tNick[sizeof(char) * 8];
    memcpy(&tNick, bobj, sizeof(char) * 8);
    nick = tNick;
    //
    bobj += sizeof(char) * 8;
    char tMessage[sizeof(char) * 80];
    memcpy(&tMessage, bobj, sizeof(char) * 80);
    message = tMessage;

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
         
     	//////////////std::cout << "AMOGOS\n";
         
        // declarar mensaje de chat y socket
    	ChatMessage* em = new ChatMessage();
        Socket* soc;
        
        //Recibir Mensajes de red
        if (socket.recv(*em, soc) == -1) {
        	std::cout << "_error_recv_servidor_\n";
        	continue;
        }
        
        if (em->type == ChatMessage::LOGIN) {
        	auto it = clients.begin();
        	while(it != clients.end()) {
            	if(*it->get() == *soc) ////////////////////
                	break;
                it++;
            }

            if (it == clients.end()) {
            	// crear unique_ptr
        		std::unique_ptr<Socket> socUP = std::unique_ptr<Socket>(new Socket(*soc));
        		clients.push_back(std::move(socUP));
            }
            else {
            	std::cout << "_imposible_el_doble_inicio_de_sesion_\n";
            }
            
            std::cout << em->nick << " ha entrado\n";///////////////////
        }
        else if (em->type == ChatMessage::MESSAGE) {
        	for (auto &c : clients) {
                if (*c.get() == *soc) /////////////////
                    continue;
                socket.send(*em, *c.get());
            }
        }
        else if (em->type == ChatMessage::LOGOUT) {
        	auto it = clients.begin();
            while(it != clients.end()) {
            	if(*it->get() == *soc) ////////////////////
                    break;
                it++;
            }
            
            if(it != clients.end())
            	clients.erase(it);
                
            std::cout << em->nick << " ha salido\n";///////////////////
        }
        else {
        	std::cout << "_mensaje_invalido_\n";
        }
        
        delete em;
        delete soc;

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
	// configurar mensaje de chat
	ChatMessage em = ChatMessage(nick, ""); //////////////////////
	em.type = ChatMessage::MESSAGE;
	
    while (true)
    {
        // Leer stdin con std::getline
        char msg[sizeof(char) * 80];
        std::cin.getline(msg, sizeof(char) * 80);
        /////std::string msg;
        /////std::getline(std::cin, msg);
        
        // configurar mensaje de chat
        em.message = msg;
        
        // Enviar al servidor usando socket
        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    // declarar mensaje de chat
    ChatMessage em = ChatMessage(nick, ""); //////////////ChatMessage em;////////////
    
    while(true)
    {
        //Recibir Mensajes de red
        socket.recv(em);
        
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << em.nick << ": " << em.message << '\n';
    }
}
