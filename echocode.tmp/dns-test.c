#define _WIN32_WINNT 0x0600
#include <ws2tcpip.h>
#include <windows.h>
#include <windns.h>
#include <stdio.h>

int GetARecord(LPCSTR lpDomain, LPDWORD lpAddr) {
	PDNS_RECORD pRecord;
	DNS_A_DATA aRecord;

	DNS_STATUS ret = DnsQuery_A(lpDomain, DNS_TYPE_A, DNS_QUERY_STANDARD, NULL, &pRecord, NULL);
	aRecord = pRecord->Data.A;

	if(ret != ERROR_SUCCESS) {
		printf("err %d\n", ret);
		return 1;
	}

	*lpAddr = aRecord.IpAddress;

	return 0;
}
/*
int main() {
	PDNS_RECORD res;
	DNS_A_DATA record;
	char dst[0x40];
	DNS_STATUS ds = DnsQuery_A("1.2.3.4", DNS_TYPE_A, DNS_QUERY_STANDARD, NULL, &res, NULL);
	record = res->Data.A;
	printf("%d->%s\n", ds, inet_ntop(AF_INET,&record,dst,16));
}
*/

int main() {
	DWORD addr;
	char dst[0x40];
	int ret = GetARecord("google.com", &addr);
	printf("ret %d\n", ret);
	printf("%s\n", inet_ntop(AF_INET,&addr,dst,16));

}
