#include "StdAfx.h"
#include "SafeDataBuffer.h"


CSafeDataBuffer::CSafeDataBuffer(void)
{
	InitializeCriticalSectionAndSpinCount( &m_cs , 4000 );
}

CSafeDataBuffer::~CSafeDataBuffer(void)
{
	Clear();
	DeleteCriticalSection( &m_cs );
}

void CSafeDataBuffer::Clear()
{
	EnterCriticalSection( &m_cs );
	for( list<RAW_BUFFER*>::iterator it = m_listRaw.begin() ; it != m_listRaw.end() ; it++ )
		delete (*it);

	m_listRaw.clear();
	LeaveCriticalSection( &m_cs );
}

RAW_BUFFER* CSafeDataBuffer::Alloc()
{
	RAW_BUFFER* pRet = NULL;
	EnterCriticalSection( &m_cs );
	if( m_listRaw.size() < MaxSize )
	{
		pRet = new RAW_BUFFER;
		ASSERT( pRet );
		pRet->rb_size = 0;
		m_listRaw.push_back( pRet );
	}
	LeaveCriticalSection( &m_cs );
	return pRet;
}

RAW_BUFFER* CSafeDataBuffer::Get()
{
	RAW_BUFFER* pRet = NULL;
	EnterCriticalSection( &m_cs );
	if( m_listRaw.size() > 0 )
	{
		pRet =  m_listRaw.front() ;
		m_listRaw.pop_front();
	}
	LeaveCriticalSection( &m_cs );
	return pRet;
}
