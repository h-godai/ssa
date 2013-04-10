/* TSSocket.cpp  -*-tab-width:4-*-
//
//
//
// CopyRight(C) 2005 Technicalarts h.godai
//
*/
#include "TSSocket.h"

#ifdef	TS_PLATFORM_LINUX

#include <stdio.h>

#include <string.h> /* for strncpy */
#include <sys/ioctl.h>
#include <net/if.h>


namespace ts {



  void Socket::EnumMacAddress(MacAddressList_t& macList) {

	// Ethernetのインターフェイス名を列挙する
	FILE* f = fopen("/proc/net/dev", "r");
	if (f == 0) return;
	char buf[1024];
	while (fgets(buf, 1024, f) != 0) {
	  char* coron = strchr(buf, ':');
	  if (coron == 0) continue;
	  *coron= 0; // ':'の位置で切る
	  char* name = buf;
	  while (*name != 0 && *name <= ' ') ++name; // trim space
	  if (*name == 0) continue;
	  if (strcmp(name, "lo") == 0) continue; // localhost;

	  // インターフェイスが見つかったのでMACアドレスを取得する

	  int fd;
	  struct ifreq ifr;
	  memset(&ifr, 0, sizeof(ifr));
	  fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	  ifr.ifr_addr.sa_family = AF_INET;
	  strncpy(ifr.ifr_name, name, IFNAMSIZ-1);
	  
	  uint64_t mac = 0;
	  if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
		for (int n = 0; n < 6; ++n) {
		  mac = (mac << 8) + (ifr.ifr_hwaddr.sa_data[n] & 0xff);
		}
		macList[std::string(name)] = mac;
	  }
	  
	  ::close(fd);
#ifdef TEST
	  printf("%s: %012llx\n", name, mac);
#endif
	}
	fclose(f);
  }

} // namespace

#endif

#ifdef TEST
using namespace ts;
int main(int ac, char*av[]) {
  Socket::MacAddressList_t list;
  Socket::EnumMacAddress(list);
}
#endif

  
