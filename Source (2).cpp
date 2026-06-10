#include <iostream>
#include <string>
#include <climits>
#include <ctime>
#include <cstdlib>
using namespace std;

const int MAX_ORDER = 100;
const int MAX_REST = 10;
const int MAX_RIDER = 10;
const int MAX_NODES = 8;
const int INF = 9999;

// order ki basic info
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
        cout << "ID:" << orderId << " | " << customerName << " | " << restaurantName;
        cout << " | Pri:" << priority << " | " << category << " | " << status << endl;
    }
};

// STACK - cancel undo ke liye
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

// QUEUE - kitchen line
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

// LINKED LIST - history
struct HistNode {
    int orderId;
    string status;
    string tm;
    HistNode* next;
};

class HistoryList {
    HistNode* head;
    int count;
public:
    HistoryList() {
        head = NULL;
        count = 0;
    }

    string nowTime() {
        time_t t = time(0);
        char buf[20];
        struct tm info;
#ifdef _MSC_VER
        localtime_s(&info, &t);
#else
        localtime_r(&t, &info);
#endif
        strftime(buf, 20, "%H:%M:%S", &info);
        return string(buf);
    }

    void add(int id, string st) {
        HistNode* n = new HistNode;
        n->orderId = id;
        n->status = st;
        n->tm = nowTime();
        n->next = head;
        head = n;
        count++;
    }

    void showAll() {
        cout << "\n--- Order History ---" << endl;
        HistNode* p = head;
        if (p == NULL) {
            cout << "No record" << endl;
            return;
        }
        while (p != NULL) {
            cout << p->tm << " | #" << p->orderId << " | " << p->status << endl;
            p = p->next;
        }
    }

    void timeline(int id) {
        cout << "\n--- Timeline #" << id << " ---" << endl;
        HistNode* p = head;
        bool ok = false;
        while (p != NULL) {
            if (p->orderId == id) {
                cout << p->tm << " -> " << p->status << endl;
                ok = true;
            }
            p = p->next;
        }
        if (!ok) cout << "Not found" << endl;
    }

    void filter(string st) {
        cout << "\n--- Filter: " << st << " ---" << endl;
        HistNode* p = head;
        bool ok = false;
        while (p != NULL) {
            if (p->status == st) {
                cout << "#" << p->orderId << " at " << p->tm << endl;
                ok = true;
            }
            p = p->next;
        }
        if (!ok) cout << "Nothing found" << endl;
    }

    int total() { return count; }
};

// MAX HEAP - priority scheduler
class MaxHeap {
    Order heap[MAX_ORDER];
    int n;
    HistoryList* hist;
    bool msg;
public:
    MaxHeap() {
        n = 0;
        hist = NULL;
        msg = true;
    }

    void linkHist(HistoryList* h) { hist = h; }
    void silent(bool x) { msg = x; }
    bool empty() { return n == 0; }
    int size() { return n; }

    void swapO(int i, int j) {
        Order t = heap[i];
        heap[i] = heap[j];
        heap[j] = t;
    }

