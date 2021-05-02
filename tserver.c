/*
TCP server
Description: Simulate TCP 3-way handshake and closing a TCP connection in the application layer
using a client-server architecture.
Author: Shangda Li
*/

#include "tcp.h"
#include <stddef.h> //use offsetof()


int main(int argc, char *argv[]) {
    int sockfd, connfd;
    struct sockaddr_in servaddr;
    struct tcp_hdr tcp_seg, recv_seg;
    unsigned short int d_offset = offsetof(struct tcp_hdr, data);
    FILE *fp;

    int PORT;
    if(argc == 2) {
      PORT = atoi(argv[1]);
    }
    else {
      printf("Please run server with a valid port number\n");
      exit(0);
    }

    // creat socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind socket
    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    //listen to incoming connections
    listen(sockfd, 10);

    // Accept connection
    connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);

    /*--------------------------------------------------------
            TCP handshaking
     ------------------------------------------------------------*/
    printf("Start TCP handshaking...\n\n");

    bzero(&tcp_seg, sizeof(tcp_seg));
    bzero(&recv_seg, sizeof(recv_seg));

    //First time received from client
    if (recv(connfd, &recv_seg, sizeof(recv_seg), 0) > 0) {
      printf("First time received from client.\n");
      //check checksum
      if (checksum(recv_seg) == 0) {
        //check flags
        if (recv_seg.hdr_flags = 4096*d_offset + 2){
          bzero(&tcp_seg, sizeof(tcp_seg));
          tcp_seg.src = (unsigned short int)PORT;
          tcp_seg.des = (unsigned short int)PORT;
          tcp_seg.seq = rand(); //Assign a random value for initial server sequence number
          tcp_seg.ack = recv_seg.seq + 1; //assign the received sequence number from the client + 1 as the server acknowledgement number
          tcp_seg.hdr_flags = 4096*d_offset + 16 + 2;  //Set both the SYN bit and ACK bit to 1
          tcp_seg.cksum = checksum(tcp_seg); //Compute checksum

          //print out variables' values of TCP segment on console and write to the file
          printval(tcp_seg, "server.out", "w");

          //Send the created connection granted (SYNACK) segment to the client.
          send(connfd, &tcp_seg, sizeof(tcp_seg), 0);
          printf("Sent the created connection granted (SYNACK) segment to the client.\n");

        }

      }
      else{
        //If the segment has errors after checksum calculation, discard the segment
        bzero(&tcp_seg, sizeof(tcp_seg));
        bzero(&recv_seg, sizeof(recv_seg));
        printf("checksum error!");
      }

    }

    //bzero(&tcp_seg, sizeof(tcp_seg));
    bzero(&recv_seg, sizeof(recv_seg));

    //Second time received from client
    if (recv(connfd, &recv_seg, sizeof(recv_seg), 0) > 0) {
      printf("Second time received from client.\n");

      //check checksum
      if (checksum(recv_seg) == 0) {
        // If the client has acknowledged for the SYNACK segment
        //check flags
        if (recv_seg.hdr_flags = 4096*d_offset + 16) {
          //print on the console
          printf("The connection has been established.\n\n");

          //write to the file
          fp = fopen("server.out", "a");
          fprintf(fp, "The connection has been established.\n\n");
          fclose(fp);

          printf("wrote to server.out file\n");

        }

      }
      else{
        //If the segment has errors after checksum calculation, discard the segment
        bzero(&tcp_seg, sizeof(tcp_seg));
        bzero(&recv_seg, sizeof(recv_seg));
        printf("checksum error!");
      }

    }

    /*------------------------------------------------------------
      closing of a TCP connection
     ----------------------------------------------------------*/
    printf("\n\nClosing TCP connection...\n\n");

    //bzero(&tcp_seg, sizeof(tcp_seg));
    bzero(&recv_seg, sizeof(recv_seg));

    if (recv(connfd, &recv_seg, sizeof(recv_seg), 0) > 0) {
      printf("Received close request segment from client.\n");

      //compute checksum
      if (checksum(recv_seg) == 0) {
        //check flags
        if (recv_seg.hdr_flags = 4096*d_offset + 1) {
          bzero(&tcp_seg, sizeof(tcp_seg));
          tcp_seg.src = (unsigned short int)PORT;
          tcp_seg.des = (unsigned short int)PORT;
          tcp_seg.seq = 512;
          tcp_seg.ack = recv_seg.seq + 1; //assign the received sequence number from the client + 1 as the server acknowledgement number
          tcp_seg.hdr_flags = 4096*d_offset + 16;//Set the ACK bit to 1
          tcp_seg.hdr_flags = (unsigned short int)sizeof(tcp_seg);
          tcp_seg.cksum = checksum(tcp_seg); //Compute checksum

          //print out variables' values of TCP segment on console and write to the file
          printval(tcp_seg, "server.out", "a");

          //Send the created acknowledgment segment to the client
          send(connfd, &tcp_seg, sizeof(tcp_seg), 0);
          printf("Sent the created acknowledgment segment to the client.\n");

        }

      }
      else{
        //If the segment has errors after checksum calculation, discard the segment
        bzero(&tcp_seg, sizeof(tcp_seg));
        bzero(&recv_seg, sizeof(recv_seg));
        printf("checksum error!");
      }

    }

    bzero(&tcp_seg, sizeof(tcp_seg));
    tcp_seg.src = (unsigned short int)PORT;
    tcp_seg.des = (unsigned short int)PORT;
    tcp_seg.seq = 512;
    tcp_seg.ack = recv_seg.seq + 1; //assign the received sequence number from the client + 1 as the server acknowledgement number
    tcp_seg.hdr_flags = 4096*d_offset + 1;//Set the FIN bit to 1
    tcp_seg.cksum = checksum(tcp_seg); //Compute checksum

    //print out variables' values of TCP segment on console and write to the file
    printval(tcp_seg, "server.out", "a");

    //Send the created close acknowledgment segment to the client
    send(connfd, &tcp_seg, sizeof(tcp_seg), 0);
    printf("Sent the created close acknowledgment segment to the client.\n");

    //bzero(&tcp_seg, sizeof(tcp_seg));
    bzero(&recv_seg, sizeof(recv_seg));

    if (recv(connfd, &recv_seg, sizeof(recv_seg), 0) > 0) {
      printf("Received acknowledgement segment from client.\n");

      //compute checksum
      if (checksum(recv_seg) == 0) {
        // check flags
        if (recv_seg.hdr_flags = 4096*d_offset + 16) {
          //print on the console
          printf("The connection has been closed.\n\n");

          //write to the file
          fp = fopen("server.out", "a");
          fprintf(fp, "The connection has been closed.\n\n");
          fclose(fp);

          printf("wrote to server.out file\n\n");
        }

      }
      else{
        //If the segment has errors after checksum calculation, discard the segment
        bzero(&tcp_seg, sizeof(tcp_seg));
        bzero(&recv_seg, sizeof(recv_seg));
        printf("checksum error!");
      }

    }

    //close the socket
    close(connfd);
}
