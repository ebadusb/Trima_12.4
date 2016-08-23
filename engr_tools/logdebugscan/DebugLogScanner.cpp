
#include "stdafx.h"
#include "DebugLogScanner.h"
#include "winsock2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DebugLogScanner::DebugLogScanner()
{
   m_fileHandle = INVALID_HANDLE_VALUE;

   memset(&m_dlogStream, 0, sizeof(m_dlogStream));
   m_dlogStreamInitialized = false;

	m_lastTaskEntry = 0;
}

DebugLogScanner::~DebugLogScanner()
{
}

bool DebugLogScanner::Convert(HANDLE fileHandle, DWORD node)
{
   m_fileHandle = fileHandle;
   m_node = node;

   bool result = true;
   result &= ReadDLogHeader();

	WORD  recordType;
	while ( result && ReadDLogData(&recordType, sizeof(recordType)) )
	{
		switch (recordType)
		{
		case CDLogFormat::TaskCreateID:
			result = ProcessTaskCreate();
			break;

		case CDLogFormat::BinaryID:
			result = ProcessBinary();
			break;

		default:
			result = SkipRecord(recordType);
         break;
      }
	}

   return result;
}

bool DebugLogScanner::ReadDLogHeader(void)
{
   bool  result = true;

   // Skip over confidential/proprietary message at start of file
   Byte  ch;
   result = ReadUncompressedData(&ch, sizeof(ch));
   while ( result && ch != 0 )
   {
      result = ReadUncompressedData(&ch, sizeof(ch));
   }

   if ( !result )
   {
      printf("Unexpected end of file\n");
      return result;
   }

   // Verify that we have the expected byte order and header record ID
   DWORD byteOrder;
   WORD  headerType;

   result &= ReadUncompressedData(&byteOrder, sizeof(byteOrder));
   result &= ReadUncompressedData(&headerType, sizeof(headerType));
   if ( !result )
   {
      printf("Unexpected end of file\n");
      return result;
   }

   if ( byteOrder != 0x12345678 || headerType != CDLogFormat::HeaderID )
   {
      result = false;
      printf("Bad header format\n");
      return result;
   }

   // Read header record from file
   CDLogFormat::Header  header;

   result &= ReadUncompressedData(&header, sizeof(header));
   if ( !result )
   {
      printf("Unexpected end of file\n");
      return result;
   }

   if ( (header.version & 0xff00) != 0x0300 )
   {
      result = false;
      printf("Unrecognized data log version\n");
      return result;
   }

   // Read platform name from file
   Byte  expectedPlatformName[] = { 0x05, 0x00, 0x54, 0x52, 0x49, 0x4d, 0x41 };  // length, "TRIMA"
   Byte  platformName[sizeof(expectedPlatformName)];

   result &= ReadUncompressedData(platformName, sizeof(platformName));  
   if ( !result )
   {
      printf("Unexpected end of file\n");
      return result;
   }

   if ( memcmp(expectedPlatformName, platformName, sizeof(platformName)) != 0 || header.sizeofNodeID != 4 )
   {
      result = false;
      printf("Unrecognized platform\n");
      return result;
   }

   // Skip to log start time and set the base time for log information
   CDLogFormat::TimeOfDay  timeOfDay;
   
   SetFilePointer(m_fileHandle, header.sizeofNodeID, NULL, FILE_CURRENT);
   result &= ReadUncompressedData(&timeOfDay, sizeof(timeOfDay));
   if ( !result )
   {
      printf("Unexpected end of file\n");
      return result;
   }

   CTime dlogTime(timeOfDay.year, timeOfDay.month, timeOfDay.day, timeOfDay.hour, timeOfDay.minute, timeOfDay.second);
   m_baseTime = (DWORD)dlogTime.GetTime();

   // Skip node name information
   WORD  nodeNameLen;

   result &= ReadUncompressedData(&nodeNameLen, sizeof(nodeNameLen));
   if ( !result )
   {
      printf("Unexpected end of file\n");
      return result;
   }

   SetFilePointer(m_fileHandle, nodeNameLen, NULL, FILE_CURRENT);

   // Read magic bytes for zlib header
   WORD  zlibMagic;
   result &= ReadUncompressedData(&zlibMagic, sizeof(zlibMagic));
   if ( !result )
   {
      printf("Unexpected end of file\n");
      return result;
   }

   if ( zlibMagic != 0x8b1f )
   {
      result = false;
      printf("Bad compression format\n");
      return result;
   }

   // Skip over gz header
   SetFilePointer(m_fileHandle, 8, NULL, FILE_CURRENT);
   return result;
}

