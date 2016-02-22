#include <iostream>
#include <vector>
#include <stdexcept>
#include <utility>
#include <cmath>
using std::vector;
using std::cout;
using std::endl;
using std::out_of_range;
using std::exception;
using std::make_pair;
using std::pair;
using std::abs;
//This union-find class implements two optimization ideas:
//1)path compression
//2)union by rank
//https://en.wikipedia.org/wiki/Disjoint-set_data_structure

class DisjointSet {
    public:
	DisjointSet(int num_nodes);
	int find(int element);
	void do_union(int elment_a, int element_b);
	void dup(vector<int> &v);
    private:
	vector<int> parent_, rank_;
};

DisjointSet::DisjointSet(int num_nodes):
    parent_(num_nodes),rank_(num_nodes, 0)
{
    for(int i=0; i<num_nodes; ++i) parent_[i] = i;
}

int DisjointSet::find(int element)
{
    if(parent_[element] == element) return element;
    //1)path compression
    return parent_[element] = find(parent_[element]);
}

void DisjointSet::do_union(int a, int b)
{
    if(parent_[a] == parent_[b]) return;
    int fa = find(a), fb = find(b);
    //2)union by rank
    if(rank_[fa] < rank_[fb]) {
	parent_[fa] = fb;
    } else {
	parent_[fb] = fa;
	if(rank_[fa] == rank_[fb]) rank_[fa]++;
    }
}

void DisjointSet::dup(vector<int> &v)
{
    if(v.size() < parent_.size()) throw out_of_range("DisjointSet: dup destination size too small");
    copy(begin(parent_), end(parent_), begin(v));
}

class IndexConv {
    public:
	IndexConv(int rows, int cols);
	bool check_i(int i) const;
	bool check_xy(int x, int y) const;
	pair<int, int> to_xy(int i) const;
	int to_i(int x, int y) const;
    private:
	const int rows_;
	const int cols_;
};

IndexConv::IndexConv(int rows, int cols)
    :rows_(rows), cols_(cols) {}

bool IndexConv::check_i(int i) const
{
    return i>=0 || i<rows_*cols_;
}

bool IndexConv::check_xy(int x, int y) const
{
    return x>=0 && y>=0 && x<rows_ && y<cols_;
}
pair<int, int> IndexConv::to_xy(int i) const
{
    if(!check_i(i)) throw out_of_range("IndexConv: i out of bounds.");
    int x = i/cols_;
    int y = i-x*cols_;
    return make_pair(x,y);
}

int IndexConv::to_i(int x, int y) const
{
    if(!check_xy(x,y)) throw out_of_range("IndexConv: xy out of bounds.");
    int i = x*cols_+ y;
    return  i;
}


int main(int argc, char *argv[])
{
    vector<vector<double>> test = {
	{0,0,2.1,2.2},
	{0,0,0,2.3},
	{7,0,2.5,2.4}};

    size_t rows = test.size();
    size_t cols = test[0].size();
    try{
	IndexConv idx(rows, cols);
	DisjointSet uf = DisjointSet(rows*cols);
	auto try_union = [&test, &idx, &uf] (int x0, int y0, int x1, int y1) {
	    const double threshold = 0.5;
	    if(idx.check_xy(x0,y0) 
		    && idx.check_xy(x1, y1) 
		    && abs(test[x0][y0] - test[x1][y1])<threshold) {
		uf.do_union(idx.to_i(x0,y0), idx.to_i(x1,y1));
	    }
	};

	for(size_t i=0; i<test.size(); ++i){
	    for(size_t j=0; j<test[i].size(); ++j){
		try_union(i,j,i+1,j);
		try_union(i,j,i,j+1);
	    }
	}

	vector<int> parent(rows*cols);
	uf.dup(parent);
	for(size_t i=0; i<parent.size(); ++i) {
	    auto xy = idx.to_xy(i);
	    auto x = xy.first;
	    auto y = xy.second;
	    cout <<"point("<<x<<','<<y<<")="<<test[x][y]<<" belongs to label#"<<parent[i]<<endl;
	}
    } catch (exception &e) {
	cout <<"exception:"<<e.what()<<endl;
    }
}
