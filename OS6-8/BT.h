#ifndef TOPOLOGY_H
#define TOPOLOGY_H
#include <bits/stdc++.h>
class BalancedTree {
    class BalancedTreeNode {
    public:
        int id;
        BalancedTreeNode* left;
        BalancedTreeNode* right;
        int height;
        bool available;
        BalancedTreeNode(int id) {
            this->id = id;
            available = true;
            left = NULL;
            right = NULL;
        }
        void Available_Check(int id) {
            if (this->id == id) {
                available = false;
            }
            else {
                if (left != NULL) {
                    left->Available_Check(id);
                }
                if (right != NULL) {
                    right->Available_Check(id);
                }
            }
        }
        void Delete(int id, std::set<int>& id_set) {
            if (left != NULL && left->id == id) {
                left->Delete_Recursion(id_set);
                id_set.erase(left->id);
                delete left;
                left = NULL;
            }
            else if (right != NULL && right->id == id) {
                right->Delete_Recursion(id_set);
                id_set.erase(right->id);
                delete right;
                right = NULL;
            }
            else {
                if (left != NULL) {
                    left->Delete(id, id_set);
                }
                if (right != NULL) {
                    right->Delete(id, id_set);
                }
            }
        }
        void Delete_Recursion(std::set<int>& id_set) {
            if (left != NULL) {
                left->Delete_Recursion(id_set);
                id_set.erase(left->id);
                delete left;
                left = NULL;
            }
            if (right != NULL) {
                right->Delete_Recursion(id_set);
                id_set.erase(right->id);
                delete right;
                right = NULL;
            }
        }
        void Adding(int id, int parent_id, std::set<int>& id_set) {
            if (this->id == parent_id) {
                if (left == NULL) {
                    left = new BalancedTreeNode(id);
                }
                else {
                    right = new BalancedTreeNode(id);
                }
                id_set.insert(id);
            }
            else {
                if (left != NULL) {
                    left->Adding(id, parent_id, id_set);
                }
                if (right != nullptr) {
                    right->Adding(id, parent_id, id_set);
                }
            }
        }
        int Min_Height() {
            if (left == NULL || right == NULL) {
                return 0;
            }
            int left_height = -1;
            int right_height = -1;
            if (left != NULL && left->available == true) {
                left_height = left->Min_Height();
            }
            if (right != NULL && right->available == true) {
                right_height = right->Min_Height();
            }
            if (right_height == -1 && left_height == -1) {
                available = false;
                return -1;
            }
            else if (right_height == -1) {
                return left_height + 1;
            }
            else if (left_height == -1) {
                return right_height + 1;
            }
            else {
                return std::min(left_height, right_height) + 1;
            }
        }
        int Min_Height_ID(int height, int current_height) {
            if (height < current_height) {
                return -2;
            }
            else if (height > current_height) {
                int current_id = -2;
                if (left != NULL && left->available == true) {
                    current_id = left->Min_Height_ID(height, (current_height + 1));
                }
                if (right != NULL && right->available == true && current_id == -2) {
                    current_id = right->Min_Height_ID(height, (current_height + 1));
                }
                return current_id;
            }
            else {
                if (left == NULL || right == NULL) {
                    return id;
                }
                return -2;
            }
        }
        ~BalancedTreeNode() {}
    };
private:
    BalancedTreeNode* root;
public:
    std::set<int> id_set;
    BalancedTree() {
        root = new BalancedTreeNode(-1);
    }
    bool Exist(int id) {
        if (id_set.find(id) != id_set.end()) {
            return true;
        }
        return false;
    }
    void AvailabilityCheck(int id) {
        root->Available_Check(id);
    }
    int IDSearch() {
        int h = root->Min_Height();
        return root->Min_Height_ID(h, 0);
    }
    void AddInTree(int id, int parent) {
        root->Adding(id, parent, id_set);
    }
    void Delete_from_Root(int idElem) {
        root->Delete(idElem, id_set);
    }
    ~BalancedTree() {
        root->Delete_Recursion(id_set);
        delete root;
    }
};
#endif 