bool DebugLogScanner::ReadDLogData(void * data, size_t size)
{
   bool     result = true;
   size_t   readSize = 0;

   while ( result && readSize < size )
   {
      if ( !m_dlogStreamInitialized )
      {
         // Generate expected header information for inflate
         m_dlogInputBuffer[0] = 0x08;
         m_dlogInputBuffer[1] = 0x1d;
         m_dlogStream.avail_in = 2;
         m_dlogStream.next_in = m_dlogInputBuffer;

         inflateInit(&m_dlogStream);
         m_dlogStreamInitialized = true;
      }

      if ( !m_dlogStream.avail_in )
      {
         DWORD bytesRead;

         ReadFile(m_fileHandle, m_dlogInputBuffer, DLogInputBufferSize, &bytesRead, NULL);
         if ( bytesRead == 0 )
         {
            result = false;
         }
         else
         {
            m_dlogStream.next_in = m_dlogInputBuffer;
            m_dlogStream.avail_in = bytesRead;
         }
      }

      m_dlogStream.next_out = ((Bytef *)data) + readSize;
      m_dlogStream.avail_out = size - readSize;

      int inflateResult = inflate(&m_dlogStream, 0);
      readSize = size - m_dlogStream.avail_out;
      if ( readSize != size && inflateResult != Z_OK )
      {
         result = false;
      }
   }

   if (result)
   {
      for (size_t index=0; index<size; index++)
      {
         ((Bytef *)data)[index] ^= 0xa5;
      }
   }

   return result;
}

void DebugLogScanner::SkipDLogData(size_t size)
{
	char	buffer[256];
	size_t	sizeRemaining = size;
	while ( sizeRemaining > 0 )
	{
		size_t	sizeRead = (sizeRemaining > 256) ? 256 : sizeRemaining;
		ReadDLogData(buffer, sizeRead);
		sizeRemaining -= sizeRead;
	}
}

bool DebugLogScanner::ReadUncompressedData(void * data, size_t size)
{
   DWORD bytesRead;

   ReadFile(m_fileHandle, data, size, &bytesRead, NULL);
   return (bytesRead == size);
}

bool DebugLogScanner::SkipRecord(WORD recordType)
{
   bool  result = true;

   switch ( recordType )
   {
	case CDLogFormat::LogLevelID:
      result &= SkipLogLevel(); 
      break;

	case CDLogFormat::PrintOutputID:
      result &= SkipPrintOutput();
      break;

	case CDLogFormat::StreamOutputID:
      result &= SkipStreamOutput();
      break;

	case CDLogFormat::PeriodicOutputID:
      result &= SkipPeriodicOutput();
      break;

	case CDLogFormat::PeriodicSetID:
      result &= SkipPeriodicSet();
      break;

	case CDLogFormat::PeriodicItemID:
      result &= SkipPeriodicItem();
      break;

	case CDLogFormat::TaskCreateID:
      result &= SkipTaskCreate();
      break;

	case CDLogFormat::TaskDeleteID:
      SkipDLogData(18);
      break;

	case CDLogFormat::NetworkHeaderID:
      result &= SkipNetworkHeader();
      break;

	case CDLogFormat::EndOfNetworkOutputID:
      SkipDLogData(8);
      break;

	case CDLogFormat::FileCloseID:
      SkipDLogData(8);
      break;

	case CDLogFormat::WriteTimeID:
      SkipDLogData(8);
      break;

	case CDLogFormat::MissedDataID:
      break;

	default:
      printf("Bad record type: %d\n", (int)recordType);
		result = false;
		break;
   }

   return result;
}

