


#include <SFML/Graphics.hpp>
#include <time.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdio> 
using namespace std;
using namespace sf;


////////////////////////////////  THEME /////////////////////////////////////

struct Theme {
    int id;                   // unique theme ID
    std::string name;         // display name
    std::string description;  // brief description

    Theme(int _id = 0,
        const std::string& _name = "",
        const std::string& _desc = "")
        : id(_id),
        name(_name),
        description(_desc)
    {
    }
};

struct AVLNode {
    Theme data;        // stored theme
    AVLNode* left;     // left subtree
    AVLNode* right;    // right subtree
    int height;        // node height for balancing

    AVLNode(const Theme& t)
        : data(t),
        left(nullptr),
        right(nullptr),
        height(1)
    {
    }
};

// — AVL Rotation Helpers —

// Returns height of node (0 if null)
int nodeHeight(AVLNode* n) {
    return n ? n->height : 0;
}

// Updates height of a node based on children
void updateHeight(AVLNode* n) {
    int lh = nodeHeight(n->left);
    int rh = nodeHeight(n->right);
    n->height = 1 + (lh > rh ? lh : rh);
}

// Balance factor = left height minus right height
int getBalance(AVLNode* n) {
    return n ? nodeHeight(n->left) - nodeHeight(n->right) : 0;
}

// Right rotation
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    // Perform rotation
    x->right = y;
    y->left = T2;
    // Update heights
    updateHeight(y);
    updateHeight(x);
    return x;
}

// Left rotation
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    // Perform rotation
    y->left = x;
    x->right = T2;
    // Update heights
    updateHeight(x);
    updateHeight(y);
    return y;
}

// Left-Right rotation
AVLNode* rotateLeftRight(AVLNode* n) {
    n->left = rotateLeft(n->left);
    return rotateRight(n);
}

// Right-Left rotation
AVLNode* rotateRightLeft(AVLNode* n) {
    n->right = rotateRight(n->right);
    return rotateLeft(n);
}

// — AVL Insertion —
// Inserts Theme t into AVL tree rooted at node, returns new root.

AVLNode* insert(AVLNode* node, const Theme& t) {
    // 1. Normal BST insert
    if (!node)
        return new AVLNode(t);
    if (t.id < node->data.id)
        node->left = insert(node->left, t);
    else if (t.id > node->data.id)
        node->right = insert(node->right, t);
    else
        return node; // duplicate IDs not allowed

    // 2. Update height
    updateHeight(node);

    // 3. Get balance factor
    int balance = getBalance(node);

    // 4. Rebalance if needed
    // Left Left Case
    if (balance > 1 && t.id < node->left->data.id)
        return rotateRight(node);

    // Right Right Case
    if (balance < -1 && t.id > node->right->data.id)
        return rotateLeft(node);

    // Left Right Case
    if (balance > 1 && t.id > node->left->data.id) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left Case
    if (balance < -1 && t.id < node->right->data.id) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    // Balanced
    return node;
}

// — AVL Deletion: Remove a Theme by ID and rebalance —

// Helper to find the node with minimum ID in a subtree
AVLNode* minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left)
        current = current->left;
    return current;
}

AVLNode* removeNode(AVLNode* root, int id) {
    if (!root)
        return nullptr;

    // 1. Standard BST deletion
    if (id < root->data.id) {
        root->left = removeNode(root->left, id);
    }
    else if (id > root->data.id) {
        root->right = removeNode(root->right, id);
    }
    else {
        // node to be deleted found
        if (!root->left || !root->right) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (!temp) {
                // no child
                temp = root;
                root = nullptr;
            }
            else {
                // one child
                *root = *temp;  // copy contents
            }
            delete temp;
        }
        else {
            // two children: get inorder successor (smallest in right subtree)
            AVLNode* temp = minValueNode(root->right);
            // copy successor’s data
            root->data = temp->data;
            // delete the successor
            root->right = removeNode(root->right, temp->data.id);
        }
    }

    // if tree had only one node
    if (!root)
        return nullptr;

    // 2. Update height
    updateHeight(root);

    // 3. Get balance factor
    int balance = getBalance(root);

    // 4. Rebalance if needed
    // Left Left
    if (balance > 1 && getBalance(root->left) >= 0)
        return rotateRight(root);

    // Left Right
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right Right
    if (balance < -1 && getBalance(root->right) <= 0)
        return rotateLeft(root);

    // Right Left
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}



// — In‑Order Traversal: Print Theme IDs and Names —
void inorderPrint(AVLNode* root) {
    if (!root) return;
    inorderPrint(root->left);
    // Print ID and name
    std::cout << "Theme " << root->data.id
        << ": " << root->data.name << std::endl;
    inorderPrint(root->right);
}


// — Search by Theme ID: BST-style lookup —
AVLNode* findByID(AVLNode* root, int id) {
    if (!root) return nullptr;
    if (id == root->data.id)
        return root;
    else if (id < root->data.id)
        return findByID(root->left, id);
    else
        return findByID(root->right, id);
}


// — Search by Theme Name: full-tree traversal lookup —
AVLNode* findByName(AVLNode* root, const std::string& name) {
    if (!root) return nullptr;
    if (root->data.name == name)
        return root;
    // Search left subtree
    AVLNode* found = findByName(root->left, name);
    if (found) return found;
    // Otherwise search right subtree
    return findByName(root->right, name);
}

// — Update Theme by ID —
// Finds a theme by ID and updates its name/description.
// Uses findByID internally.

void updateTheme(AVLNode* root, int id,
    const std::string& newName,
    const std::string& newDesc) {
    AVLNode* node = findByID(root, id);
    if (!node) {
        std::cout << "No theme with ID " << id << " found.\n";
        return;
    }
    node->data.name = newName;
    node->data.description = newDesc;
    std::cout << "Theme " << id << " updated to \""
        << newName << "\" - " << newDesc << "\n";
}


enum PauseAction { PAUSE_RESUME = 0, PAUSE_SAVE, PAUSE_LOAD, PAUSE_EXIT }; // for pause menu //


////////////////////////////////////  SAVE GAME LOAD GAME FUNC //////////////////////////////////////



const int M = 25, N = 40, ts = 18;
int grid[M][N];
struct Enemy
{
    int x, y, dx, dy; Enemy() { x = y = 300; dx = 4 - rand() % 8; dy = 4 - rand() % 8; }
    void move()
    {
        x += dx;
        if (grid[y / ts][x / ts] == 1)
        {
            dx = -dx; x += dx;
        }
        y += dy;
        if (grid[y / ts][x / ts] == 1)
        {
            dy = -dy; y += dy;
        }
    }
};
void drop(int r, int c)
{
    if (grid[r][c] == 0) grid[r][c] = -1;
    if (grid[r - 1][c] == 0)
        drop(r - 1, c);
    if (grid[r + 1][c] == 0)
        drop(r + 1, c);
    if (grid[r][c - 1] == 0)
        drop(r, c - 1);
    if (grid[r][c + 1] == 0)
        drop(r, c + 1);
}



