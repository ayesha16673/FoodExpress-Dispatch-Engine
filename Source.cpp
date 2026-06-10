#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;

const int MAX_ORDER = 100;

// ==================== ORDER CLASS ====================
class Order {
public:
    int orderId;
    string customerName;
    string restaurantName;
    int priority;
    int prepTime;
    int deadline;
    string category;
    string status;

    Order() {
        orderId = 0;
        priority = 0;
        prepTime = 0;
        deadline = 0;
        customerName = "";
        restaurantName = "";
        category = "Regular";
        status = "PLACED";
    }

    Order(int id, string cust, string rest, int pri, int prep, int dead, string cat) {
        orderId = id;
        customerName = cust;
        restaurantName = rest;
        priority = pri;
        prepTime = prep;
        deadline = dead;
        category = cat;
        status = "QUEUED";
    }

    void show() {
        cout << "ID:" << orderId
            << " | " << customerName
            << " | " << restaurantName
            << " | Pri:" << priority
            << " | " << category
            << " | " << status << endl;
    }
};

// ==================== STACK (Cancel/Undo ke liye) ====================
class OrderStack {
    Order data[20];
    int top;
public:
    OrderStack() { top = -1; }

    bool empty() { return top == -1; }

    void push(Order o) {
        if (top < 19) {
            top++;
            data[top] = o;
        }
    }

    Order pop() {
        Order temp;
        if (top == -1) return temp;
        temp = data[top];
        top--;
        return temp;
    }
};

// ==================== MAX HEAP (Priority Scheduler) ====================
class MaxHeap {
    Order heap[MAX_ORDER];
    int n;
    bool msg;

public:
    MaxHeap() {
        n = 0;
        msg = true;
    }

    void silent(bool x) { msg = x; }
    bool empty() { return n == 0; }
    int size() { return n; }

    void swapO(int i, int j) {
        Order t = heap[i];
        heap[i] = heap[j];
        heap[j] = t;
    }

    // Zyada priority pehle, same priority pe kam deadline pehle
    bool isMoreUrgent(int a, int b) {
        if (heap[a].priority != heap[b].priority)
            return heap[a].priority > heap[b].priority;
        return heap[a].deadline < heap[b].deadline;
    }

    void up(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (isMoreUrgent(i, p)) {
                swapO(i, p);
                i = p;
            }
            else break;
        }
    }

    void down(int i) {
        while (1) {
            int l = 2 * i + 1;
            int r = 2 * i + 2;
            int big = i;
            if (l < n && isMoreUrgent(l, big)) big = l;
            if (r < n && isMoreUrgent(r, big)) big = r;
            if (big != i) { swapO(i, big); i = big; }
            else break;
        }
    }

    int findId(int id) {
        for (int i = 0; i < n; i++)
            if (heap[i].orderId == id) return i;
        return -1;
    }

    // Order add karo
    void insert(Order o) {
        if (n >= MAX_ORDER) { cout << "Heap full" << endl; return; }
        heap[n] = o;
        heap[n].status = "QUEUED";
        up(n);
        n++;
        if (msg) cout << "Order added successfully!" << endl;
    }

    Order top() {
        Order x;
        if (n == 0) return x;
        return heap[0];
    }

    // Highest priority order process karo
    Order removeMax() {
        Order x;
        if (n == 0) {
            if (msg) cout << "No orders in queue." << endl;
            return x;
        }
        Order ans = heap[0];
        ans.status = "PREPARED";
        heap[0] = heap[n - 1];
        n--;
        if (n > 0) down(0);
        if (msg) cout << "Processing order #" << ans.orderId << endl;
        return ans;
    }

    // Priority update karo
    void changePri(int id, int p) {
        int i = findId(id);
        if (i == -1) { cout << "Order not found." << endl; return; }
        int old = heap[i].priority;
        heap[i].priority = p;
        if (p > old) up(i); else down(i);
        cout << "Priority updated successfully." << endl;
    }

    // Order cancel karo (stack mein push)
    bool cancel(int id, OrderStack& stk) {
        int i = findId(id);
        if (i == -1) { cout << "Order not found." << endl; return false; }
        Order rem = heap[i];
        rem.status = "CANCELLED";
        stk.push(rem);
        heap[i] = heap[n - 1];
        n--;
        if (i < n) { down(i); up(i); }
        cout << "Order #" << id << " cancelled." << endl;
        return true;
    }

    // Order delay karo
    void delay(int id, int less) {
        int i = findId(id);
        if (i == -1) { cout << "Order not found." << endl; return; }
        heap[i].priority -= less;
        if (heap[i].priority < 1) heap[i].priority = 1;
        heap[i].deadline += 5;
        down(i);
        cout << "Order #" << id << " delayed." << endl;
    }

    // Reschedule - priority aur deadline dono change
    void reschedule(int id, int newPri, int newDead) {
        int i = findId(id);
        if (i == -1) { cout << "Order not found." << endl; return; }
        heap[i].priority = newPri;
        heap[i].deadline = newDead;
        up(i);
        down(i);
        cout << "Order rescheduled (Pri:" << newPri << " Deadline:" << newDead << ")" << endl;
    }

    // Sab active orders dikhao
    void show() {
        cout << "\n--- Active Orders ---" << endl;
        if (n == 0) { cout << "No active orders." << endl; return; }
        for (int i = 0; i < n; i++) heap[i].show();
    }

    // Category se filter karo
    void byCategory(string cat) {
        cout << "\n--- " << cat << " Orders ---" << endl;
        bool f = false;
        for (int i = 0; i < n; i++) {
            if (heap[i].category == cat) { heap[i].show(); f = true; }
        }
        if (!f) cout << "No orders found in this category." << endl;
    }
};