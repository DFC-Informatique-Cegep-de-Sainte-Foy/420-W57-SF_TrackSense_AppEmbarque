#pragma once

#include <Arduino.h>

class StringQueue
{
private:
  struct QueueNode
  {
    String data;
    QueueNode *next;
  };

  QueueNode *front;
  QueueNode *rear;
  int size;

public:
  StringQueue();

  void enqueue(const String &data);
  String dequeue();
  bool isEmpty();
  int getSize();
  bool contains(const String &data);
  // API added
  String getNode(int);
};
