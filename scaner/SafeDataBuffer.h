#pragma once
#include "windows.h"
#include "list"
using namespace std;


#define BUFFER_SIZE (64-sizeof(DWORD))
struct RAW_BUFFER{
	unsigned char rb_data[BUFFER_SIZE];
	DWORD rb_size;
};


class CSafeDataBuffer
{
public:
	CSafeDataBuffer(void);
	virtual ~CSafeDataBuffer(void);

	enum{
		MaxSize = 256
	};

	void Clear();
	RAW_BUFFER* Alloc();
	RAW_BUFFER* Get();

protected:
	CRITICAL_SECTION m_cs;
	list<RAW_BUFFER*> m_listRaw;
};

