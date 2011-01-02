// depkg by geohot
// needs openssl and zlib
// 100 lines for your pkg file needs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/aes.h>
#include "zlib.h"

#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short int
#define u8 unsigned char

u64 get_u64(void* vd) {
  u8 *d = (u8*)vd;
  return ((u64)d[0]<<56) | ((u64)d[1]<<48) | ((u64)d[2]<<40) | ((u64)d[3]<<32) | (d[4]<<24) | (d[5]<<16) | (d[6]<<8) | d[7];
}

u32 get_u32(void* vd) {
  u8 *d = (u8*)vd;
  return (d[0]<<24) | (d[1]<<16) | (d[2]<<8) | d[3];
}

u8 pkg_riv[] = {0x4A,0xCE,0xF0,0x12,0x24,0xFB,0xEE,0xDF,0x82,0x45,0xF8,0xFF,0x10,0x21,0x1E,0x6E};
u8 pkg_erk[] = {0xA9,0x78,0x18,0xBD,0x19,0x3A,0x67,0xA1,0x6F,0xE8,0x3A,0x85,0x5E,0x1B,0xE9,0xFB,0x56,0x40,0x93,0x8D,0x4D,0xBC,0xB2,0xCB,0x52,0xC5,0xA2,0xF8,0xB0,0x2B,0x10,0x31};

AES_KEY aes_key;
u8* data;

#define INFLATION_BUFFER_SIZE 0x1000000
u8 inf_buffer[INFLATION_BUFFER_SIZE];

int inf(u8 *source, int source_size, u8 *dest, int* dest_size) {
  int ret;
  unsigned have;
  z_stream strm;

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = source_size;
  strm.next_in = source;
  strm.avail_out = *dest_size;
  strm.next_out = dest;
  ret = inflateInit(&strm);
  if(ret != Z_OK)
    return ret;

  ret = inflate(&strm, Z_NO_FLUSH);
  (*dest_size) -= strm.avail_out;

  (void)inflateEnd(&strm);
  return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int main(int argc, char *argv[]) {
  u8 iv[0x10], ecount_buf[0x10];
  int num;
  FILE *f=fopen(argv[1], "rb");
  fseek(f, 0, SEEK_END);
  int nlen = ftell(f);
  fseek(f, 0, SEEK_SET);
  data = (u8*)malloc(nlen);
  fread(data, 1, nlen, f);
  fclose(f);
  printf("read 0x%X bytes of pkg\n", nlen);

  int metadata_offset = 0x20+get_u32(data+0xC);
  int file_offset = get_u64(data+0x10);

  AES_set_decrypt_key(pkg_erk, 256, &aes_key);
  AES_cbc_encrypt(&data[metadata_offset], &data[metadata_offset], 0x40, &aes_key, pkg_riv, AES_DECRYPT);

  memset(ecount_buf, 0, 16); num=0;
  AES_set_encrypt_key(&data[metadata_offset], 128, &aes_key);
  AES_ctr128_encrypt(&data[metadata_offset+0x40], &data[metadata_offset+0x40], file_offset-0x40-metadata_offset, &aes_key, &data[metadata_offset+0x20], ecount_buf, &num);

  u64 pkg_start = get_u64(data+0xE0);
  u64 pkg_size = get_u64(data+0xE8);

  printf("pkg data @ %llx with size %llx\n", pkg_start, pkg_size);

  memset(ecount_buf, 0, 16); num=0;
  AES_set_encrypt_key(&data[0x230], 128, &aes_key);
  AES_ctr128_encrypt(&data[pkg_start], &data[pkg_start], pkg_size, &aes_key, &data[0x240], ecount_buf, &num);

  int real_size = INFLATION_BUFFER_SIZE;
  printf("inflated: %d\n", inf(&data[pkg_start], pkg_size, inf_buffer, &real_size));

  printf("writing %X\n", real_size);

  FILE *fout = fopen(argv[2], "wb");
  fwrite(inf_buffer, 1, real_size, fout);
  fclose(fout);
exit:
  free(data);
}
