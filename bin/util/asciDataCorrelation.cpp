#include <iomanip>

#include "oceancv/util/cli_args.h"
#include "oceancv/util/file_parser.h"

using namespace std;

pair<size_t,float> correlation(std::map<long,float>& v1, std::map<long,float>& v2, long offset_v2 = 0) {
	
	float mean_1 = 0;
	float mean_2 = 0;
	size_t num_in_both = 0;
	
	for(auto p : v1) {
		if(v2.find(p.first + offset_v2) != v2.end()) {
			mean_1 += v1[p.first];
			mean_2 += v2[p.first];
			num_in_both++;
		}
	}
	mean_1 /= num_in_both;
	mean_2 /= num_in_both;
	
	float cov = 0;
	float sig_1 = 0;
	float sig_2 = 0;
	
	for(auto p : v1) {
		if(v2.find(p.first + offset_v2) != v2.end()) {
			sig_1 += (v1[p.first] - mean_1) * (v1[p.first] - mean_1);
			sig_2 += (v2[p.first] - mean_2) * (v2[p.first] - mean_2);
			cov += (v1[p.first] - mean_1) * (v2[p.first] - mean_2);
		}
	}
	
	return {num_in_both,1.f * cov / sqrt(sig_1 * sig_2)};
	
}

std::map<long,float> toRanks(const std::map<long,float>& v) {

	std::vector<std::pair<long,float>> data(v.size());
	size_t index = 0;
	for(auto p : v) {
		data[index++] = p;
	}
	
	std::sort(data.begin(), data.end(), [](const std::pair<long,float>& a, const std::pair<long,float>& b) -> bool { return a.second < b.second; });

	std::map<long,float> ret;
	
	float mean_rank, prev_val = data[0].second;
	size_t num_vals = 0;
	std::vector<long> keys = {};
	
	size_t rank = 0;
	
	for(auto p : data) {
		
		if(prev_val != std::get<1>(p)) {
			mean_rank = rank + 0.5 * (num_vals - 1);
			for(long k : keys)
				ret.insert(std::pair<long,float>(k,mean_rank));
			
			rank += num_vals;
			num_vals = 0;
			keys.clear();
		}
		
		num_vals++;
		keys.push_back(std::get<0>(p));
		
		prev_val = std::get<1>(p);
		
	}
	
	// Add remaining items
	mean_rank = rank + 0.5 * (num_vals - 1);
	for(long k : keys)
		ret.insert(std::pair<long,float>(k,mean_rank));
	
	return ret;

}

int main(int argc, char** argv) {

	ocv::cli_args args(argc, argv, {{"i1","Input file one"},{"k1","Key column one"},{"v1","Value column one"},{"k12","Optional additional column to be appended by blank to k1 (handy for date time appending)","-1"},{"d1","Optional Date Time parsing string for file one","-1"},{"i2","Input file two"},{"k2","Key column two"},{"v2","Value column two"},{"k22","Optional additional column to be appended by blank to k2 (handy for date time appending)","-1"},{"d2","Optional Date Time parsing string for file two","-1"},{"max_v1","Maximum value v1 may attain","10000000"},{"min_v1","Minimum value v1 may attain","0"},{"max_v2","Maximum value v2 may attain","10000000"},{"min_v2","Minimum value v2 may attain","0"}});
	
	ocv::forceFileExists(args["i1"]);
	ocv::forceFileExists(args["i2"]);
	
	vector<vector<string>> raw_data_1 = ocv::readASCIFile(args["i1"]);
	vector<vector<string>> raw_data_2 = ocv::readASCIFile(args["i2"]);
	
	vector<std::string> header_1 = ocv::getASCIFileHeader(args["i1"]);
	vector<std::string> header_2 = ocv::getASCIFileHeader(args["i2"]);
	
	size_t max_k1 = max(max(args.i("k1"),args.i("v1")),args.i("k12"));
	size_t max_k2 = max(max(args.i("k2"),args.i("v2")),args.i("k22"));
	
	cout << "Using key " << header_1[args.i("k1")] << " and value " << header_1[args.i("v1")] << " from " << args.s("i1") << endl;
	cout << "Using key " << header_2[args.i("k2")] << " and value " << header_2[args.i("v2")] << " from " << args.s("i2") << endl;
	
	
	
	// Parse numeric data
	long key;
	string key_str;
	vector<string> v;
	map<long,float> data_1;
	float val;
	std::tm t = {};
	
	for(size_t i = 1; i < raw_data_1.size(); i++) {
		v = raw_data_1[i];
		if(max_k1 > v.size() || v[args.i("k1")] == "" || v[args.i("v1")] == "")
			continue;
		key_str = v[args.i("k1")];
		if(args.s("k12") != "-1")
			key_str += " " + v[args.i("k12")];
		if(args.s("d1") != "-1") {
			// Parse date
			std::istringstream ss(key_str);
			ss >> get_time(&t, args.s("d1").c_str());
			key = mktime(&t);
		} else {
			key = stoi(key_str);
		}
		val = stof(v[args.i("v1")]);
		if(val > args.f("max_v1") || val < args.f("min_v1"))
			continue;
		data_1.insert(pair<long,float>(key,val));
	}
	
	map<long,float> data_2;
	for(size_t i = 1; i < raw_data_2.size(); i++) {
		v = raw_data_2[i];
		if(max_k2 > v.size() || v[args.i("k2")] == "" || v[args.i("v2")] == "")
			continue;
		key_str = v[args.i("k2")];
		if(args.s("k22") != "-1")
			key_str += " " + v[args.i("k22")];
		if(args.s("d2") != "-1") {
			// Parse date
			std::istringstream ss(key_str);
			ss >> get_time(&t, args.s("d2").c_str());
			key = mktime(&t);
		} else {
			key = stoi(key_str);
		}
		val = stof(v[args.i("v2")]);
		if(val > args.f("max_v2") || val < args.f("min_v2"))
			continue;
		data_2.insert(pair<long,float>(key,val));
	}
	
	// Compute the correlation between the files!
	auto corr = correlation(data_1,data_2);
	cout << "Pearson: " << corr.first << " " << corr.second << endl;
	
	// Compute the rank-correlation between the files!
	auto ranks_1 = toRanks(data_1);
	auto ranks_2 = toRanks(data_2);
	corr = correlation(ranks_1,ranks_2);
	cout << "Spearman: " << corr.first << " " << corr.second << endl;
	
}