    // zyada priority pehle, same priority pe kam deadline pehle
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
            if (big != i) {
                swapO(i, big);
                i = big;
            }
            else break;
        }
    }

    int findId(int id) {
        for (int i = 0; i < n; i++)
            if (heap[i].orderId == id) return i;
        return -1;
    }

    void insert(Order o) {
        if (n >= MAX_ORDER) {
            cout << "Heap full" << endl;
            return;
        }
        heap[n] = o;
        heap[n].status = "QUEUED";
        if (hist != NULL) {
            hist->add(o.orderId, "PLACED");
            hist->add(o.orderId, "QUEUED");
        }
        up(n);
        n++;
        if (msg) cout << "Order added" << endl;
    }

    Order top() {
        Order x;
        if (n == 0) return x;
        return heap[0];
    }

    Order removeMax() {
        Order x;
        if (n == 0) {
            if (msg) cout << "No order" << endl;
            return x;
        }
        Order ans = heap[0];
        ans.status = "PREPARED";
        if (hist != NULL) hist->add(ans.orderId, "PREPARED");

        heap[0] = heap[n - 1];
        n--;
        if (n > 0) down(0);

        if (msg) cout << "Processing #" << ans.orderId << endl;
        return ans;
    }

    void changePri(int id, int p) {
        int i = findId(id);
        if (i == -1) {
            cout << "Not found" << endl;
            return;
        }
        int old = heap[i].priority;
        heap[i].priority = p;
        if (p > old) up(i);
        else down(i);
        if (hist != NULL) hist->add(id, "PRIORITY_UPDATED");
        cout << "Priority changed" << endl;
    }

    bool cancel(int id, OrderStack& stk) {
        int i = findId(id);
        if (i == -1) {
            cout << "Not found" << endl;
            return false;
        }
        Order rem = heap[i];
        rem.status = "CANCELLED";
        stk.push(rem);
        if (hist != NULL) hist->add(id, "CANCELLED");

        heap[i] = heap[n - 1];
        n--;
        if (i < n) {
            down(i);
            up(i);
        }
        cout << "Cancelled" << endl;
        return true;
    }

    void delay(int id, int less) {
        int i = findId(id);
        if (i == -1) {
            cout << "Not found" << endl;
            return;
        }
        heap[i].priority = heap[i].priority - less;
        if (heap[i].priority < 1) heap[i].priority = 1;
        heap[i].deadline = heap[i].deadline + 5; // delay se deadline badhti hai
        down(i);
        if (hist != NULL) hist->add(id, "DELAYED");
        cout << "Order delayed" << endl;
    }

    // dynamic rescheduling - priority aur deadline dono change
    void reschedule(int id, int newPri, int newDead) {
        int i = findId(id);
        if (i == -1) {
            cout << "Not found" << endl;
            return;
        }
        heap[i].priority = newPri;
        heap[i].deadline = newDead;
        up(i);
        down(i);
        if (hist != NULL) hist->add(id, "RESCHEDULED");
        cout << "Order rescheduled (Pri:" << newPri << " Dead:" << newDead << ")" << endl;
        cout << "Heap position updated by priority + deadline" << endl;
    }

    void show() {
        cout << "\n--- Active Orders ---" << endl;
        if (n == 0) {
            cout << "Empty" << endl;
            return;
        }
        for (int i = 0; i < n; i++) heap[i].show();
    }

    void byCategory(string cat) {
        cout << "\n--- " << cat << " Orders ---" << endl;
        bool f = false;
        for (int i = 0; i < n; i++) {
            if (heap[i].category == cat) {
                heap[i].show();
                f = true;
            }
        }
        if (!f) cout << "Not found" << endl;
    }
};

// BST - search records
struct Record {
    int orderId;
    string customer;
    string restaurant;
    string category;
    string status;
};

struct TNode {
    Record data;
    TNode* left;
    TNode* right;
};

class BST {
    TNode* root;
public:
    BST() { root = NULL; }

    TNode* addNode(TNode* node, Record r) {
        if (node == NULL) {
            TNode* n = new TNode;
            n->data = r;
            n->left = NULL;
            n->right = NULL;
            return n;
        }
        if (r.orderId < node->data.orderId)
            node->left = addNode(node->left, r);
        else if (r.orderId > node->data.orderId)
            node->right = addNode(node->right, r);
        else
            node->data = r;
        return node;
    }

    void insert(Record r) {
        root = addNode(root, r);
    }

    TNode* findNode(TNode* node, int id) {
        if (node == NULL) return NULL;
        if (id == node->data.orderId) return node;
        if (id < node->data.orderId) return findNode(node->left, id);
        return findNode(node->right, id);
    }

    void update(int id, string st) {
        TNode* f = findNode(root, id);
        if (f != NULL) f->data.status = st;
    }

