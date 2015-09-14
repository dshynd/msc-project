/*
 * Source file for print_tc function which prints a String
 * to a SerialPacket conforming to the TinyOs protocol
 * used by the serial forwarder in the TmoteConnect
 * A checksum is calculated over the header and the String to be sent
 * according to the algorithm from the TinyOS source tree
 *
 * @author: Jerome Bucka
 *
 */

#include <stdio.h> /* For printf() */
#include <string.h>
#include <stdint.h>

/*static uint16_t crcByte(uint16_t crc, uint8_t b) {
	uint8_t i;

	crc = crc ^ b << 8;
	i = 8;
	do
		if (crc & 0x8000)
			crc = crc << 1 ^ 0x1021;
		else
			crc = crc << 1;
	while (--i);

	return crc;
}

static void print_tc(char *s) {

	int len = strlen(s) + 1; // + 1 for terminator

	//Create and put initial flags on serial stream
	int flag = 0x7E;
	int nak = 0x45;
	putchar(flag);
	putchar(nak);

	int header[] = { 0x00, 0xFF, 0xFF, 0x00, 0x01, len, 0x00, 0x93 }; //create header data

	int i;
	i = 0;

	//putchar header and data
	int j;
	for (j = 0; j < 8; j++) {

		putchar(header[j]);

	}
	for (j = 0; j < len; j++) {

		putchar(s[j]);

	}

	//CRC CALCULATION!
	uint16_t crc;
	uint8_t byte;

	crc = 0;
	byte = 0x45;
	uint16_t result2;
	result2 = crcByte(crc, byte);

	int k;
	for (k = 0; k < 8; k++) {
		result2 = crcByte(result2, header[k]); //first compute CRC on the header
	}
	for (k = 0; k < len; k++) {

		result2 = crcByte(result2, s[k]);  //then take that CRC and modify it using data bytes

	}

	//Finally put CRC (2 bytes) on the serial
	putchar(result2);
	putchar(result2 >> 8);
	putchar(flag);

}*/

