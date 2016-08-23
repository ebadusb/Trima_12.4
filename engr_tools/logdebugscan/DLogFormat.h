//
// Defines format of records in data log file
//

#ifndef _DLOG_FORMAT_INCLUDE
#define _DLOG_FORMAT_INCLUDE

// These structures must be packed to match the format from the data log file
#pragma pack(push, _dlog_previous_pack)
#pragma pack(1)

class CDLogFormat
{
public:
	enum
	{
		HeaderID = 0x5500,
		LogLevelID = 0x5501,
		PrintOutputID = 0x5502,
		StreamOutputID = 0x5503,
		PeriodicOutputID = 0x5504,
		PeriodicSetID = 0x5505,
		PeriodicItemID = 0x5506,
		TaskCreateID = 0x5507,
		TaskDeleteID = 0x5508,
		NetworkHeaderID = 0x5509,

		BinaryID = 0x55f0,
		EndOfNetworkOutputID = 0x55fc,
		FileCloseID = 0x55fd,
		WriteTimeID = 0x55fe,
		MissedDataID = 0x55ff
	};

	enum
	{
		STREAM_SignedChar = 1,
		STREAM_UnsignedChar = 2,
		STREAM_SignedInt = 3,
		STREAM_UnsignedInt = 4,
		STREAM_SignedLong = 5,
		STREAM_UnsignedLong = 6,
		STREAM_String = 7,
		STREAM_Float = 8,
		STREAM_Double = 9,
		STREAM_Bool = 10,
		STREAM_Flags = 100,
		STREAM_Precision = 101
	};

	struct Header
	{
		WORD sizeofChar;
		WORD sizeofInt;
		WORD sizeofLong;
		WORD sizeofFloat;
		WORD sizeofDouble;
		WORD sizeofTaskID;
		DWORD currentTaskID;
		WORD sizeofNodeID;
		WORD version;
	};

	struct TimeOfDay
	{
		BYTE day;
		BYTE month;
		WORD year;
		BYTE hour;
		BYTE minute;
		BYTE second;
	};

	struct TaskCreate
	{
		DWORD	second;
		DWORD	nanosecond;
		WORD	id;
		DWORD	taskID;
		DWORD	nodeID;
		WORD	nameLen;
	};

	struct PeriodicSet
	{
		DWORD	second;
		DWORD	nanosecond;
		WORD	id;
		DWORD	nodeID;
		WORD	nameLen;
	};

	struct PeriodicItem
	{
		DWORD	second;
		DWORD	nanosecond;
		WORD	keyCode;
		DWORD	nodeID;

		WORD	keyLen;
		WORD	descLen;
		WORD	formatLen;
	};

	struct BinaryDbgTaskRecord
	{
		DWORD	oldTask;
		DWORD	newTask;
		DWORD	seconds;
		DWORD	nanoSeconds;
	};

	struct BinaryDbgMsgRecord
	{
		DWORD	sendTask;
		DWORD	rcvTask;
		DWORD	msgID;
		DWORD	seconds;
		DWORD	nanoSeconds;
	};

	struct DbgPacketHeader
	{
		DWORD	seconds;
		DWORD	nanoSeconds;
		WORD	len;
		WORD  index;
	};

	struct ipstat
	{
		DWORD	ips_total;		/* total packets received */
		DWORD	ips_badsum;		/* checksum bad */
		DWORD	ips_tooshort;		/* packet too short */
		DWORD	ips_toosmall;		/* not enough data */
		DWORD	ips_badhlen;		/* ip header length < data size */
		DWORD	ips_badlen;		/* ip length < ip header length */
		DWORD	ips_fragments;		/* fragments received */
		DWORD	ips_fragdropped;	/* frags dropped (dups, out of space) */
		DWORD	ips_fragtimeout;	/* fragments timed out */
		DWORD	ips_forward;		/* packets forwarded */
		DWORD	ips_cantforward;	/* packets rcvd for unreachable dest */
		DWORD	ips_redirectsent;	/* packets forwarded on same net */
		DWORD	ips_noproto;		/* unknown or unsupported protocol */
		DWORD	ips_delivered;		/* datagrams delivered to upper level*/
		DWORD	ips_localout;		/* total ip packets generated here */
		DWORD	ips_odropped;		/* lost packets due to nobufs, etc. */
		DWORD	ips_reassembled;	/* total packets reassembled ok */
		DWORD	ips_fragmented;		/* datagrams sucessfully fragmented */
		DWORD	ips_ofragments;		/* output fragments created */
		DWORD	ips_cantfrag;		/* don't fragment flag was set, etc. */
		DWORD	ips_badoptions;		/* error in option processing */
		DWORD	ips_noroute;		/* packets discarded due to no route */
		DWORD	ips_badvers;		/* ip version != 4 */
		DWORD	ips_rawout;		/* total raw ip packets generated */
	};

