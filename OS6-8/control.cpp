#include <bits/stdc++.h>
#include "node_funcs.h"
#include "zmq_funcs.h"
#include "BT.h"

int main() {
    std::string command;
    Nodemaster this_nodule(-1, -1, -1);
    std::string answer;
    std::cout << "create id" << std::endl;
    std::cout << "exec id" << std::endl;
    std::cout << "pingall" << std::endl;
    std::cout << "kill id" << std::endl;
    BalancedTree tree;
    while (std::cin >> command) {
        if (command == "create") {
            int new_node_id;
            std::cin >> new_node_id;
            if (tree.Exist(new_node_id)) {
                std::cout << "Error: Already exists" << std::endl;
            }
            else {
                while (true) {
                    int Parent_ID = tree.IDSearch();
                    if (Parent_ID == this_nodule.id) {
                        answer = this_nodule.create(new_node_id);
                        tree.AddInTree(new_node_id, Parent_ID);
                        break;
                    }
                    else {
                        std::string message = "create " + std::to_string(new_node_id);
                        answer = this_nodule.sending_msg_part(message, Parent_ID);
                        if (answer == "Error: Parent not found") {
                            tree.AvailabilityCheck(Parent_ID);
                        }
                        else {
                            tree.AddInTree(new_node_id, Parent_ID);
                            break;
                        }
                    }
                }
                std::cout << answer << std::endl;
            }
        }
        else if (command == "exec") {
            std::string pattern, text;
            int node_id;
            std::cin >> node_id;
            if (!tree.Exist(node_id)) {
                std::cout << "Error: Parent is not existed" << std::endl;
            }
            else {
                std::cout << "Enter pattern text: ";
                std::cin >> pattern;
                std::cout << "Enter text: ";
                std::cin >> text;
                std::string message = "exec " + pattern + " " + text;
                answer = this_nodule.sending_msg_part(message, node_id);
                std::cout << answer << std::endl;
            }
        }
        else if (command == "pingall") {
            std::string str;
            std::vector<int> unavailable;

            for (int j : tree.id_set) {
                std::string answer = this_nodule.ping(j);
                if (answer != "Ok: 1") {
                    unavailable.push_back(j);
                }
            }
            if (unavailable.empty()) {
                std::cout << "Ok: -1" << std::endl;
            }
            else {
                std::cout << "Ok: ";
                for (int z = 0; z < unavailable.size(); ++z) {
                    std::cout << unavailable[z] << " ";
                }
                std::cout << std::endl;
                unavailable.clear();
            }

        }
        else if (command == "kill") {
            int node_id;
            std::cin >> node_id;
            std::string message = "kill";
            if (!tree.Exist(node_id)) {
                std::cout << "Error: Parent  not exist" << std::endl;
            }
            else {
                answer = this_nodule.sending_msg_part(message, node_id);
                if (answer != "Error: Parent did not found") {
                    tree.Delete_from_Root(node_id);
                    if (node_id == this_nodule.left_id) {
                        unbind(this_nodule.left, this_nodule.left_port);
                        this_nodule.left_id = -2;
                        answer = "Ok";
                    }
                    else if (node_id == this_nodule.right_id) {
                        this_nodule.right_id = -2;
                        unbind(this_nodule.right, this_nodule.right_port);
                        answer = "Ok";
                    }
                    else {
                        message = "clear " + std::to_string(node_id);
                        answer = this_nodule.sending_msg_part(message, std::stoi(answer));
                    }
                    std::cout << answer << std::endl;
                }
            }
        }
        else {
            std::cout << "Please enter correct command!" << std::endl;
        }
    }
    this_nodule.kill();
    return 0;
}