
#ifndef __STHREAD_H
#define __STHREAD_H
//---------------------------------------------------------------------------
#include <assert.h>
#include <functional>
#include<String>
using namespace std;
#include "SSyncObj.h"

//#define use_CriticalSection
#define use_Mutex

//---------------------------------------------------------------------------
namespace su {

   //---------------------------------------------------------------------------
   class SThread
   {
	   public:
		  SThread(void* aOwner=NULL, LPCTSTR lpName=_T(""));
		  virtual ~SThread(void);

		  virtual bool  IsThreadAlive(void);

		  virtual int   Run(void* d=0);
		 
		  virtual DWORD 	MainLoop(void) = 0;	//交由繼承的類別實作

		  virtual void		Stop(void);
		  virtual bool		Wait(DWORD dwMilliseconds=INFINITE);
		  virtual bool		MsgWait(DWORD dwMilliseconds=INFINITE);

		  virtual void		Close(void);

		  virtual bool		Initialize(void);
		  virtual void		Finialize(void);


		  //包裝thread API的函式				
		  virtual DWORD		Suspend(void);//暫停/恢復 線程 但有可能讓還沒完成的工作也暫停	
		  virtual DWORD		Resume(void);			
		  virtual void		Terminate(DWORD exit_code=0);//終止 線程
		  virtual bool		GetExitCode(LPDWORD);
		  //包裝thread API的函式

		  inline  bool  IsRunning() const { return m_bRunning; }

		  HANDLE   GetHandle(void) const { return m_Handle; }
		  

	   protected:
		  static DWORD WINAPI SThreadFunc(LPVOID);

		  LPCTSTR     m_Name;
		  SEvent      m_evStarted;	//目前沒使用
		  SEvent      m_evStop;		//目前沒使用 適用MainLoop時作在此類別中時使用
		  HANDLE      m_Handle;
		  DWORD       m_ThreadID;
		  SCriticalSection  m_ThreadLocker;
		  //SMutex			m_ThreadLocker;
	
		  bool        m_bRunning;
	

		  class Lock;
		  friend class Lock;
		  class Lock
		  {
			 Lock();
			 Lock(const Lock&);
			 Lock& operator=(const Lock&);
			 SThread& m_Host;

			 public:
			 explicit Lock(SThread& host) : m_Host(host) //可傳入自己Thread所在的位址
			 {
				m_Host.m_ThreadLocker.Lock();
			 }
			 ~Lock(void) 
			 {
				m_Host.m_ThreadLocker.Unlock();
			 }
		  };

   };
   //---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif // __STHREAD_H