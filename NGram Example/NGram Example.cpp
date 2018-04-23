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
	char c[] = { ',','.','/','!','?','-','*','_','(',')','&','%','$','#',':',';', '\\', '"','\'', '@' };

	for (int a = 0; a < sizeof(c) / sizeof(c[0]); a++) {
		in.erase(remove(in.begin(), in.end(), c[a]), in.end());
	}

	transform(in.begin(), in.end(), in.begin(), ::tolower);
}

void split(string s, char delim, vector<string> &result) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == delim) {
			result.push_back(s.substr(0, i));
			s = s.substr(i + 1);
			i = 0;
		}
	}
	if (s != "") {
		result.push_back(s);
	}
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
		if (ng[i].phrase == "") {
			continue;
		}
		string info = ng[i].phrase + "|";
		map<string, int>::iterator it;
		for (it = ng[i].ng.begin(); it != ng[i].ng.end(); it++)
		{
			if (it->first == "") {
				continue;
			}
			info += it->first + "*" + to_string(it->second) + "|";
		}

		file << info << endl;
	}

	file.close();
}

void readNG(vector<NGram> &ng, string out) {
	ifstream file;
	file.open(out);

	string x;
	while (getline(file, x)) {
		vector<string> all;
		split(x, '|', all);
		NGram n(all[0]);
		for (int i = 1; i < all.size(); i++) {
			vector<string> next;
			split(all[i], '*', next);
			n.add(next[0], stoi(next[1]));
		}
		ng.push_back(n);
	}


	file.close();
}

int main() {
	vector<string> all;
	vector<vector<NGram>> masterNG;
	vector<NGram> two;
	vector<NGram> three;
	vector<NGram> one;
	string root = "trump";
	ifstream file;
	int accuracy = -1;
	do {
		cout << "sherlock holmes, fairy tales, trump, romance, gucci gang, or green eggs and ham" << endl;
		getline(cin, root);
		file = ifstream(root + ".txt");
		cout << "Accuracy desired [1-10]" << endl;
		string l;
		getline(cin, l);
		try {
			accuracy = stoi(l);
		}
		catch (...) {
			cout << "Invalid input" << endl;
		}
	} while (!file.good() && accuracy >= 1 && accuracy <= 10);

	for (int z = 1; z <= accuracy; z++) {
		string fileStr = root + to_string(z) + ".txt";
		ifstream f(fileStr);
		masterNG.push_back(vector<NGram>());
		int i = z - 1;
		if (!f.good()) {
			cout << "Creating " << i << endl;
			if (all.size() == 0) {
				string x;
				while (file >> x) {
					split(x, ' ', all);
				}
				file.close();

				for (int i = 0; i < all.size(); i++) {
					sanitize(all[i]);
				}
			}

			int maxIteration = 20000;
			NGram *n = new NGram("");
			for (int a = 0; a < all.size() && a < maxIteration; a++) {
				if (a + i + 1 >= all.size()) {
					break;
				}

				string search = "";
				for (int b = 0; b < i + 1 && b + a < all.size(); b++) {
					search += all[a+b] + " ";
				}

				n = find(masterNG[i], search);

				n->add(all[a + i + 1]);
			}

			writeNG(masterNG[i], fileStr);
		} else {
			cout << "Loading " << i << endl;
			readNG(masterNG[i], fileStr);
		}
	}

	while (true) {
		string x = "";
		cout << "starting word " << endl;
		cin >> x;
		x += " ";
		vector<string> words;
		vector<NGram*> usedNG;
		for (int i = 0; true; i++) {
			words.push_back(x);
			vector<NGram*> possibleNG;
			possibleNG.push_back(find(masterNG[0], x));

			for (int a = 2; a <= accuracy; a++) {
				if (a < words.size()) {
					string search = "";
					for (int b = i - a + 1; b < words.size(); b++) {
						search += words[b];
					}

					possibleNG.push_back(find(masterNG[a - 1], search));
				}
			}

			vector<string> possibleStr;

			for (int a = 0; a < possibleNG.size(); a++) {
				if (possibleNG[a] != nullptr && possibleNG[a]->ng.size() > 0) {
					possibleStr.push_back(possibleNG[a]->getMax());
				}
				else {
					possibleStr.push_back("");
				}
			}

			for (int a = possibleStr.size()-1; a >= 0; a--) {
				if (possibleStr[a] != "" && possibleStr[a] != " ") {
					x = possibleStr[a];
					break;
				}
			}
			x += " ";
			bool cont = true;
			if (words.size() > accuracy*2) {
				for (int start = 0, end = accuracy-1; end < words.size(); end++, start++) {
					string check = "";
					for (int b = start; b <= end; b++) {
						check += words[b];
					}
					for (int startA = end, endA = end+accuracy-1; endA < words.size(); endA++, startA++) {
						string checkA = "";
						for (int b = startA; b <= endA; b++) {
							checkA += words[b];
						}
						if (checkA == check) {
							int startSize = words.size();
							for (int b = words.size()-1; b >= startSize-accuracy; b--) {
								words.erase(words.begin() + b);
							}
							cont = false;
							break;
						}
					}
				}
			}
			if (!cont) {
				for (int b = 0; b < words.size(); b++) {
					cout << words[b];
				}
				break;
			}
		}
		cout << endl;
		cout << endl;
	}
	return 0;
}

