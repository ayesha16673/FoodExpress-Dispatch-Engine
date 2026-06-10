// ================================================
// FoodExpress Dispatch Engine
// File: kitchen_rider.cpp
// Member: Ayesha Yaseen | Roll No: 79
// Contains: IntQueue (Queue), Kitchen Module, RiderMgr Module
// ================================================

#include <iostream>
#include <string>
using namespace std;

const int MAX_REST = 10;
const int MAX_RIDER = 10;

// ==================== QUEUE (Kitchen Line) ====================
class IntQueue {
    int arr[50];
    int front, rear, size;

public:
    IntQueue() {
        front = 0;
        rear = -1;
        size = 0;
    }

    bool empty() { return size == 0; }

    void enq(int x) {
        if (size < 50) {
            rear = (rear + 1) % 50;
            arr[rear] = x;
            size++;
        }
    }

    bool deq(int& x) {
        if (size == 0) return false;
        x = arr[front];
        front = (front + 1) % 50;
        size--;
        return true;
    }

    void display() {
        if (size == 0) {
            cout << "Queue empty" << endl;
            return;
        }
        int i = front;
        cout << "Queue: ";
        for (int k = 0; k < size; k++) {
            cout << arr[i];
            if (k < size - 1) cout << " -> ";
            i = (i + 1) % 50;
        }
        cout << endl;
    }
};

// ==================== KITCHEN MODULE ====================
struct Restaurant {
    int id;
    string name;
    int capacity;
    int load;
    IntQueue q;
};

class Kitchen {
    Restaurant r[MAX_REST];
    int total;

public:
    Kitchen() { total = 0; }

    void add(int id, string name, int cap) {
        if (total < MAX_REST) {
            r[total].id = id;
            r[total].name = name;
            r[total].capacity = cap;
            r[total].load = 0;
            total++;
        }
    }

    int find(string name) {
        for (int i = 0; i < total; i++)
            if (r[i].name == name) return i;
        return -1;
    }

    // Overloaded kitchen ka load doosri pe shift karo
    bool rebalance(int oid, string skip) {
        int best = -1, minL = 9999;
        for (int i = 0; i < total; i++) {
            if (r[i].name == skip) continue;
            if (r[i].load < r[i].capacity && r[i].load < minL) {
                minL = r[i].load;
                best = i;
            }
        }
        if (best == -1) {
            cout << "All kitchens full!" << endl;
            return false;
        }
        r[best].load++;
        r[best].q.enq(oid);
        cout << "Order rebalanced to " << r[best].name << endl;
        return true;
    }

    // Order kitchen ko assign karo
    bool assign(int oid, string name) {
        int i = find(name);
        if (i == -1) {
            cout << "Restaurant not found." << endl;
            return false;
        }
        if (r[i].load >= r[i].capacity) {
            cout << name << " is full, rebalancing..." << endl;
            return rebalance(oid, name);
        }
        r[i].load++;
        r[i].q.enq(oid);
        cout << "Order sent to " << name << ", estimated wait: ~" << (r[i].load * 5) << " min" << endl;
        return true;
    }

    // Order complete karo
    void complete(string name) {
        int i = find(name);
        if (i == -1) {
            cout << "Restaurant not found." << endl;
            return;
        }
        int oid;
        if (r[i].q.deq(oid)) {
            if (r[i].load > 0) r[i].load--;
            cout << "Order #" << oid << " is ready at " << name << "!" << endl;
        }
        else {
            cout << "No orders in queue." << endl;
        }
    }

    // Sab kitchens ka status dikhao
    void show() {
        cout << "\n--- Kitchen Status ---" << endl;
        for (int i = 0; i < total; i++) {
            cout << r[i].name << " | Load: " << r[i].load << "/" << r[i].capacity;
            if (r[i].load >= r[i].capacity) cout << " [FULL]";
            cout << endl;
            r[i].q.display();
        }
    }
};

// ==================== RIDER MODULE ====================
struct Rider {
    int id;
    string name;
    int load;
    int cap;
    int node;
};

class RiderMgr {
    Rider list[MAX_RIDER];
    int total;

public:
    RiderMgr() { total = 0; }

    void add(int id, string name, int cap, int node) {
        if (total < MAX_RIDER) {
            list[total].id = id;
            list[total].name = name;
            list[total].cap = cap;
            list[total].load = 0;
            list[total].node = node;
            total++;
        }
    }

    // Best available rider ko assign karo
    bool assign(int oid, int dest) {
        int best = -1, score = 9999;
        for (int i = 0; i < total; i++) {
            if (list[i].load >= list[i].cap) continue;
            int d = list[i].node - dest;
            if (d < 0) d = -d;
            int s = list[i].load * 10 + d;
            if (s < score) {
                score = s;
                best = i;
            }
        }
        if (best == -1) {
            cout << "No rider available right now." << endl;
            return false;
        }
        list[best].load++;
        list[best].node = dest;
        cout << list[best].name << " assigned to order #" << oid << endl;
        return true;
    }

    // Delivery complete
    void done(int rid, int oid) {
        for (int i = 0; i < total; i++) {
            if (list[i].id == rid) {
                if (list[i].load > 0) list[i].load--;
                cout << "Order #" << oid << " delivered by " << list[i].name << "!" << endl;
                return;
            }
        }
        cout << "Rider not found." << endl;
    }

    // Sab riders dikhao
    void show() {
        cout << "\n--- Riders ---" << endl;
        for (int i = 0; i < total; i++) {
            cout << "ID:" << list[i].id
                << " | " << list[i].name
                << " | Load:" << list[i].load << "/" << list[i].cap
                << " | Node:" << list[i].node << endl;
        }
    }
};