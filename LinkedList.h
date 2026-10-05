#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include <string>

using namespace std;

struct Node {
    std::string name;
    int cost;
    int owner; // 0 = Unowned, 1 = P1, 2 = P2
    
    Node* next;
    Node* prev;
};

class CircularLinkedList {
private:
    Node* head;

public:
    CircularLinkedList() : head(nullptr) {}

    ~CircularLinkedList() {
        if (!head) return;
        Node* current = head;
        do {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        } while (current != head);
    }

   void append(std::string propertyName, int propertyCost) {
       Node* newNode = new Node;
        newNode->name = propertyName;
        newNode->cost = propertyCost;
        newNode->owner = 0;
        
        if (head == nullptr) {
            head = newNode;
            head->next = head;
            head->prev = head;
            return;
        }
        
        Node* tail = head->prev; 
        
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = head;
        head->prev = newNode;
    }

    bool search(std::string searchName) const {
        if (!head) return false;
        Node* current = head;
        do {
            if (current->name == searchName) return true;
            current = current->next;
        } while (current != head); 
        return false;
    }

    void remove(std::string searchName) {
        if (!head) return;
        Node* current = head;
        do {
            if (current->name == searchName) break;
            current = current->next;
        } while (current != head);

        if (current->name != searchName) return;

        if (current->next == current) {
            head = nullptr;
        } else {
            current->prev->next = current->next;
            current->next->prev = current->prev;
            if (current == head) {
                head = current->next;
            }
        }
        delete current;
    }

    void print() const {
        if (!head) return;
        Node* current = head;
        do {
            cout << "[" << current->name << " costs " << current->cost << " bones] <-> ";
            current = current->next;
        } while (current != head);
        cout << "(loops to " << head->name << ")" << endl;
    }

    Node* getHead() const {
        return head;
    }
};

#endif