const int ENEMY_COUNT = 4;

struct GameState {
    int grid[M][N];
    int px, py, dx, dy;
    bool inGame, drawing, moveQ, frozen;
    float freezeElapsed;
    float timer, delay;
    int score, bonus, powerUps, par;
    Enemy enemies[ENEMY_COUNT];
};

bool saveGame(const GameState& s, const char* fname) {
    std::ofstream out(fname, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(&s), sizeof(s));
    return true;
}

bool loadGame(GameState& s, const char* fname) {
    std::ifstream in(fname, std::ios::binary);
    if (!in) return false;
    in.read(reinterpret_cast<char*>(&s), sizeof(s));
    return in.good();
}

// ────────────────────────────────────────────────────────────────────────────

// — Save Player’s Theme to File —
// Appends the selected theme ID for a user to “user_themes.txt”
// Uses only <fstream>

void savePlayerTheme(const std::string& username, int themeID) {
    std::ofstream out("user_themes.txt", std::ios::app);
    if (!out) {
        std::cerr << "Error: could not open user_themes.txt for writing\n";
        return;
    }
    out << username << " " << themeID << "\n";
    // file is closed automatically when out goes out of scope
}

// — Load Player’s Theme from File —
// Reads “user_themes.txt” for the given username and returns the last saved theme ID.
// Uses only <fstream> and <string>.

int loadPlayerTheme(const std::string& username) {
    std::ifstream in("user_themes.txt");
    if (!in) {
        std::cerr << "Warning: could not open user_themes.txt for reading\n";
        return 0;  // 0 = no theme saved
    }
    std::string u;
    int id;
    int lastID = 0;
    // Read all entries; keep the last matching one
    while (in >> u >> id) {
        if (u == username) {
            lastID = id;
        }
    }
    return lastID;
}



struct Player {
    std::string username;
    int score;
    Player* next;
    Player(const std::string& u, int s) : username(u), score(s), next(nullptr) {}
};

class MatchQueue {
    Player* head;
    Player* tail;
public:
    MatchQueue() : head(nullptr), tail(nullptr) {}
    void enqueue(Player* p) {
        p->next = nullptr;
        if (!tail) head = tail = p;
        else { tail->next = p; tail = p; }
    }
    Player* dequeue() {
        if (!head) return nullptr;
        Player* p = head;
        head = head->next;
        if (!head) tail = nullptr;
        p->next = nullptr;
        return p;
    }
    bool isEmpty() const { return head == nullptr; }
};

static MatchQueue g_matchQueue;


/////////////////////// LEADERBOARD ///////////////////////
#define MAX_NAME_LEN 32
#define LB_CAPACITY 10

struct LBEntry {
    char name[MAX_NAME_LEN];
    int score;
};

class Leaderboard {
    LBEntry heap[LB_CAPACITY];
    int size;

    void swapEntry(int i, int j) { LBEntry tmp = heap[i]; heap[i] = heap[j]; heap[j] = tmp; }
    void heapifyUp(int idx) {
        while (idx > 0) {
            int p = (idx - 1) / 2;
            if (heap[idx].score < heap[p].score) { swapEntry(idx, p); idx = p; }
            else break;
        }
    }
    void heapifyDown(int idx) {
        for (;;) {
            int l = 2 * idx + 1, r = 2 * idx + 2, smallest = idx;
            if (l < size && heap[l].score < heap[smallest].score) smallest = l;
            if (r < size && heap[r].score < heap[smallest].score) smallest = r;
            if (smallest != idx) { swapEntry(idx, smallest); idx = smallest; }
            else break;
        }
    }

public:
    Leaderboard() : size(0) {}
    void load(const char* fname) {
        ifstream in(fname);
        if (!in) return;
        char uname[MAX_NAME_LEN]; int sc;
        while (in >> uname >> sc) add(uname, sc);
    }
    void save(const char* fname) {
        LBEntry tmp[LB_CAPACITY]; int ts = size;
        for (int i = 0; i < ts; ++i) tmp[i] = heap[i];
        for (int i = 0; i < ts - 1; ++i) {
            int mx = i;
            for (int j = i + 1; j < ts; ++j)
                if (tmp[j].score > tmp[mx].score) mx = j;
            swap(tmp[i], tmp[mx]);
        }
        ofstream out(fname);
        for (int i = 0; i < ts; ++i)
            out << tmp[i].name << " " << tmp[i].score << "\n";
    }
    void add(const char* uname, int sc) {
        for (int i = 0; i < size; ++i) {
            if (strcmp(heap[i].name, uname) == 0) {
                heap[i].score = sc;
                heapifyDown(i); heapifyUp(i);
                return;
            }
        }
        if (size < LB_CAPACITY) {
            strncpy_s(heap[size].name, uname, MAX_NAME_LEN - 1);
            heap[size].name[MAX_NAME_LEN - 1] = '\0';
            heap[size].score = sc;
            heapifyUp(size++);
        }
        else if (sc > heap[0].score) {
            strncpy_s(heap[0].name, uname, MAX_NAME_LEN - 1);
            heap[0].name[MAX_NAME_LEN - 1] = '\0';
            heap[0].score = sc;
            heapifyDown(0);
        }
    }
    void draw(RenderWindow& win, Font& font) {
        LBEntry tmp[LB_CAPACITY]; int ts = size;
        for (int i = 0; i < ts; ++i) tmp[i] = heap[i];
        for (int i = 0; i < ts - 1; ++i) {
            int mx = i;
            for (int j = i + 1; j < ts; ++j)
                if (tmp[j].score > tmp[mx].score) mx = j;
            swap(tmp[i], tmp[mx]);
        }
        Text title("--  LEADERBOARD  --", font, 30);
        title.setFillColor(Color::Yellow);
        title.setPosition(200, 20);
        win.draw(title);
        for (int i = 0; i < ts; ++i) {
            char buf[64];
            sprintf_s(buf, "%2d. %-15s %5d", i + 1, tmp[i].name, tmp[i].score);
            Text line(buf, font, 24);
            line.setPosition(200, 70 + i * 30);
            win.draw(line);
        }
        Text f("Press Esc to return", font, 20);
        f.setFillColor(Color::Cyan);
        f.setPosition(220, 70 + ts * 30 + 20);
        win.draw(f);
    }
};

static Leaderboard gLeader;


