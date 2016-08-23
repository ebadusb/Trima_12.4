#include "sockinet.h"
#include <errnoLib.h>

void tcpServer ()
{
   cout << "testServer started" << endl;

   sockinetbuf serverSocket(sockbuf::sock_stream);

   serverSocket.bind(INADDR_ANY, 2222);

   cout << "targetServer address: " << serverSocket.localhost() << " " 
        << serverSocket.localport() << endl;

   serverSocket.listen();

   sockinetaddr clientAddress;
   
   sockbuf clientSocket = serverSocket.accept(clientAddress);

   iosockstream clientConnection(clientSocket);

   cout << "Accepted connection from <" << clientAddress.gethostname() << "> port " 
        << clientAddress.getport() << " errno "<< errnoGet() << endl;

   char buf[1024];
   int byte_count = 1; 

   while (byte_count > 0)
   {
      int byte_count = clientSocket.recv(buf, sizeof(buf), 0);
      
      if (byte_count > 0)
         cout << "rx msg(" << byte_count << "): " << buf << endl;
      else
         cout << "Error:  rx msg.  byte_count " << byte_count << ", errno " << errnoGet() << endl;
   }

/*
   while (s >> buf)
   {
      cout << buf << ' ' ;
      s << ::strlen(buf) << endl;
   }

   cout << endl;
*/

   cout << "shutting down" << endl;

   serverSocket.shutdown(sockbuf::shut_readwrite);
}
