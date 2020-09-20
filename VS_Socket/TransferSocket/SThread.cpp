
#include "stdafx.h"
#include "SThread.h"

//---------------------------------------------------------------------------
namespace su {
    //---------------------------------------------------------------------------
    DWORD WINAPI SThread::SThreadFunc(LPVOID data)
    {	//透過Run中的CreateThread(...) 執行

		SThread* thread = (SThread*)data;
		{//創立流程
			SThread::Lock lock(*thread);
			thread->m_evStop.Reset();
			thread->m_bRunning = true;
			thread->m_ThreadID = ::GetCurrentThreadId();
			thread->m_evStarted.Set();
		}
		DWORD rtn = thread->MainLoop(); //MainLoop會一直執行直到關閉才會執行下方程式
		{//離開線程
			SThread::Lock lock(*thread); 
			thread->Finialize();
			thread->m_bRunning = false;
		
		}

        if (thread)
            delete thread;

        return rtn;
    }
    //---------------------------------------------------------------------------
    SThread::SThread(void* aOwner, LPCTSTR lpName)
        :  m_evStarted(),  m_evStop(NULL,TRUE,FALSE,NULL),
        m_Handle(0), m_ThreadID(0), 
        m_ThreadLocker(_T("SThread_Locker")),
		m_bRunning(false),
        m_Name(lpName)
    {
    }
    //---------------------------------------------------------------------------
    SThread::~SThread(void)
    {
       Close();
    }
    //---------------------------------------------------------------------------
    bool SThread::Initialize(void)
    {
        return true;
    }
    //---------------------------------------------------------------------------
    void SThread::Finialize(void)
    {
    }
 
    //---------------------------------------------------------------------------

    int SThread::Run(void* data)
    {
        assert( m_Handle == 0 && "Must Close Thread Handle Before Run Again!");
        int ok = false;
        {
            Lock lock(*this);
            
            m_evStarted.Reset();
            m_evStop.Reset();
            ok = Initialize();
            m_bRunning = true;
        }
        if (ok) {
            m_Handle =  CreateThread(NULL, 0, SThread::SThreadFunc, this, 0, NULL);
            if (m_Handle) {
                Sleep(0);
                m_evStarted.Wait(INFINITE);
            }
        }
        return (int) m_Handle;
    }
    //---------------------------------------------------------------------------
    DWORD SThread::Resume(void)
    {
        Lock lock(*this);
        if (m_Handle)
            return ResumeThread(m_Handle);
        return 0xffffffff;
    }
    //---------------------------------------------------------------------------
    DWORD SThread::Suspend(void)
    {
        Lock lock(*this);
        if (m_Handle)
            return SuspendThread(m_Handle);
        return 0xffffffff;
    }
    //---------------------------------------------------------------------------
    bool SThread::GetExitCode(LPDWORD exit_code)
    {
        Lock lock(*this);
        if (m_Handle)
            return GetExitCodeThread(m_Handle, exit_code) != 0;
        return false;
    }
    //---------------------------------------------------------------------------
    void SThread::Close(void)
    {
        Lock lock(*this);
        if (m_Handle) {
            CloseHandle(m_Handle);
            m_Handle = 0;
        }
    }
    //---------------------------------------------------------------------------
    void SThread::Stop(void)
    {
        m_bRunning = false;
        m_evStop.Set();
    }
    //---------------------------------------------------------------------------
    bool SThread::Wait(DWORD dwMilliseconds)
    {
        // only wait if not active
        if ( IsThreadAlive())
            return WaitForSingleObject(m_Handle, dwMilliseconds)!=WAIT_TIMEOUT;
        return true;
    }
    //---------------------------------------------------------------------------
    void SThread::Terminate(DWORD exit_code)
    {
        if (IsThreadAlive())  {
            ::TerminateThread(m_Handle, exit_code);
            CloseHandle(m_Handle);
            m_Handle = 0;
        }
    }
    //---------------------------------------------------------------------------
    bool  SThread::IsThreadAlive(void) 
    {
        DWORD ExitCode;
        if ( (m_Handle != NULL) && ::GetExitCodeThread(m_Handle, &ExitCode))
            return (STILL_ACTIVE == ExitCode);
        else
            return false;
    }
    //---------------------------------------------------------------------------
    bool SThread::MsgWait(DWORD dwMilliseconds)
    {
        if (m_Handle==0)  return true;

        DWORD tick = GetTickCount();

        bool done = false;
        while ( !done )
        {
            DWORD ev = MsgWaitForMultipleObjects(1, &m_Handle, FALSE, 
                (dwMilliseconds==INFINITE) ?  INFINITE : dwMilliseconds - ((DWORD)(GetTickCount()-tick)), QS_ALLEVENTS);
            if ( ev == WAIT_OBJECT_0)
            {
                return true;
            }
            else if ( ev == WAIT_OBJECT_0 + 1)
            {
                MSG msg;
                while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                {
                    if ( msg.message == WM_QUIT)
                    {
                        bool rtn = Wait( (dwMilliseconds==INFINITE) ?  INFINITE : ((DWORD) (GetTickCount() - tick)) - dwMilliseconds);
                        PostQuitMessage(msg.wParam);
                        return rtn;
                    }
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else
            {
                done = true;
            }
        }
        return false;
    }
}
//---------------------------------------------------------------------------