/////////////////// FOR GAMEROOM ///////////////////////
struct MatchPlayer {
    std::string name;
    int score;
};

// maximum number of players we’ll ever load (same as LB_CAPACITY)
const int MAX_CANDS = LB_CAPACITY;

MatchPlayer gCands[MAX_CANDS];
int   gCandCount = 0;

void loadMatchCandidates(const char* filename) {
    std::ifstream in(filename);
    gCandCount = 0;
    std::string uname;
    int sc;
    while (gCandCount < MAX_CANDS && (in >> uname >> sc)) {
        gCands[gCandCount++] = { uname, sc };
    }
    // selection‐sort descending by score
    for (int i = 0; i < gCandCount; ++i) {
        for (int j = i + 1; j < gCandCount; ++j) {
            if (gCands[j].score > gCands[i].score) {
                MatchPlayer tmp = gCands[i];
                gCands[i] = gCands[j];
                gCands[j] = tmp;
            }

        }
    }
}


std::pair<MatchPlayer, MatchPlayer> findBestMatch() {
    if (gCandCount < 2) return {};
    int bestDiff = 1000000000;
    int bestIdx = 0;
    for (int i = 0; i + 1 < gCandCount; ++i) {
        int diff = gCands[i].score - gCands[i + 1].score;
        if (diff < 0) diff = -diff;
        if (diff < bestDiff) {
            bestDiff = diff;
            bestIdx = i;
        }
    }
    return { gCands[bestIdx], gCands[bestIdx + 1] };
}


std::pair<MatchPlayer, MatchPlayer> runMatchmaking(const char* file) {
    loadMatchCandidates(file);
    auto match = findBestMatch();
    // if no two players, get out
    if (match.first.name.empty()) return {};

    // remove the matched two by shifting left
    bool removed1 = false, removed2 = false;
    MatchPlayer a = match.first, b = match.second;
    int write = 0;
    for (int read = 0; read < gCandCount; ++read) {
        auto& mp = gCands[read];
        if (!removed1 && mp.name == a.name && mp.score == a.score) { removed1 = true; continue; }
        if (!removed2 && mp.name == b.name && mp.score == b.score) { removed2 = true; continue; }
        gCands[write++] = mp;
    }
    gCandCount = write;

    // re-enqueue the rest
    for (int i = 0; i < gCandCount; ++i) {
        g_matchQueue.enqueue(new Player(gCands[i].name, gCands[i].score));
    }
    return match;
}


/////////////////////// LOGIN/SIGNUP //////////////////////////
class UserManager {
    string filename;
public:
    UserManager(const string& file) : filename(file) {}
    bool usernameExists(const string& user) {
        ifstream in(filename); if (!in) return false;
        string u, p; while (in >> u >> p) if (u == user) return true;
        return false;
    }
    bool registerUser(const string& user, const string& pwd) {
        if (usernameExists(user) || pwd.size() < 4) return false;
        ofstream out(filename, ios::app);
        out << user << " " << pwd << "\n";
        return true;
    }
    bool loginUser(const string& user, const string& pwd) {
        ifstream in(filename); if (!in) return false;
        string u, p; while (in >> u >> p) if (u == user && p == pwd) return true;
        return false;
    }
};
enum AuthAction { ACT_LOGIN = 0, ACT_REGISTER = 1, ACT_DONE = 2 };

bool showAuthScreen(RenderWindow& window, Font& font, string& outUsername) {
    UserManager mgr("users.txt");
    AuthAction sel = ACT_LOGIN;
    bool inForm = false; int field = 0;
    string username, password, msg;
    Text title("Please Login or Register", font, 30);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2, tb.height / 2);
    title.setPosition(window.getSize().x / 2, 50);
    Text btn[2]; const char* labels[2] = { "Login","Register" };
    for (int i = 0; i < 2; i++) {
        btn[i].setFont(font);
        btn[i].setString(labels[i]);
        btn[i].setCharacterSize(24);
        FloatRect b = btn[i].getLocalBounds();
        btn[i].setOrigin(b.width / 2, b.height / 2);
        btn[i].setPosition(window.getSize().x / 2 + (i * 200 - 100), 120);
    }
    Text userLbl("Username:", font, 20), passLbl("Password:", font, 20), info("", font, 18);
    userLbl.setPosition(200, 200); passLbl.setPosition(200, 250);
    info.setFillColor(Color::Red); info.setPosition(200, 320);
    Text userIn("", font, 20), passIn("", font, 20);
    userIn.setPosition(320, 200); passIn.setPosition(320, 250);
    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) return false;
            if (e.type == Event::KeyPressed) {
                if (!inForm) {
                    if (e.key.code == Keyboard::Left || e.key.code == Keyboard::Right)
                        sel = AuthAction(1 - sel);
                    else if (e.key.code == Keyboard::Enter)
                    {
                        inForm = true; field = 0;
                        username.clear();
                        password.clear();
                        msg.clear();
                    }
                }
                else {
                    if (e.key.code == Keyboard::Tab) field = 1 - field;
                    else if
                        (e.key.code == Keyboard::Backspace)
                    {
                        auto& s = (field == 0 ? username : password); if (!s.empty()) s.pop_back();
                    }
                    else if (e.key.code == Keyboard::Enter)
                    {
                        if (field == 0) field = 1;
                        else {
                            bool ok = (sel == ACT_LOGIN) ? mgr.loginUser(username, password) : mgr.registerUser(username, password);
                            if (ok) { outUsername = username; return true; }
                            else msg = (sel == ACT_LOGIN ? "Login failed" : "Register failed");
                        }
                    }
                }
            }
            else if (inForm && e.type == Event::TextEntered)
            {
                char c = (char)e.text.unicode;
                if (c >= 32 && c < 127) { auto& s = (field == 0 ? username : password); s.push_back(c); }
            }
        }
        for (int i = 0; i < 2; i++) btn[i].setFillColor(i == sel ? Color::Yellow : Color::White);
        userIn.setString(username + (inForm && field == 0 ? "_" : ""));
        passIn.setString(string(password.size(), '*') + (inForm && field == 1 ? "_" : ""));
        info.setString(msg);
        window.clear(Color::Black);
        window.draw(title); window.draw(btn[0]); window.draw(btn[1]);
        if (inForm) { window.draw(userLbl); window.draw(userIn); window.draw(passLbl); window.draw(passIn); window.draw(info); }
        window.display();
    }
    return false;
}