    void searchId(int id) {
        TNode* f = findNode(root, id);
        if (f == NULL) {
            cout << "Not found" << endl;
            return;
        }
        cout << "\nRecord Found:" << endl;
        cout << "ID: " << f->data.orderId << endl;
        cout << "Customer: " << f->data.customer << endl;
        cout << "Restaurant: " << f->data.restaurant << endl;
        cout << "Status: " << f->data.status << endl;
    }

    void inorder(TNode* node) {
        if (node == NULL) return;
        inorder(node->left);
        cout << "#" << node->data.orderId << " | " << node->data.customer;
        cout << " | " << node->data.restaurant << " | " << node->data.status << endl;
        inorder(node->right);
    }

    void showAll() {
        cout << "\n--- All Records ---" << endl;
        inorder(root);
    }

    void toArray(TNode* node, Record arr[], int& c) {
        if (node == NULL) return;
        toArray(node->left, arr, c);
        arr[c] = node->data;
        c++;
        toArray(node->right, arr, c);
    }

    void searchName(string name) {
        Record arr[100];
        int c = 0;
        toArray(root, arr, c);
        cout << "\n--- Customer: " << name << " ---" << endl;
        bool f = false;
        for (int i = 0; i < c; i++) {
            if (arr[i].customer == name) {
                cout << "#" << arr[i].orderId << " | " << arr[i].status << endl;
                f = true;
            }
        }
        if (!f) cout << "Not found" << endl;
    }

    void searchRest(string name) {
        Record arr[100];
        int c = 0;
        toArray(root, arr, c);
        cout << "\n--- Restaurant: " << name << " ---" << endl;
        bool f = false;
        for (int i = 0; i < c; i++) {
            if (arr[i].restaurant == name) {
                cout << "#" << arr[i].orderId << " | " << arr[i].status << endl;
                f = true;
            }
        }
        if (!f) cout << "Not found" << endl;
    }

    void searchStatus(string st) {
        Record arr[100];
        int c = 0;
        toArray(root, arr, c);
        cout << "\n--- Status: " << st << " ---" << endl;
        bool f = false;
        for (int i = 0; i < c; i++) {
            if (arr[i].status == st) {
                cout << "#" << arr[i].orderId << " | " << arr[i].customer << endl;
                f = true;
            }
        }
        if (!f) cout << "Not found" << endl;
    }

    // bubble sort
    void sortedList() {
        Record arr[100];
        int c = 0;
        toArray(root, arr, c);

        for (int i = 0; i < c - 1; i++) {
            for (int j = 0; j < c - i - 1; j++) {
                if (arr[j].orderId > arr[j + 1].orderId) {
                    Record t = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = t;
                }
            }
        }

        cout << "\n--- Sorted By ID ---" << endl;
        for (int i = 0; i < c; i++)
            cout << "#" << arr[i].orderId << " | " << arr[i].customer << endl;
    }
};

// kitchen module
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

    bool assign(int oid, string name) {
        int i = find(name);
        if (i == -1) {
            cout << "Restaurant not found" << endl;
            return false;
        }
        if (r[i].load >= r[i].capacity) {
            cout << name << " full, rebalancing..." << endl;
            return rebalance(oid, name);
        }
        r[i].load++;
        r[i].q.enq(oid);
        cout << "Sent to " << name << ", wait ~" << (r[i].load * 5) << " min" << endl;
        return true;
    }

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
            cout << "All kitchens full" << endl;
            return false;
        }
        r[best].load++;
        r[best].q.enq(oid);
        cout << "Sent to " << r[best].name << endl;
        return true;
    }

    void complete(string name) {
        int i = find(name);
        if (i == -1) {
            cout << "Not found" << endl;
            return;
        }
        int oid;
        if (r[i].q.deq(oid)) {
            if (r[i].load > 0) r[i].load--;
            cout << "Order #" << oid << " ready at " << name << endl;
        }
        else {
            cout << "Queue empty" << endl;
        }
    }

    void show() {
        cout << "\n--- Kitchen Status ---" << endl;
        for (int i = 0; i < total; i++) {
            cout << r[i].name << " | " << r[i].load << "/" << r[i].capacity;
            if (r[i].load >= r[i].capacity) cout << " [FULL]";
            cout << endl;
            r[i].q.display();
        }
    }
};