bool DebugLogScanner::SkipLogLevel(void)
{
   bool  result = true; 
   WORD  nameLen;

   SkipDLogData(14);
   result &= ReadDLogData(&nameLen, sizeof(nameLen));

   if ( result )
   {
      SkipDLogData(nameLen);
   }

   return result;
}

bool DebugLogScanner::SkipPrintOutput(void)
{
   bool  result = true;
   WORD  formatLen;
   WORD  fileLen;
   char *   format = NULL;

   SkipDLogData(18);
   result &= ReadDLogData(&formatLen, sizeof(formatLen));
   result &= ReadDLogData(&fileLen, sizeof(fileLen));
   SkipDLogData(2);

   if ( result )
   {
      format = new char[formatLen+1];
      result &= ReadDLogData(format, formatLen);
      format[formatLen] = 0;
   }

   SkipDLogData(fileLen);
   ParsePrint(format);
   delete[] format;

   return result;
}

bool DebugLogScanner::SkipStreamOutput(void)
{
   bool  result = true;
   WORD  fileLen;

   SkipDLogData(18);
   result &= ReadDLogData(&fileLen, sizeof(fileLen));

   if ( result )
   {
      SkipDLogData(fileLen+5);
   }

   WORD  argLen;
   WORD	currentLen = 0;
   result &= ReadDLogData(&argLen, sizeof(argLen));

   while ( currentLen < argLen && result )
	{
      char  type;
      result &= ReadDLogData(&type, sizeof(type));

      if ( result )
      {
         switch ( type )
         {
			case CDLogFormat::STREAM_SignedChar:
         case CDLogFormat::STREAM_UnsignedChar:
         case CDLogFormat::STREAM_Bool:
         case CDLogFormat::STREAM_Precision:
            SkipDLogData(1);
            currentLen += 2;
            break;

         case CDLogFormat::STREAM_Flags:
            SkipDLogData(2);
            currentLen += 3;
            break;

         case CDLogFormat::STREAM_SignedInt:
         case CDLogFormat::STREAM_UnsignedInt:
         case CDLogFormat::STREAM_SignedLong:
         case CDLogFormat::STREAM_UnsignedLong:
         case CDLogFormat::STREAM_Float:
            SkipDLogData(4);
            currentLen += 5;
            break;

         case CDLogFormat::STREAM_String:
            result &= ReadDLogData(&fileLen, sizeof(fileLen));
            SkipDLogData(fileLen);
            currentLen += fileLen+3;
            break;

         case CDLogFormat::STREAM_Double:
            SkipDLogData(8);
            currentLen += 9;
            break;

         default:
            printf("Bad stream record\n");
            result = false;
            break;
         }
      }
   }

   return result;
}

bool DebugLogScanner::SkipPeriodicOutput(void)
{
   bool  result = true;
   SkipDLogData(10);

   DWORD nodeID;
   WORD  itemCount;

   result &= ReadDLogData(&nodeID, sizeof(nodeID));
   result &= ReadDLogData(&itemCount, sizeof(itemCount));

   for ( int i=0; i<itemCount && result; i++ )
   {
      WORD  itemSize;

      result &= ReadDLogData(&itemSize, sizeof(itemSize));
      SkipDLogData(itemSize+2);
   }

   return result; 
}

bool DebugLogScanner::SkipPeriodicSet(void)
{
   bool  result;
   CDLogFormat::PeriodicSet   periodicSet;

   result = ReadDLogData(&periodicSet, sizeof(periodicSet));
   if ( result )
   {
      SkipDLogData(periodicSet.nameLen);
   }

   return result;
}