/////////////////////// POINTS & POWERUPS //////////////////////
class PointsTracker {
    int score = 0, bonus = 0, powerUps = 0, par = 50;
    Font f; Text txt;
public:
    PointsTracker() {
        f.loadFromFile("C:/Users/yahya/OneDrive/Desktop/Xonix DS project/fonts/Roboto_Condensed-Bold.ttf");
        txt.setFont(f); txt.setCharacterSize(20); txt.setPosition(10, 10); updateText();
    }
    void updateText() { txt.setString("Score: " + to_string(score) + "   Power-ups: " + to_string(powerUps)); }
    void Pointscounter(int tiles) {
        int pts = tiles;
        if ((bonus < 3 && tiles>10) || (bonus >= 3 && bonus < 5 && tiles>5)) { pts *= 2; bonus++; }
        else if (bonus >= 5 && tiles > 5) pts *= 4;
        score += pts;
        while (score >= par) { powerUps++; par += (par == 50 ? 20 : 30); }
        updateText();
    }
    void UsePowerUp() { if (powerUps > 0) powerUps--; updateText(); }
    void draw(RenderWindow& w) { w.draw(txt); }
    int getScore()const { return score; }  int getPowerUps()const { return powerUps; }
};



/////////////////////// PAUSE MENU ////////////////////

PauseAction showPauseMenu(RenderWindow& window, Font& font) {
    const int opts = 4;
    const char* labels[opts] = { "Resume", "Save Game", "Load Game", "Exit to Menu" };
    Text items[opts];
    int sel = 0;

    // Setup Text objects
    for (int i = 0; i < opts; ++i) {
        items[i].setFont(font);
        items[i].setString(labels[i]);
        items[i].setCharacterSize(28);
        FloatRect r = items[i].getLocalBounds();
        items[i].setOrigin(r.width / 2, r.height / 2);
        items[i].setPosition(window.getSize().x / 2, window.getSize().y / 2 + i * 40);
    }

    // Loop until selection
    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();
            else if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Up)    sel = (sel - 1 + opts) % opts;
                else if (e.key.code == Keyboard::Down)  sel = (sel + 1) % opts;
                else if (e.key.code == Keyboard::Enter) return PauseAction(sel);
            }
        }
        window.clear(Color(0, 0, 0, 150));
        for (int i = 0; i < opts; ++i) {
            items[i].setFillColor(i == sel ? Color::Yellow : Color::White);
            window.draw(items[i]);
        }
        window.display();
    }
    return PAUSE_RESUME;
}

template<typename T>
T clamp(T v, T lo, T hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}


int selectLevel(RenderWindow& w, Font& f) {
    const int opt = 3;
    const char* labels[opt] = { "Level 01", "Level 02", "Level 03" };
    Text m[opt];
    int sel = 0;

    for (int i = 0; i < opt; i++) {
        m[i].setFont(f);
        m[i].setString(labels[i]);
        m[i].setCharacterSize(28);
        FloatRect r = m[i].getLocalBounds();
        m[i].setOrigin(r.width / 2, r.height / 2);
        m[i].setPosition(w.getSize().x / 2, w.getSize().y / 2 + i * 50);
    }

    while (w.isOpen()) {
        Event e;
        while (w.pollEvent(e)) {
            if (e.type == Event::Closed) w.close();
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Up) sel = (sel - 1 + opt) % opt;
                if (e.key.code == Keyboard::Down) sel = (sel + 1) % opt;
                if (e.key.code == Keyboard::Enter) return sel; // Return 1 for Level 01, 2 for Level 02, 3 for Level 03
            }
        }

        w.clear(Color::Black);
        for (int i = 0; i < opt; i++) {
            m[i].setFillColor(i == sel ? Color::Yellow : Color::White);
            w.draw(m[i]);
        }
        w.display();
    }

    return 0;
}

