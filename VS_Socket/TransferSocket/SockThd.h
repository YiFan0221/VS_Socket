#pragma once
#include "SThread.h"
#include <Time.h>
#include <winsock2.h>
#pragma  comment(lib,"WS2_32.lib")

#include <string>
#include <cstring>

#include <string.h>
#include <locale>
#include <codecvt>

#define BUFFER_SIZE                1024  
#define FILE_NAME_MAX_SIZE         512  

class SockThd : public su::SThread
{
 	protected:

		DWORD MainLoop(void);


	public:
		SockThd(HWND HwndOwner, void* aOwner = 0);
		~SockThd(void);

	protected:
		HWND						hWnd_Parent;   // main window

		#ifdef use_CriticalSection
			su::SCriticalSection		m_ResultLocker;
		#else
			su::SMutex					m_ResultLocker;
		#endif

		//---------socket--Init----
		su::SEvent					m_evSocketInit;
		su::SEvent					m_evSockCreate;
		su::SEvent					m_evSockInfoSet;
		//---------socket--server--
		su::SEvent					  m_evSockAccept;
		//---------socket--client--
		su::SEvent					m_evSockConnect;
		su::SEvent					m_evSockSendFile;
		su::SEvent					m_evSockRecvFile;

		//---------socket para---------
		WSAData						wsaData;
		struct	sockaddr_in			ClientInfo; // Server用來取得連入對象資訊用client 端位址資訊
		struct	sockaddr_in			ServerAddr; // Server 端位址資訊
		SOCKET						sSocket;
		SOCKET						sListen;
		SOCKET						sAccept;
		//---------fileIO---------
		HWND						hWnd_DispText;
		int							IDB_DISPLBLSTATUS;
		// Flag
		bool						bConn;
		bool						bRcv;
		// Buffer
		char						buf_TSFILE[BUFFER_SIZE];		//用來 接收 檔案用的Buffer 暫時宣告為成員
		string						st_TSFILE;


	public: //公用方法 介面
		void SET_REGIST_DISPDLG(HWND DlgItem,int IDCItem);
		bool SET_SOCK_INFO(std::string s_ip, string s_port);
		bool SET_SOCK_CREATE();
		bool SET_SOCK_ACCEPT();
		bool SET_SOCK_CONNECT();
		bool SET_SOCK_SENDFILE();
		bool SET_SOCK_RECVFILE();
		void SET_TRANSFER_FILENAME(std::string);
		void DISPANDLOGS(std::string strLog);
		

	private: //內部方法
		//--------------------------
		int  Fuc_SocketInit();
		int  Fuc_Sock_Create();
		//------socket--server--
		int  Fuc_Sock_Bind();
		int  Fuc_Sock_Listen();
		int  Fuc_Sock_Accept();
		void Fuc_Sock_Recvfile(SOCKET sSock);
		//------socket--client--
		int  Fuc_Sock_Connect();
		void Fuc_Sock_Send(SOCKET sSock);
		//------Wnd Disp--------
		bool Fuc_DispTextToDlg(string st_Text);
		//------ELSE-----------
		inline void Set_Sock_Connstatus(bool truefalse) {	bConn = truefalse;	}
		inline void Set_Sock_Recvenable(bool truefalse) {	bRcv  = truefalse;	}
		inline bool Get_Sock_Connstatus()				{	return bConn;		}
		inline bool Get_Sock_Recvenable()				{	return bRcv;		}
};
//
static string GetPathStr(string stFileName) {

	char		file_path[FILE_NAME_MAX_SIZE];
	string st_rtn = "";
	memset(file_path, '0', sizeof(file_path));

	//暫時自定義檔名
	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0; // 刪除檔名，只獲得路徑字串
	sprintf(file_path, "%S", (LPCTSTR)szFilePath);

	st_rtn = file_path;
	st_rtn = st_rtn + stFileName;
	return st_rtn;
	//暫時自定義檔名

}
static void HostLogs(std::string strLog)
{
	SYSTEMTIME DT;
	GetLocalTime(&DT);
	time_t t1 = time(NULL);
	struct tm *nPtr = localtime(&t1);

	//char UTF8_Head[] = { "\xef\xbb\xbf" };

	int year	= DT.wYear;
	int month	= DT.wMonth;
	int mday	= DT.wDay;

	int hour	= DT.wHour;
	int min		= DT.wMinute;
	int sec		= DT.wSecond;
	int mill	= DT.wMilliseconds;

	int   nBuffszie = strLog.length() + 256;
	char *lpBuffer  = nullptr;
	lpBuffer = (char*)malloc(nBuffszie);

	char buff[256];

	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0; // 刪除檔名，只獲得路徑字串
	
	//string stFilePath = GetPathStr("");//取當下路徑

	memset(buff, 0, sizeof(char) * 256);
	sprintf(buff, "%STransfer_%04d%02d%02d.log", szFilePath, year, month, mday);

	memset(lpBuffer, 0, sizeof(char)* nBuffszie);
	sprintf(lpBuffer, "[%02d:%02d:%02d %d] %s\n", hour, min, sec, mill, strLog.c_str());

	FILE*    pFile = nullptr;


	pFile = fopen(buff, "a+");
	if (NULL == pFile) {
		return;
	}
	else {
		fwrite(lpBuffer, sizeof(char), strlen(lpBuffer), pFile);
		fclose(pFile);
	}
	if (lpBuffer != nullptr)
	{
		free(lpBuffer);
	}
};
static wchar_t* s2ws(const std::string& str) {
	/*using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);*/

	size_t size = str.length();
	wchar_t *buffer = new wchar_t[size + 1];
	memset(buffer, '\0', size * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;
	return buffer;

}