// rider module
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

    bool assign(int oid, int dest, HistoryList* h) {
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
            cout << "No rider free" << endl;
            return false;
        }
        list[best].load++;
        list[best].node = dest;
        cout << list[best].name << " got order #" << oid << endl;
        if (h != NULL) {
            h->add(oid, "ASSIGNED");
            h->add(oid, "PICKED");
        }
        return true;
    }

    void done(int rid, int oid, HistoryList* h) {
        for (int i = 0; i < total; i++) {
            if (list[i].id == rid) {
                if (list[i].load > 0) list[i].load--;
                cout << "Delivered by " << list[i].name << endl;
                if (h != NULL) h->add(oid, "DELIVERED");
                return;
            }
        }
        cout << "Rider not found" << endl;
    }

    void show() {
        cout << "\n--- Riders ---" << endl;
        for (int i = 0; i < total; i++) {
            cout << "ID:" << list[i].id << " | " << list[i].name;
            cout << " | Load:" << list[i].load << "/" << list[i].cap;
            cout << " | Node:" << list[i].node << endl;
        }
    }
};

// graph module
class Graph {
    int mat[MAX_NODES][MAX_NODES];
    bool block[MAX_NODES][MAX_NODES];
    string name[MAX_NODES];
public:
    Graph() {
        for (int i = 0; i < MAX_NODES; i++) {
            name[i] = "Node" + to_string(i);
            for (int j = 0; j < MAX_NODES; j++) {
                if (i == j) mat[i][j] = 0;
                else mat[i][j] = INF;
                block[i][j] = false;
            }
        }
    }

    void setName(int i, string n) { name[i] = n; }

    void road(int u, int v, int w) {
        mat[u][v] = w;
        mat[v][u] = w;
    }

    void blockRoad(int u, int v) {
        block[u][v] = true;
        block[v][u] = true;
        cout << "Road blocked" << endl;
    }

    void openRoad(int u, int v) {
        block[u][v] = false;
        block[v][u] = false;
        cout << "Road opened" << endl;
    }

    bool open(int u, int v) {
        if (block[u][v]) return false;
        if (mat[u][v] == INF) return false;
        return true;
    }

    void dijkstra(int src, int dest) {
        int dist[MAX_NODES], parent[MAX_NODES];
        bool vis[MAX_NODES];

        for (int i = 0; i < MAX_NODES; i++) {
            dist[i] = INF;
            parent[i] = -1;
            vis[i] = false;
        }
        dist[src] = 0;

        for (int k = 0; k < MAX_NODES; k++) {
            int u = -1;
            for (int i = 0; i < MAX_NODES; i++) {
                if (!vis[i] && (u == -1 || dist[i] < dist[u])) u = i;
            }
            if (u == -1 || dist[u] == INF) break;
            vis[u] = true;

            for (int v = 0; v < MAX_NODES; v++) {
                if (!open(u, v)) continue;
                if (dist[u] + mat[u][v] < dist[v]) {
                    dist[v] = dist[u] + mat[u][v];
                    parent[v] = u;
                }
            }
        }

        if (dest == -1) {
            cout << "\nFrom " << name[src] << endl;
            for (int i = 0; i < MAX_NODES; i++) {
                if (i == src) continue;
                cout << name[src] << " -> " << name[i] << " : ";
                if (dist[i] == INF) cout << "No path";
                else cout << dist[i] << " km";
                cout << endl;
            }
            return;
        }

        if (dist[dest] == INF) {
            cout << "No path" << endl;
            return;
        }

        int path[20], len = 0, cur = dest;
        while (cur != -1) {
            path[len] = cur;
            len++;
            cur = parent[cur];
        }

        cout << "\nShortest: " << name[src] << " to " << name[dest];
        cout << " = " << dist[dest] << " km" << endl;
        cout << "Path: ";
        for (int i = len - 1; i >= 0; i--) {
            cout << name[path[i]];
            if (i > 0) cout << " -> ";
        }
        cout << endl;
        cout << "Cost: Rs." << dist[dest] * 10 << endl;
    }