bool DebugLogScanner::SkipPeriodicItem(void)
{
   bool  result;
   CDLogFormat::PeriodicItem  periodicItem;

   result = ReadDLogData(&periodicItem, sizeof(periodicItem));
   if ( result )
   {
      SkipDLogData(periodicItem.keyLen);
      SkipDLogData(periodicItem.descLen+periodicItem.formatLen);
   }

   return result;
}

bool DebugLogScanner::SkipTaskCreate(void)
{
   bool  result = true; 
   WORD  nameLen;

   SkipDLogData(18);
   result &= ReadDLogData(&nameLen, sizeof(nameLen));

   if ( result )
   {
      SkipDLogData(nameLen);
   }

   return result;
}

bool DebugLogScanner::SkipNetworkHeader(void)
{
   bool  result = true; 
   WORD  nameLen;

   SkipDLogData(11);
   result &= ReadDLogData(&nameLen, sizeof(nameLen));

   if ( result )
   {
      SkipDLogData(nameLen);
   }

   return result;
}

bool DebugLogScanner::ProcessBinary(void)
{
	enum { DEBUG_RecordType = 0x0001 };
	enum { DEBUG_TaskData = 0x0001 };
	enum { DEBUG_MsgData = 0x0002 };
	enum { DEBUG_InputPacketData = 0x0003 };
	enum { DEBUG_OutputPacketData = 0x0004 };
	enum { DEBUG_NetworkStatData = 0x0005 };

   bool  result = true;
   long  size;

   result &= ReadDLogData(&size, sizeof(size));
   if ( result && size > 4 )
   {
		WORD	type;
		WORD	subType;

		result &= ReadDLogData(&type, sizeof(type));
		result &= ReadDLogData(&subType, sizeof(subType));
		size -= sizeof(type)+sizeof(subType);

		if ( type == DEBUG_RecordType )
		{
			switch ( subType )
			{
			case DEBUG_TaskData:
				ProcessDbgTaskData(size);
				size = 0;
				break;

			case DEBUG_MsgData:
				ProcessDbgMsgData(size);
				size = 0;
				break;

			case DEBUG_InputPacketData:
				ProcessDbgPacketData("input", size);
				size = 0;
				break;

			case DEBUG_OutputPacketData:
				ProcessDbgPacketData("output", size);
				size = 0;
				break;

			case DEBUG_NetworkStatData:
				ProcessDbgNetworkStatData(size);
				size = 0;
				break;

			default:
				break;
			}
		}
   }

	if ( result && size < 0 )
	{
		result = false;
	}
	else
	{
		SkipDLogData(size);
	}

   return result;
}

bool DebugLogScanner::ProcessTaskCreate(void)
{
	CDLogFormat::TaskCreate	record;
	bool 	result = ReadDLogData(&record, sizeof(record));
	char	taskName[MaxTaskNameSize];

	memset(taskName, ' ', sizeof(taskName));
	taskName[MaxTaskNameSize-1] = '\0';

	if ( result )
	{
		int	copyBytes = (record.nameLen >= MaxTaskNameSize) ? MaxTaskNameSize-1 : record.nameLen;
		ReadDLogData(taskName, copyBytes);

		if ( copyBytes < record.nameLen )
		{
			SkipDLogData(record.nameLen-copyBytes);
		}

		if ( m_lastTaskEntry < MaxTaskEntry && record.nodeID == m_node )
		{
			m_taskEntry[m_lastTaskEntry].id = record.taskID;
			strcpy(m_taskEntry[m_lastTaskEntry].name, taskName);
			m_lastTaskEntry += 1;
		}
   }

   return result;
}

