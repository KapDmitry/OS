#include <iostream>
#include "zmq.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>
int flag = 0;
std::string winner;
std::vector<std::string> waiter(int mode,int& current_players, int& all_players, zmq::socket_t& sock_sub, zmq::message_t& topic, zmq::message_t& payload,zmq::message_t& game_number ) {
	if (payload.to_string().length() != 0 || mode == 1) {
		for (int i = current_players; i < all_players; i++) {
			sock_sub.recv(topic);
			sock_sub.recv(payload);
			if (payload.to_string().length() == 0) {
				break;
			}
			else {
				current_players += 1;
				std::cout << "Connected: " << payload.to_string() << "! " << current_players << "/" << all_players << "\n";
			}
		}
		int timeout = 180000;
		sock_sub.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
		sock_sub.recv(topic);
		sock_sub.recv(payload);
		sock_sub.recv(game_number);
		std::cout << "game number(test)"<< game_number.to_string() << "\n";
		std::string gamenumber = game_number.to_string();
		std::vector<std::string> v{ payload.to_string(),gamenumber };
		return v;
	}
	else {
			std::string s =  "timeout";
			std::vector<std::string> v{s};
			return  v;
	}
}
std::string getGameName(std::string& str) {
	std::string st = "Connected to game: ";
	int n = st.length();
	std::string num;
	int i = n;
	while (str[i] != ' ') {
		num += str[i];
		++i;
	}
	return num;
}
void waiting(zmq::socket_t& sub,std::string &gamename, int& realpid) {
	sub.setsockopt(ZMQ_SUBSCRIBE, gamename.c_str(), gamename.size());
	zmq::message_t topic;
	zmq::message_t payload;
	zmq::message_t winner_name;
	sub.recv(topic);
	sub.recv(payload);
	sub.recv(winner_name);
	winner = winner_name.to_string();
	if (payload.to_string() != std::to_string(realpid)) {

		flag += 1;
     }
	else if (payload.to_string() == std::to_string(realpid)) {
		flag = -1;
	}
}
int  checknumber(std::string& inp,std::string &gamenumber) {
	int cows = 0;
	int bulls = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (inp[j] == gamenumber[i] && j == i)
				bulls += 1;
			if (inp[j] == gamenumber[i] && j != i) {
				cows += 1;
			}
		}
	}
	if (bulls == 4) {
		return 1;
	}
	else {
		std::cout << "bulls: " << bulls << " " << "cows: " << cows << "\n";
		return -1;
	}
}
int game(std::string& gamenumber, zmq::socket_t& sub, zmq::socket_t& req,std::string& gamename) {
	int realpid = getpid();
	std::thread sender_thread(waiting,std::ref(sub),std::ref(gamename),std::ref(realpid));
	int win_condition = 0;
	while (flag != 1) {
		std::cout << "input number: ";
		std::string inp;
		std::cin >> inp;
		if (checknumber(inp, gamenumber) == 1) {
			win_condition += 1;
			break;
		}
	}
	if (win_condition == 1) {
		std::string message = "4 " + gamename + " "+ std::to_string(realpid);
		zmq::message_t m_out(message);
		req.send(m_out, zmq::send_flags::none);
		zmq::message_t m_in_topic;
		zmq::message_t m_in_payload;
		req.recv(m_in_topic);
		req.recv(m_in_payload);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	if (flag == -1) {
		std::cout << "Your win\n";
	}
	else {
		std::cout << "You lose\n";
		std::cout << "Winner is: " << winner << "\n";
	}
	sender_thread.join();
	flag = 0;
	return 0;
}


int main() {
	zmq::context_t ctx;
	zmq::socket_t sock0(ctx, zmq::socket_type::req);
	sock0.connect("tcp://127.0.0.1:5555");
	zmq::socket_t sock_sub(ctx, zmq::socket_type::sub);
	sock_sub.connect("tcp://127.0.0.1:5545");
	std::string player_name;
	std::cout << "Input your player name\n";
	std::cin >> player_name;
	std::cout << "Your name is: " << player_name << "\n";
	while (1) {
		std::cout << "1.Create game 2.Connect to the game 3.Find game \n";
		int c;
		std::cin >> c;
		if (c == 1) {
			std::cout << "Input game name\n";  //ввод названия игры
			std::string gamename;
			std::cin >> gamename;
			sock_sub.setsockopt(ZMQ_SUBSCRIBE, gamename.c_str(), gamename.size());  // устанрвка фильтра сообщений соекта PUB-SUB 
			zmq::message_t topic;													// инициализация сообщений
			zmq::message_t payload;
			zmq::message_t game_number;
			zmq::message_t m_in_topic;
			zmq::message_t m_in_payload;
			std::string number_of_players;											// ввод кол-ва игроков
			std::cout << "cin number of players\n";
			std::cin >> number_of_players;
			int my_pid = getpid();													// получаем  pid (будет нужен как id игрока у сервера)
			//std::cout << "pid before sending: " << my_pid << "\n";
			std::string message = "1 " + gamename + " " + std::to_string(my_pid) + " " + player_name + " " + number_of_players;   // формируем сообщение для отправки на сервер
			zmq::message_t m_out(message);
			sock0.send(m_out, zmq::send_flags::none);
			sock0.recv(m_in_topic);   //принимаем ответ
			sock0.recv(m_in_payload);
			int code = 0;
			int topico = stoi(m_in_topic.to_string());
			if (topico == code) { 
				std::cout << "waiting for other people in lobby...\n";
				int  timeout = 180000;
				sock_sub.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
				int h = stoi(number_of_players);
				//std::cout << "h " << h << "\n";
				int current_players = 1;
				std::vector<std::string> v = waiter(1,current_players, h, sock_sub, topic, payload, game_number);
				if (v[0] == "ready"){
					std::cout << "the game is ready\n";
					std::cout << "Game starting\n";
					game(v[1], sock_sub, sock0, gamename);
				}
				else {
					std::cout << "error timeout\n";
					std::cout << v[0];
				}
			}
			else {
				std::cout << m_in_payload.to_string() << "\n";
			}
		}
		if (c == 2) {
			std::cout << "cin name of the game\n";
			std::string gamename;
			std::cin >> gamename;
		    sock_sub.setsockopt(ZMQ_SUBSCRIBE, gamename.c_str(), gamename.size());
			zmq::message_t topic;
			zmq::message_t payload;
			zmq::message_t game_number;
			zmq::message_t m_in_topic;
			zmq::message_t m_in_payload;
			int my_pid = getpid();
			std::string message = "2 " + gamename + " " + player_name+ " " + std::to_string(my_pid);
			zmq::message_t m_out(message);
			sock0.send(m_out, zmq::send_flags::none);
			sock0.recv(m_in_topic);
			sock0.recv(m_in_payload);
			int code = 0;
			int topico = stoi(m_in_topic.to_string());
			if (topico == code) {
				std::cout << "waiting for other people\n";
				std::string new_message = m_in_payload.to_string();
				char str1 = new_message[new_message.length() - 3];
				char str2 = new_message[new_message.length() - 1];
				int current_players = str1 - '0';
				int all_players = str2 - '0';
				int timeout = 180000;
				sock_sub.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
				sock_sub.recv(topic);
				sock_sub.recv(payload);
				std::vector<std::string> v = waiter(0,current_players, all_players, sock_sub, topic, payload, game_number);
				if (v[0] == "ready") {
					std::cout << "the game is ready\n";
					std::string gamenumber = game_number.to_string();
					game(v[1], sock_sub, sock0, gamename);
				}
				else {
					std::cout << "error timeout\n";
				}
			}
			else {
				std::cout << m_in_payload << "\n";
			}
		}
		if (c == 3) {
			std::cout << "Waitging for the game\n";
			int my_pid = getpid();
			std::string message = "3 " + player_name + " " + std::to_string(my_pid);
			zmq::message_t m_out(message);
			sock0.send(m_out, zmq::send_flags::none);
			zmq::message_t m_in_topic;
			zmq::message_t m_in_payload;
			sock0.recv(m_in_topic);
			sock0.recv(m_in_payload);
			int code = 0;
			int topico = stoi(m_in_topic.to_string());
			if (topico == code) {
				std::cout << m_in_payload.to_string() << "\n";
				std::cout << "waiting for other people\n";
				std::string message_in = m_in_payload.to_string();
				std::string gamename = getGameName(message_in);
				std::cout << gamename;
				sock_sub.setsockopt(ZMQ_SUBSCRIBE, gamename.c_str(), gamename.size());
				zmq::message_t topic;
				zmq::message_t payload;
				zmq::message_t game_number;
				std::string new_message = m_in_payload.to_string();
				char str1 = new_message[new_message.length() - 3];
				char str2 = new_message[new_message.length() - 1];
				int current_players = str1 - '0';
				int all_players = str2 - '0';
				int timeout = 180000;
				sock_sub.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
				sock_sub.recv(topic);
				sock_sub.recv(payload);
				std::vector<std::string> v = waiter(0, current_players, all_players, sock_sub, topic, payload, game_number);
				if (v[0] == "ready") {
					std::cout << "the game is ready\n";
					game(v[1], sock_sub, sock0, gamename);
				}
				else {
					std::cout << "error\n";
				}
			}
			else {
				std::cout << m_in_payload.to_string() << "\n";
			}
		}
		if (c == 4) {
			exit(1);
		}
	}

	 
	return 0;

}