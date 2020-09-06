#include<iostream>
#include<fstream>
#include<map>
#include<set>
#include<string>
#include<vector>
#include<regex>
#include<utility> 
#include<algorithm>
#include<cstdio>
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
				else if (regex_search(line, list, regex("s|S *-> *([0-9]+)"))) {
					start_in_file = true;
					int start_node = 0;
					for (int i = 0; i < list[1].str().length(); i++) {
						start_node = start_node * 10 + list[1].str()[i] - '0';
					}
					this->start.insert(start_node);
				}
				else if (regex_search(line, list, regex("([0-9]+) *-> *f|F"))) {
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
			cout << "PLease enter the number of start nodes :";
			int n; cin >> n;
			cout << "PLease enter the start nodes : ";
			while (n--) {
				int t; cin >> t;
				this->start.insert(t);
			}
		}
		if (!end_in_file) {
			cout << "PLease enter the number of end nodes :";
			int n; cin >> n;
			cout << "PLease enter the end nodes : ";
			while (n--) {
				int t; cin >> t;
				this->end[t] = true;
			}
		}
		this->in.close();
	}
	void traverse(ostream& off,int n) {
		if (!this->Processed[n]) {
			this->Processed[n] = true;
			for (map<string, set<int>>::iterator i = this->node[n].begin(); i != this->node[n].end(); i++) {
				if (i->first != "0") {
					for (auto m : i->second) {
						traverse(off,m);
						if (this->end[n]) {
							off << n << " [shape = doublecircle];" << endl;
							//this->end[n] = false;//to be removed
						}
						if (this->end[m]) {
							off << m << " [shape = doublecircle];" << endl;
							//this->end[m] = false;//to be removed
						}
						off << n << "->" << m << " [label=\"" << i->first << "\"];" << endl;
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
		this->start = r.start;
		this->Processed = r.Processed;
		this->end = r.end;
	}
	void phase_four() {
		map<int, map<int, string>> result;
		for (map<int, map<string, set<int>>>::iterator i = this->node.begin(); i != this->node.end(); i++) {
			for (map<string, set<int>>::iterator j = i->second.begin(); j != i->second.end(); j++) {
				for (auto k : j->second) {
					result[i->first][k] += result[i->first][k] == "" ? j->first : "," + j->first;
				}
			}
		}
		graph r = graph(result);
		this->node = r.node;
		this->Processed = r.Processed;
	}
	vector<set<int>> minimize(vector<set<int>> classes, vector<vector<int>>& r_signature) {
		vector<set<int>> new_classes;
		vector<vector<int>> signatures;
		for (int i = 0; i < classes.size(); i++) {
			map<vector<int>, set<int>> class_signature;
			for (auto j : classes[i]) {
				vector<int> signature(this->language.size(), -1);
				for (int k = 0; k < this->language.size(); k++) {
					for (int s = 0; s < classes.size(); s++) {
						for (auto node : classes[s]) {
							for (auto nodes : this->node[j][this->language[k]]) {
								if (node == nodes) signature[k] = s;
							}
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
		while (classes != minimize(classes, signatures))classes = minimize(classes, signatures);
		graph r = graph(classes, signatures, this->language, this->start);
		this->node = r.node;
		this->Processed = r.Processed;
		this->start = r.start;
	}
	bool traverseCheck(graph* g, int state, string& key, int it) {
		if (it == key.size()) return false;
		string s = key.substr(it, 1);
		for (map<string, set<int>>::iterator i = g->node[state].begin(); i != g->node[state].end(); i++) {
			if (i->first == s) {
				if (it == key.size() - 1) {
					if (g->end[state])
						return true;
					for (auto states : i->second) {
						if (g->end[states]) return true;
					}
					return false;

				}
				else {
					bool valid = false;
					for (int newState : i->second) {
						valid |= traverseCheck(g, newState, key, ++it);
						if (valid) return valid;
					}
					return valid;
				}
			}
		}
		return false;
	}
	void regexprExtratction(int state, string& s,map<int,bool>& p) {
		if(p[state]){
			s+=")*"; return;
		} 
		p[state]=true;
		s+="(";
		int lCount=this->node[state].size();
		for(auto i=this->node[state].begin();i!=this->node[state].end();i++){
			if(this->node[state].size()>1)s+="(";
			int count =i->second.size(); 
			for(auto j:i->second){
				if(j==state){
					s+=i->first+"*)";
				}else{
					s+=i->first;
					regexprExtratction(j,s,p);
					s+=")";
				}
				if(--count) s+="|";
			}
			if(--lCount) s+="|";
		}
	}
	void extractREGEX() {
		this->Gregex="";
		cout << this->Gregex << endl;
		bool beg=true;
		map<int,bool> processed;
		for (auto i:this->start) {
			this->Gregex += beg?"(^":"|(^";beg=false;
			regexprExtratction(i, this->Gregex,processed);
		}
	}
	ofstream out;
	ifstream in;
public:

	graph(string f_loc) {
		this->in.open(f_loc);
		this->path = f_loc;
		this->read_f();
		this->isMinDfa = false;
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
		this->start.insert(0);
	}
	graph(vector<set<int>> classes, vector<vector<int>> signatures, vector<string> lang, set<int> old_s) {
		for (int i = 0; i < classes.size(); i++) {
			for (int j = 0; j < lang.size(); j++) {
				if (signatures[i][j] != -1) {
					for (auto k : classes[i]) {
						if (old_s.find(k) != old_s.end()) this->start.insert(i);
					}
					this->insert(i, signatures[i][j], lang[j]);
				}

			}
		}
	}
	graph(graph* source) {
		this->end = source->end;
		this->start = source->start;
		this->language = source->language;
		this->Processed = source->Processed;
		this->node = source->node;
	}
	void insert(int s, int f, string a) {
		this->node[s][a].insert(f);
		this->Processed[s] = false;
		if (a != "0" && find(this->language.begin(), this->language.end(), a) == this->language.end()) this->language.push_back(a);
	}
	void optimize(int mode) {
		if (mode >= 1) this->phase_one();
		if (mode >= 2) this->phase_two();
		if (mode >= 3) {
			this->phase_three();
			this->isMinDfa = true;
		}
		//this->phase_four();
	}
	bool check(string s) {
		bool valid = false;
		for (auto state : this->start) {
			valid |= this->traverseCheck(this, state, s, 0);
		}
		return valid;
	}
	void write(ostream& off) {
		off << "digraph " << this->name << " {" << endl;
		off << "\"\" [shape=none,width=0,height=0];" << endl;
		off << "\" \" [shape=none,width=0,height=0];" << endl;
		for (auto i : this->start) {
			off << "\"\"->" << i << ";" << endl;
			this->traverse(off,i);
		}
		off << "}";
	}
	void PrintToFile(){
		string out_path = regex_replace(this->path, regex("\\w+\\.dot"), "output.dot");
		this->out.open(out_path);
		this->write(this->out);
		this->out.close();
		system(out_path.c_str());
	}
	void render(bool mode = 1) {
		string command = "dot -Tpng file1 -o file2";
		if (mode) {
			string out_path = regex_replace(this->path, regex("\\w+\\.dot"), "output.dot");
			command = regex_replace(command, regex("file1"), out_path);
			command = regex_replace(command, regex("file2"), regex_replace(out_path, regex("\\.dot"), ".png"));
		}
		else {
			command = regex_replace(command, regex("file1"), this->path);
			command = regex_replace(command, regex("file2"), regex_replace(this->path, regex("\\.dot"), ".png"));
		}
		system(command.c_str());
	}
	void process(int mode=3) {
		this->render(0);
		this->optimize(mode);
		//this->write();
		//this->render();
	}
	void Search() {
		cout << "please enter ths string to check :";
		string s; cin >> s;
		bool b = this->check(s);
		cout << "The word " << s << (b?" does ":" doesn't ") <<"belong to the language described by " << this->name << endl;;
	}
	graph operator+(graph &rhs){ // we use the + operation as a concatination function cause why not lol
		graph res = graph(this); // we create a copy of the current graph using the copy constructor
		int offset = this->node.size(); // we create an offset so the numbers of of the nodes in the second graph
		// don't overlapp with those in the first , if the first graph has 5 nodes the scond graph's node will start at 5 not 0 
		
		//to concat 2 automatas, for each end node in the first graph we add an epsilon edge between it and all the starting nodes
		// of the second graph, then we add all the node&relations in the second to the first with the offset set ofcourse

		for(auto i=res.end.begin();i!=res.end.end();i++){//we loop through all the nodes in the first graph
			if(i->second){ // we check if it's an end node if it is 
				for(auto start:rhs.start) // we loop through all the starting nodes of the second graph
				{
					res.insert(i->first,start+offset,"0");// we insert an empty relation between the end nodes of the first graph and start nodes of the second graph 
					// we did start+offset to avoid overlapps between the starts of the first with the second's
				}
				for(auto j=rhs.node.begin();j!=rhs.node.end();j++){ // we loop through all the nodes in the second graph
					for(auto c=j->second.begin();c!=j->second.end();c++){// for each node we loop through edges coming out of it (edge is the realtion or arrow we saw in graph theory)
						for(auto n:c->second){// for each edge we get the nodes reachable by it
							res.insert(j->first+offset,n+offset,c->first);// and we insert them in the result graph with the offset added
						}
					}
					if(rhs.end[j->first]) res.end[j->first+offset]=true; // we mark the end nodes in the new graph 
					// every end node in the second graph will be an end node in the result graph too
				}
			}
			res.end[i->first]=false; // end nodes of the first are no longer end node so we unmark them
		}
		return res;// we return the result graph
	}
	string regExpr(){
		this->extractREGEX();
		return this->Gregex;
	}
map<int, map<string, set<int>>> node;
map<int, bool> Processed;
vector<string> language;
set<int> start;
map<int, bool> end;
string name;
string path;
string Gregex;
bool isMinDfa;
};
ostream& operator<<(ostream& dest,graph& g){
	g.write(dest);
	for(auto i=g.Processed.begin();i!=g.Processed.end();i++) i->second=false; // remaks all the nodes as not proccessed so the optimizing algorithm can process them again
	return dest;
}
int check_v(string path) {
	bool valid = false;
	smatch match;
	if(regex_search(path,match,regex("\\.(dot)|(gv)|(txt)"))) valid=true;
	if (valid) {
		system("cls"); return valid;
	}
	printf("WARNING : %s is not a VALID gv/txt File!\n", path);
	return 0;
}
bool ask(string c) {
	int b;
	printf("are you sure you want to open this file : %s ?\n", c);
	printf("	   print 0 for NO and 1 for yes.\n");
	cin >> b;
	return b && check_v(c);
}
bool confirm(bool& b) {
	if (b) { system("cls"); b = !b; return !b; }
	printf("would you like to process another GraphViz file ?\n1->\"YES\"  0->\"NO\" : ");
	int ans; cin >> ans;
	return ans == 1 ? 1 : (ans == 0 ? 0 : confirm(b));
}
int printMENU(){
	system("cls");
	cout <<  "###################[MAIN MENU]######################";
	cout << "\n|| ~~~~~~> {0}       getRegexExpt     {0} <~~~~~~~~ ||";
    cout << "\n|| ~~~~~~> {1}      Optimize Graph    {1} <~~~~~~~~ ||";
    cout << "\n|| ~~~~~~> {2}      Match Sentence    {2} <~~~~~~~~ ||";
    cout << "\n|| ~~~~~~> {3} Write Graph in console {3} <~~~~~~~~ ||";
    cout << "\n|| ~~~~~~> {4}   Write Graph in File  {4} <~~~~~~~~ ||";
    cout << "\n|| ~~~~~~> {5}           exit         {5} <~~~~~~~~ ||";
    cout << "\n######################################################";
	cout << "\nplease enter your choice :";int ans;cin >> ans;
	return ans;
}
int OptMenu(){
	system("cls");
	cout << "###################[ MENU]#####################";
	cout << "\n|| ~~~~~~> {1}   eNFA 2 NFA   {1} <~~~~~~~~ ||";
	cout << "\n|| ~~~~~~> {2}    NFA 2 DFA   {2} <~~~~~~~~ ||";
	cout << "\n|| ~~~~~~> {3}   DFA 2 minDFA {3} <~~~~~~~~ ||";
	cout << "\n###############################################";
	cout << "\n||please enter your choice :";int ans;cin >> ans;
	return ans;
}
int main() {
	bool start = true;
	while (confirm(start)) {
		input("please enter the path of the file/name of file : ", path, 50);
		graph g = graph(path);
		int exit=0;
		while(!exit){
			int ans = printMENU();
			switch(ans){
				case 0 :{
					cout << g.regExpr() << endl;
					break;
				}
				case 1 : {
					g.process(OptMenu());
					break;
				}
				case 2 : {
					g.Search();
					break;
				}
				case 3 : {
					cout << g;
					break;
				}
				case 4 : {
					g.PrintToFile();
					g.render();
				}
				case 5 : {
					exit=true;
				}
			}
			system("pause");
		}
	}
}