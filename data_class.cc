#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cassert>

using namespace std;
enum Text_type {
	STRING_DATA=0,
	DIGIT_DATA,
	MIXED_DATA	
};
// class 1

template<class T>
class Stats {
	public:
	T zero_patterns;
	T lower_patterns;
	T rep_patterns;
	T similar_patterns;
	T no_patterns;
	void print_stats() {
		cout << "Zero Patterns : " << zero_patterns << endl;
		cout << "Lower Patterns : " << lower_patterns << endl;
		cout << "Repetition Patterns : " << rep_patterns << endl;
		cout << "Similar Patterns : " << similar_patterns << endl;
		cout << "No Patterns : " << no_patterns << endl;
	}
};

// class 2

template<class T>
class Data_Categ {
	public:
	void process_data(vector<char> data,Stats<double>& stats_ob);
	void process_data(vector<string> data,Stats<double>& stats_ob);
	private:
	bool check_all_zero(string data); 
	bool check_sim_pattern(string data);
	bool check_two(string a,string b);
	bool check_rep_patterns(string data);
	bool check_lower_words(string data);
	void fill_data_values(string data,Stats<double>& stats_ob);
};
template<class T>
void Data_Categ<T>::process_data(vector<char> data,Stats<double>& stats_ob) {
	string cur_data(data.begin(),data.end());
	fill_data_values(cur_data,stats_ob);	
}
template<class T>
void Data_Categ<T>::fill_data_values(string data,Stats<double>& stats_ob) {
	if(check_all_zero(data)) {
		stats_ob.zero_patterns++;
	}else if(check_rep_patterns(data)) {
		stats_ob.rep_patterns++;
	}else if(check_sim_pattern(data)) {
		stats_ob.similar_patterns++;
	}else if(check_lower_words(data)) {
		stats_ob.lower_patterns++;
	}else {
		stats_ob.no_patterns++;
	}
}
template<class T>
void Data_Categ<T>::process_data(vector<string> data,Stats<double>& stats_ob) {
	for(int i=0;i<data.size();i++) {
		string cur_data = data[i];
		fill_data_values(cur_data,stats_ob);
	}
}
template<class T>
bool Data_Categ<T>::check_all_zero(string data) {
	string cmp(data.size(),'0');
	if(data.compare(cmp)==0) return true;
	return false;
}
template<class T>
bool Data_Categ<T>::check_rep_patterns(string data) {
	unordered_map<string,int> track;
	for(int i=0;i<data.size();i+=8) {
		string cur  = data.substr(i,8);
		track[cur]++;
	}
	int limit = data.size()/16;
	for(pair<string,int> p : track) {
		if(p.second>=limit) 
		{
			return true;
		}
	}
	return false;
}
template<class T>
bool Data_Categ<T>::check_lower_words(string data) {
	string tmp(2,'0');
	int ct =0;
	for(int i=0;i<data.size();i+=8) {
		string cur = data.substr(i,8);
		int it = cur.find(tmp);
		if(it!=string::npos && it+2!=8) {
			ct++;
		}
	}
	if(ct>=(data.size()/16)) return true;
	return false;
}
template<class T>
bool Data_Categ<T>::check_two(string a,string b) {
	int ct=0;
	for(int i=0;i<a.size();i++) {
		if(a[i]==b[i] && a[i]!='0') ct++;
	}
	if(ct>=2) return true;
	return false;	
}
template<class T>
bool Data_Categ<T>::check_sim_pattern(string data) {
	vector<string> res;
	for(int i=0;i<data.size();i+=8) {
		res.push_back(data.substr(i,8));
	}
	int ct =0;
	for(int i=1;i<res.size();i++) {
		if(check_two(res[i],res[i-1])) ct++;
	}
	int limit = res.size()-1;
	if(ct>=limit/2) return true;
	return false;	
}

// class 3
template<class T>
class TextIden {
	public:
	double threshold;
	unordered_set<char> char_track;
	unique_ptr<Data_Categ<T>> datacat_ob;
	TextIden(double threshold,vector<string>& valid_char,string special_char);
	~TextIden() {}
	enum Text_type classify_text(vector<T>& cache_block,Stats<double>& stats_ob);
	private:
	void no_text_type(char segment,int& text_count,int& dig_count,int& total_count) {
		if(char_track.count(segment)>0) text_count++;
		else dig_count++;
		total_count+=1;
	}
	void no_text_type(string segment,int& text_count,int& dig_count,int& total_count) {
		for(int i=0;i<segment.size();i++) {
			if(char_track.count(segment)>0) text_count++;
			else dig_count++;
			total_count++;
		}
	}
};
template<class T>
TextIden<T>::TextIden(double threshold,vector<string>& valid_char,string special_char) {
	this->threshold = threshold;
	for(int i=0;i<valid_char.size();i++) {
		string cur  = valid_char[i];
		auto index = cur.find('-');
		if(index!=string::npos) {
			// range specified // eliminate char -
			if(index==0) continue; // invalid range -> no start specified
			assert(cur.size()==3); // invalid range -> a-z, not consider case like 0-67// invalid
			for(char c=cur[index-1];c<=cur[index+1];c++) {
				char_track.insert(c);
			}
			continue;
		} 
		// single number
		for(int i=0;i<cur.size();i++) char_track.insert(cur[i]);
	}
	// special char eg -
	for(int i=0;i<special_char.size();i++) char_track.insert(special_char[i]);
}
template<class T>
enum Text_type TextIden<T>::classify_text(vector<T>& cache_block,Stats<double>& stats_ob) {
	int text_count=0,dig_count=0,total_count=0;
	for(int i=0;i<cache_block.size();i++) {
		no_text_type(cache_block[i],text_count,dig_count,total_count);
	}
	// category data
	double per_text = (double)text_count/(double)total_count;
	double per_dig = (double)dig_count/(double)total_count;
	cout << per_text << "\t" << per_dig <<  endl;
	if(per_text > threshold) {
		return STRING_DATA;
	}
	if(per_dig > threshold) {
		// look for patterns
		datacat_ob->process_data(cache_block,stats_ob);
		stats_ob.print_stats();
		return DIGIT_DATA;
	}
 	return MIXED_DATA;
}

/* Test code */

int main() {
	char arr[] = {'M','N','A','3','B','S','8'};
	vector<char> test_str(arr,arr+sizeof(arr)/sizeof(char));
	string str_arr[] = {"a-z","A-Z","!%"};
	vector<string> stringlist(str_arr,str_arr+3);
	string special_char = "-";// as it's ambi in org list
	TextIden<char> obtest(0.70,stringlist,special_char);
	Stats<double> stats_ob;
	if(obtest.classify_text(test_str,stats_ob)==STRING_DATA) {
		cout << "String data " << endl;
	} else {
		cout << "Digits " << endl;
	}	
	return 0;
}