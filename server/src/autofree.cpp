#include "autofree.h"
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <atomic>

AutoFreeVoidPtr::AutoFreeVoidPtr()
{
	this->data = NULL;
}
AutoFreeVoidPtr::~AutoFreeVoidPtr()
{
	free(this->data);
}

void AutoFreeVoidPtr::Create(int sz)
{
	assert (this->data == NULL);
	this->data = malloc(sz);
	assert(this->data != NULL);
}

AutoRemoveTempFile::AutoRemoveTempFile()
{
	this->fname = NULL;
}

AutoRemoveTempFile::~AutoRemoveTempFile()
{
	if (this->fname != NULL)
	{
		remove(this->fname);
		delete(this->fname);
	}
}

std::atomic<int> gImageId(0);

void AutoRemoveTempFile::Create(const std::string& content)
{
	assert(this->fname == NULL);
	int id = ++gImageId;
	this->fname = new char[100];
	sprintf(this->fname, "image_%d.dat", id);
	FILE* fp = fopen(this->fname, "wb");
	fwrite(content.data(), content.size(), 1, fp);
	fclose(fp);
}


