/* Copyright 1998 Cobe BCT, Inc.
 *
 * FILENAME: UdpSocket.cpp
 * PURPOSE: UdpSocket class implemenation
 * CHANGELOG:
 *   12/18/98 - dyes - initial version
 *   01/27/2000 - dyes - see .hpp for new functions added
 *   04/30/2002 - dyes - port for vxworks
 */

#include <stdlib.h>
#include <fcntl.h>

#include "udpsocket.hpp"  // must include before Logger.hpp to avoid redef errors
#include "logger.hpp"


#ifdef BUILD_FOR_UNIX 
#include <netdb.h> // for h_errno
#include <string.h> // for memset
#include <arpa/inet.h> // for inet_addr
#endif // BUILD_FOR_UNIX

#ifdef BUILD_FOR_VXWORKS
#include <sockLib.h>
#include <hostLib.h>
#include <inetLib.h>
#define h_errno (errno) // vxworks does not support h_errno
#endif // BUILD_FOR_VXWORKS


// initialize statics
int UdpSocket::_Ref_count = 0;


// FUNCTION: UdpSocket
// PURPOSE:  this constructor sets up a server that listens on specified port
UdpSocket::UdpSocket(unsigned short port, const char* name)
 : _name(name), _logsends_fd(-1), _logsends_filename(0)
{
	init(0, port, 0, 0);
}


// FUNCTION: UdpSocket
// PURPOSE:  this constructor sets up a udp client that initiates communications
// with a udp server at the specified IP and port
UdpSocket::UdpSocket(const char* dest_ip, unsigned short dest_port,
                     const char* name)
 : _name(name), _logsends_fd(-1), _logsends_filename(0)
{
	init(0, 0, dest_ip, dest_port);
}


// FUNCTION: UdpSocket
// PURPOSE:  this constructor sets up a more general purpose socket that can be used
// as client or server.  
UdpSocket::UdpSocket(const char* source_ip, unsigned short source_port,
                    const char* dest_ip, unsigned short dest_port,
                    const char* name)
 : _name(name), _logsends_fd(-1), _logsends_filename(0)
{
	init(source_ip, source_port, dest_ip, dest_port);
}



UdpSocket::~UdpSocket()
{
	_Ref_count--;
	closesocket(_sockfd);
    LOG_DEBUG("~UdpSocket refcount=%d closed fd=%d", _Ref_count, _sockfd);

	if(_Ref_count == 0)
	{
#ifdef BUILD_FOR_NT
		WSACleanup();
#endif
	}

	ASSERT(_Ref_count >= 0);
    _name = 0;
}


// FUNCTION: send
// PURPOSE: sends a datagram to the other end.  
// NOTE: send returns as soon as the buffer has been copied into
// the local udp stack.  udp does not guarantee data integrity or
// reliable delivery.  If a buffer fills or the other end is simply
// not listening, data will be dropped or truncated.  No data integrity
// checking is done for you.  It's up to you to decide whether integrity 
// or delivery is at risk and to implement whatever detection or 
// retransmission mechanism you find necessary.
void UdpSocket::send(const char* buf, int bytes_to_send)
{
    ASSERT(buf != 0);
    ASSERT(bytes_to_send > 0);
    if(Logger::Is_debug())
    {
        LOG_DEBUG("send: name=%s fd=%d packet=%d size=%d, contents:",
            _name, _sockfd, _sent_count, bytes_to_send);
        LOG_DUMP(buf, bytes_to_send);
    }

	int n = sendto(_sockfd, (char*)buf, bytes_to_send, 0,
		(struct sockaddr*)&_other_addr, sizeof(_other_addr));

	if (n == SOCKET_ERROR)
	{
		LOG_FATAL("sendto failed, h_errno=%d", h_errno);
	}

	if (n != bytes_to_send) 
	{
		LOG_FATAL("sendto failed, attempted n=%d, sent n=%d", 
			bytes_to_send, n);
	}

    // copy data to evr file if enabled
    if(_logsends_fd != -1)
    {
        int bytes_written = write(_logsends_fd, (char*)buf, bytes_to_send);
        if(bytes_written == -1)
        {
            LOG_ERROR("write failed to %s", _logsends_filename);
        } 
        if(bytes_written != bytes_to_send)
        {
            LOG_ERROR("incomplete write to %s, %d of %d bytes written",
                _logsends_filename, bytes_written, bytes_to_send);
        }
    }

    _sent_count++;
}


