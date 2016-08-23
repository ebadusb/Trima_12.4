#include "stdafx.h"
#include "DebugLogScanner.h"

int main(int argc, char* argv[])
{
	DebugLogScanner	converter;

	HANDLE	handle = CreateFile("d:/trimavx/log/1T00254_20030527_001_000434.dlog", GENERIC_READ,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	converter.Convert(handle, 0xa0000001);
	CloseHandle(handle);

	return 0;
}
