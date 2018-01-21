#include "stdafx.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <fstream>
#include <iostream>
#include "NGram.h"
using namespace std;

void sanitize(string &in) {
	char c[] = { ',','.','/','!','?','-','*','_','(',')','&','%','$','#',':',';', '\\', '"','\''};

	for (int a = 0; a < sizeof(c) / sizeof(c[0]); a++) {
		in.erase(remove(in.begin(), in.end(), c[a]), in.end());
	}

	transform(in.begin(), in.end(), in.begin(), ::tolower);
}

void split(const std::string &s, char delim, vector<string> &result) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		result.push_back(item);
	}
}

void splitMain(const string &s, char delim, vector<string> &elems) {
	split(s, delim, elems);
}

NGram* find(vector<NGram> &stuff, string z) {
	for (int i = 0; i < stuff.size(); i++) {
		if (stuff[i].phrase == z) {
			return &stuff[i];
		}
	}

	stuff.push_back(NGram(z));
	return &stuff.back();
}

void writeNG(vector<NGram> &ng, string out) {
	ofstream file;
	file.open(out);

	for (int i = 0; i < ng.size(); i++) {
		string info = ng[i].phrase+"|";
		map<string, int>::iterator it;
		for (it = ng[i].ng.begin(); it != ng[i].ng.end(); it++)
		{
			info += it->first + " " + to_string(it->second)+"|";
		}

		file << info << endl;
	}

	file.close();
}

void readNG(vector<NGram> &ng, string out) {
	ifstream file;
	file.open(out);

	string x;
	while (file >> x) {
		vector<string> all;
		splitMain(x, '|', all);
		NGram n(all[0]);
		for (int i = 0; i < all.size(); i++) {
			vector<string> next;
			splitMain(all[i], ' ', next);
			n.add(next[0], stoi(next[1]));
		}
	}
	
	
	file.close();
}

int main() {
	vector<string> all;
	vector<NGram> two;
	vector<NGram> three;
	vector<NGram> one;
	ifstream f("one.txt");
	//this creates
	if (!f.good()) {
		ifstream file;
		file.open("big.txt");

		cout << f.good();
		string x;
		while (file >> x) {
			splitMain(x, ' ', all);
		}
		file.close();

		for (int i = 0; i < all.size(); i++) {
			sanitize(all[i]);
		}

		int maxIteration = 20000;
		NGram *n = new NGram("");
		for (int i = 0; i < maxIteration; i++) {
			for (int size = 1; size <= 3; size++) {
				if (i + size + 1 >= maxIteration) {
					break;
				}

				string search = "";
				for (int a = 0; a < size && a + i < maxIteration; a++) {
					search += all[i + a] + " "; //this could be problematic with a hanging space but we type in that manner
				}

				switch (size) {
				case 2:
					n = find(two, search);
					break;
				case 3:
					n = find(three, search);
					break;
				case 1:
					n = find(one, search);
					break;
				}
				n->add(all[i + size + 1]);
			}
		}

		writeNG(one,"one.txt");
		writeNG(two, "two.txt");
		writeNG(three, "three.txt");

		delete n;
	}
	else {
		readNG(one, "one.txt");
		readNG(two, "two.txt");
		readNG(three, "three.txt");
	}
	return 0;
}