// FUNCTION: receive
// PURPOSE:  blocks until a datagram is received into the
// specified buffer.
// NOTE: recvfrom is used to set other side IP/port for next send 
int UdpSocket::receive(char* buf, int size)
{
	ASSERT(buf);
	ASSERT(size >= 0);
	int	other_len = sizeof(_other_addr);
	int n = recvfrom(_sockfd, buf, size, 0,
		(struct sockaddr*)&_other_addr, &other_len);

	if (n == SOCKET_ERROR)
	{
		LOG_FATAL("recvfrom failed, h_errno=%d", h_errno);
	}

    _received_count++;

    if(Logger::Is_debug())
    {
        LOG_DEBUG("receive: name=%s fd=%d packet=%d size=%d, contents:",
            _name, _sockfd, _received_count, n);
        LOG_DUMP(buf, n);
    }

	return n;
}


void UdpSocket::init(const char* our_ip, unsigned short our_port, 
                     const char* other_ip, unsigned short other_port)
{
    const char* pOur_ip = our_ip;
    if(pOur_ip == 0)
        pOur_ip = "NULL";
    const char* pOther_ip = other_ip;
    if(pOther_ip == 0)
        pOther_ip = "NULL";
    LOG_DEBUG("init: our_ip=%s our_port=%d other_ip=%s other_port=%d",
        pOur_ip, our_port, pOther_ip, other_port);

	_Ref_count++;
	if (_Ref_count == 1)
	{
#ifdef BUILD_FOR_NT
		winsock_startup();
#endif
	}

    _received_count = 0;
    _sent_count = 0;

	// create our socket
	if ((_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) 
	{
		LOG_FATAL("init: socket failed, h_errno=%d", h_errno);
	}

	// setup our local address
	memset((char*)&_our_addr, 0, sizeof(_our_addr));
	_our_addr.sin_family = AF_INET;
	_our_addr.sin_port = htons(our_port);
    if(our_ip)
    {
		_our_addr.sin_addr.s_addr = inet_addr(const_cast<char*>(our_ip));
	}
    else // if IP not specified, default to 1st one available
    {
    	_our_addr.sin_addr.s_addr = inet_addr(const_cast<char*>(getFirstIP()));
    }

	// bind local address to socket
	if (bind(_sockfd, (struct sockaddr*)&_our_addr, sizeof(_our_addr)) == SOCKET_ERROR) 
	{
		LOG_FATAL("bind failed, h_errno=%d", h_errno);
	}

	// setup other addr
	memset((char*)&_other_addr, 0, sizeof(_other_addr));
	_other_addr.sin_family = AF_INET;
	_other_addr.sin_port = htons(other_port);
	if (other_ip)
    {
		_other_addr.sin_addr.s_addr = inet_addr(const_cast<char*>(other_ip));
	}
    else // if IP not specified, default to 1st local interface
	{
    	_other_addr.sin_addr.s_addr = inet_addr(const_cast<char*>(getFirstIP()));
    }

	LOG_DEBUG("UdpSocket: initialized name=%s fd=%d", _name, _sockfd);

} // end init


#ifndef BUILD_FOR_VXWORKS // gethostbyname() not supported in vxworks
// FUNCTION: getFirstIP()
// PURPOSE: returns 1st IP for this host in ASCII
// NOTE: return string must be copied or converted as needed, memory is owned
// by socket layer and may change with next socket call.
// NOTE: if buf is non-null, assumes it points to storage that is large enough
// to hold the IP.
const char* UdpSocket::getFirstIP(char* buf)
{
    char namebuf[100];
    if(gethostname(namebuf, sizeof(namebuf)) != 0)
        LOG_FATAL("gethostname failed, h_errno=%d", h_errno);

    struct hostent* pHost = gethostbyname(namebuf);
    if(pHost == 0)
        LOG_FATAL("gethostbyname failed for name=%s h_errno=%d", 
                        namebuf, h_errno);

    ASSERT(pHost->h_length == 4); 
    struct in_addr in_ip;
    memcpy(&in_ip, pHost->h_addr_list[0], 4);

    char* ip = inet_ntoa(in_ip);

    if(buf == 0)
        return(ip);

    if(ip == 0)
    {
        strcpy(buf, "NULL");
        return buf;
    }

    strcpy(buf, ip);
    return buf;
}
#endif // BUILD_FOR_VXWORKS


#ifdef BUILD_FOR_NT

void UdpSocket::winsock_startup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err; 
	wVersionRequested = MAKEWORD( 2, 0 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		LOG_FATAL("winsock_startup: WSAStartup failed, err=%d", err);
	}

//    atexit(WSACleanup); 
	
	/* Confirm that the WinSock DLL supports 2.0.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.0 in addition to 2.0, it will still return */
	/* 2.0 in wVersion since that is the version we      */
	/* requested.*/
	int major_version = LOBYTE(wsaData.wVersion);
	int minor_version = HIBYTE(wsaData.wVersion);
	if ( major_version != 2 || minor_version != 0 ) 
	{
		LOG_FATAL("winsock_startup: no compatible dll found will support %d.%d",
			major_version, minor_version);
	}

	LOG_DEBUG("winsock_startup: using WinSock DLL version %d.%d",
		major_version, minor_version);
}

