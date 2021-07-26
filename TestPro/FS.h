#pragma once
#include <iostream>
#include <stdio.h>
#include <dirent.h>


using namespace std;

int sel(const struct dirent* d);

class FS
{
	struct dirent** entry;


private:


public:
	FS();
	~FS();
	bool ScanCustomDir(std::string *in_dir);
	
};

