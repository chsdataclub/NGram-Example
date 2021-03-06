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
		cin >> x; //gets input as the starting word
		x += " "; //adds space for querying ngrams
		vector<string> words; //holds of the words as seperate strings
		vector<NGram*> usedNG; //the ngrams already used
		for (int i = 0; true; i++) {
			words.push_back(x); //adds words to the word list
			vector<NGram*> possibleNG; //list of ngrams that are possible to choose from
			possibleNG.push_back(find(masterNG[0], x)); //starts out by finding unigram

			//populates the ngram possibilities
			for (int a = 2; a <= accuracy; a++) {
				if (a < words.size()) {
					string search = ""; //the desired phrase for the ngram
					for (int b = i - a + 1; b < words.size(); b++) { //creates the string from the last few words
						search += words[b];
					}

					possibleNG.push_back(find(masterNG[a - 1], search)); //adds the result
				}
			}

			vector<string> possibleStr; //list of the string possibilities

			//populates the string possibility
			for (int a = 0; a < possibleNG.size(); a++) {
				if (possibleNG[a] != nullptr && possibleNG[a]->ng.size() > 0) { //checks to see if the ngram is an actual value 
					possibleStr.push_back(possibleNG[a]->getMax()); //gets the most frequently occuring string that follows that ngram
				}
				else {
					possibleStr.push_back(""); //if there is nothing then add a default value to keep position
				}
			}

			NGram* currentNg = nullptr; //ngram to be used
			
			/*
			finds the current ngram and string
			it stops when it finds the highest order ngram (hexagram is higher than unigram) which is why it iterates in reverse
			*/
			for (int a = possibleStr.size()-1; a >= 0; a--) {
				if (possibleStr[a] != "" && possibleStr[a] != " ") {
					currentNg = possibleNG[a];
					x = possibleStr[a];
					break;
				}
			}

			x += " "; //adds extra space for next word

			bool cont = true; //if cont is true than the program continues running else it ends

			//checks to prevent the repeat
			for (int a = 0; a < usedNG.size(); a++) {
				if (usedNG[a] == currentNg) {
					cont = false;
					break;
				}
			}

			cout << x; //prints the result

			if (!cont) {
				break;
			}

			usedNG.push_back(currentNg); //adds to used ngram to prevent repeats
		}
		cout << endl;
		cout << endl;
	}
	return 0;
}

