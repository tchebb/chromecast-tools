#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s INFILE OUTFILE PADDING\n", argv[0]);
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

	// Write the requested padding.
	char *endptr;
	long padding_len = strtol(argv[3], &endptr, 0);
	if (*argv[3] == '\0' || *endptr != '\0') {
		fprintf(stderr, "Invalid padding value\n");
		exit(EXIT_FAILURE);
	}

	char buffer[0x1000] = {0};
	while (padding_len >= 0x1000) {
		fwrite(buffer, 1, 0x1000, out);
		padding_len -= 0x1000;
	}
	fwrite(buffer, 1, padding_len, out);

	// Read in the Android header, but don't write it back out just yet.
	char header[0x240] = {0};
	fread(header, 1, 0x240, in);

	// Add the signature magic.
	*((unsigned*)(header + 0x40)) = 0x82;

	// Add the length. The bootloader will add the 0x80 back on.
	*((unsigned*)(header + 0x7c)) = (unsigned)in_len - 0x80;

	// Now write the header out.
	fwrite(header, 1, 0x240, out);

	// Copy the rest of the file in 4k chunks.
	size_t i;
	while ((i = fread(buffer, 1, 0x1000, in)) != 0)
		fwrite(buffer, 1, i, out);

	return EXIT_SUCCESS;
}

