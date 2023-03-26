#include <bits/stdc++.h>
#include "node_funcs.h"
#include "zmq_funcs.h"
#include "BT.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: 1)./main, 2) child_id, 3) parent_port, 4) parent_id" << std::endl;
        exit(EXIT_FAILURE);
    }
    Nodemaster calc_node(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    while (true) {
        std::string message;
        std::string command;
        message = message_receiving(calc_node.parent);
        std::istringstream request(message);
        request >> command;
        if (command == "pid") {
            std::string reply = std::to_string(getpid());
            message_sending(calc_node.parent, reply);
        }
        else if (command == "ping") {
            int node_id;
            request >> node_id;
            std::string reply = calc_node.ping(node_id);
            message_sending(calc_node.parent, reply);
        }
        else if (command == "create") {
            int node_id;
            request >> node_id;
            std::string reply = calc_node.create(node_id);
            message_sending(calc_node.parent, reply);
        }
        else if (command == "send") {
            int node_id;
            std::string str;
            request >> node_id;
            getline(request, str);
            str.erase(0, 1);
            std::string reply = calc_node.sending_msg_part(str, node_id);
            message_sending(calc_node.parent, reply);
        }
        else if (command == "exec") {
            std::string str;
            getline(request, str);
            std::string answer = calc_node.exec(str);
            message_sending(calc_node.parent, answer);
        }
        else if (command == "kill") {
            std::string reply = calc_node.kill();
            message_sending(calc_node.parent, reply);
            disconnect(calc_node.parent, calc_node.parent_port);
            calc_node.parent.close();
            break;
        }
        else if (command == "clear") {
            int node_id;
            request >> node_id;
            std::string reply = calc_node.treeclear(node_id);
            message_sending(calc_node.parent, reply);
        }
    }
    return 0;
}