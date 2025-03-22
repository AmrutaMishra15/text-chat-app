#include "../include/simulator.h"
#include <cstring>
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
#define WINDOW 10
#define BUFFER_LEN 1000

struct pkt packetSent[WINDOW];
int seq_a = 0; 
int next_seq_a = 0;

struct msg buffer[BUFFER_LEN];
int buffer_next = 0;

int expect_seq_b = 0;


//checksum implementation
int checksum(struct pkt *packet) {
  int checksum = packet->seqnum + packet->acknum;
  for (int i = 0; i < 20; i++) {
    checksum += packet->payload[i];
  }
  return checksum;
}

// set and return packet
struct pkt getPacketDetail(int seqnum, int acknum, struct msg *buffer) {
  struct pkt packet;
  int index = seqnum % BUFFER_LEN;
  packet.seqnum = seqnum;
  packet.acknum = acknum;
  strncpy(packet.payload, buffer[index].data, 20);
  packet.checksum = checksum(&packet);
  return packet;
}

void send_packets() {
  while ((next_seq_a < seq_a + WINDOW) && (next_seq_a < buffer_next)) {
    struct pkt packet = getPacketDetail(next_seq_a, 0, buffer);
    int index = next_seq_a % WINDOW;
    packetSent[index] = packet;
    tolayer3(0, packet);
    if (seq_a == next_seq_a) {
      starttimer(0, 20.0);
    }
    next_seq_a++;
  }
}
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  if (buffer_next - seq_a >= WINDOW && seq_a + WINDOW > buffer_next) {
        return;
  }
  int index = buffer_next % BUFFER_LEN;
  buffer[index] = message;
  buffer_next++;
  send_packets();

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  if (packet.checksum != checksum(&packet)) {
    return;
  }
  if (packet.acknum >= seq_a && packet.acknum < next_seq_a) {
    seq_a = packet.acknum + 1;
    if (seq_a == next_seq_a) {
      stoptimer(0);
    } else {
      starttimer(0, 20.0);
    }
    send_packets();
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  starttimer(0, 20.0);
  for (int i = seq_a; i < next_seq_a; i++) {
    int index = i % WINDOW;
    tolayer3(0, packetSent[index]);
  }
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  seq_a = 0;
  next_seq_a = 0;
  buffer_next = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  if (packet.checksum != checksum(&packet)) {
    return;
  }
  if (packet.seqnum == expect_seq_b) {
    tolayer5(1, packet.payload);
    struct pkt ackpkt;
    ackpkt.acknum = expect_seq_b;
    ackpkt.seqnum = 0;
    memset(ackpkt.payload, 0, 20);
    ackpkt.checksum = checksum(&ackpkt);
    tolayer3(1, ackpkt);
    expect_seq_b++;
  } else {
    struct pkt ackpkt;
    ackpkt.acknum = expect_seq_b - 1;
    ackpkt.seqnum = 0;
    memset(ackpkt.payload, 0, 20);
    ackpkt.checksum = checksum(&ackpkt);
    tolayer3(1, ackpkt);
  }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  expect_seq_b = 0;
}
