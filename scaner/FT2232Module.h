#pragma once
#include "windows.h"
#include "vector"
#include "assert.h"
#include "ftd2xx.h"

class CFT2232Module
{
public:
	CFT2232Module(void);
	virtual ~CFT2232Module(void);

	
	BOOL Open(int nIndex , CEdit* pEdit );
	BOOL Close();
	BOOL Read(LPVOID pBuffer , DWORD dwBufSize , DWORD* lpdwReadSize );

protected:
	/*设备以及制造商id号，详情请见FT2232H的datasheet，12页*/
	enum{
		ModuleVendorID = 0x0403,
		ModuleProductID = 0x6010
	};

	FT_HANDLE m_ftHanle;
};




