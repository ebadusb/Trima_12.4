/* Copywrite 1998 Cobe BCT, Inc.
 *
 * FILENAME: UdpSocket.hpp
 * PURPOSE: header file for UdpSocket class
 * NOTES:
 *   This implementation is by default for a Win32 platform, and uses the WinSock DLL
 *   and wsock32.lib.  To build for unix machine, simply #define BUILD_FOR_UNIX
 * CHANGELOG:
 *   12/18/98 - dyes - initial version
 *   01/24/2000 - dyes - add more general constructor
 *                UdpSocket(source_ip, source_port, dest_ip, dest_port)
 *   01/27/2000 - dyes - add functions setBroadcastMode, getPort, getOtherPort,
 *                       getFirstIP, getIP, getOtherIP, poll, print,
 *                       getSentCount, getReceivedCount
 *   03/30/2000 - dyes - add name to the socket
 *   04/30/2002 - dyes - support vxworks
 */

#ifndef __UDPSOCKET_HPP__ //  prevents multiple inclusion
#define __UDPSOCKET_HPP__

#include "logger.hpp" // gets platform env there

#ifndef BUILD_FOR_NT // i.e. BUILD_FOR_UNIX or BUILD_FOR_VXWORKS 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // for sockaddr_in
#include <unistd.h>
#define SOCKET_ERROR   -1
#define INVALID_SOCKET -1
typedef int SOCKET;
#define closesocket(s) (close(s))
#endif // BUILD_FOR_UNIX



// CLASSNAME: UdpSocket
// RESPONSIBILITIES:
//   1. Does udp socket setup
//   2. Provides send and receive functions
// COLLABORATIONS:
//   1. created and used by whoever needs udp socket services for
//      transmission/reception of udp packets.
class UdpSocket
{
    public:

        UdpSocket(unsigned short server_port, const char* name="unknown"); // setup a server socket
		UdpSocket(const char* server_ip, unsigned short server_port,
                  const char* name="unknown"); // used to setup a client socket
		UdpSocket(const char* source_ip, unsigned short source_port,
                  const char* dest_ip, unsigned short dest_port,
                  const char* name="unknown"); 

        virtual ~UdpSocket();
		virtual void send(const char* buf, int bytes_to_send);
		virtual int receive(char* buf, int size); // blocks until data is received
        virtual void print(); // prints socket info

        void setBroadcastMode(); // user must have permission to do setsockopt, exits on failure
        bool poll(int timeout=0); // returns true if data ready to receive, blocks for up to timeout seconds

        int getPort();
        int getOtherPort();
        int getSentCount() { return _sent_count; };
        int getReceivedCount() { return _received_count; };

        // NOTE: for getIP calls, return string must be copied or converted as
        // needed, memory is owned by socket layer and may change with next socket call.
        const char* getIP(char* buf=0);
        const char* getOtherIP(char* buf=0);
        const char* getFirstIP(char* buf=0); // 1st IP configured for this host
        void logSends(const char* filename); // logs sent msgs to filename (binary)

    protected:

        const char* _name;
		static int _Ref_count; // used for winsock init and cleanup for Win32 apps
		struct sockaddr_in _our_addr; // our IP and port
		struct sockaddr_in _other_addr; // other side's IP and port
		SOCKET _sockfd;
        int _sent_count;
        int _received_count;
        int _logsends_fd;
        const char* _logsends_filename;

		void init(const char* local_ip, unsigned short local_port,
                  const char* dest_ip, unsigned short dest_port);

#ifdef BUILD_FOR_NT
		void winsock_startup();
#endif

    private:

        UdpSocket(); // catch unauthorized use
        UdpSocket(const UdpSocket&); // catch unauthorized use
        UdpSocket& operator=(const UdpSocket& rhs); // catch unauthorized use
};


#endif // __UDPSOCKET_HPP__

