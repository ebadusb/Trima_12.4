#include <stdlib.h>
#include <errnoLib.h>
#include <time.h>

#include "sockinet.h"


void tcpClient ()
{
   cout << "testClient started" << endl;

   sockinetbuf si(sockbuf::sock_stream);

   //si.bind();
   //cout << si.localhost() << " " << si.localport() << endl;

   int status = si.connect("90.0.0.1", 2222);

   if (status != 0)
   {
      cout << "Connect failed with status " << status << " errno " << errnoGet() << endl;
      exit (-1);
   }
   
   char msg[50];
   int i = 0;
   const timespec sleepTime = {1, 0};

   while (1)
   {
      sprintf(msg, "Message %d", i++);
      int bytes_sent = si.send(msg, strlen(msg) + 1, 0);
      cout << "Sent message " << msg << "(" << bytes_sent << ")" << endl;

      nanosleep(&sleepTime, 0);
   }

}
