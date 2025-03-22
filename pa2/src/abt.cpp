#include "../include/simulator.h"
#include <string.h>
#include <stdio.h>
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
#define   A    0
#define   B    1
#define   BUFFER_LEN    1000
int next_seq_A = 0; 
int next_seq_B = 0; 
int ack = 0; 
struct pkt packet_sent; 
struct msg buffer[BUFFER_LEN];
int head_pointer = 0;  
int tail_pointer = 0;
int count = 0;

//checksum implementation
int checksum(struct pkt packet) {
    int checksum = packet.seqnum + packet.acknum;
    for (int i = 0; i < 20; i++) {
        checksum += packet.payload[i];
    }
    return checksum;

}

// set and return packet
struct pkt getPacketDetail(int seqnum, int acknum, const char* data) {
    struct pkt packet;
    packet.seqnum = seqnum;
    packet.acknum = acknum;
    strncpy(packet.payload, data, 20);
    packet.checksum = checksum(packet);
    return packet;
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
   if (ack == 0) { 
        struct pkt packet = getPacketDetail(next_seq_A, 0, message.data);
        tolayer3(A, packet);
        starttimer(A, 20.0);
        packet_sent = packet;
        ack = 1;
    } else {
        
        if (count < BUFFER_LEN) {
            buffer[tail_pointer] = message;
            tail_pointer = (tail_pointer + 1) % BUFFER_LEN; 
            count++;
        }
    }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
   if (packet.checksum == checksum(packet) && packet.acknum == packet_sent.seqnum) {
        stoptimer(A);
        next_seq_A = 1 - next_seq_A;
        ack = 0;

        if (count > 0) {
            struct msg message = buffer[head_pointer];
            head_pointer = (head_pointer + 1) % BUFFER_LEN;
            count--;
            A_output(message);
        
        }
    }

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
   //retransmission only if A is waiting
    if (ack == 1) {
        tolayer3(A, packet_sent);
        starttimer(A, 20.0);
    }
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  next_seq_A = 0;
  ack = 0;

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    
     if (packet.checksum == checksum(packet)) {
        if (packet.seqnum == next_seq_B) {
            tolayer5(B, packet.payload);
            tolayer3(B, getPacketDetail(0, next_seq_B, ""));
            next_seq_B = 1 - next_seq_B;
        } else {
            //resending the last sent packet
            tolayer3(B, getPacketDetail(0, 1 - next_seq_B, ""));
        }
    }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  next_seq_B = 0;
}