bool DebugLogScanner::ProcessDbgTaskData(size_t size)
{
	bool	result = true;
	size_t	sizeRemaining = size;

	printf("Task switch data: time/old task/new task\n");

	CDLogFormat::BinaryDbgTaskRecord	record;
	while ( result && sizeRemaining >= sizeof(record) )
	{
		result = ReadDLogData(&record, sizeof(record));
		if ( result )
		{
			sizeRemaining -= sizeof(record);
			char	oldTaskName[MaxTaskNameSize];
			char	newTaskName[MaxTaskNameSize];

			GetTaskName(record.oldTask, oldTaskName);
			GetTaskName(record.newTask, newTaskName);

			printf("%10.6lf %s %s\n", (double)record.seconds+(double)record.nanoSeconds/1000000000.0, oldTaskName, newTaskName);
		}
   }

	return result;
}

bool DebugLogScanner::ProcessDbgMsgData(size_t size)
{
	bool	result = true;
	size_t	sizeRemaining = size;

	printf("Message data: time/send task/rcv task/message ID\n");

	CDLogFormat::BinaryDbgMsgRecord	record;
	while ( result && sizeRemaining >= sizeof(record) )
	{
		result = ReadDLogData(&record, sizeof(record));
		if ( result )
		{
			sizeRemaining -= sizeof(record);
			char	sendTaskName[MaxTaskNameSize];
			char	rcvTaskName[MaxTaskNameSize];

			GetTaskName(record.sendTask, sendTaskName);
			GetTaskName(record.rcvTask, rcvTaskName);

			printf("%10.6lf %s %s %08lx\n", (double)record.seconds+(double)record.nanoSeconds/1000000000.0, sendTaskName, rcvTaskName, record.msgID);
		}
   }

	return result;
}

static void readByteBuffer(char * data, size_t dataSize, int& currentIdx, void * result, size_t resultSize)
{
	int copyLen = 0;
	char * copyTo = (char *)result;

	while (copyLen < resultSize)
	{
		copyTo[resultSize-copyLen-1] = data[currentIdx];
		currentIdx = (currentIdx+dataSize-1) % dataSize;
		copyLen++;
	}
}

#define	ETHERTYPE_IP		0x0800	/* IP protocol */
#define	ETHERTYPE_ARP		0x0806	/* address resolution protocol */

struct   ether_header
{
   u_char   ether_dhost[6];
   u_char   ether_shost[6];
   u_short  ether_type;
};

/*
 * Structure of an internet header, naked of options.
 *
 * We declare ip_len and ip_off to be short, rather than u_short
 * pragmatically since otherwise unsigned comparisons can result
 * against negative integers quite easily, and fail in subtle ways.
 */
struct ip
{
   u_int ip_v:4,        /* version */
   ip_hl:4,    /* header length */
   ip_tos:8,      /* type of service */
   ip_len:16;     /* total length */
   u_short  ip_id;         /* identification */
   short ip_off;        /* fragment offset field */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
   u_char   ip_ttl;        /* time to live */
   u_char   ip_p;       /* protocol */
   u_short  ip_sum;        /* checksum */
   struct   in_addr ip_src,ip_dst;  /* source and dest address */
};


#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_IGMP		2		/* group mgmt protocol */
#define	IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_TP		29 		/* tp-4 w/ class negotiation */
#define	IPPROTO_EON		80		/* ISO cnlp */
#define	IPPROTO_OSPF		89		/* OSPF version 2  */
#define	IPPROTO_ENCAP		98		/* encapsulation header */

#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256

typedef	u_long	tcp_seq;

struct tcphdr {
	u_short	th_sport;		/* source port */
	u_short	th_dport;		/* destination port */
	tcp_seq	th_seq;			/* sequence number */
	tcp_seq	th_ack;			/* acknowledgement number */
	u_int	th_x2:4,		/* (unused) */
		th_off:4,		/* data offset */
		th_flags:8,
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
		th_win:16;		/* window */
	u_short	th_sum;			/* checksum */
	u_short	th_urp;			/* urgent pointer */
};

struct udphdr {
	u_short	uh_sport;		/* source port */
	u_short	uh_dport;		/* destination port */
	short	uh_ulen;		/* udp length */
	u_short	uh_sum;			/* udp checksum */
};


