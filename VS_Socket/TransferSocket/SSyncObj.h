/*
* 宣告&實作
*互斥鎖:
*	SCriticalSection
*	SMutex
*自動解構的殼
*	AutoLocker<template>
*供控制的事件
*	SEvent
*/

#pragma once
#include <assert.h>
namespace su
{

   //---------------------------------------------------------------------------
   class SCriticalSection 
   {
			public:
			CRITICAL_SECTION  m_Section;


			SCriticalSection(void)
			{
				InitializeCriticalSection( &m_Section );
			}

			SCriticalSection(LPCTSTR name)
			{//讓呼叫的程式相容於 SMutex(LPCTSTR lpName=_T("")) 		
				InitializeCriticalSection( &m_Section );
			}

			~SCriticalSection(void)
			{
				DeleteCriticalSection( &m_Section );
			}

			inline operator LPCRITICAL_SECTION() { return &m_Section; }

			void Lock(void)
			{ 
				EnterCriticalSection(&m_Section); 
			}

			void Unlock(void) 
			{
				LeaveCriticalSection(&m_Section); 
			}

   };
   //---------------------------------------------------------------------------
   template <class LOCKER>
   class AutoLocker
   {
   public:
      AutoLocker( LOCKER& locker ) 
         : m_Locker(locker), bLock_(true)
      {
         m_Locker.Lock();
      }

      ~AutoLocker(void) 
      {
         Unlock();
      }

      void Unlock(void)
      {
         if (bLock_) {
            bLock_ = false;
            m_Locker.Unlock();
         }
      }

      void Lock(void)
      {
          if (!bLock_) {
              bLock_ = true;
              m_Locker.Lock();
          }
      }

   private:
      bool bLock_;
      LOCKER& m_Locker;

      // dummy, prevent copy
      AutoLocker(const AutoLocker&);
      AutoLocker& operator=(const AutoLocker&);

   };


   //---------------------------------------------------------------------------
   class SMutex 
   {
	   protected:
	   HANDLE      m_hMutex;
	   public:
		  SMutex(LPCTSTR lpName=_T("")) 
		  {
			 m_hMutex = CreateMutex(NULL,FALSE,lpName);
		  }

		  virtual ~SMutex(void) 
		  {
			 if (m_hMutex!=NULL)
				::CloseHandle(m_hMutex);
			 m_hMutex = 0;
		  }

		  virtual DWORD Lock(DWORD dwMilliseconds=INFINITE) 
		  {
			 return ::WaitForSingleObject( m_hMutex, dwMilliseconds);
		  }

		  virtual void Unlock(void) 
		  {
			 ::ReleaseMutex(m_hMutex);
		  }

		  virtual bool CanLock(void) 
		  {
			 return m_hMutex != NULL;
		  }
   };

#define AUTO_LOCKER(type,locker) su::AutoLocker<type> locker##_Locker(locker)
   typedef su::AutoLocker<su::SCriticalSection> SCSAutoLocker;
   typedef su::AutoLocker<su::SMutex>			SMutexAutoLocker;


   //---------------------------------------------------------------------------
   class SEvent
   {
	  public:
      HANDLE   FEvent;
      SEvent(LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL, BOOL bManualReset=FALSE,
         BOOL bInitialState=FALSE, LPCTSTR lpName=NULL) 
         : FEvent( CreateEvent(lpSecurityAttributes, bManualReset, bInitialState, lpName))
      {
      }

      SEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
         : FEvent( OpenEvent(dwDesiredAccess, bInheritHandle, lpName))
      {
      }

      ~SEvent(void)
      {
         if (FEvent) {
            CloseHandle(FEvent);
            FEvent = 0;
         }
      }

      inline operator HANDLE() {return FEvent; }
      inline operator const HANDLE() const { return FEvent; }

      inline BOOL Reset(void) const
      {
         assert( FEvent );
         return ResetEvent( FEvent );
      }

      inline BOOL Set(void) const
      {
         assert( FEvent );
         return SetEvent( FEvent );
      }

      inline BOOL Pulse(void) const
      {
         assert(FEvent);
         return PulseEvent(FEvent);
      }

      inline DWORD Wait(DWORD dwMilliseconds = INFINITE) const
      {
         assert(FEvent);
         return WaitForSingleObject( FEvent, dwMilliseconds);
      }

      inline bool IsSet() const
      {
          assert(FEvent);
          return WAIT_OBJECT_0 == WaitForSingleObject(FEvent, 0);
      }

   };


}