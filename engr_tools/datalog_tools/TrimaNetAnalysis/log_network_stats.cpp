#include <vxworks.h>
#include <ioLib.h>
#include <sioLib.h>
#include <netShow.h>
#include <time.h>

extern "C" void logNetworkStats(void)
{
	static const timespec timeDelay = {10, 0};

	tcpShowInit();

	for (;;) {
		nanosleep(&timeDelay, NULL);
		tcpstatShow();
		ipstatShow(0);
	}
}