void PrintIpHeader (const struct ip *hdr)
{
	char	protoName[32];
   switch (hdr->ip_p)
   {
   case IPPROTO_ICMP:	strcpy(protoName, "ICMP");	break;
   case IPPROTO_IGMP:	strcpy(protoName, "IGMP");	break;
   case IPPROTO_TCP:		strcpy(protoName, "TCP");	break;
   case IPPROTO_UDP:		strcpy(protoName, "UDP");	break;
	default:					sprintf(protoName, "ip_p=%d\n"); break;
   }

   char sourceAddr[80];
   char destAddr[80];
   strncpy(sourceAddr, inet_ntoa(hdr->ip_src), 80);
   strncpy(destAddr, inet_ntoa(hdr->ip_dst), 80);
   printf(" IP hdr: prot=%s, src=%s dst=%s raw=%08x %08x %08x\n", protoName, sourceAddr, destAddr, *(((long *)hdr)+0), *(((long *)hdr)+1), *(((long *)hdr)+2));

	tcphdr * tcp_hdr;
	udphdr * udp_hdr;

   switch (hdr->ip_p)
   {
   case IPPROTO_TCP:
		tcp_hdr = (tcphdr *)&((char *)hdr)[sizeof(ip)];
		printf(" TCP hdr: s_port=%d d_port=%d seq=%lu ack=%lu flags=%d window=%d\n",
					ntohs(tcp_hdr->th_sport), ntohs(tcp_hdr->th_dport), ntohl(tcp_hdr->th_seq), ntohl(tcp_hdr->th_ack), tcp_hdr->th_flags, ntohs(tcp_hdr->th_win));
      break;

   case IPPROTO_ICMP:
   case IPPROTO_IGMP:
		break;

   case IPPROTO_UDP:
		udp_hdr = (udphdr *)&((char *)hdr)[sizeof(ip)];
		printf(" UDP hdr: s_port=%d d_port=%d len=%d\n",
					ntohs(udp_hdr->uh_sport), ntohs(udp_hdr->uh_dport), ntohs(udp_hdr->uh_ulen));
      break;

   default:
      printf("Uknown Protocol!\n");
   }
};



bool DebugLogScanner::ProcessDbgPacketData(const char * type, size_t size)
{
	char * data = new char[size];
	bool result = ReadDLogData(data, size);
	if ( result && size > 4 )
	{
		int	currentIdx = *(DWORD *)(&data[size-sizeof(DWORD)]);
		size -= 4;
		currentIdx = (currentIdx+size-1) % size;

		printf("Processing %s packet data - size=%d, start=%d\n", type, size, currentIdx);

		int	bytesDone = 0;
		CDLogFormat::DbgPacketHeader	dbgPacketHdr;

		while ( size-bytesDone > sizeof(dbgPacketHdr) )
		{
			readByteBuffer(data, size, currentIdx, &dbgPacketHdr, sizeof(dbgPacketHdr));
			bytesDone += sizeof(dbgPacketHdr);

			if ( size-bytesDone >= dbgPacketHdr.len )
			{
				unsigned char * packet = new unsigned char[dbgPacketHdr.len];
				readByteBuffer(data, size, currentIdx, packet, dbgPacketHdr.len);
				bytesDone += dbgPacketHdr.len;

				printf("%10.6lf len=%d index=%d\n", (double)dbgPacketHdr.seconds+(double)dbgPacketHdr.nanoSeconds/1000000000.0, dbgPacketHdr.len, dbgPacketHdr.index);

            const ether_header *ether_hdr = (ether_header *)packet;
            printf(" ether hdr: dhost=");
            for ( int d=0; d<6; d++ )
				{
					printf("%02X", ether_hdr->ether_dhost[d]);
				}

				printf(" shost=");
            for ( int s=0; s<6; s++ )
				{
					printf("%02X", ether_hdr->ether_shost[s]);
				}
				printf("\n");

            switch (ntohs(ether_hdr->ether_type))
            {
            case ETHERTYPE_IP:
               PrintIpHeader ((struct ip *)&packet[sizeof(ether_header)]);
               break;

            case ETHERTYPE_ARP:
               break;

            default:
					printf("Unknown ether_type=0x%4X\n", ntohs(ether_hdr->ether_type));
					break;
            }


				printf(" raw packet:\n  ");
				for ( int i=0; i<dbgPacketHdr.len; i++ )
				{
					printf("%02X ", packet[i]);
					if ( i%16 == 15 && i != dbgPacketHdr.len-1 )
					{
						printf("\n  ");
					}
				}

				delete packet;
				printf("\n\n");
			}
			else
			{
				break;
			}
		}
	}

	delete data;
	return result;
}

