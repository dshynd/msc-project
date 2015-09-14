/*
 * Header file for print_tc function which prints a String
 * to a SerialPacket conforming to the TinyOs protocol
 * used by the serial forwarder in the TmoteConnect
 * A checksum is calculated over the header and the String to be sent
 * according to the algorithm from the TinyOS source tree
 *
 * @author: Jerome Bucka
 *
 */


extern void print_tc(char *s);