    void compareRoute(int s, int d) {
        cout << "\n--- Main Route ---" << endl;
        dijkstra(s, d);
        if (mat[s][d] != INF) {
            int save = mat[s][d];
            mat[s][d] = INF;
            mat[d][s] = INF;
            cout << "\n--- Alternate Route ---" << endl;
            dijkstra(s, d);
            mat[s][d] = save;
            mat[d][s] = save;
        }
    }

    void bfs(int src) {
        bool vis[MAX_NODES];
        int q[20], f = 0, r = 0;
        for (int i = 0; i < MAX_NODES; i++) vis[i] = false;

        vis[src] = true;
        q[r] = src;
        r++;

        cout << "\nBFS: ";
        while (f < r) {
            int u = q[f];
            f++;
            cout << name[u] << " ";
            for (int v = 0; v < MAX_NODES; v++) {
                if (!vis[v] && open(u, v)) {
                    vis[v] = true;
                    q[r] = v;
                    r++;
                }
            }
        }
        cout << endl;
    }

    void dfsRec(int u, bool vis[]) {
        vis[u] = true;
        cout << name[u] << " ";
        for (int v = 0; v < MAX_NODES; v++) {
            if (!vis[v] && open(u, v))
                dfsRec(v, vis);
        }
    }

    void dfs(int src) {
        bool vis[MAX_NODES];
        for (int i = 0; i < MAX_NODES; i++) vis[i] = false;
        cout << "\nDFS: ";
        dfsRec(src, vis);
        cout << endl;
    }

    // road block hone par naya route + REROUTED history
    void reroute(int s, int d, int bu, int bv, int orderId, HistoryList* h) {
        cout << "\n--- Reroute Demo ---" << endl;
        cout << "Pehle wala route:" << endl;
        dijkstra(s, d);

        blockRoad(bu, bv);
        cout << "Road block ke baad naya route:" << endl;
        dijkstra(s, d);

        if (h != NULL) {
            h->add(orderId, "REROUTED");
            cout << "Order #" << orderId << " marked as REROUTED" << endl;
        }

        openRoad(bu, bv);
    }
};

void perfInsert(int n) {
    MaxHeap h;
    h.silent(true);
    clock_t s = clock();
    for (int i = 0; i < n; i++) {
        Order o(i + 1, "Ali", "KFC", rand() % 10 + 1, 10, 30, "Regular");
        h.insert(o);
    }
    double t = (double)(clock() - s) / CLOCKS_PER_SEC;
    cout << "Insert " << n << " = " << t << " sec" << endl;
}

void perfDelete(int n) {
    MaxHeap h;
    h.silent(true);
    if (n > MAX_ORDER) n = MAX_ORDER;
    for (int i = 0; i < n; i++) {
        Order o(i + 1, "Ali", "KFC", 5, 10, 30, "Regular");
        h.insert(o);
    }
    clock_t s = clock();
    while (!h.empty()) h.removeMax();
    double t = (double)(clock() - s) / CLOCKS_PER_SEC;
    cout << "Delete " << n << " = " << t << " sec" << endl;
}

void showComplexity() {
    cout << "\n--- Time Complexity ---" << endl;
    cout << "Heap insert/delete : O(log n)" << endl;
    cout << "Reschedule         : O(log n)" << endl;
    cout << "Linked list add    : O(1)" << endl;
    cout << "Stack push/pop     : O(1)" << endl;
    cout << "Queue enq/deq      : O(1)" << endl;
    cout << "BST search         : O(log n)" << endl;
    cout << "Bubble sort        : O(n^2)" << endl;
    cout << "Dijkstra           : O(V^2)" << endl;
    cout << "BFS/DFS            : O(V+E)" << endl;
}

