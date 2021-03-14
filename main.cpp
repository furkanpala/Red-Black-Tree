/*
    Furkan Pala

    How to compile
    g++ main.cpp -std=c++11

    How to run
    ./a.out filename
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Node
{
    string season, name, team;
    int rebound, assist, point;
    char colour;
    Node *right, *left, *parent;
    Node();
    Node(string, string, string, int, int, int);
    Node *getGrandparent() const;
    Node *getUncle() const;
    bool operator>(const Node &) const;
    bool operator==(const Node &) const;
};

Node::Node() : season(""), name(""), team(""), rebound(0), assist(0), point(0), colour('r'), right(nullptr), left(nullptr), parent(nullptr)
{
}

Node::Node(string season, string name, string team, int rebound, int assist, int point)
    : season(season), name(name), team(team), rebound(rebound), assist(assist), point(point), colour('r'), right(nullptr), left(nullptr), parent(nullptr)
{
}

Node *Node::getGrandparent() const
{
    return this->parent->parent;
}

Node *Node::getUncle() const
{
    if (this->parent == this->getGrandparent()->left)
        return this->getGrandparent()->right;
    else
        return this->getGrandparent()->left;
}

bool Node::operator>(const Node &node) const
{
    if (name.compare(node.name) > 0)
        return true;
    else
        return false;
}

bool Node::operator==(const Node &node) const
{
    if (name.compare(node.name) == 0)
        return true;
    else
        return false;
}

struct RB_Tree
{
    Node *root;
    Node *pointLeader;
    Node *assistLeader;
    Node *reboundLeader;

    RB_Tree();
    void RotateRight(Node *);
    void RotateLeft(Node *);
    void LL(Node *);
    void LR(Node *);
    void RR(Node *);
    void RL(Node *);
    void Insert(const Node &);
    void PreOrderPrint(const Node *const, int) const;
    void DeleteTree(Node *);
    ~RB_Tree();
};

RB_Tree::RB_Tree()
    : root(nullptr)
{
}

void RB_Tree::RotateRight(Node *node)
{
    Node *temp = node->left;
    node->left = temp->right;
    if (temp->right)
        temp->right->parent = node;
    temp->right = node;
    node->parent = temp;
}

void RB_Tree::RotateLeft(Node *node)
{
    Node *temp = node->right;
    node->right = temp->left;
    if (temp->left)
        temp->left->parent = node;
    temp->left = node;
    node->parent = temp;
}

void RB_Tree::LL(Node *node)
{
    // Connect parent of grandparent, if any, to parent of node
    // Otherwise make parent of node root
    Node *grandparent = node->getGrandparent();
    if (node->getGrandparent()->parent)
    {
        if (node->getGrandparent() == node->getGrandparent()->parent->left)
        {
            node->getGrandparent()->parent->left = node->parent;
            node->parent->parent = node->getGrandparent()->parent;
        }
        else
        {
            node->getGrandparent()->parent->right = node->parent;
            node->parent->parent = node->getGrandparent()->parent;
        }
    }
    else
    {
        root = node->parent;
        root->parent = nullptr;
    }
    // Rotate right grandparent
    RotateRight(grandparent);

    // Swap colours of grandparent and parent
    // Grandparent becomes red
    // Parent becomes black
    node->parent->colour = 'b';
    node->parent->right->colour = 'r';
}

void RB_Tree::LR(Node *node)
{
    // Make node left child of grandparent
    Node *parent = node->parent;
    node->getGrandparent()->left = node;
    node->parent = node->getGrandparent();
    // Rotate left parent
    RotateLeft(parent);

    // Apply LL case
    LL(node->left);
}

void RB_Tree::RR(Node *node)
{
    // Connect parent of grandparent, if any, to parent of node
    // Otherwise make parent of node root
    Node *grandparent = node->getGrandparent();
    if (node->getGrandparent()->parent)
    {
        if (node->getGrandparent() == node->getGrandparent()->parent->left)
        {
            node->getGrandparent()->parent->left = node->parent;
            node->parent->parent = node->getGrandparent()->parent;
        }
        else
        {
            node->getGrandparent()->parent->right = node->parent;
            node->parent->parent = node->getGrandparent()->parent;
        }
    }
    else
    {
        root = node->parent;
        root->parent = nullptr;
    }

    // Rotate left grandparent
    RotateLeft(grandparent);

    // Swap colours of grandparent and parent
    // Grandparent becomes red
    // Parent becomes black
    node->parent->colour = 'b';
    node->parent->left->colour = 'r';
}

void RB_Tree::RL(Node *node)
{
    // Make node left child of grandparent
    Node *parent = node->parent;
    node->getGrandparent()->right = node;
    node->parent = node->getGrandparent();

    // Rotate left grandparent
    RotateRight(parent);

    // Apply RR case
    RR(node->right);
}

void RB_Tree::Insert(const Node &newNode)
{
    Node *traverse = root;
    Node *tail = traverse;
    char insertPos = 'l';
    Node *node = new Node(newNode.season, newNode.name, newNode.team, newNode.rebound, newNode.assist, newNode.point);

    // ---BST INSERT---
    if (root == nullptr)
    {
        root = node;
        node->colour = 'b';
        pointLeader = root;
        assistLeader = root;
        reboundLeader = root;
    }
    else
    {
        // Search for correct position
        while (traverse)
        {
            tail = traverse;

            // If player already exists
            // accumulate new statistics
            if (newNode == *traverse)
            {
                traverse->point += newNode.point;
                traverse->assist += newNode.assist;
                traverse->rebound += newNode.rebound;

                // Check for leaders and update if necessary
                if (traverse->point > pointLeader->point)
                    pointLeader = traverse;

                if (traverse->assist > assistLeader->assist)
                    assistLeader = traverse;

                if (traverse->rebound > reboundLeader->rebound)
                    reboundLeader = traverse;

                return;
            }

            else if (newNode > *traverse)
            {
                traverse = traverse->right;
                insertPos = 'r';
            }
            else
            {
                traverse = traverse->left;
                insertPos = 'l';
            }
        }

        if (insertPos == 'l')
            tail->left = node;
        else
            tail->right = node;

        node->parent = tail;
    }

    // Check for leaders and update if necessary
    if (node->point > pointLeader->point)
        pointLeader = node;

    if (node->assist > assistLeader->assist)
        assistLeader = node;

    if (node->rebound > reboundLeader->rebound)
        reboundLeader = node;

    //---RB TREE FIX VIOLATION---
    while (node != root && node->colour == 'r' && node->parent->colour == 'r')
    {
        // Recolouring
        // Parent is red and uncle is red
        if (node->getUncle() && node->getUncle()->colour == 'r')
        {
            node->parent->colour = 'b';
            node->getUncle()->colour = 'b';
            node->getGrandparent()->colour = 'r';
            node = node->getGrandparent();
        }
        // Rotate
        // Parent is red and uncle is black
        else if (node->parent->colour == 'r' && (!node->getUncle() || node->getUncle()->colour == 'b'))
        {
            // LL Case
            // Node is left child of parent
            // Parent is left child of grandparent
            if (node == node->parent->left && node->parent == node->getGrandparent()->left)
                LL(node);

            // LR Case
            // Node is right child of parent
            // Parent is left child of grandparent
            else if (node == node->parent->right && node->parent == node->getGrandparent()->left)
                LR(node);

            // RR Case
            // Node is right child of parent
            // Parent is right child of grandparent
            else if (node == node->parent->right && node->parent == node->getGrandparent()->right)
                RR(node);

            // RL Case
            // Node is left child of parent
            // Parent is right child of grandparent
            else if (node == node->parent->left && node->parent == node->getGrandparent()->right)
                RL(node);
        }
    }
    root->colour = 'b';
}

void RB_Tree::PreOrderPrint(const Node *const node, int depth) const
{
    if (node == nullptr)
        return;

    for (int i = 0; i < depth; i++)
        cout << "-";

    if (node->colour == 'b')
        cout << "(BLACK) ";
    else
        cout << "(RED) ";

    cout << node->name << endl;

    depth++;
    PreOrderPrint(node->left, depth);
    PreOrderPrint(node->right, depth);
}

// Deletes tree in postorder traversal
void RB_Tree::DeleteTree(Node *node)
{
    if (node == nullptr)
        return;

    DeleteTree(node->left);
    DeleteTree(node->right);

    delete node;
}

RB_Tree::~RB_Tree()
{
    DeleteTree(root);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Filename is required" << endl;
        return 1;
    }

    ifstream inFile;

    inFile.open(argv[1]);

    if (!inFile.is_open())
    {
        cerr << "File could not be opened" << endl;
        return 1;
    }

    RB_Tree rb;

    string row;
    getline(inFile, row);

    string prevSeason = " ";
    bool isTreePrinted = false;

    while (getline(inFile, row))
    {
        stringstream ss(row);
        Node node;
        string s;
        getline(ss, node.season, ',');

        if (prevSeason != " " && prevSeason != node.season)
        {
            cout << "End of the " << prevSeason << " Season" << endl;
            cout << "Max Points: " << rb.pointLeader->point << " - "
                 << "Player Name: " << rb.pointLeader->name << endl;
            cout << "Max Assists: " << rb.assistLeader->assist << " - "
                 << "Player Name: " << rb.assistLeader->name << endl;
            cout << "Max Rebs: " << rb.reboundLeader->rebound << " - "
                 << "Player Name: " << rb.reboundLeader->name << endl;

            if (!isTreePrinted)
            {
                rb.PreOrderPrint(rb.root, 0);
                isTreePrinted = true;
            }
        }

        prevSeason = node.season;

        getline(ss, node.name, ',');
        getline(ss, node.team, ',');
        getline(ss, s, ',');
        node.rebound = stoi(s);
        getline(ss, s, ',');
        node.assist = stoi(s);
        getline(ss, s, ',');
        node.point = stoi(s);

        rb.Insert(node);
    }

    cout << "End of the " << prevSeason << " Season" << endl;
    cout << "Max Points: " << rb.pointLeader->point << " - "
         << "Player Name: " << rb.pointLeader->name << endl;
    cout << "Max Assists: " << rb.assistLeader->assist << " - "
         << "Player Name: " << rb.assistLeader->name << endl;
    cout << "Max Rebs: " << rb.reboundLeader->rebound << " - "
         << "Player Name: " << rb.reboundLeader->name << endl;

    if (!isTreePrinted)
    {
        rb.PreOrderPrint(rb.root, 0);
        isTreePrinted = true;
    }

    return 0;
}