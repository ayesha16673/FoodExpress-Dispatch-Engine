

#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;

// ==================== LINKED LIST (Order History) ====================
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

    // History mein naya record add karo
    void add(int id, string st) {
        HistNode* n = new HistNode;
        n->orderId = id;
        n->status = st;
        n->tm = nowTime();
        n->next = head;
        head = n;
        count++;
    }

    // Poori history dikhao
    void showAll() {
        cout << "\n--- Order History ---" << endl;
        HistNode* p = head;
        if (p == NULL) {
            cout << "No history record found." << endl;
            return;
        }
        while (p != NULL) {
            cout << p->tm << " | #" << p->orderId << " | " << p->status << endl;
            p = p->next;
        }
    }

    // Ek order ki timeline dikhao
    void timeline(int id) {
        cout << "\n--- Timeline for Order #" << id << " ---" << endl;
        HistNode* p = head;
        bool ok = false;
        while (p != NULL) {
            if (p->orderId == id) {
                cout << p->tm << " -> " << p->status << endl;
                ok = true;
            }
            p = p->next;
        }
        if (!ok) cout << "Order #" << id << " not found in history." << endl;
    }

    // Status se filter karo
    void filter(string st) {
        cout << "\n--- Orders with status: " << st << " ---" << endl;
        HistNode* p = head;
        bool ok = false;
        while (p != NULL) {
            if (p->status == st) {
                cout << "#" << p->orderId << " at " << p->tm << endl;
                ok = true;
            }
            p = p->next;
        }
        if (!ok) cout << "No orders found with this status." << endl;
    }

    int total() { return count; }
};

// ==================== BST (Search Records) ====================
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

    void insert(Record r) { root = addNode(root, r); }

    TNode* findNode(TNode* node, int id) {
        if (node == NULL) return NULL;
        if (id == node->data.orderId) return node;
        if (id < node->data.orderId) return findNode(node->left, id);
        return findNode(node->right, id);
    }

    // Status update karo
    void update(int id, string st) {
        TNode* f = findNode(root, id);
        if (f != NULL) f->data.status = st;
    }

    // ID se search karo
    void searchId(int id) {
        TNode* f = findNode(root, id);
        if (f == NULL) {
            cout << "Order not found." << endl;
            return;
        }
        cout << "\n--- Record Found ---" << endl;
        cout << "ID       : " << f->data.orderId << endl;
        cout << "Customer : " << f->data.customer << endl;
        cout << "Restaurant: " << f->data.restaurant << endl;
        cout << "Category : " << f->data.category << endl;
        cout << "Status   : " << f->data.status << endl;
    }

    void inorder(TNode* node) {
        if (node == NULL) return;
        inorder(node->left);
        cout << "#" << node->data.orderId
            << " | " << node->data.customer
            << " | " << node->data.restaurant
            << " | " << node->data.status << endl;
        inorder(node->right);
    }

    void showAll() {
        cout << "\n--- All Records (Inorder) ---" << endl;
        inorder(root);
    }

    void toArray(TNode* node, Record arr[], int& c) {
        if (node == NULL) return;
        toArray(node->left, arr, c);
        arr[c] = node->data;
        c++;
        toArray(node->right, arr, c);
    }

    // Customer name se search
    void searchName(string name) {
        Record arr[100]; int c = 0;
        toArray(root, arr, c);
        cout << "\n--- Orders for Customer: " << name << " ---" << endl;
        bool f = false;
        for (int i = 0; i < c; i++) {
            if (arr[i].customer == name) {
                cout << "#" << arr[i].orderId << " | " << arr[i].status << endl;
                f = true;
            }
        }
        if (!f) cout << "No orders found." << endl;
    }

    // Restaurant se search
    void searchRest(string name) {
        Record arr[100]; int c = 0;
        toArray(root, arr, c);
        cout << "\n--- Orders for Restaurant: " << name << " ---" << endl;
        bool f = false;
        for (int i = 0; i < c; i++) {
            if (arr[i].restaurant == name) {
                cout << "#" << arr[i].orderId << " | " << arr[i].status << endl;
                f = true;
            }
        }
        if (!f) cout << "No orders found." << endl;
    }

    // Status se search
    void searchStatus(string st) {
        Record arr[100]; int c = 0;
        toArray(root, arr, c);
        cout << "\n--- Orders with Status: " << st << " ---" << endl;
        bool f = false;
        for (int i = 0; i < c; i++) {
            if (arr[i].status == st) {
                cout << "#" << arr[i].orderId << " | " << arr[i].customer << endl;
                f = true;
            }
        }
        if (!f) cout << "No orders found." << endl;
    }

    // Bubble sort by ID
    void sortedList() {
        Record arr[100]; int c = 0;
        toArray(root, arr, c);
        for (int i = 0; i < c - 1; i++)
            for (int j = 0; j < c - i - 1; j++)
                if (arr[j].orderId > arr[j + 1].orderId) {
                    Record t = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = t;
                }
        cout << "\n--- All Orders Sorted by ID ---" << endl;
        for (int i = 0; i < c; i++)
            cout << "#" << arr[i].orderId << " | " << arr[i].customer << endl;
    }
};

