#include<iostream>
#include<fstream>
#include<map>
#include<set>
#include<string>
#include<regex>
#include<utility> 
#include<algorithm>
#include<conio.h>
#define input(s,p) system("cls");string p;cout << s;cin >> p;
using namespace std;
class graph {
private:
	void read_f() {
		string line;
		bool start_in_file = false;
		bool end_in_file = false;
		bool beggining = true;
		while (getline(this->in, line)) {
			smatch list;
			if (beggining) {
				beggining = false;
				if (regex_search(line, list, regex("digraph (\\w+)"))) {
					this->name = list[1].str();
				}
			}
			else {
				if (regex_search(line, list, regex("([0-9]+) *-> *([0-9]+).*\"(\\w+)\""))) {
					int n[] = { 0, 0 }; string a;
					for (int k = 0; k < 2; k++) {
						for (int i = 0; i < list[k + 1].str().length(); i++) {
							n[k] = n[k] * 10 + list[k + 1].str()[i] - '0';
						}
					}
					this->insert(n[0], n[1], list[3].str());
				}
				else if (regex_search(line, list, regex("\" \" *-> *([0-9]+)"))) {
					start_in_file = true;
					int start_node = 0;
					for (int i = 0; i < list[1].str().length(); i++) {
						start_node = start_node * 10 + list[1].str()[i] - '0';
					}
					this->start.insert(start_node);
				}
				else if (regex_search(line, list, regex("([0-9]+) *-> *\" \""))) {
					end_in_file = true;
					int end_node = 0;
					for (int i = 0; i < list[1].str().length(); i++) {
						end_node = end_node * 10 + list[1].str()[i] - '0';
					}
					this->end[end_node] = true;
				}
			}
		}
		if (!start_in_file) {
			cout << "PLease enter the number of start nodes :\n";
			int n; cin >> n;
			cout << "PLease enter the start nodes : ";
			while (n--) {
				int t; cin >> t;
				this->start.insert(t);
			}
		}
		if (!end_in_file) {
			cout << "PLease enter the number of end nodes :\n";
			int n; cin >> n;
			cout << "PLease enter the end nodes : ";
			while (n--) {
				int t; cin >> t;
				this->end[t] = true;
			}
		}
		this->in.close();
	}
	void traverse(int n) {
		if (!this->Processed[n]) {
			this->Processed[n] = true;
			for (map<string, set<int>>::iterator i = this->node[n].begin(); i != this->node[n].end(); i++) {
				if (i->first != "0") {
					for (auto m : i->second) {
						traverse(m);
						this->out << n << "->" << m << " [label=\"" << i->first << "\"];" << endl;
					}
				}
			}
		}
	}
	set<int> extract_closure(int i, map<int, set<int>>& closures) {
		if (closures.find(i) == closures.end() && this->node.find(i) != this->node.end()) {
			closures[i].insert(i);
			for (auto j : this->node[i]["0"]) {
				closures[i].insert(j);
				for (auto k : extract_closure(j, closures)) {
					if (this->end[k]) {
						this->end[i] = true;
					}
					closures[i].insert(k);
				}
			}
		}
		return closures[i];
	}
	void phase_one() {
		map<int, set<int>> closures;
		for (map<int, map<string, set<int>>>::iterator i = this->node.begin(); i != this->node.end(); i++) {
			extract_closure(i->first, closures);
		}
		for (map<int, set<int>>::iterator i = closures.begin(); i != closures.end(); i++) {
			for (auto j : i->second) {
				for (map<string, set<int>>::iterator k = this->node[j].begin(); k != this->node[j].end(); k++) {
					for (auto m : k->second) {
						this->node[i->first][k->first].insert(m);
					}
				}
			}
		}
	}
	void determine(vector<vector<set<int>>>& arr, vector<set<int>>& memo, set<int> s) {
		if (find(memo.begin(), memo.end(), s) == memo.end()) {
			vector<set<int>> v_temp;
			memo.push_back(s);
			for (int l = 0; l < this->language.size(); l++) {
				set<int> s_temp;
				for (auto i : s) {
					for (auto j : this->node[i][this->language[l]]) {
						s_temp.insert(j);
					}
				}
				v_temp.push_back(s_temp);
			}
			arr.push_back(v_temp);
		}
	}
	void phase_two() {
		vector<vector<set<int>>> arr;
		vector<set<int>> memo;
		determine(arr, memo, this->start);
		int j = 0;
		for (int i = 0; i < this->language.size(); i++) {
			if (!arr[j][i].empty())
				determine(arr, memo, arr[j][i]);
			if (i == this->language.size() - 1 && j < arr.size() - 1) {
				i = -1; j++;
			}
		}
		graph r = graph(arr, this->language, memo, this->end);
		this->node = r.node;
		this->Processed = r.Processed;
		this->end = r.end;
	}
	void phase_four() {
		map<int, map<int, string>> result;
		for (map<int, map<string, set<int>>>::iterator i = this->node.begin(); i != this->node.end(); i++) {
			for (map<string, set<int>>::iterator j = i->second.begin(); j != i->second.end(); j++) {
				for (auto k : j->second) {
					result[i->first][k] += result[i->first][k] == "" ? j->first : ", " + j->first;
				}
			}
		}
		graph r = graph(result);
		this->node = r.node;
		this->Processed = r.Processed;
	}
	vector<set<int>> minimize(vector<set<int>> classes,vector<vector<int>>& r_signature) {
		vector<set<int>> new_classes;
		vector<vector<int>> signatures;
		for (int i = 0; i < classes.size(); i++) {
			map<vector<int>, set<int>> class_signature;
			for (auto j : classes[i]) {
				vector<int> signature(this->language.size(),-1);
				for (int k = 0; k < this->language.size(); k++) {
					for (int s = 0; s < classes.size(); s++) {
						int n;
						for (auto t : this->node[j][this->language[k]]) n = t;
						if (classes[s].find(n)!=classes[s].end()) {
							signature[k] = s;
						}
					}
				}
				class_signature[signature].insert(j);
			}
			for (map<vector<int>, set<int>>::iterator j = class_signature.begin(); j != class_signature.end(); j++) {
				new_classes.push_back(j->second);
				signatures.push_back(j->first);
			}
		}
		r_signature = signatures;
		return new_classes;
	}
	void phase_three() {
		vector<set<int>> classes(2);
		vector<vector<int>> signatures;
		for (map<int, map<string, set<int>>>::iterator i = this->node.begin(); i != this->node.end(); i++) {
			if (!this->end[i->first]) classes[0].insert(i->first);
		}
		for (map<int, bool>::iterator i = this->end.begin(); i != this->end.end(); i++) {
			if (i->second) classes[1].insert(i->first);
		}
		while (classes != minimize(classes,signatures)) classes=minimize(classes, signatures);
		graph r = graph(classes, signatures,this->language);
		this->node = r.node;
		this->Processed = r.Processed;
	}
	ofstream out;
	ifstream in;
public:
	graph(string f_loc) {
		this->in.open(f_loc);
		this->path = f_loc;
		this->read_f();
	}
	graph(map<int, map<int, string>> map_in) {
		for (map<int, map<int, string>>::iterator i = map_in.begin(); i != map_in.end(); i++) {
			for (map<int, string>::iterator j = i->second.begin(); j != i->second.end(); j++) {
				this->insert(i->first, j->first, j->second);
			}
		}
	}
	graph(vector<vector<set<int>>> arr, vector<string> alph, vector<set<int>> s, map<int, bool> end) {
		for (int i = 0; i < arr.size(); i++) {
			for (int j = 0; j < arr[0].size(); j++) {
				for (int f = 0; f < s.size(); f++) {
					if (s[f] == arr[i][j]) {
						this->insert(i, f, alph[j]);
						for (auto k : s[f]) {
							if (end[k]) this->end[f] = true;
						}
						for (auto k : s[i]) {
							if (end[k]) this->end[i] = true;
						}
						break;
					}
				}
			}
		}
	}
	graph(vector<set<int>> classes, vector<vector<int>> signatures,vector<string> lang) {
		for (int i = 0; i < classes.size(); i++) {
			for(int j=0;j<lang.size();j++){
				if(signatures[i][j]!=-1)
					this->insert(i,signatures[i][j],lang[j]);
			}
		}
	}
	void insert(int s, int f, string a) {
		this->node[s][a].insert(f);
		this->Processed[s] = false;
		if (a != "0" && find(this->language.begin(), this->language.end(), a) == this->language.end()) this->language.push_back(a);
	}
	void optimize() {
		this->phase_one();
		this->phase_two();
		this->phase_three();
	}
	void write() {
		this->path = regex_replace(this->path, regex("\\w+\\.dot"), "output.dot");
		this->out.open(this->path);
		out << "digraph " << this->name << " {" << endl;
		this->traverse(0);
		out << "}";
		this->out.close();
		system(this->path.c_str());
	}
	void render() {
		string command = "dot -Tpng file1 -o file2";
		command = regex_replace(command, regex("file1"), this->path);
		command = regex_replace(command, regex("file2"), regex_replace(this->path, regex("\\.dot"), ".png"));
		system(command.c_str());
	}
	void process() {
		this->optimize();
		this->write();
		this->render();
	}
	map<int, map<string, set<int>>> node;
	map<int, bool> Processed;
	vector<string> language;
	set<int> start;
	map<int, bool> end;
	string name;
	string path;
};
int main() {
	input("please enter the path of the file/name of file : ", path);
	graph g = graph(path);
	g.process();
}