/*
TCP client
Description: Simulate TCP 3-way handshake and closing a TCP connection in the application layer
using a client-server architecture.
Author: Shangda Li
*/

#include "tcp.h"
#include <stddef.h> //use offsetof()


#define SERVER_IP "129.120.151.96"


int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;
    struct tcp_hdr tcp_seg, recv_seg;
    unsigned short int d_offset = offsetof(struct tcp_hdr, data);
    FILE *fp;

    int PORT;
    if(argc == 2) {
      PORT = atoi(argv[1]);
    }
    else {
      printf("Please run client with a valid port number\n");
      exit(0);
    }

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = INADDR_ANY;
    /* Convert IPv4 and IPv6 addresses from text to binary form */
    inet_pton(AF_INET,SERVER_IP,&(servaddr.sin_addr));
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    /*--------------------------------------------------
            TCP handshaking
     ---------------------------------------------------*/
    printf("Start TCP handshaking...\n\n");

    bzero(&tcp_seg, sizeof(tcp_seg));
    bzero(&recv_seg, sizeof(recv_seg));

    //Create a connection request TCP segment
    //assign segment variables
    tcp_seg.src = (unsigned short int)PORT;
    tcp_seg.des = (unsigned short int)PORT;
    tcp_seg.seq = rand(); //Assign a random value for the initial client sequence number
    tcp_seg.ack = 0; //assign zero for the client acknowledgement number
    tcp_seg.hdr_flags = 4096*d_offset + 2; //Set the SYN bit to 1
    tcp_seg.cksum = checksum(tcp_seg); //Compute the 16-bit checksum for the entire TCP segment and populate the checksum field

    //print out variables' values of TCP segment on console and write to the file
    printval(tcp_seg, "client.out", "w");

    //Send the created connection request segment to the server
    send(sockfd, &tcp_seg, sizeof(tcp_seg), 0);

    printf("Sent the created connection request segment to the server.\n");

    //bzero(&tcp_seg, sizeof(tcp_seg));
    bzero(&recv_seg, sizeof(recv_seg));

    //receive TCP segment from the server
    if (recv(sockfd, &recv_seg, sizeof(recv_seg), 0) > 0) {
      printf("Received TCP segment from the server.\n");
      // check checksum
      if (checksum(recv_seg) == 0) {
        //check flags
        if (recv_seg.hdr_flags = 4096*d_offset + 16 + 2) {
          bzero(&tcp_seg, sizeof(tcp_seg));
          tcp_seg.src = (unsigned short int)PORT;
          tcp_seg.des = (unsigned short int)PORT;
          tcp_seg.seq = recv_seg.ack; //Assign the received acknowledgment number from the server as the client sequence number
          tcp_seg.ack = recv_seg.seq + 1; //assign the received sequence number from the server + 1 as the client acknowledgement number
          tcp_seg.hdr_flags = 4096*d_offset + 16;  //Set the ACK bit to 1
          tcp_seg.cksum = checksum(tcp_seg); //Compute checksum

          //print out variables' values of TCP segment on console and write to the file
          printval(tcp_seg, "client.out", "a");

          //Send the created client ACK segment to the server.
          send(sockfd, &tcp_seg, sizeof(tcp_seg), 0);
          printf("Sent the created client ACK segment to the server.\n");

        }

      }
      else{
        //If the segment has errors after checksum calculation, discard the segment
        bzero(&tcp_seg, sizeof(tcp_seg));
        bzero(&recv_seg, sizeof(recv_seg));
        printf("checksum error!");
      }
    }

    /*----------------------------------------------------------------
      closing of a TCP connection
     ----------------------------------------------------------------*/
    printf("\n\nClosing TCP connection...\n\n");

    bzero(&tcp_seg, sizeof(tcp_seg));
    tcp_seg.src = (unsigned short int)PORT;
    tcp_seg.des = (unsigned short int)PORT;
    //assign segment variables
    tcp_seg.seq = 1024; //Assign 1024 as client sequence number
    tcp_seg.ack = 512; //assign 512 as acknowledgement number
    tcp_seg.hdr_flags = 4096*d_offset + 1;//Set the FIN bit to 1
    tcp_seg.cksum = checksum(tcp_seg); //Compute the 16-bit checksum for the entire TCP segment and populate the checksum field

    //print out variables' values of TCP segment on console and write to the file
    printval(tcp_seg, "client.out", "a");

    //Send the created close request segment to the server.
    send(sockfd, &tcp_seg, sizeof(tcp_seg), 0);
    printf("Sent the created close request segment to the server.\n");

    //bzero(&tcp_seg, sizeof(tcp_seg));
    bzero(&recv_seg, sizeof(recv_seg));

    //receive TCP segment from the server
    if (recv(sockfd, &recv_seg, sizeof(recv_seg), 0) > 0) {
      printf("Received TCP segment from the server.\n");

      // compute checksum
      if (checksum(recv_seg) == 0) {
        //check flags
        if (recv_seg.hdr_flags = 4096*d_offset + 16) {
          //wait for next segment

          bzero(&recv_seg, sizeof(recv_seg));

          //receive TCP segment from the server
          if (recv(sockfd, &recv_seg, sizeof(recv_seg), 0) > 0) {
            printf("Received TCP segment from the server.\n");
            //check checksum
            if (checksum(recv_seg) == 0) {
              //check flags
              if (recv_seg.hdr_flags = 4096*d_offset + 1) {
                bzero(&tcp_seg, sizeof(tcp_seg));
                tcp_seg.src = (unsigned short int)PORT;
                tcp_seg.des = (unsigned short int)PORT;
                tcp_seg.seq = recv_seg.ack;  //Assign acknowledgment number as client sequence number + 1
                tcp_seg.ack = recv_seg.seq + 1;  //acknowledgement number equal to server sequence number + 1
                tcp_seg.hdr_flags = 4096*d_offset + 16;//Set the ACK bit to 1
                tcp_seg.cksum = checksum(tcp_seg); //Compute the 16-bit checksum for the entire TCP segment and populate the checksum field

                //print out variables' values of TCP segment on console and write to the file
                printval(tcp_seg, "client.out", "a");

                //Send the created acknowledgement segment to the server .
                send(sockfd, &tcp_seg, sizeof(tcp_seg), 0);
                printf("Sent the created acknowledgement segment to the server .\n");

                printf("Sleep for 2 sec...\n");
                sleep(2); //exit the client program after a sleep of 2 seconds.
              }
            }
            else{
              //If the segment has errors after checksum calculation, discard the segment
              bzero(&tcp_seg, sizeof(tcp_seg));
              bzero(&recv_seg, sizeof(recv_seg));
              printf("checksum error!");
            }
          }
        }
      }
      else{
        //If the segment has errors after checksum calculation, discard the segment
        bzero(&tcp_seg, sizeof(tcp_seg));
        bzero(&recv_seg, sizeof(recv_seg));
        printf("checksum error!");
      }
    }


    // close the socket
    close(sockfd);
  }