void showMemory(MaxHeap& heap, HistoryList& hist) {
    cout << "\n--- Memory Usage ---" << endl;
    cout << "1 Order object     : " << sizeof(Order) << " bytes" << endl;
    cout << "Heap (max)         : " << MAX_ORDER << " x " << sizeof(Order)
        << " = " << (MAX_ORDER * sizeof(Order)) << " bytes" << endl;
    cout << "Heap (used now)    : " << heap.size() << " orders" << endl;
    cout << "Graph matrix       : " << MAX_NODES << "x" << MAX_NODES
        << " ints = " << (MAX_NODES * MAX_NODES * sizeof(int)) << " bytes" << endl;
    cout << "Stack (undo)       : 20 orders max" << endl;
    cout << "Queue (kitchen)    : 50 ids per restaurant" << endl;
    cout << "History nodes      : " << hist.total() << " (linked list)" << endl;
    cout << "BST nodes          : dynamic (new per record)" << endl;
    cout << "Total fixed memory : mostly arrays in classes" << endl;
}

void scaleTest() {
    int a[] = { 10, 20, 50, 100 };
    cout << "\n--- Scalability ---" << endl;
    for (int i = 0; i < 4; i++) {
        int n = a[i];
        MaxHeap h;
        h.silent(true);
        clock_t s = clock();
        for (int j = 0; j < n; j++) {
            Order o(j + 1, "U", "R", 5, 5, 20, "Regular");
            h.insert(o);
        }
        double t = (double)(clock() - s) / CLOCKS_PER_SEC;
        cout << "N=" << n << " time=" << t << " sec" << endl;
    }
}

void ethics() {
    cout << "\n===== ETHICAL CONSIDERATIONS =====" << endl;
    cout << "1. Customer data should be stored securely." << endl;
    cout << "2. Rider locations should only be shared with user consent." << endl;
    cout << "3. Priority scheduling should remain fair for all customers." << endl;
    cout << "4. Real systems should use encryption and secure authentication." << endl;
}

