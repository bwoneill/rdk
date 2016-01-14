//permissions.cpp

void permissions(const char* name){
	cout<<name<<endl;
	TStringLong grant;
	grant.Form("icacls \"%s\\*\" /grant Everyone:F",name);
	gSystem->Exec(grant.Data());
	TSystemFile here(name,name);
	if(here.IsDirectory()){
		TSystemDirectory dir(name,name);
		TList *list=dir.GetListOfFiles();
		for(int i=2;i<list->GetSize();i++){
			TSystemFile *file=list->At(i);
			if(file && file->IsDirectory()){
				permissions(file->GetTitle());
			}
		}
		delete list;
	}
	gSystem->ChangeDirectory(Form("%s\Code",RDKHOME));
}