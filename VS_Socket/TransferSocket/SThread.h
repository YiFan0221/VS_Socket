
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
		 
		  virtual DWORD 	MainLoop(void) = 0;	//����~�Ӫ����O��@

		  virtual void		Stop(void);
		  virtual bool		Wait(DWORD dwMilliseconds=INFINITE);
		  virtual bool		MsgWait(DWORD dwMilliseconds=INFINITE);

		  virtual void		Close(void);

		  virtual bool		Initialize(void);
		  virtual void		Finialize(void);


		  //�]��thread API���禡				
		  virtual DWORD		Suspend(void);//�Ȱ�/��_ �u�{ �����i�����٨S�������u�@�]�Ȱ�	
		  virtual DWORD		Resume(void);			
		  virtual void		Terminate(DWORD exit_code=0);//�פ� �u�{
		  virtual bool		GetExitCode(LPDWORD);
		  //�]��thread API���禡

		  inline  bool  IsRunning() const { return m_bRunning; }

		  HANDLE   GetHandle(void) const { return m_Handle; }
		  

	   protected:
		  static DWORD WINAPI SThreadFunc(LPVOID);

		  LPCTSTR     m_Name;
		  SEvent      m_evStarted;	//�ثe�S�ϥ�
		  SEvent      m_evStop;		//�ثe�S�ϥ� �A��MainLoop�ɧ@�b�����O���ɨϥ�
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
			 explicit Lock(SThread& host) : m_Host(host) //�i�ǤJ�ۤvThread�Ҧb����}
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