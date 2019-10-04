#pragma once
#include <string>

class AutoFreeVoidPtr
{
public:
	AutoFreeVoidPtr();
	~AutoFreeVoidPtr();
	AutoFreeVoidPtr(const AutoFreeVoidPtr&) = delete;
	AutoFreeVoidPtr operator=(const AutoFreeVoidPtr&) = delete;

	void Create(int sz);
	void *data;
};

class AutoRemoveTempFile
{
public:
	AutoRemoveTempFile();
	~AutoRemoveTempFile();
	AutoRemoveTempFile(const AutoRemoveTempFile&) = delete;
	AutoRemoveTempFile& operator=(const AutoRemoveTempFile&) = delete;
	
	void Create(const std::string& content);
	char* fname;
};
