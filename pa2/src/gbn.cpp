#include "../include/simulator.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;
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

#define A 0
#define B 1
#define timeout 30.0

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
int base;
int next_seq_num;
int window_size;

int b_ack;
vector<struct pkt> send_window;

int find_checksum(struct pkt packet);
struct pkt create_pkt(int seqnum, int acknum, const char* payload);

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  struct pkt packet = create_pkt(next_seq_num, -1, message.data);
  send_window.push_back(packet);
  if(base==next_seq_num)
  {
    starttimer(A,timeout);  
  }
  if(next_seq_num < base + window_size)
  {
     tolayer3(A,send_window[next_seq_num-1]);
  }
  next_seq_num++;
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  if(find_checksum(packet) == packet.checksum && packet.acknum >= base)
  {
    int prev_base = base;
    if(base < packet.acknum + 1)
    {
      base = packet.acknum + 1;
      stoptimer(A);
    }
    for(int i = prev_base + window_size; i < next_seq_num && i < base + window_size; i++)
    {
      tolayer3(A,send_window[i-1]);
    }
    starttimer(A,timeout);
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  // Handled timeout
  for(int i = base; i < base + window_size && i < next_seq_num; i++)
  {
    tolayer3(A,send_window[i-1]);
  }
  starttimer(A,timeout);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  window_size = getwinsize();
  base = 1;
  next_seq_num = 1;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */
/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  if(find_checksum(packet) == packet.checksum && packet.seqnum == b_ack){
    tolayer5(B, packet.payload);
    struct pkt ack_packet = create_pkt(0, b_ack, "");
    tolayer3(B, ack_packet);
    b_ack++;
  }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  b_ack=1;
}

int find_checksum(struct pkt packet){
  int checksum = packet.seqnum + packet.acknum;
  for(int i = 0; i < 20; i++)
  {
    checksum += packet.payload[i];
  }
  return checksum;
}

struct pkt create_pkt(int seqnum, int acknum, const char* payload) {
  struct pkt send_packet;
  send_packet.seqnum = seqnum;
  send_packet.acknum = acknum;
  strncpy(send_packet.payload, payload, 20);
  send_packet.checksum = find_checksum(send_packet);
  return send_packet;
}