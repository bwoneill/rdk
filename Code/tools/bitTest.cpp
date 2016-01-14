//bitTest.cpp

void bitTest(){
	TreeReader reader;
	reader.openSeries(257,"S");
	ofstream output("bitTest.log");
	reader.tree->GetEntry(0);
	Entry e0=reader.entry,e1;
	for(int i=1;i<reader.tree->GetEntries();i++){
		reader.tree->GetEntry(i);
		e1=reader.entry;
		output<<e1.timestamp[1]-e0.timestamp[1]<<endl;
		e0=e1;
	}
	output.close();
}