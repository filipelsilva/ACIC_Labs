#include "MessageQueue.h"

void MessageQueue::push(Message &msg) {
  queue[tail % QUEUE_SIZE] = msg;
  tail++;
}

Message& MessageQueue::pop() {
  Message& msg = queue[head % QUEUE_SIZE];
  head++;

  return msg;
}

bool MessageQueue::isEmpty() {
  return head == tail;
}