#endif // BUILD_FOR_WINNT


// FUNCTION: getPort
// PURPOSE: return socket's local port number
int UdpSocket::getPort()
{
    return( ntohs(_our_addr.sin_port) );
}


// FUNCTION: getOtherPort
// PURPOSE: return socket's destination port number
int UdpSocket::getOtherPort()
{
    return( ntohs(_other_addr.sin_port) );
}


// FUNCTION: getIP
// PURPOSE: return socket's local IP, NULL if none
// NOTE: return string must be copied or converted as needed, memory is owned
// by socket layer and may change with next socket call.
// NOTE: if buf is non-null, assumes it points to storage that is large enough
// to hold the IP.
const char* UdpSocket::getIP(char* buf)
{
    char* ip = inet_ntoa(_our_addr.sin_addr);

    if(buf == 0)
        return(ip);

    if(ip == 0)
    {
        strcpy(buf, "NULL");
        return buf;
    }

    strcpy(buf, ip);
    return buf;
}


// FUNCTION: getOtherIP
// PURPOSE: return socket's destination IP, NULL if none
// NOTE: return string must be copied or converted as needed, memory is owned
// by socket layer and may change with next socket call.
// NOTE: if buf is non-null, assumes it points to storage that is large enough
// to hold the IP.
const char* UdpSocket::getOtherIP(char* buf)
{
    char* ip = inet_ntoa(_other_addr.sin_addr);

    if(buf == 0)
        return(ip);

    if(ip == 0)
    {
        strcpy(buf, "NULL");
        return buf;
    }

    strcpy(buf, ip);
    return buf;
}


// FUNCTION: print
// PURPOSE: prints current socket info
void UdpSocket::print()
{
    char ourIP[32];
    getIP(ourIP);

    char otherIP[32];
    getOtherIP(otherIP);

    LOG_INFO("UdpSocket: name=%s IP=%s port=%d  otherIP=%s otherPort=%d  sent=%d received=%d",
            _name, ourIP, getPort(), otherIP, getOtherPort(), getSentCount(), getReceivedCount());
}


// FUNCTION: BroadcastUdpSocket
// PURPOSE:  this constructor sets up a more general purpose socket that can be used
// as client or server, but is setup for broadcast.  
void UdpSocket::setBroadcastMode()
{
    // turn broadcast on
    int on = 1;
    if(setsockopt(_sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)) != 0)
    {
        LOG_FATAL("setsockopt failed for SO_BROADCAST, h_errno=%d", 
                   h_errno);
    }

    LOG_DEBUG("broadcast socket initialized");
}


// FUNCTION: poll
// PURPOSE: returns true if socket has data ready to be received
bool UdpSocket::poll(int time)
{
    struct timeval timeout; // setup for non-blocking poll
    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    fd_set readset; // put socket fd into readset
    FD_ZERO(&readset);
    FD_SET(_sockfd, &readset);

    int rval = select(32, &readset, 0, 0, &timeout);
    if(rval == SOCKET_ERROR)
        LOG_FATAL("select failed, h_errno=%d", h_errno);

    if(rval == 0)
        return false;

    return true;
}


// FUNCTION: logSends
// PURPOSE: opens logfile for logging binary data sent
void UdpSocket::logSends(const char* filename)
{
    ASSERT(filename);
    _logsends_filename = filename;
    _logsends_fd = open(filename, O_CREAT | O_WRONLY, 0666);
    
    if(_logsends_fd == -1)
    {
        LOG_ERROR("failed to open %s", filename);
    }
}

