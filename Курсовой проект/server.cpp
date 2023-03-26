#include <iostream>
#include "zmq.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
struct Player {
	int player_pid;
	std::string player_name;
};
class Game {
public:
	std::string game_id;
	std::vector<Player>players;
	int players_quantity;
	std::string number;
	int current_players;
	int cond;
	Game()
	{
		game_id;
		players;
		players_quantity = 0;
		current_players = 0;
		cond = 0;
		std::string number = "";
	}
	void generate_number() {
		srand(static_cast<unsigned int>(time(0)));
		int a = (rand() % 10);
		int b = (rand() % 10);
		while (a == b) {
			b = (rand() % 10);
		}
		int c = (rand() % 10);
		while (a == c || b == c) {
			c = (rand() % 10);
		}
		int d = (rand() % 10);
		while (a == d || b == d || c == d) {
			d = (rand() % 10);
		}
		number = std::to_string(a) + std::to_string(b) + std::to_string(c) + std::to_string(d);
	} 
};
std::string getName(std::string& str) {
	int n = 2;
	std::string name;
	while (str[n] != ' ') {
		name += str[n];
		n += 1;
	}
	return name;
}
int getPid(std::string& str,int n) {
	std::string pid;
	for (int i = n; i < str.length(); i++) {
		if (str[i] == ' ')
			break;
		pid += str[i];
	}
	int user_pid = stoi(pid);
	return user_pid;
}
int getNum(std::string& str) {
	std::string num;
	int i = str.length();
	while (str[i] != ' ') {
		num += str[i];
		--i;
	}
	(std::reverse(num.begin(), num.end()));
	int res = stoi(num);
	return res;

}
std::string getPlayer(std::string& str,int n) {
	std::string player_name;
	while (str[n] != ' ') {
		player_name += str[n];
		n += 1;
	}
	return player_name;
}
bool findgame(std::vector<Game>& g, Player player, std::string name) {
	for (int i = 0; i < g.size(); i++) {
		if (g[i].game_id == name) {
			if (g[i].current_players + 1 <= g[i].players_quantity) {
				g[i].players.push_back(player);
				g[i].current_players += 1;
				return true;
			}
		}
	}
	return false;
}
bool checkgame(std::vector<Game>& g, std::string name) {
	for (int i = 0; i < g.size(); i++) {
		if (g[i].game_id == name) {
			if (g[i].current_players  == g[i].players_quantity) {
				return true;
			}
		}
	}
	return false;
}
bool checkname(std::string& gamename, std::vector<Game>& g) {
	for (int i = 0; i < g.size(); i++) {
		if (g[i].game_id == gamename) {
			return false;
		}
	}
	return true;
}
void deletegame(std::string& gamename, std::vector<Game>& g) {
	int iter_position = 0;
	for (int i = 0; i < g.size(); i++) {
		if (g[i].game_id == gamename) {
			 iter_position = i;
			break;
		}
	}
	std::vector<Game>::iterator my_iterator = g.begin();
	my_iterator += iter_position;
	g.erase(my_iterator);
}
void sending(zmq::socket_t &sock,std::string rep,int code) {
	if (code == 0) {
		zmq::message_t reply_topic("0");
		zmq::message_t reply(rep);
		sock.send(reply_topic, zmq::send_flags::sndmore);
		sock.send(reply, zmq::send_flags::none);
	}
	if (code != 0) {
		zmq::message_t reply_topic(std::to_string(code));
		zmq::message_t reply(rep);
		sock.send(reply_topic, zmq::send_flags::sndmore);
		sock.send(reply, zmq::send_flags::none);
	}

}
std::string getWinner(std::vector<Game>& g, int& win_pid) {
	std::string winner;
	for (int i = 0; i < g.size(); i++) {
		for (int j = 0; j < g[i].players.size(); j++) {
			if (g[i].players[j].player_pid == win_pid) {
				winner = g[i].players[j].player_name;
			}
		}
	}
	return winner;
}
std::string getCurrentstring(std::string& name, std::vector<Game>& g) {
	std::string current_players;
	for (int i = 0; i < g.size(); i++) {
		if (g[i].game_id == name) {
			for (int j = 0; j < g[i].players.size() ; j++) {
				current_players += g[i].players[j].player_name + " ";
			}
		}
	}
	return current_players;
}
std::string getCurrentnumber(std::string& name, std::vector<Game>& g) {
	std::string current_players;
	for (int i = 0; i < g.size(); i++) {
		if (g[i].game_id == name) {
			current_players = std::to_string(g[i].current_players) + "/" + std::to_string(g[i].players_quantity);
			
		}
	}
	return current_players;
}
int check_game_condition(std::vector<Game>& g, std::string& name) {
	for (int i = 0; i < g.size(); i++) {
		if (g[i].game_id == name) {
			if (g[i].cond == 0) {
				g[i].cond = 1;
				return 0;
			}
		}
	}
		return 1;
}
int main() {
	zmq::context_t ctx;
	zmq::socket_t sock_rep(ctx,zmq::socket_type::rep);
	sock_rep.bind("tcp://127.0.0.1:5555");
	zmq::socket_t sock_pub(ctx, zmq::socket_type::pub);
	sock_pub.bind("tcp://127.0.0.1:5545");
	std::vector<Game>games;
	while (1) {
		zmq::message_t m_in;
		sock_rep.recv(m_in);
		std::string message = m_in.to_string();
		std::cout << message;
		if (message[0] == '1') {
			int user_pid;
			std::string name;
			int n = 2;
			name = getName(message);
			n += name.length() + 1;
			std::string player_name;
			user_pid = getPid(message, n);
			n += std::to_string(user_pid).length()+1;
			player_name = getPlayer(message, n);
			int num_player;
			num_player = getNum(message);
			if (checkname(name, games) == true) {
				Game game;
				Player player;
				game.game_id = name;
				player.player_name = player_name;
				player.player_pid = user_pid;
				game.players.push_back(player);
				game.players_quantity = num_player;
				game.current_players += 1;
				game.cond = 0;
				game.generate_number();
				std::string gamenumber = game.number;
				games.push_back(game);
				std::string reply = "Game: " + name + " created!";
				sending(sock_rep, reply,0);
				if (game.players_quantity == game.current_players) {
					std::string str = "ready";
					zmq::message_t topic(name);
					zmq::message_t payload(str);
					zmq::message_t game_number(gamenumber);
					sock_pub.send(topic, zmq::send_flags::sndmore);
					sock_pub.send(payload, zmq::send_flags::sndmore);
					sock_pub.send(game_number, zmq::send_flags::none);
				}
			}
			else {
				std::string reply = "Game already exists. Please choose other name.";
				sending(sock_rep, reply,1);
			}
		}
		if (message[0] == '2') {
			int user_pid;
			std::string name;
			name = getName(message);
			std::string player_name;
			player_name = getPlayer(message, 2 + name.length() + 1);
			user_pid = getPid(message, 2 + name.length() + 1+player_name.length()+1);
			Player player;
			player.player_name = player_name;
			player.player_pid = user_pid;
			if (findgame(games, player, name) == true) {
				std::string reply;
				std::string current_players = getCurrentstring(name, games);
				std::string current_number = getCurrentnumber(name, games);
				reply = "Connected to game:" + name + " in lobby now: " + current_players + current_number;
				sending(sock_rep,reply,0);
				zmq::message_t  top1(name);
				zmq::message_t pay1(player_name);
				sock_pub.send(top1, zmq::send_flags::sndmore);
				sock_pub.send(pay1, zmq::send_flags::none);
				if (checkgame(games, name) == true) {
					std::string str = "ready";
					zmq::message_t topic(name);
					zmq::message_t payload(str);
					std::string gamenumber;
					for (int i = 0; i < games.size(); i++) {
						if (games[i].game_id == name) {
							gamenumber = games[i].number;
						}
					}
					zmq::message_t game_number(gamenumber);
					sock_pub.send(topic, zmq::send_flags::sndmore);
					sock_pub.send(payload, zmq::send_flags::sndmore);
					sock_pub.send(game_number, zmq::send_flags::none);
				}
			}
			else {
				std::string reply;
				reply = "Game not found :(";
				sending(sock_rep, reply,1);
			}

		}
		if (message[0] == '3') {
			std::string player_name = getPlayer(message, 2);
			int user_pid = getPid(message, 2+player_name.length()+1);
			Player player;
			player.player_name = player_name;
			player.player_pid = user_pid;
			std::string gamename;
			std::string gamenumber;
			for (int i = 0; i < games.size(); i++) {
				if (games[i].current_players < games[i].players_quantity) {
					gamename = games[i].game_id;
					gamenumber = games[i].number;
					break;
				}

			}
			if (findgame(games, player, gamename) == true) {
				std::string reply;
				std::string current_players = getCurrentstring(gamename, games);
				std::string current_number = getCurrentnumber(gamename, games);
				reply = "Connected to game: " + gamename+" "+current_players + current_number;;
				sending(sock_rep, reply,0);
				sleep(3);
				zmq::message_t  top1(gamename);
				zmq::message_t pay1(player_name);
				sock_pub.send(top1, zmq::send_flags::sndmore);
				sock_pub.send(pay1, zmq::send_flags::none);
				if (checkgame(games, gamename) == true) {
					std::string str = "ready";
					zmq::message_t topic(gamename);
					zmq::message_t payload(str);
					sleep(3);
					zmq::message_t game_number(gamenumber);
					sock_pub.send(topic, zmq::send_flags::sndmore);
					sock_pub.send(payload, zmq::send_flags::sndmore);
					sock_pub.send(game_number, zmq::send_flags::none);
				}
			}
			else {
				std::string reply;
				reply = "Game not found :(";
				sending(sock_rep, reply,1);
			}
		}
		if (message[0] == '4') {
			std::string gamename = getName(message);
				int win_pid = getPid(message, gamename.length() + 3);
				std::string reply = "ok";
				sending(sock_rep, reply, 0);
				if (check_game_condition(games,gamename)==0) {
				zmq::message_t topic(gamename);
				std::string pid = std::to_string(win_pid);
				zmq::message_t payload(pid);
				std::string winner = getWinner(games, win_pid);
				zmq::message_t payload2(winner);
				sock_pub.send(topic, zmq::send_flags::sndmore);
				sock_pub.send(payload, zmq::send_flags::sndmore);
				sock_pub.send(payload2, zmq::send_flags::none);
				deletegame(gamename, games);
				}
		}
	}
	return 0;
}