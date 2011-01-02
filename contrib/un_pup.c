/*
 - un_pup 1.1
  © dasda 2009 <dasda42@yah00.com>
  Licensed under GNU GPL v3:
    http://www.gnu.org/licenses/gpl.txt
*/
 
/* OBS! when compiling on non-intel platforms - comment the line below: */
#define BIG_ENDIAN
 
#include <stdio.h>
#include <stdlib.h>
typedef unsigned long long int u64;
typedef unsigned           int u32;
 
/* big endian conversions for intel cpus */
#ifdef BIG_ENDIAN
  u32 be_32 (u32 num)
  {
    return (((num&0x000000FF)<<24)|((num&0x0000FF00)<< 8)
           |((num&0x00FF0000)>> 8)|((num&0xFF000000)>>24));
  }
  u64 be_64 (u64 num)
  {
    return   ((u64) be_32(num&0x00000000FFFFFFFFull)<<32)
           | ((u64) be_32((num&0xFFFFFFFF00000000ull)>>32));
  }
#endif /* BIG_ENDIAN */
 
/* version numbers constants */
#define UNPUP_VER 1
#define UNPUP_REV 1
 
/* pup structs and related values */
#define PUP_MAGIC "SCEUF"
typedef struct _pup_h {
   u64 magic;
   u64 ver;
   u64 ver_img;
   u64 num_files;
   u64 len_header;
   u64 len_pup;
} pup_h;
typedef struct _file_h {
   u64 id;
   u64 off;
   u64 len;
   u64 nothing;
} file_h;
 
/* function declarations */
char* get_name_from_id(u64 id);
void exit_err(char* msg);
 
/* public variables */
char* file_buf=NULL;
 
/* extract a file */
void do_file(file_h* file, FILE* f_pup)
{
  /* store old pos to restore position later */
  int old_pos = ftell(f_pup);
 
  /* allocate memory for file */
  file_buf = malloc(file->len);
  if(file_buf == NULL)
    exit_err("not enough memory for out-file allocation");
 
  /* read file from offset */
  if(fseek(f_pup, file->off, SEEK_SET) != 0)
    exit_err("setting out-file pointer failed");
  if(fread(file_buf, file->len, 1, f_pup) != 1)
    exit_err("out-file cannot be read");
 
  /* create new file plus print info */
  char* file_name = get_name_from_id(file->id);
  printf("    ->name   %s\n", file_name);
  FILE* f_out = fopen(file_name, "wb");
  if(f_out == NULL)
    exit_err("out-file cannot be created");
 
  /* write data to file */
  if(fwrite(file_buf, file->len, 1, f_out) != 1)
    exit_err("cannot write to out-file");
 
  /* restore old file position */
  if(fseek(f_pup, old_pos, SEEK_SET) != 0)
    exit_err("out-file pointer cannot be restored");
 
  /* free memory and make sure
     it won't get free'd twice */
  free(file_buf);
  file_buf = NULL;
}
 
/* main function */
int main(int argc, char* argv[])
{
  /* warm welcome message plus usage message */
  printf("\n ---------------------------------");
  printf("\n - un_pup %d.%d", UNPUP_VER, UNPUP_REV);
  printf("\n  (c) dasda 2009 <dasda42@yah00.com>");
  printf("\n ---------------------------------\n\n");
  if(argc != 2)
  {
    printf("  USAGE: %s PS3UPDAT.PUP\n", argv[0]);
    return 0;
  }
 
  /* open file */
  FILE* f_pup = fopen(argv[1], "rb");
  if(f_pup == NULL)
    exit_err("package-file not found");
 
  /* read header */
  pup_h header;
  if(fread(&header, sizeof(pup_h), 1, f_pup) != 1)
    exit_err("package-file cannot be read");
 
  /* confirm header magic value */
  if(strcmp((char*)&header.magic, PUP_MAGIC))
    exit_err("invalid package header");
 
  /* fix proper endianness if needed */
  #ifdef BIG_ENDIAN
    header.ver_img = be_64(header.ver_img);
    header.num_files = be_64(header.num_files);
    header.len_header = be_64(header.len_header);
    header.len_pup = be_64(header.len_pup);
  #endif /* BIG_ENDIAN */
 
  /* print package info */
  printf("   ImageVersion: 0x%llx\n", header.ver_img);
  printf("   NumberOfFiles: 0x%llx\n", header.num_files);
  printf("   HeaderSize: 0x%llx\n", header.len_header);
  printf("   PackageSize: 0x%llx\n\n", header.len_pup);
 
  /* start reading the file table */
  int i;
  file_h file_header;
  for(i=0; i<header.num_files; i++)
  {
    /* read file header */
    printf("   Processing file #%d..\n", i);
    if(fread(&file_header, sizeof(file_h), 1, f_pup) != 1)
      exit_err("entry couldn't be read");
 
    /* fix proper endianness if needed */
    #ifdef BIG_ENDIAN
      file_header.len = be_64(file_header.len);
      file_header.off = be_64(file_header.off);
      file_header.id = be_64(file_header.id);
    #endif /* BIG_ENDIAN */
 
    /* print file info */
    printf("    ->length 0x%llx\n", file_header.len);
    printf("    ->offset 0x%llx\n", file_header.off);
    printf("    ->id     0x%llx\n", file_header.id);
 
    /* extract file! */
    do_file(&file_header, f_pup);
  }
 
  /* good night! */
  fclose(f_pup);
  printf("\n  OK: Exiting!\n");
  return 0;
}
 
/* convert from a fixed id
   to a made-up file-name */
char* get_name_from_id(u64 id)
{
  if(id == 0x100)
    return "ps3version.txt";
  if(id == 0x101)
    return "license.txt";
  if(id == 0x102)
    return "TOP_SIKRIT.txt";
  if(id == 0x103)
    return "console_type.txt";
  if(id == 0x200)
    return "ps3swu.self";
  if(id == 0x201)
    return "vsh.tar";
  if(id == 0x202)
    return "dots.txt";
  if(id == 0x300)
    return "update_files.tar";
  else
    exit_err("unknown file-type");
}
 
/* exit clean-up with error message */
void exit_err(char* msg)
{
  printf("   ERROR: %s\n", msg);
  free(file_buf);
  exit(1);
}
 
