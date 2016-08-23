#include "sockinet.h"
#include <errnoLib.h>

void udpListen ()
{
   cout << "udpListen started" << endl;

   sockinetbuf listenSocket(sockbuf::sock_dgram);

   listenSocket.bind(INADDR_ANY, 2222);

   cout << "targetServer address: " << listenSocket.localhost() << " " 
        << listenSocket.localport() << endl;


   char buf[1024];
   int byte_count = 1; 

   while (byte_count > 0)
   {
      int byte_count = listenSocket.recv(buf, sizeof(buf), 0);
      
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

   listenSocket.shutdown(sockbuf::shut_readwrite);
}
