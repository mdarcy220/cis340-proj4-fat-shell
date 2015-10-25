

// Concatenates two bytes into an unsigned short (uint16)
unsigned short concat_uint8_uint16(unsigned char b1, unsigned char b2) {
	return (unsigned short)(((unsigned short) b1) << 8) | ((unsigned short) b2);
}
