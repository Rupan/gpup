#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "hmac.h"

static const unsigned char jig_master_key[20] = {
  0x46, 0xDC, 0xEA, 0xD3, 0x17, 0xFE, 0x45, 0xD8, 0x09, 0x23,
  0xEB, 0x97, 0xE4, 0x95, 0x64, 0x10, 0xD4, 0xCD, 0xB2, 0xC2
};

/*
Revocation list (as of firmware 3.15):
0, 2, 13, 32, 34, 176, 241
*/

int jig_key(uint16_t id, unsigned char out[20]) {
  uint16_t id_be;
  hmac_ctx ctx[1];

  id_be = htons(id);
  hmac_sha_begin(ctx);
  if( hmac_sha_key(jig_master_key, 20, ctx) != HMAC_OK ) {
    return -1;
  }
  hmac_sha_data((unsigned char *)&id_be, 2, ctx);
  hmac_sha_end(out, 20, ctx);
  return 0;
}

int main(int argc, char **argv) {
  int32_t fd, i;
  unsigned char all_mac[20*65536], *cur_mac;

  if(argc != 2) {
    printf("Usage: ./jig_key <outfile.bin>\n");
    return -1;
  }

  cur_mac = all_mac;
  for(i=0; i<=65535; i++) {
    if( jig_key(i, cur_mac) < 0 ) {
      printf("Unable to generate jig key.\n");
      return -1;
    }
    cur_mac += 20;
  }

  fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0644);
  if( fd == -1 ) {
    printf("Unable to open jig_keys.bin.\n");
    return -1;
  }
  if( write(fd, all_mac, 20*65536) != (20*65536) ) {
    printf("Unable to write out jig key data.\n");
    return -1;
  }
  close(fd);

  return 0;
}