/////////////////////// SINGLE-PLAYER ////////////////////////
int runSinglePlayerMode(RenderWindow& window, Sprite& sTile, Sprite& sEnemy, Font& font, int level) {
    GameState state;

    RectangleShape sidePanel(Vector2f(200, M * ts)); sidePanel.setFillColor(Color(50, 50, 50)); sidePanel.setPosition(N * ts, 0);
    Text hudTitle("STATS", font, 24); hudTitle.setFillColor(Color::Yellow); hudTitle.setPosition(N * ts + 20, 20);
    Text scoreLabel("", font, 18), powerLabel("", font, 18);
    scoreLabel.setPosition(N * ts + 20, 60); powerLabel.setPosition(N * ts + 20, 90);
    PointsTracker tracker; int enemyCount; Enemy enemies[10];

    if (level == 0) {
        enemyCount = 4;
    }
    else if (level == 1) {
        enemyCount = 6;
    }
    else if (level == 2) {
        enemyCount = 8;
    }
    bool inGame = true, drawing = false, moveQ = false; bool frozen = false;
    int px = 10, py = 0, dx = 0, dy = 0;
    Clock freezeC, clock; float timer = 0, delay = 0.07f;
    for (int i = 0; i < M; i++) for (int j = 0; j < N; j++) grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
    for (int k = 0; k < enemyCount; k++) enemies[k] = Enemy();
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds(); timer += dt;
        if (frozen && freezeC.getElapsedTime().asSeconds() >= 3.f) frozen = false;
        Event e; while (window.pollEvent(e)) {
            if (e.type == Event::Closed) return tracker.getScore();
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                PauseAction act = showPauseMenu(window, font);
                if (act == PAUSE_RESUME) {


                }
                else if (act == PAUSE_SAVE) {
                    for (int i = 0; i < M; ++i)
                        for (int j = 0; j < N; ++j)
                            state.grid[i][j] = grid[i][j];

                    state.px = px;
                    state.py = py;
                    state.dx = dx;
                    state.dy = dy;
                    state.inGame = inGame;
                    state.drawing = drawing;
                    state.moveQ = moveQ;
                    state.frozen = frozen;
                    state.freezeElapsed = freezeC.getElapsedTime().asSeconds();
                    state.timer = timer;
                    state.delay = delay;

                    state.score = tracker.getScore();
                    state.powerUps = tracker.getPowerUps();
                    state.bonus = 0;
                    state.par = 50;

                    for (int i = 0; i < ENEMY_COUNT; ++i)
                        state.enemies[i] = enemies[i];

                    saveGame(state, "save.dat");

                }
                else if (act == PAUSE_LOAD) {
                    if (loadGame(state, "save.dat")) {
                        for (int i = 0; i < M; ++i)
                            for (int j = 0; j < N; ++j)
                                grid[i][j] = state.grid[i][j];

                        px = state.px;
                        py = state.py;
                        dx = state.dx;
                        dy = state.dy;
                        inGame = state.inGame;
                        drawing = state.drawing;
                        moveQ = state.moveQ;
                        frozen = state.frozen;
                        timer = state.timer;
                        delay = state.delay;
                        freezeC.restart();
                        if (state.frozen)
                            freezeC.restart();

                        for (int i = 0; i < ENEMY_COUNT; ++i)
                            enemies[i] = state.enemies[i];


                        for (int i = 0; i < state.powerUps; ++i)
                            tracker.UsePowerUp();


                        for (int i = 0; i < state.score; i += 10)
                            tracker.Pointscounter(10);

                        Clock c;
                        while (c.getElapsedTime().asSeconds() < 3.f) {}
                        while (c.getElapsedTime().asSeconds() < 3.f) {}
                    }
                }
                else if (act == PAUSE_EXIT) {
                    return tracker.getScore();
                }
            }

            if (e.type == Event::KeyPressed && inGame && grid[py][px] == 1 && !moveQ) {
                if (e.key.code == Keyboard::Left) { dx = -1; dy = 0; moveQ = true; }
                if (e.key.code == Keyboard::Right) { dx = 1; dy = 0; moveQ = true; }
                if (e.key.code == Keyboard::Up) { dx = 0; dy = -1; moveQ = true; }
                if (e.key.code == Keyboard::Down) { dx = 0; dy = 1; moveQ = true; }
            }
            if (e.type == Event::KeyPressed && inGame && e.key.code == Keyboard::T && tracker.getPowerUps() > 0 && !frozen)
            {
                tracker.UsePowerUp();
                frozen = true;
                freezeC.restart();
            }
        }
        if (inGame && grid[py][px] != 1)
        {
            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                dx = -1; dy = 0;
            }
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                dx = 1; dy = 0;
            }
            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                dx = 0; dy = -1;
            }
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                dx = 0; dy = 1;
            }
        }
        if (inGame && timer > delay)
        {
            timer = 0;

            if (grid[py][px] == 1)
            {
                // We’re on safe tile: only move if a single‐step was queued
                if (moveQ)
                {
                    px = clamp(px + dx, 0, N - 1);
                    py = clamp(py + dy, 0, M - 1);
                    moveQ = false;
                }
            }
            else
            {
                // Off safe: we slide continuously in the current direction
                px = clamp(px + dx, 0, N - 1);
                py = clamp(py + dy, 0, M - 1);
            }

            // Start trail if leaving safe
            if (grid[py][px] == 0)
            {
                grid[py][px] = 2;
                drawing = true;
            }
            // If we run into our own trail while drawing → game over
            else if (grid[py][px] == 2 && drawing)
            {
                inGame = false;
            }
        }
        if (inGame && grid[py][px] == 1 && drawing)
        {
            dx = dy = 0; drawing = false;
            int trail = 0, before = 0;
            for (int i = 0; i < M; i++)for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 2)trail++; else if (grid[i][j] == 0)before++;
            }
            for (int k = 0; k < enemyCount; k++) drop(enemies[k].y / ts, enemies[k].x / ts);
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++) grid[i][j] = (grid[i][j] == -1 ? 0 : 1);

            int after = 0;
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                    if (grid[i][j] == 0) after++;
            tracker.Pointscounter(trail + (before - after));
        }
        if (inGame && !frozen) {
            for (int k = 0; k < enemyCount; k++) {
                enemies[k].move();
                int gi = enemies[k].y / ts, gj = enemies[k].x / ts;
                if (gi > 0 && gi < M && gj > 0 && gj < N && grid[gi][gj] == 2)
                    inGame = false;
            }
        }
        window.clear();
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0) continue;
                sTile.setTextureRect(grid[i][j] == 1 ? IntRect(0, 0, ts, ts) : IntRect(54, 0, ts, ts));
                sTile.setPosition(j * ts, i * ts); window.draw(sTile);
            }
        sTile.setTextureRect(IntRect(36, 0, ts, ts));
        sTile.setPosition(px * ts, py * ts); window.draw(sTile);
        for (int k = 0; k < enemyCount; k++)
        {
            sEnemy.rotate(2.f); sEnemy.setPosition(enemies[k].x, enemies[k].y);
            window.draw(sEnemy);
        }
        window.draw(sidePanel); window.draw(hudTitle);
        scoreLabel.setString("Score: " + to_string(tracker.getScore()));
        powerLabel.setString("PU: " + to_string(tracker.getPowerUps()));
        window.draw(scoreLabel);
        window.draw(powerLabel);
        if (!inGame) {
            Text over("Game Over\nEsc=Menu", font, 28);
            over.setFillColor(Color::Red);
            FloatRect b = over.getLocalBounds();
            over.setOrigin(b.width / 2, b.height / 2);
            over.setPosition((N * ts) / 2, (M * ts) / 2);
            window.draw(over);
        }

        window.display();
    }
    return tracker.getScore();
}


/////////////////////// MULTIPLAYER /////////////////////////