	struct tcpstat
	{
		DWORD	tcps_connattempt;	/* connections initiated */
		DWORD	tcps_accepts;		/* connections accepted */
		DWORD	tcps_connects;		/* connections established */
		DWORD	tcps_drops;		/* connections dropped */
		DWORD	tcps_conndrops;		/* embryonic connections dropped */
		DWORD	tcps_closed;		/* conn. closed (includes drops) */
		DWORD	tcps_segstimed;		/* segs where we tried to get rtt */
		DWORD	tcps_rttupdated;	/* times we succeeded */
		DWORD	tcps_delack;		/* delayed acks sent */
		DWORD	tcps_timeoutdrop;	/* conn. dropped in rxmt timeout */
		DWORD	tcps_rexmttimeo;	/* retransmit timeouts */
		DWORD	tcps_persisttimeo;	/* persist timeouts */
		DWORD	tcps_keeptimeo;		/* keepalive timeouts */
		DWORD	tcps_keepprobe;		/* keepalive probes sent */
		DWORD	tcps_keepdrops;		/* connections dropped in keepalive */

		DWORD	tcps_sndtotal;		/* total packets sent */
		DWORD	tcps_sndpack;		/* data packets sent */
		DWORD	tcps_sndbyte;		/* data bytes sent */
		DWORD	tcps_sndrexmitpack;	/* data packets retransmitted */
		DWORD	tcps_sndrexmitbyte;	/* data bytes retransmitted */
		DWORD	tcps_sndacks;		/* ack-only packets sent */
		DWORD	tcps_sndprobe;		/* window probes sent */
		DWORD	tcps_sndurg;		/* packets sent with URG only */
		DWORD	tcps_sndwinup;		/* window update-only packets sent */
		DWORD	tcps_sndctrl;		/* control (SYN|FIN|RST) packets sent */

		DWORD	tcps_rcvtotal;		/* total packets received */
		DWORD	tcps_rcvpack;		/* packets received in sequence */
		DWORD	tcps_rcvbyte;		/* bytes received in sequence */
		DWORD	tcps_rcvbadsum;		/* packets received with ccksum errs */
		DWORD	tcps_rcvbadoff;		/* packets received with bad offset */
		DWORD	tcps_rcvshort;		/* packets received too short */
		DWORD	tcps_rcvduppack;	/* duplicate-only packets received */
		DWORD	tcps_rcvdupbyte;	/* duplicate-only bytes received */
		DWORD	tcps_rcvpartduppack;	/* packets with some duplicate data */
		DWORD	tcps_rcvpartdupbyte;	/* dup. bytes in part-dup. packets */
		DWORD	tcps_rcvoopack;		/* out-of-order packets received */
		DWORD	tcps_rcvoobyte;		/* out-of-order bytes received */
		DWORD	tcps_rcvpackafterwin;	/* packets with data after window */
		DWORD	tcps_rcvbyteafterwin;	/* bytes rcvd after window */
		DWORD	tcps_rcvafterclose;	/* packets rcvd after "close" */
		DWORD	tcps_rcvwinprobe;	/* rcvd window probe packets */
		DWORD	tcps_rcvdupack;		/* rcvd duplicate acks */
		DWORD	tcps_rcvacktoomuch;	/* rcvd acks for unsent data */
		DWORD	tcps_rcvackpack;	/* rcvd ack packets */
		DWORD	tcps_rcvackbyte;	/* bytes acked by rcvd acks */
		DWORD	tcps_rcvwinupd;		/* rcvd window update packets */
		DWORD	tcps_pawsdrop;		/* segments dropped due to PAWS */
		DWORD	tcps_predack;		/* times hdr predict ok for acks */
		DWORD	tcps_preddat;		/* times hdr predict ok for data pkts */
		DWORD	tcps_pcbcachemiss;
		DWORD	tcps_persistdrop;		/* timeout in persist state */
		DWORD	tcps_badsyn;		/* bogus SYN, e.g. premature ACK */
	};

	struct mbstat
   {
		DWORD	mNum;			/* mBlks obtained from page pool */
		DWORD	mDrops;			/* times failed to find space */
		DWORD	mWait;			/* times waited for space */
		DWORD	mDrain;			/* times drained protocols for space */
		DWORD	mTypes[256];		/* type specific mBlk allocations */
   };

	struct NetworkStatData
	{
	DWORD	seconds;
	DWORD	nanoSeconds;

	struct ipstat	ipstat;
	struct tcpstat	tcpstat;
	struct mbstat	data_mbstat;
	struct mbstat	sys_mbstat;
	};
};

#pragma pack(pop, _dlog_previous_pack)

#endif /* ifndef _DLOG_FORMAT_INCLUDE */
