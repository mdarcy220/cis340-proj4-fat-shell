

// Concatenates two bytes into an unsigned short (uint16)
unsigned short concat_uint8_uint16(unsigned char b1, unsigned char b2) {
	return (unsigned short)(((unsigned short) b1) << 8) | ((unsigned short) b2);
}


// Concatenates four bytes into an unsigned int (uint32)
unsigned int concat_uint8_uint32(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4) {
	return (unsigned int)((((unsigned int) b1) << 24) | (((unsigned int) b2) << 16) | (((unsigned int) b3) << 8) | ((unsigned int) b4));
}