void runMultiplayerMode(
    RenderWindow& window,
    Sprite& sTile,
    Sprite& sEnemySprite,
    Font& font
) {
    // --- HUD setup ---
    RectangleShape sidePanel(Vector2f(200.f, M * ts));
    sidePanel.setFillColor(Color(50, 50, 50));
    sidePanel.setPosition(N * ts, 0.f);

    Text hudTitle("STATS", font, 24);
    hudTitle.setFillColor(Color::Yellow);
    hudTitle.setPosition(N * ts + 20.f, 20.f);

    Text p1Label("", font, 18), p2Label("", font, 18);
    p1Label.setFillColor(Color::Red);
    p1Label.setPosition(N * ts + 20.f, 60.f);
    p2Label.setFillColor(Color(0, 255, 255));
    p2Label.setPosition(N * ts + 20.f, 100.f);

    PointsTracker tracker1, tracker2;
    const int enemyCount = 4;

    // Local Enemy struct
    struct EnemyLocal {
        int x, y, dx, dy;
        void init() {
            x = y = 300;
            dx = 4 - rand() % 8;
            dy = 4 - rand() % 8;
            if (dx == 0 && dy == 0) dx = 1;
        }
        void move(int G[][N], int t) {
            x += dx;
            int gy = y / t, gx = x / t;
            if (gx <= 0 || gx >= N - 1 || G[gy][gx] == 1) { dx = -dx; x += dx; }
            y += dy;
            gy = y / t; gx = x / t;
            if (gy <= 0 || gy >= M - 1 || G[gy][gx] == 1) { dy = -dy; y += dy; }
            if (x < 0) x = 0; else if (x > N * t) x = N * t;
            if (y < 0) y = 0; else if (y > M * t) y = M * t;
        }
    };

    EnemyLocal enemies[10];
    for (int i = 0; i < enemyCount; ++i)
        enemies[i].init();

    // Shared grid
    static int G[M][N];
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            G[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;

    // Recursive flood fill for capture
    auto dropL = [&](auto&& self, int r, int c)->void {
        if (r <= 0 || r >= M - 1 || c <= 0 || c >= N - 1) return;
        if (G[r][c] == 0) {
            G[r][c] = -1;
            self(self, r - 1, c);
            self(self, r + 1, c);
            self(self, r, c - 1);
            self(self, r, c + 1);
        }
        };

    // Player state
    bool alive1 = true, alive2 = true;
    bool draw1 = false, draw2 = false;
    bool moveQ1 = false, moveQ2 = false;
    int px1 = 10, py1 = 0, dx1 = 0, dy1 = 0;
    int px2 = N - 11, py2 = 0, dx2 = 0, dy2 = 0;

    // Freeze state
    bool freezeEnemies = false;
    bool freezeP1 = false, freezeP2 = false;
    Clock freezeC1, freezeC2;
    const float freezeDuration = 3.f;

    Clock clock;
    float timer = 0.f, delay = 0.07f;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        timer += dt;

        // Auto-unfreeze after duration
        if (freezeEnemies) {
            if ((freezeP2 && freezeC1.getElapsedTime().asSeconds() >= freezeDuration) ||
                (freezeP1 && freezeC2.getElapsedTime().asSeconds() >= freezeDuration))
            {
                freezeEnemies = freezeP1 = freezeP2 = false;
            }
        }

        // Event handling
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) return;
            if (!alive1 && !alive2 &&
                e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
            {
                return; // back to menu
            }

            if (e.type == Event::KeyPressed) {
                // P1 boundary move
                if (alive1 && G[py1][px1] == 1 && !moveQ1 && !freezeP1) {
                    if (e.key.code == Keyboard::Left) { dx1 = -1; dy1 = 0; moveQ1 = true; }
                    if (e.key.code == Keyboard::Right) { dx1 = 1; dy1 = 0; moveQ1 = true; }
                    if (e.key.code == Keyboard::Up) { dx1 = 0; dy1 = -1; moveQ1 = true; }
                    if (e.key.code == Keyboard::Down) { dx1 = 0; dy1 = 1; moveQ1 = true; }
                }
                // P2 boundary move
                if (alive2 && G[py2][px2] == 1 && !moveQ2 && !freezeP2) {
                    if (e.key.code == Keyboard::A) { dx2 = -1; dy2 = 0; moveQ2 = true; }
                    if (e.key.code == Keyboard::D) { dx2 = 1; dy2 = 0; moveQ2 = true; }
                    if (e.key.code == Keyboard::W) { dx2 = 0; dy2 = -1; moveQ2 = true; }
                    if (e.key.code == Keyboard::S) { dx2 = 0; dy2 = 1; moveQ2 = true; }
                }
                // Freeze power‑ups
                if (e.key.code == Keyboard::T && alive1 && tracker1.getPowerUps() > 0 && !freezeEnemies) {
                    tracker1.UsePowerUp();
                    freezeEnemies = true;
                    freezeP2 = true;
                    freezeC1.restart();
                }
                if (e.key.code == Keyboard::P && alive2 && tracker2.getPowerUps() > 0 && !freezeEnemies) {
                    tracker2.UsePowerUp();
                    freezeEnemies = true;
                    freezeP1 = true;
                    freezeC2.restart();
                }
            }
        }

        // Continuous sliding off boundary
        if (alive1 && G[py1][px1] != 1 && !freezeP1) {
            if (Keyboard::isKeyPressed(Keyboard::Left)) { dx1 = -1; dy1 = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Right)) { dx1 = 1; dy1 = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Up)) { dx1 = 0; dy1 = -1; }
            if (Keyboard::isKeyPressed(Keyboard::Down)) { dx1 = 0; dy1 = 1; }
        }
        if (alive2 && G[py2][px2] != 1 && !freezeP2) {
            if (Keyboard::isKeyPressed(Keyboard::A)) { dx2 = -1; dy2 = 0; }
            if (Keyboard::isKeyPressed(Keyboard::D)) { dx2 = 1; dy2 = 0; }
            if (Keyboard::isKeyPressed(Keyboard::W)) { dx2 = 0; dy2 = -1; }
            if (Keyboard::isKeyPressed(Keyboard::S)) { dx2 = 0; dy2 = 1; }
        }

        // Movement + drawing on grid
        if (timer > delay) {
            timer = 0.f;

            // Player 1 step & trail
            if (alive1 && !freezeP1) {
                if (G[py1][px1] == 1 && moveQ1) {
                    px1 = max(0, min(px1 + dx1, N - 1));
                    py1 = max(0, min(py1 + dy1, M - 1));
                    moveQ1 = false;
                }
                else if (G[py1][px1] != 1) {
                    px1 = max(0, min(px1 + dx1, N - 1));
                    py1 = max(0, min(py1 + dy1, M - 1));
                }
                int& c1 = G[py1][px1];
                if (c1 == 0) { c1 = 2; draw1 = true; }
                else if (c1 == 2 && draw1) alive1 = false;
            }

            // Player 2 step & trail
            if (alive2 && !freezeP2) {
                if (G[py2][px2] == 1 && moveQ2) {
                    px2 = max(0, min(px2 + dx2, N - 1));
                    py2 = max(0, min(py2 + dy2, M - 1));
                    moveQ2 = false;
                }
                else if (G[py2][px2] != 1) {
                    px2 = max(0, min(px2 + dx2, N - 1));
                    py2 = max(0, min(py2 + dy2, M - 1));
                }
                int& c2 = G[py2][px2];
                if (c2 == 0) { c2 = 3; draw2 = true; }
                else if (c2 == 3 && draw2) alive2 = false;
            }

            if (alive1 && G[py1][px1] == 3) {
                alive1 = false;
            }
            // (2) P2 intercepts P1's trail?
            if (alive2 && G[py2][px2] == 2) {
                alive2 = false;
            }

            bool safe1 = (G[py1][px1] == 1);
            bool safe2 = (G[py2][px2] == 1);

            // (3) P1 drawing and steps onto P2 in safe zone
            if (alive1 && draw1 && safe2 && px1 == px2 && py1 == py2) {
                alive1 = false;
            }
            // (4) P2 drawing and steps onto P1 in safe zone
            if (alive2 && draw2 && safe1 && px1 == px2 && py1 == py2) {
                alive2 = false;
            }

            // Player 1 capture region
            if (alive1 && G[py1][px1] == 1 && draw1) {
                draw1 = false;
                int trail = 0, before = 0;
                for (int i = 0; i < M; ++i)
                    for (int j = 0; j < N; ++j) {
                        if (G[i][j] == 2) ++trail;
                        else if (G[i][j] == 0) ++before;
                    }
                for (int k = 0; k < enemyCount; ++k)
                    dropL(dropL, enemies[k].y / ts, enemies[k].x / ts);
                for (int i = 0; i < M; ++i)
                    for (int j = 0; j < N; ++j)
                        G[i][j] = (G[i][j] == -1 ? 0 : 1);
                int after = 0;
                for (int i = 0; i < M; ++i)
                    for (int j = 0; j < N; ++j)
                        if (G[i][j] == 0) ++after;
                tracker1.Pointscounter(trail + (before - after));
            }

            // Player 2 capture region
            if (alive2 && G[py2][px2] == 1 && draw2) {
                draw2 = false;
                int trail = 0, before = 0;
                for (int i = 0; i < M; ++i)
                    for (int j = 0; j < N; ++j) {
                        if (G[i][j] == 3) ++trail;
                        else if (G[i][j] == 0) ++before;
                    }
                for (int k = 0; k < enemyCount; ++k)
                    dropL(dropL, enemies[k].y / ts, enemies[k].x / ts);
                for (int i = 0; i < M; ++i)
                    for (int j = 0; j < N; ++j)
                        G[i][j] = (G[i][j] == -1 ? 0 : 1);
                int after = 0;
                for (int i = 0; i < M; ++i)
                    for (int j = 0; j < N; ++j)
                        if (G[i][j] == 0) ++after;
                tracker2.Pointscounter(trail + (before - after));
            }
        }

        // Enemies move if not frozen
        if (!freezeEnemies) {
            for (int k = 0; k < enemyCount; ++k) {
                enemies[k].move(G, ts);
                int gy = enemies[k].y / ts, gx = enemies[k].x / ts;
                if (alive1 && gy > 0 && gy < M && gx>0 && gx < N && G[gy][gx] == 2) alive1 = false;
                if (alive2 && gy > 0 && gy < M && gx>0 && gx < N && G[gy][gx] == 3) alive2 = false;
            }
        }

        // Player‐player collision logic
        bool safe1 = (G[py1][px1] == 1);
        bool safe2 = (G[py2][px2] == 1);
        bool cons1 = (!safe1 && draw1);
        bool cons2 = (!safe2 && draw2);

        if (px1 == px2 && py1 == py2) {
            if (cons1 && cons2)         alive1 = alive2 = false;
            else if (cons1 && !cons2)        alive1 = false;
            else if (!cons1 && cons2)        alive2 = false;
            // both safe → continue
        }

        // --- RENDER ---
        window.clear();

        // draw grid
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                int v = G[i][j];
                if (v == 0) continue;
                sTile.setPosition(j * ts, i * ts);
                sTile.setTextureRect(v == 1
                    ? IntRect(0, 0, ts, ts)
                    : IntRect(54, 0, ts, ts));
                sTile.setColor(v == 3 ? Color(0, 255, 255) : Color::White);
                window.draw(sTile);
            }
        }
        sTile.setColor(Color::White);

        // draw players
        if (alive1) {
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(px1 * ts, py1 * ts);
            sTile.setColor(Color::Red);
            window.draw(sTile);
        }
        if (alive2) {
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(px2 * ts, py2 * ts);
            sTile.setColor(Color(0, 255, 255));
            window.draw(sTile);
        }
        sTile.setColor(Color::White);

        // draw enemies
        for (int k = 0; k < enemyCount; ++k) {
            sEnemySprite.rotate(2.f);
            sEnemySprite.setPosition(enemies[k].x, enemies[k].y);
            window.draw(sEnemySprite);
        }

        // HUD
        window.draw(sidePanel);
        window.draw(hudTitle);
        p1Label.setString("P1: S=" + to_string(tracker1.getScore()) + " PU=" + to_string(tracker1.getPowerUps()));
        p2Label.setString("P2: S=" + to_string(tracker2.getScore()) + " PU=" + to_string(tracker2.getPowerUps()));
        window.draw(p1Label);
        window.draw(p2Label);

        // GAME OVER / RESULT
        if (!alive1 || !alive2) {
            Text over("", font, 24);
            over.setFillColor(Color::Yellow);

            if (!alive1 && !alive2) {
                int s1 = tracker1.getScore();
                int s2 = tracker2.getScore();
                if (s1 > s2)       over.setString("P1 Wins by Score!\nEsc=Menu");
                else if (s2 > s1)  over.setString("P2 Wins by Score!\nEsc=Menu");
                else               over.setString("Tie Game!\nEsc=Menu");
            }
            else if (!alive1)   over.setString("P2 Wins!\nEsc=Menu");
            else                over.setString("P1 Wins!\nEsc=Menu");

            FloatRect b = over.getLocalBounds();
            over.setOrigin(b.width / 2, b.height / 2);
            over.setPosition((N * ts) / 2.f, (M * ts) / 2.f);
            window.draw(over);
        }

        window.display();
    }
}