// ==================== PERFORMANCE FUNCTIONS ====================
void showComplexity() {
    cout << "\n--- Time Complexity Analysis ---" << endl;
    cout << "Heap insert/delete  : O(log n)" << endl;
    cout << "Reschedule          : O(log n)" << endl;
    cout << "Linked list add     : O(1)" << endl;
    cout << "Stack push/pop      : O(1)" << endl;
    cout << "Queue enq/deq       : O(1)" << endl;
    cout << "BST search          : O(log n)" << endl;
    cout << "Bubble sort         : O(n^2)" << endl;
    cout << "Dijkstra            : O(V^2)" << endl;
    cout << "BFS / DFS           : O(V+E)" << endl;
}

void ethics() {
    cout << "\n===== ETHICAL CONSIDERATIONS =====" << endl;
    cout << "1. Customer data should be stored securely." << endl;
    cout << "2. Rider locations only shared with user consent." << endl;
    cout << "3. Priority scheduling should remain fair for all." << endl;
    cout << "4. Real systems must use encryption & secure auth." << endl;
}


class Order;
class OrderStack;
class MaxHeap;
class Kitchen;
class RiderMgr;
class Graph;

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

    // Kitchens setup
    kitchen.add(1, "PizzaHut", 5);
    kitchen.add(2, "KFC", 4);
    kitchen.add(3, "McDonalds", 3);

    // Riders setup
    rider.add(1, "Ali", 5, 0);
    rider.add(2, "Ahmed", 4, 2);
    rider.add(3, "Bilal", 3, 4);

    // Graph setup
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
        cout << "1.  Dynamic Order Scheduling" << endl;
        cout << "2.  Kitchen Load Analysis" << endl;
        cout << "3.  Rider Dispatch" << endl;
        cout << "4.  Route Optimization" << endl;
        cout << "5.  Search and Retrieval" << endl;
        cout << "6.  Order History Tracking" << endl;
        cout << "7.  Time Complexity" << endl;
        cout << "8.  System Report" << endl;
        cout << "9.  Ethical Considerations" << endl;
        cout << "10. Exit" << endl;
        cout << "Choice: ";
        cin >> ch;

        if (ch == 1) {
            int sub;
            cout << "\n1.Add  2.View  3.Process  4.Cancel" << endl;
            cout << "5.Update Priority  6.Delay  7.Undo  8.Reschedule" << endl;
            cout << "Sub choice: ";
            cin >> sub;

            if (sub == 1) {
                int id, pri, prep, dead;
                string cust, rest, cat;
                cout << "Order ID: "; cin >> id;
                cin.ignore();
                cout << "Customer: "; getline(cin, cust);
                cout << "Restaurant: "; getline(cin, rest);
                cout << "Priority (1-10): "; cin >> pri;
                cout << "Prep time: "; cin >> prep;
                cout << "Deadline: "; cin >> dead;
                cin.ignore();
                cout << "Category (VIP/Regular): "; getline(cin, cat);
                if (cat != "VIP") cat = "Regular";

                Order o(id, cust, rest, pri, prep, dead, cat);
                heap.insert(o);
                history.add(id, "PLACED");

                Record rec;
                rec.orderId = id; rec.customer = cust;
                rec.restaurant = rest; rec.category = cat;
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
                    rider.assign(o.orderId, node);
                    rider.done(1, o.orderId);
                    history.add(o.orderId, "DELIVERED");
                    tree.update(o.orderId, "DELIVERED");
                }
            }
            else if (sub == 4) {
                int id; cout << "Order ID: "; cin >> id;
                heap.cancel(id, stk);
                tree.update(id, "CANCELLED");
                history.add(id, "CANCELLED");
            }
            else if (sub == 5) {
                int id, p;
                cout << "Order ID: "; cin >> id;
                cout << "New Priority: "; cin >> p;
                heap.changePri(id, p);
            }
            else if (sub == 6) {
                int id, x;
                cout << "Order ID: "; cin >> id;
                cout << "Reduce by: "; cin >> x;
                heap.delay(id, x);
            }
            else if (sub == 7) {
                if (stk.empty()) cout << "Nothing to undo." << endl;
                else {
                    Order o = stk.pop();
                    o.status = "QUEUED";
                    heap.insert(o);
                    tree.update(o.orderId, "QUEUED");
                    history.add(o.orderId, "UNDO");
                    cout << "Order #" << o.orderId << " restored!" << endl;
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
            cout << "\n1.Status  2.Complete  3.Rebalance" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) kitchen.show();
            else if (sub == 2) {
                string n; cin.ignore();
                cout << "Restaurant: "; getline(cin, n);
                kitchen.complete(n);
            }
            else if (sub == 3) {
                int id; cout << "Order ID: "; cin >> id;
                kitchen.rebalance(id, "");
            }
        }
        else if (ch == 3) {
            int sub;
            cout << "\n1.Show  2.Assign  3.Complete" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) rider.show();
            else if (sub == 2) {
                int oid, node;
                cout << "Order ID: "; cin >> oid;
                cout << "Node: "; cin >> node;
                rider.assign(oid, node);
            }
            else if (sub == 3) {
                int rid, oid;
                cout << "Rider ID: "; cin >> rid;
                cout << "Order ID: "; cin >> oid;
                rider.done(rid, oid);
                tree.update(oid, "DELIVERED");
                history.add(oid, "DELIVERED");
            }
        }
        else if (ch == 4) {
            int sub;
            cout << "\n1.All Routes  2.One Route  3.Compare" << endl;
            cout << "4.Block Road  5.Unblock  6.BFS  7.DFS  8.Reroute" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) { int s; cout << "Start node: "; cin >> s; g.dijkstra(s, -1); }
            else if (sub == 2) { int s, d; cout << "From: "; cin >> s; cout << "To: "; cin >> d; g.dijkstra(s, d); }
            else if (sub == 3) { int s, d; cout << "From: "; cin >> s; cout << "To: "; cin >> d; g.compareRoute(s, d); }
            else if (sub == 4) { int u, v; cout << "Node1: "; cin >> u; cout << "Node2: "; cin >> v; g.blockRoad(u, v); }
            else if (sub == 5) { int u, v; cout << "Node1: "; cin >> u; cout << "Node2: "; cin >> v; g.openRoad(u, v); }
            else if (sub == 6) { int s; cout << "Start: "; cin >> s; g.bfs(s); }
            else if (sub == 7) { int s; cout << "Start: "; cin >> s; g.dfs(s); }
            else if (sub == 8) {
                int s, d, u, v, oid;
                cout << "From: "; cin >> s;
                cout << "To: "; cin >> d;
                cout << "Block road u: "; cin >> u;
                cout << "Block road v: "; cin >> v;
                cout << "Order ID: "; cin >> oid;
                g.reroute(s, d, u, v);
                history.add(oid, "REROUTED");
                tree.update(oid, "REROUTED");
            }
        }
        else if (ch == 5) {
            int sub;
            cout << "\n1.By ID  2.Customer  3.Restaurant  4.Status  5.Category  6.All  7.Sorted" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) { int id; cout << "ID: "; cin >> id; tree.searchId(id); }
            else if (sub == 2) { string n; cin.ignore(); cout << "Name: "; getline(cin, n); tree.searchName(n); }
            else if (sub == 3) { string n; cin.ignore(); cout << "Restaurant: "; getline(cin, n); tree.searchRest(n); }
            else if (sub == 4) { string st; cin.ignore(); cout << "Status: "; getline(cin, st); tree.searchStatus(st); }
            else if (sub == 5) { string c; cin.ignore(); cout << "Category: "; getline(cin, c); heap.byCategory(c); }
            else if (sub == 6) tree.showAll();
            else if (sub == 7) tree.sortedList();
        }
        else if (ch == 6) {
            int sub;
            cout << "\n1.Full History  2.Timeline  3.Filter by Status" << endl;
            cout << "Sub choice: "; cin >> sub;
            if (sub == 1) history.showAll();
            else if (sub == 2) { int id; cout << "Order ID: "; cin >> id; history.timeline(id); }
            else if (sub == 3) { string st; cin.ignore(); cout << "Status: "; getline(cin, st); history.filter(st); }
        }
        else if (ch == 7) showComplexity();
        else if (ch == 8) {
            cout << "\n===== SYSTEM REPORT =====" << endl;
            cout << "Active orders : " << heap.size() << endl;
            if (!heap.empty()) heap.top().show();
            kitchen.show();
            rider.show();
            cout << "History count : " << history.total() << endl;
        }
        else if (ch == 9) ethics();
        else if (ch == 10) {
            cout << "\nThank you for using FoodExpress!" << endl;
            break;
        }
        else cout << "Invalid choice. Try again." << endl;
    }

    return 0;
}