#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include <unordered_set>
#include <cmath>
#include <algorithm>
struct Tree{
	int el;
std::vector<Tree> children;

int level = 0;
bool isNumber(char c) { return (c >= '0' && c <= '9'); };
int extractInt(const char*& str) {
	std::string ns;
	while (isNumber(*str)) {
		ns += *str;
		str++;
	}
	return atoi(ns.c_str());
}
public:

	// Дърво се представя чрез (елемент {дете1, дете2, дете3, ...}). Дърво без деца е просто елемента, без допълнителни скоби.
	void constructTree(const char* & str) {
		if (isNumber(*str)) {
			el = extractInt(str);
		}
		str += 2;
		while (*str && *str != ')') {
			if (isNumber(*str))
				children.push_back({ extractInt(str),{} });
			if (*str == '(') {
				str++;
				children.push_back({});
				children[children.size() - 1].constructTree(str);

			}
			str++;
		}
	}

	void readTree(const std::string& filePath) {
		std::ifstream file(filePath);
		std::string treeData(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
		 const char* str =  treeData.c_str();
		
		constructTree(++str);
	}
void getElements(std::unordered_set<int>& s) {
	s.insert(el);
	for (auto t : children) {
		t.getElements(s);
	}
}

bool isOdd() {
	std::unordered_set<int> els;
	getElements(els);
	std::vector<int> oddNumbers(els.size());
	for (int i = 1; i < 50; i += 2) {
		oddNumbers.push_back(i);
	}
	for (int i = 0; i < els.size(); i++) {
		if (els.find(oddNumbers[i]) == els.end())
			return false;
	}
	return true;

}
bool populate_odd() {
	std::unordered_set<int> els;
	getElements(els);
	std::vector<int> oddNumbers(els.size());
	int curr = 1;
	for (int i = 0; i < els.size(); i ++) {
		oddNumbers[i] = curr;
		curr += 2;
	}
	do {
		populate(oddNumbers, 0);
		if (isGracious())
			return true;
	} while (std::next_permutation(oddNumbers.begin(), oddNumbers.end()));

	return false;
		
	

}
void populate(std::vector<int> elements, int i) {
	if(i<elements.size())
	el = elements[i];
	if (children.size() > 0) {
		for (auto& child : children) {
			if (child.children.size() > 0) {
				child.populate(elements, ++i);
			}
			else {
				if (i + 1 < elements.size())
					child.el = elements[++i];

			}
		}
	}
}

bool isGraciousHelper (std::unordered_set<int> diffs){
	for (auto t : children) {
		int n = std::abs(t.el - el);
		if (diffs.find(n) != diffs.end()) {
			return false;
		}
		else {
			diffs.insert(n);

			 if (!t.isGraciousHelper(diffs))
				 return false;
		}
	}
	
	return true;
}


bool isGracious() {
	

		std::unordered_set<int> diffs; 
		return isGraciousHelper(diffs);
}

std::string to_str(){
	std::string str;
	str += "(";
	str += std::to_string(el);
	str += " {";
		for (auto& child : children) {
			if (child.children.size() > 0)
				str += child.to_str();
			else
				str += std::to_string(child.el);
			str += " ";
	}
		str += "}";
		str += ")";

		return str;

}
void save_to_file(std::string filePath) {
	std::ofstream f(filePath);
	std::string str = to_str();
	f << str;
}
};




int main() {
	Tree gracious;
	// Четем дърво с четни елементи.
	gracious.readTree("tree.txt");

	bool f = gracious.populate_odd();

	if (f) {
		gracious.save_to_file("gracious.txt");
		std::cout << "Found Gracious!" << std::endl;
	}
	else {
		std::cout << "Not possible to be made gracious.";
	}



	return 0;
}