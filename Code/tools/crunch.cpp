//crunch.cpp

#include <vector>
#include <queue>
using namespace std;

queue<Task*> tasks(0);

void crunch(const Char_t *types){
	while(!tasks.empty()){
		tasks.pop();
	}
	vector<Int_t> series=TreeReader::getSeries(RDKRAW);
	for(Int_t i=0;i<series.size();i++){
		vector<Int_t> list=TreeReader::getRawList(series[i],"D");
		for(Int_t j=0;j<list.size();j++){
			tasks.push(new Task(RDKRAW,series[i],list[j],'D'));
		}
	}
	crunch(tasks,-1);
}