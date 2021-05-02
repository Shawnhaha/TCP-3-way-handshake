/*
TCP header file
Description: Simulate TCP 3-way handshake and closing a TCP connection in the application layer
using a client-server architecture.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//TCP Segment struct
struct tcp_hdr{
                unsigned short int src; //16-bit source port
                unsigned short int des; //destination port
                unsigned int seq; //32-bit sequence number
                unsigned int ack; //32-bit acknowledgement number
                unsigned short int hdr_flags; //4-bit data offset or header length (in multiples of 32-bit)
                //6-bit reserved section
                //6-bit flags
                unsigned short int rec; //16-bit receive window for flow control
                unsigned short int cksum; //16-bit checksum
                unsigned short int ptr; //16-bit urgent data pointer
                unsigned int opt;  //32-bit Options
                char data [1024];// = NULL; //1024-byte data
};



unsigned short int checksum(struct tcp_hdr tcp_seg) {
  unsigned short int cksum_arr[524];
  unsigned int i,sum=0, cksum, wrap;

  memcpy(cksum_arr, &tcp_seg, 1048); // Copying 1048 bytes


  for (i=0;i<524;i++)            // Compute sum
  {
	 sum = sum + cksum_arr[i];
  }

  wrap = sum >> 16;             // Wrap around
  sum = sum & 0x0000FFFF;
  sum = wrap + sum;

  wrap = sum >> 16;             // Wrap around once more as the previous sum could have generated a carry
  sum = sum & 0x0000FFFF;
  cksum = wrap + sum;


  return 0xFFFF^cksum;

}

//print out variables' values of TCP segment on console and write to the file
void printval(struct tcp_hdr tcp_seg, char* filename, char* condi) {
  //print out on console
  printf("source port: 0x%04X\n"
  "destination port: 0x%04X\n"
  "sequence number: 0x%08X\n"
  "acknowledgement number: 0x%08X\n"
  "header length and flags: 0x%04X\n"
  "checksum: 0x%04X\n\n",
  tcp_seg.src, tcp_seg.des, tcp_seg.seq, tcp_seg.ack, tcp_seg.hdr_flags, tcp_seg.cksum);

  //write to the file
  FILE *fp;

  fp = fopen(filename, condi);
  fprintf(fp, "source port: 0x%04X\n"
  "destination port: 0x%04X\n"
  "sequence number: 0x%08X\n"
  "acknowledgement number: 0x%08X\n"
  "header length and flags: 0x%04X\n"
  "checksum: 0x%04X\n\n",
  tcp_seg.src, tcp_seg.des, tcp_seg.seq, tcp_seg.ack, tcp_seg.hdr_flags, tcp_seg.cksum);
  fclose(fp);
  printf("Wrote to %s file.\n\n", filename);

}
