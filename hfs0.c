#include <string.h>
#include "hfs0.h"


validity_t check_memory_hash_table(FILE * fp, unsigned char * hash_table, uint64_t data_offset, uint64_t data_length, uint64_t block_size, int fblock) {
	if(block_size == 0)
		return 0;
	
	unsigned char curhash[0x20];
	uint64_t read_size = block_size;
	unsigned char * block = malloc(block_size);
	if(block ==NULL) {
		return VALIDITY_UNCHECKED;
	}
	
	
	validity_t res = VALIDITY_VALID;
	unsigned char * cur_entry = hash_table;
	for(long off=0; off < data_length; off+= read_size) {
		fseeko64(fp, off + data_offset, SEEK_SET);
		if(off+read_size > data_length) {
			memset(block, 0, read_size);
			read_size = data_length - off;
		}
		if(fread(block, 1, read_size, fp) != read_size) {
			res= VALIDITY_INVALID;
			break;
		}
		sha256_hash_buffer(curhash, block, fblock ? block_size : read_size);
		if(memcmp(curhash, cur_entry, 0x20) != 0){
			res= VALIDITY_INVALID;
			break;
		}
		cur_entry += 0x20;
	}
	free(block);
	return res;
	
}

int err=0;
void hfs0_process(hfs0_ctx_t *ctx) {
	hfs0_header_t raw_hdr;
	fseeko64(ctx->file, ctx->offset, SEEK_SET);
	if(fread(&raw_hdr, 1, sizeof(raw_hdr), ctx->file) != sizeof(raw_hdr)) {
		err = 1;
		return;
	}
	
	if(strncmp(&raw_hdr.magic, "HFS0", 4)) {
		err = 2;
		return;
	}
	
	uint64_t hdr_size = hfs0_get_header_size(&raw_hdr);
	ctx->header = malloc(hdr_size);
	
	fseeko64(ctx->file, ctx->offset, SEEK_SET);
	if(fread(ctx->header, 1, hdr_size, ctx->file) != hdr_size) {
		err = 3;
		return;
	}
	
	//validate files
	uint64_t max_size = 0x1ULL;
	max_size <<= 48;
	uint64_t cur_off = 0;
	for(int i = 0; i < ctx->header->num_files; i++) {
		hfs0_file_entry_t  *cur_file = hfs0_get_file_entry(ctx->header, i);
		if(cur_file->offset < cur_off) {
			err = 4;
		}
		cur_off += cur_file->size;
	}
	
	hfs0_save(ctx);
}


void hfs0_save(hfs0_ctx_t *ctx) {
	
}