/////////////////////// INSTRUCTIONS /////////////////////////


void showInstructions(RenderWindow& window, Font& font) {
    static const char* lines[] = {
        "Xonix Instructions", "",
        "Objective:",
        "  • Capture playfield", "",
        "Controls:",
        "  • P1: Arrows", "  • P2: WASD", "  • Esc: Menu", "",
        "Gameplay:",
        "  • Form trail outside blue",
        "  • Return fills area",
        "  • Touch trail = lose", "",
        "Press Esc to return"
    };
    const int lineCount = sizeof(lines) / sizeof(lines[0]);

    // prepare texts
    Text txt[lineCount];
    for (int i = 0; i < lineCount; ++i) {
        txt[i].setFont(font);
        txt[i].setString(lines[i]);
        txt[i].setCharacterSize(20);
        txt[i].setFillColor(Color::White);
        txt[i].setPosition(50, 30 + i * 30);
    }

    // loop
    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) { window.close(); return; }
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
                return;
        }
        window.clear(Color::Black);
        for (int i = 0; i < lineCount; ++i)
            window.draw(txt[i]);
        window.display();
    }
}



/////////////////////// MENUS ///////////////////////////////
int showMainMenu(RenderWindow& w, Font& f) {
    const int opt = 4; const char* labels[opt] = { "Start Game","Instructions","View Scores","Exit" };
    Text m[opt]; int sel = 0;
    for (int i = 0; i < opt; i++)
    {
        m[i].setFont(f); m[i].setString(labels[i]); m[i].setCharacterSize(24); FloatRect r = m[i].getLocalBounds();
        m[i].setOrigin(r.width / 2, r.height / 2); m[i].setPosition(w.getSize().x / 2, w.getSize().y / 2 + i * 40);
    }
    while (w.isOpen()) {
        Event e; while (w.pollEvent(e)) {
            if (e.type == Event::Closed) w.close();
            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up) sel = (sel - 1 + opt) % opt;
                if (e.key.code == Keyboard::Down) sel = (sel + 1) % opt;
                if (e.key.code == Keyboard::Enter) return sel;
            }
        }
        w.clear(Color::Black);
        for (int i = 0; i < opt; i++)
        {
            m[i].setFillColor(i == sel ? Color::Yellow : Color::White); w.draw(m[i]);
        }
        w.display();
    }
    return 3;
}