bool DebugLogScanner::ProcessDbgNetworkStatData(size_t size)
{
	bool	result = true;
	size_t	sizeRemaining = size;

	printf("Network stat data: time - data\n");

	CDLogFormat::NetworkStatData	record;
	while ( result && sizeRemaining >= sizeof(record) )
	{
		result = ReadDLogData(&record, sizeof(record));
		if ( result )
		{
			sizeRemaining -= sizeof(record);
			printf("%10.6lf ", (double)record.seconds+(double)record.nanoSeconds/1000000000.0);

			DWORD * start = (DWORD *)&record;
			for ( int i=2; i<sizeof(record)/sizeof(DWORD); i++ )
			{
				printf("%10d ", start[i]);
			}

			printf("\n");
		}
   }

	return result;
}

void DebugLogScanner::GetTaskName(DWORD id, char * name)
{
	name[0] = '\0';
	for ( int i=0; i<m_lastTaskEntry; i++ )
	{
		if ( id == m_taskEntry[i].id )
		{
			strcpy(name, m_taskEntry[i].name);
		}
	}

	if ( !name[0] )
	{
		sprintf(name, "%*lx", MaxTaskNameSize-1, (unsigned long)id);
	}
}

bool DebugLogScanner::GetNextFormatArg(const char * format, size_t& currentIndex, unsigned int& starModifierCount, size_t& size)
{
   enum ParseState
   {
      WaitPercent,
      WaitType
   };

   bool formatDone = true;
   ParseState     parseState = WaitPercent;
   bool  longModifierPresent = false;
   bool  parseDone = false;
   char  type = '\0';

   starModifierCount = 0;
   while ( format &&
           format[currentIndex] != '\0' &&
           !parseDone )
   {
      if ( parseState == WaitPercent &&
           format[currentIndex] == '%' )
      {
         parseState = WaitType;
      }
      else if ( parseState == WaitType )
      {
         switch ( format[currentIndex] )
         {
         case '%':
            if ( format[currentIndex-1] == '%' )
            {
               //
               // %% combination corresponds to single % on output
               //
               parseState = WaitPercent;
            }
            else
            {
               formatDone = false;
               parseDone = true;
            }

            break;
  
         case '*':
            starModifierCount += 1;
            break;

         case 'l':
            longModifierPresent = true;
            break;

         case 'c':
            size = 1;
            formatDone = false;
            parseDone = true;
            break;

         case 'd':
         case 'u':
         case 'x':
         case 'X':
            size = 4;
            formatDone = false;
            parseDone = true;
            break;

         case 'f':
         case 'g':
            size = ( longModifierPresent ) ? 8 : 4;
            formatDone = false;
            parseDone = true;
            break;

         case 's':
            WORD  stringSize;

            ReadDLogData(&stringSize, sizeof(stringSize));
            size = stringSize; 
            formatDone = false;
            parseDone = true;
            break;
         }
      }

      currentIndex += 1;
   }

   return formatDone;
}

void DebugLogScanner::ParsePrint(const char * format)
{
   unsigned int   starModifierCount;
   size_t         currentIndex = 0;
   size_t         argSize;

   while ( !GetNextFormatArg(format, currentIndex, starModifierCount, argSize) )
   {
      SkipDLogData(starModifierCount*4 + argSize);
   }
}

