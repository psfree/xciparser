#include <string.h>
#include "xci.h"

int main() {
	FILE * fp = fopen("rrr.xci", "rb");
	if(fp ==NULL)
		return -1;
	fseek (fp , 0 , SEEK_END);
	int size = ftell (fp);
	rewind (fp);
	printf("Read %d bytes from xci\n", size);
	xci_ctx_t ctxt;
	memset(&ctxt, 0 , sizeof(ctxt));
	ctxt.file = fp;
	xci_process(&ctxt);
	fclose(fp);
	return 0;
}

int error=0;

void xci_process(xci_ctx_t *ctx) {
	fseeko64(ctx->file, 0, SEEK_SET);
	if(fread(&(ctx->header), 1, sizeof(xci_header_t), ctx->file) != sizeof(xci_header_t)) {
		printf("XCI header was incorrect size\n");
		return;
	}
	
	if(strncmp((char *)&(ctx->header.magic), "HEAD", 4)) {
		printf("XCI magic is wrong!\n");
		return;
	}

	ctx->hfs0_hash_valid = check_memory_hash_table(ctx->file, ctx->header.hfs0_header_hash, ctx->header.hfs0_offset, ctx->header.hfs0_header_size, ctx->header.hfs0_header_size, 0);
	if(ctx->hfs0_hash_valid != VALIDITY_VALID) {
		error = 1;
		return;
	}
	
	ctx->partition_ctx.file = ctx->file;
	ctx->partition_ctx.offset = ctx->header.hfs0_offset;
	ctx->partition_ctx.name ="root";
	hfs0_process(&ctx->partition_ctx);
				
}


char *xci_get_cartridge_type(xci_ctx_t *ctx) {
    cartridge_type_t cart_type = (cartridge_type_t)ctx->header.cart_type;
    switch (cart_type) {
        case CARTSIZE_2GB: return "2GB";
        case CARTSIZE_4GB: return "4GB";
        case CARTSIZE_8GB: return "8GB";
        case CARTSIZE_16GB: return "16GB";
        default:
            return "Unknown/Invalid";
    }
}