int selectGameMode(RenderWindow& w, Font& f) {
    const int opt = 2; const char* labels[opt] = { "Single Player","Multiplayer" };
    Text m[opt]; int sel = 0;
    for (int i = 0; i < opt; i++) {
        m[i].setFont(f);
        m[i].setString(labels[i]); m[i].setCharacterSize(28);
        FloatRect r = m[i].getLocalBounds(); m[i].setOrigin(r.width / 2, r.height / 2);
        m[i].setPosition(w.getSize().x / 2, w.getSize().y / 2 + i * 50);
    }
    while (w.isOpen()) {
        Event e; while (w.pollEvent(e)) {
            if (e.type == Event::Closed) w.close();
            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up) sel = (sel - 1 + opt) % opt;
                if (e.key.code == Keyboard::Down) sel = (sel + 1) % opt;
                if (e.key.code == Keyboard::Enter) return sel;
            }
        }
        w.clear(Color::Black);
        for (int i = 0; i < opt; i++)
        {
            m[i].setFillColor(i == sel ? Color::Yellow : Color::White); w.draw(m[i]);
        }
        w.display();
    }
    return 0;
}

////////////////////////////// MATCH MAKING/ GAME ROOM //////////////////////////////////////

class GameRoomManager {
public:
    void attemptMatch(
        RenderWindow& window,
        Sprite& sTile,
        Sprite& sEnemy,
        Font& font
    ) {
        auto match = runMatchmaking("leaderboard.txt");
        const auto& p1 = match.first;
        const auto& p2 = match.second;

        if (p1.name.empty() || p2.name.empty()) {
            return;
        }

        runMultiplayerMode(window, sTile, sEnemy, font);

        gLeader.add(p1.name.c_str(), p1.score);
        gLeader.add(p2.name.c_str(), p2.score);
        gLeader.save("leaderboard.txt");


    }
};

GameRoomManager roomMgr;


Theme selectTheme(AVLNode* root) {
    std::cout << "\nAvailable Themes:\n";
    inorderPrint(root);  // List ID and name

    int choice = 0;
    AVLNode* node = nullptr;
    do {
        std::cout << "Enter theme ID to select: ";
        std::cin >> choice;
        node = findByID(root, choice);
        if (!node) {
            std::cout << "Invalid ID. Please try again.\n";
        }
    } while (!node);

    std::cout << "Selected Theme: "
        << node->data.name << " ("
        << node->data.description << ")\n\n";
    return node->data;
}

/////////////////////// MAIN ////////////////////////////////
int main() {
    srand(time(0));

    // ── INVENTORY MODULE PRELOAD ──
    AVLNode* themeRoot = nullptr;
    // Hard‑coded themes (ID, name, description)
    Theme themes[] = {
        Theme(1, "Classic Blue",   "Default blue color scheme"),
        Theme(2, "Retro Green",    "Old‑school green look"),
        Theme(3, "Neon Nights",    "Bright neon highlights"),
        Theme(4, "Monochrome",     "Black & white minimal"),
        Theme(5, "Solar Flare",    "Orange & yellow sunset"),
        Theme(6, "Cyberpunk",      "Futuristic purple neon"),
        Theme(7, "Forest Whisper", "Earthy green and brown")
    };
    const int themeCount = sizeof(themes) / sizeof(themes[0]);
    for (int i = 0; i < themeCount; ++i) {
        themeRoot = insert(themeRoot, themes[i]);
    }

    RenderWindow window(VideoMode(N * ts + 200, M * ts), "Xonix Game + Leaderboard");
    window.setFramerateLimit(60);
    Texture t1, t2, t3;
    t1.loadFromFile("C:/Users/yahya/OneDrive/Desktop/Xonix DS project/sprites/tiles.png");
    t2.loadFromFile("C:/Users/yahya/OneDrive/Desktop/Xonix DS project/sprites/gameover.png");
    t3.loadFromFile("C:/Users/yahya/OneDrive/Desktop/Xonix DS project/sprites/enemy.png");
    Sprite sTile(t1), sGameover(t2), sEnemy(t3); sEnemy.setOrigin(20, 20);
    Font font; font.loadFromFile("C:/Users/yahya/OneDrive/Desktop/Xonix DS project/fonts/Roboto_Condensed-Bold.ttf");

    string user;
    if (!showAuthScreen(window, font, user)) return 0;
    gLeader.load("leaderboard.txt");

    // Load any previously saved theme
    int savedID = loadPlayerTheme(user);

    // Let player choose a theme
    Theme playerTheme = selectTheme(themeRoot);

    //  Theme playerTheme;
    if (savedID != 0) {
        AVLNode* savedNode = findByID(themeRoot, savedID);
        if (savedNode) {
            playerTheme = savedNode->data;
            std::cout << "Loaded saved theme: " << playerTheme.name << "\n";
        }
        else {
            // Fallback if ID not found
            playerTheme = selectTheme(themeRoot);
        }
    }
    else {
        // No saved theme; prompt user
        playerTheme = selectTheme(themeRoot);
    }
    // Now save (or re‑save) their choice
    // (You can now store or apply playerTheme as needed)

    // ────────────────────────────────

    // After theme selection
    savePlayerTheme(user, playerTheme.id);


    int lastScore = 0;
    GameRoomManager roomMgr;


    while (window.isOpen()) {
        int choice = showMainMenu(window, font);
        switch (choice) {


        case 0: {
            int mode = selectGameMode(window, font);
            if (mode == 0) {
                int level = selectLevel(window, font); // 0 = Level 1, 1 = Level 2
                int sc = runSinglePlayerMode(window, sTile, sEnemy, font, level);
                lastScore = sc;
                gLeader.add(user.c_str(), sc);
                gLeader.save("leaderboard.txt");
            }

            else {
                g_matchQueue.enqueue(new Player(user, lastScore));

                roomMgr.attemptMatch(window, sTile, sEnemy, font);
            }
            break;
        }

        case 1: showInstructions(window, font); break;
        case 2: {
            bool back = false;
            while (window.isOpen() && !back) {
                Event e; while (window.pollEvent(e))
                {
                    if (e.type == Event::Closed) window.close();
                    if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) back = true;
                }
                window.clear(Color::Black);
                gLeader.draw(window, font);
                window.display();
            }
            break;
        }
        case 3: window.close(); break;
        }
    }
    return 0;
}