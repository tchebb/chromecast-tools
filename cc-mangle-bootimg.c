#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s INFILE OUTFILE KEYID\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *in = fopen(argv[1], "r");
	FILE *out = fopen(argv[2], "w");
	if (in == NULL || out == NULL) {
		fprintf(stderr, "Invalid input or output file path\n");
		exit(EXIT_FAILURE);
	}

	// Get the input file length.
	fseek(in, 0, SEEK_END);
	long in_len = ftell(in);
	rewind(in);

	// Read in the Android header, but don't write it back out just yet.
	unsigned char header[0x240] = {0};
	fread(header, 1, 0x240, in);

	// Add the signature magic.
	char *endptr;
	long key_id = strtol(argv[3], &endptr, 0);
	if (*argv[3] == '\0' || *endptr != '\0') {
		fprintf(stderr, "Invalid key ID\n");
		exit(EXIT_FAILURE);
	}
	if (key_id != 0x2 && key_id != 0x82) {
		fprintf(stderr, "Warning: Key ID is neither 0x2 (NAND) nor 0x82 (USB)\n");
	}
	*((unsigned*)(header + 0x40)) = (unsigned)key_id;

	// Add the length. The bootloader will add the 0x80 back on.
	*((unsigned*)(header + 0x7c)) = (unsigned)in_len - 0x80;

	// Now write the header out.
	fwrite(header, 1, 0x240, out);

	// Copy the rest of the file in 4k chunks.
	unsigned char buffer[0x1000] = {0};
	size_t i;
	while ((i = fread(buffer, 1, 0x1000, in)) != 0)
		fwrite(buffer, 1, i, out);

	return EXIT_SUCCESS;
}

