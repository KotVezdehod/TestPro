#include "pch.h"
#include "FS.h"


FS::FS()
{


	return;
}

FS::~FS()
{

	return;
}

bool FS::ScanCustomDir(std::string *in_dir)
{
	using namespace std;
	int n;

	n = scandir(in_dir->c_str(), &entry, sel, alphasort);
	return !(n<0);
}


int sel(const struct dirent* d)
{
	return 1;
}
