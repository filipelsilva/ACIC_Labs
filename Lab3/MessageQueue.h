#ifndef MessageQueue_h
#define MessageQueue_h

#include <Arduino.h>

#define QUEUE_SIZE 25

enum Event { CLOCK = 0, CAR = 1, MODE = 2, STATUS = 3, SYNC = 4 };

class Message {
  public:
    int destination;
    Event event;
    uint32_t data;
    Message() {};
    Message(int destination, int event, uint32_t data) : destination(destination), event((Event)event), data(data) {};
};

class MessageQueue {
  public:
    MessageQueue(){};
    void push(Message &msg);
    Message& pop();
    bool isEmpty();
  
  private:
    Message queue[QUEUE_SIZE];
    int head = 0;
    int tail = 0;
};

#endif