int main() {
    srand(time(0));

    MaxHeap heap;
    Kitchen kitchen;
    RiderMgr rider;
    Graph g;
    HistoryList history;
    BST tree;
    OrderStack stk;

    heap.linkHist(&history);

    kitchen.add(1, "PizzaHut", 5);
    kitchen.add(2, "KFC", 4);
    kitchen.add(3, "McDonalds", 3);

    rider.add(1, "Ali", 5, 0);
    rider.add(2, "Ahmed", 4, 2);
    rider.add(3, "Bilal", 3, 4);

    g.setName(0, "Central");
    g.setName(1, "Gulberg");
    g.setName(2, "DHA");
    g.setName(3, "Model Town");
    g.setName(4, "Johar Town");
    g.road(0, 1, 4);
    g.road(0, 2, 2);
    g.road(1, 2, 5);
    g.road(1, 3, 10);
    g.road(2, 3, 3);
    g.road(2, 4, 6);
    g.road(3, 4, 2);

    int ch;

    while (1) {
        cout << "\n====================================" << endl;
        cout << "   FoodExpress Dispatch Engine" << endl;
        cout << "====================================" << endl;
        cout << "1. Dynamic Order Scheduling" << endl;
        cout << "2. Kitchen Load Analysis" << endl;
        cout << "3. Rider Dispatch" << endl;
        cout << "4. Route Optimization" << endl;
        cout << "5. Search and Retrieval" << endl;
        cout << "6. Order History Tracking" << endl;
        cout << "7. Performance Analysis" << endl;
        cout << "8. Scalability Simulation" << endl;
        cout << "9. System Reports" << endl;
        cout << "10. Exit" << endl;
        cout << "Choice: ";
        cin >> ch;

        if (ch == 1) {
            int sub;
            cout << "\n1.Add 2.View 3.Process 4.Cancel 5.Update Priority" << endl;
            cout << "6.Delay 7.Undo 8.Reschedule (Pri+Deadline)" << endl;
            cout << "Sub choice: ";
            cin >> sub;

            if (sub == 1) {
                int id, pri, prep, dead;
                string cust, rest, cat;
                cout << "Order ID: "; cin >> id;
                cin.ignore();
                cout << "Customer: "; getline(cin, cust);
                cout << "Restaurant: "; getline(cin, rest);
                cout << "Priority: "; cin >> pri;
                cout << "Prep time: "; cin >> prep;
                cout << "Deadline: "; cin >> dead;
                cin.ignore();
                cout << "Category (VIP/Regular): "; getline(cin, cat);
                if (cat != "VIP") cat = "Regular";

                Order o(id, cust, rest, pri, prep, dead, cat);
                heap.insert(o);

                Record rec;
                rec.orderId = id;
                rec.customer = cust;
                rec.restaurant = rest;
                rec.category = cat;
                rec.status = "QUEUED";
                tree.insert(rec);
            }
            else if (sub == 2) heap.show();
            else if (sub == 3) {
                Order o = heap.removeMax();
                if (o.orderId != 0) {
                    history.add(o.orderId, "ACCEPTED");
                    kitchen.assign(o.orderId, o.restaurantName);
                    int node = o.orderId % 5;
                    rider.assign(o.orderId, node, &history);
                    rider.done(1, o.orderId, &history);
                    tree.update(o.orderId, "DELIVERED");
                }
            }
            else if (sub == 4) {
                int id;
                cout << "Order ID: "; cin >> id;
                heap.cancel(id, stk);
                tree.update(id, "CANCELLED");
            }
            else if (sub == 5) {
                int id, p;
                cout << "Order ID: "; cin >> id;
                cout << "New priority: "; cin >> p;
                heap.changePri(id, p);
            }
            else if (sub == 6) {
                int id, x;
                cout << "Order ID: "; cin >> id;
                cout << "Reduce by: "; cin >> x;
                heap.delay(id, x);
            }
            else if (sub == 7) {
                if (stk.empty()) cout << "Nothing to undo" << endl;
                else {
                    Order o = stk.pop();
                    o.status = "QUEUED";
                    heap.insert(o);
                    tree.update(o.orderId, "QUEUED");
                    history.add(o.orderId, "UNDO");
                    cout << "Restored" << endl;
                }
            }
            else if (sub == 8) {
                int id, pri, dead;
                cout << "Order ID: "; cin >> id;
                cout << "New Priority: "; cin >> pri;
                cout << "New Deadline: "; cin >> dead;
                heap.reschedule(id, pri, dead);
            }
        }
        else if (ch == 2) {
            int sub;
            cout << "\n1.Status 2.Complete 3.Rebalance" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) kitchen.show();
            else if (sub == 2) {
                string n;
                cin.ignore();
                cout << "Restaurant: "; getline(cin, n);
                kitchen.complete(n);
            }
            else if (sub == 3) {
                int id;
                cout << "Order ID: "; cin >> id;
                kitchen.rebalance(id, "");
            }
        }
        else if (ch == 3) {
            int sub;
            cout << "\n1.Show 2.Assign 3.Complete" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) rider.show();
            else if (sub == 2) {
                int oid, node;
                cout << "Order ID: "; cin >> oid;
                cout << "Node: "; cin >> node;
                rider.assign(oid, node, &history);
            }
            else if (sub == 3) {
                int rid, oid;
                cout << "Rider ID: "; cin >> rid;
                cout << "Order ID: "; cin >> oid;
                rider.done(rid, oid, &history);
                tree.update(oid, "DELIVERED");
            }
        }
        else if (ch == 4) {
            int sub;
            cout << "\n1.All routes 2.One route 3.Compare 4.Block 5.Unblock" << endl;
            cout << "6.BFS 7.DFS 8.Reroute (REROUTED)" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) {
                int s; cout << "Start: "; cin >> s;
                g.dijkstra(s, -1);
            }
            else if (sub == 2) {
                int s, d;
                cout << "From: "; cin >> s;
                cout << "To: "; cin >> d;
                g.dijkstra(s, d);
            }
            else if (sub == 3) {
                int s, d;
                cout << "From: "; cin >> s;
                cout << "To: "; cin >> d;
                g.compareRoute(s, d);
            }
            else if (sub == 4) {
                int u, v;
                cout << "Node1: "; cin >> u;
                cout << "Node2: "; cin >> v;
                g.blockRoad(u, v);
            }
            else if (sub == 5) {
                int u, v;
                cout << "Node1: "; cin >> u;
                cout << "Node2: "; cin >> v;
                g.openRoad(u, v);
            }
            else if (sub == 6) {
                int s; cout << "Start: "; cin >> s;
                g.bfs(s);
            }
            else if (sub == 7) {
                int s; cout << "Start: "; cin >> s;
                g.dfs(s);
            }
            else if (sub == 8) {
                int s, d, u, v, oid;
                cout << "From: "; cin >> s;
                cout << "To: "; cin >> d;
                cout << "Block road (u v): "; cin >> u >> v;
                cout << "Order ID: "; cin >> oid;
                g.reroute(s, d, u, v, oid, &history);
                tree.update(oid, "REROUTED");
            }
        }
        else if (ch == 5) {
            int sub;
            cout << "\n1.By ID 2.Customer 3.Restaurant 4.Status" << endl;
            cout << "5.Category 6.All 7.Sorted" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) {
                int id; cout << "ID: "; cin >> id;
                tree.searchId(id);
            }
            else if (sub == 2) {
                string n; cin.ignore();
                cout << "Name: "; getline(cin, n);
                tree.searchName(n);
            }
            else if (sub == 3) {
                string n; cin.ignore();
                cout << "Restaurant: "; getline(cin, n);
                tree.searchRest(n);
            }
            else if (sub == 4) {
                string st; cin.ignore();
                cout << "Status: "; getline(cin, st);
                tree.searchStatus(st);
            }
            else if (sub == 5) {
                string c; cin.ignore();
                cout << "Category: "; getline(cin, c);
                heap.byCategory(c);
            }
            else if (sub == 6) tree.showAll();
            else if (sub == 7) tree.sortedList();
        }
        else if (ch == 6) {
            int sub;
            cout << "\n1.History 2.Timeline 3.Filter" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) history.showAll();
            else if (sub == 2) {
                int id; cout << "Order ID: "; cin >> id;
                history.timeline(id);
            }
            else if (sub == 3) {
                string st; cin.ignore();
                cout << "Status: "; getline(cin, st);
                history.filter(st);
            }
        }
        else if (ch == 7) {
            int sub;
            cout << "\n1.Insert test 2.Delete test 3.Complexity 4.Memory" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) {
                int n; cout << "N: "; cin >> n;
                if (n > 0 && n <= MAX_ORDER) perfInsert(n);
            }
            else if (sub == 2) {
                int n; cout << "N: "; cin >> n;
                if (n > 0) perfDelete(n);
            }
            else if (sub == 3) showComplexity();
            else if (sub == 4) showMemory(heap, history);
        }
        else if (ch == 8) scaleTest();
        else if (ch == 9) {
            cout << "\n===== SYSTEM REPORT =====" << endl;
            cout << "Active orders: " << heap.size() << endl;
            if (!heap.empty()) heap.top().show();
            kitchen.show();
            rider.show();
            cout << "History count: " << history.total() << endl;
            ethics();
        }
        else if (ch == 10) {
            cout << "Thank you" << endl;
            break;
        }
        else cout << "Wrong choice" << endl;
    }

    return 0;
}



