#include "ac_allocator.h"
#include "ac_lz4.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2 || (!strcmp(argv[1], "-d") && argc < 4)) {
    printf("%s <output>\n", argv[0]);
    printf("%s -d <input> <output>\n", argv[0]);
  }
  if (argc == 2) {
    ac_lz4_t *l = ac_lz4_init(1, s64kb, false, true);
    FILE *out = fopen(argv[1], "wb");
    uint32_t header_size = 0;
    const char *header = ac_lz4_get_header(l, &header_size);
    fwrite(header, header_size, 1, out);
    uint32_t block_size = ac_lz4_block_size(l);
    uint32_t compressed_size = ac_lz4_compressed_size(l);
    char *block = (char *)ac_malloc(block_size);
    char *cblock = (char *)ac_malloc(compressed_size);
    uint32_t len;
    for (int j = 0; j < 1000; j++) {
      for (int i = 'A'; i < 'Z'; i++) {
        memset(block, i, block_size);
        uint32_t len = ac_lz4_compress_block(l, block, block_size, cblock,
                                             compressed_size);
        fwrite(cblock, len, 1, out);
      }
    }
    uint32_t zero = 0;
    // fwrite(&zero, sizeof(uint32_t), 1, out);
    fclose(out);
    ac_lz4_destroy(l);
    ac_free(block);
    ac_free(cblock);
  } else {
    int in = open(argv[2], O_RDONLY);
    char header[7];
    if (read(in, header, 7) != 7)
      return -1;

    ac_lz4_t *l = ac_lz4_init_decompress(header, 7);
    FILE *out = NULL;
    if (strcmp(argv[3], "null")) {
      if (!strcmp(argv[3], "-"))
        out = stdout;
      else
        out = fopen(argv[3], "wb");
    }
    uint32_t block_size = ac_lz4_block_size(l);
    uint32_t compressed_size = ac_lz4_compressed_size(l);
    uint32_t block_crc_size = ac_lz4_block_header_size(l) - 4;
    uint32_t buffer_size = block_size + compressed_size;
    if (buffer_size < 10 * 1024 * 1024)
      buffer_size = 10 * 1024 * 1024;
    char *buffer = (char *)ac_malloc(buffer_size);

    char *out_buffer = (char *)ac_malloc(buffer_size);
    char *outp = out_buffer;
    char *outep = outp + buffer_size - block_size;

    int n;
    char *bufferp = buffer;
    char *bufferep = buffer + buffer_size;

    uint32_t buffer_len = buffer_size;
    while ((n = read(in, bufferp, buffer_len)) > 0) {
      char *p = buffer;
      char *ep = bufferp + n;
      while (p < ep) {
        if (p + sizeof(uint32_t) > ep) {
          memcpy(buffer, p, ep - p);
          bufferp = buffer + (ep - p);
          buffer_len = buffer_size - (ep - p);
          break;
        }
        uint32_t length = *(uint32_t *)p;
        bool compressed = true;
        if ((length & 0x80000000U) == 0x80000000U) {
          length -= 0x80000000U;
          compressed = false;
        }
        p += sizeof(uint32_t);
        if (p + length > ep) {
          memcpy(buffer, p, ep - p);
          bufferp = buffer + (ep - p);
          buffer_len = buffer_size - (ep - p);
          break;
        }
        if (outp > outep) {
          if (out)
            fwrite(out_buffer, outp - out_buffer, 1, out);
          outp = out_buffer;
        }
        if (length)
          n = ac_lz4_decompress(l, p, length, outp, block_size, compressed);
        else
          n = ac_lz4_finish(l, p);
        if (n < 0)
          return -1;
        p += length + block_crc_size;
        outp += n;
      }
    }
    if (out) {
      fwrite(out_buffer, outp - out_buffer, 1, out);
      fclose(out);
    }
    close(in);
    ac_lz4_destroy(l);
    ac_free(out_buffer);
    ac_free(buffer);
  }
  return 0;
}
