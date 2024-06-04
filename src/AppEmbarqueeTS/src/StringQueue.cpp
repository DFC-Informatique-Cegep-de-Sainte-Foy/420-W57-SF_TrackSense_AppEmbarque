#include "StringQueue.h"

StringQueue::StringQueue()
{
    front = rear = nullptr;
    size = 0;
};

void StringQueue::enqueue(const String &data)
{
    QueueNode *newNode = new QueueNode;
    newNode->data = data;
    newNode->next = nullptr;

    if (rear == nullptr)
    {
        front = rear = newNode;
    }
    else
    {
        rear->next = newNode;
        rear = newNode;
    }

    size++;
};

String StringQueue::dequeue()
{
    if (front == nullptr)
    {
        return "";
    }

    String data = front->data;
    QueueNode *temp = front;
    front = front->next;

    if (front == nullptr)
    {
        rear = nullptr;
    }

    delete temp;
    size--;
    return data;
};

bool StringQueue::isEmpty()
{
    return front == nullptr;
};

int StringQueue::getSize()
{
    return size;
};

bool StringQueue::contains(const String &data)
{
    QueueNode *current = front;
    while (current != nullptr)
    {
        if (current->data == data)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}
String StringQueue::getNode(int p_index)
{
    // if (this->getSize() <= p_index)
    // {
    //     throw std::invalid_argument("Index out of boundary");
    // }
    // TODO: parcourir la liste

    return String("Trajet _name->" + String(666));
};