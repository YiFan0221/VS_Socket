//---------------------------------------------------------------------------
#include "stdafx.h"
#include <windows.h>
#include "SockThd.h"


SockThd::SockThd(HWND aHwndOwner, void* aOwner)
	: SThread(aOwner), hWnd_Parent(aHwndOwner), m_ResultLocker(_T("TransferSocketLocker")), hWnd_DispText(NULL), IDB_DISPLBLSTATUS(NULL), bConn(false), bRcv(false) //IBONPRINTER_ResultLocker
{
}
//---------------------------------------------------------------------------
SockThd::~SockThd(void)
{
}
//---------------------------------------------------------------------------


DWORD SockThd::MainLoop(void)
{
	DWORD rtn = 0;

	HANDLE hws[5];
	
	while (m_bRunning) {
		
		hws[4] = m_evSockSendFile.FEvent;
		hws[3] = m_evSockConnect.FEvent;
		hws[2] = m_evSockAccept.FEvent;
		hws[1] = m_evSockCreate.FEvent;
		hws[0] = m_evStop.FEvent;	//signal for cmd ready in m_Cmds
									//hws[1] = m_evCmds.FEvent;	//�q����command

		//hws[7] = m_evSockInfoSet;
		DWORD ev = ::WaitForMultipleObjects(ARRAYSIZE(hws), hws, FALSE, 500); // 
		switch (ev)
		{
		case WAIT_OBJECT_0:
			m_bRunning = false;
			break;
		//----Server----
		case WAIT_OBJECT_0 + 1:
		{
			Fuc_Sock_Create();
		}
			break;
		case WAIT_OBJECT_0 + 2:
		{
			Fuc_Sock_Bind();
			Fuc_Sock_Listen();
			Fuc_Sock_Accept();
		}
			break;
		
		case WAIT_OBJECT_0 + 3:
			Fuc_Sock_Connect();
			break;
		case WAIT_OBJECT_0 + 4:
		{
			if (Get_Sock_Connstatus())
				Fuc_Sock_Send(sSocket); //Client��
			else if (Get_Sock_Recvenable())
				Fuc_Sock_Send(sAccept); //Server��
		}
			break;
		case WAIT_OBJECT_0 + 5:

			break;
		//----Client----
		case WAIT_OBJECT_0 + 6:
			
			break;

		case WAIT_TIMEOUT:
		{
			if(Get_Sock_Connstatus())
				Fuc_Sock_Recvfile(sSocket);
			else if(Get_Sock_Recvenable())
				Fuc_Sock_Recvfile(sAccept);

		}
			break;
		}
	}
	
	return rtn;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void SockThd::SET_REGIST_DISPDLG(HWND DlgItem, int IDCItem)
{
	//���U�n��ܤ�r������
	//�óz�LFuc_DispTextToDlg()�����
	hWnd_DispText		= DlgItem;
	IDB_DISPLBLSTATUS	= IDCItem;
}

bool SockThd::SET_SOCK_INFO(std::string s_ip, string s_port)
{
	int i_port=0;
	i_port = atoi(s_port.c_str());
	memset(&ServerAddr, 0, sizeof(ServerAddr)); // �M��,�N��Ƴ]�� 0
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(i_port); // �]�w port,�᭱���� htons()
	ServerAddr.sin_addr.s_addr = inet_addr(s_ip.c_str()); // addr.sin_addr.s_addr = INADDR_ANY; // �Y�]�w INADDR_ANY ��ܥ��� IP
	return 0;
}

bool SockThd::SET_SOCK_CREATE() {
	m_evSockCreate.Set();
	return true;
}

bool SockThd::SET_SOCK_ACCEPT()
{
	m_evSockAccept.Set();
	return true;
}

bool SockThd::SET_SOCK_CONNECT()
{
	m_evSockConnect.Set();
	return true;
}



bool SockThd::SET_SOCK_SENDFILE(){
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif
 
	m_evSockSendFile.Set();
	return true;
}

bool SockThd::SET_SOCK_RECVFILE()
{
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	m_evSockRecvFile.Set();
	return true;
}

void SockThd::SET_TRANSFER_FILENAME(std::string FILEPATH)
{
	st_TSFILE = FILEPATH;
}

int  SockThd::Fuc_SocketInit() {
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	closesocket(sSocket);
	closesocket(sAccept);
	closesocket(sListen);
	Set_Sock_Connstatus(false);
	Set_Sock_Recvenable(false);
	return 0;
}
int  SockThd::Fuc_Sock_Create() 
{
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	//���禡�إ�Socket�öi��򥻳]�w

	Fuc_SocketInit();

	WORD version = MAKEWORD(2, 2); // ����
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // ���\�^�� 0
	if (iResult != 0) {
		DISPANDLOGS("[Fuc_Sock_Create]::WSAStartup::FALSE");
		return 1;
	}

	if (sSocket != INVALID_SOCKET)
	{
		sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sSocket == INVALID_SOCKET) {
			DISPANDLOGS("[Fuc_Sock_Create]::FALSE");
			return 2;
		}
	}
	DISPANDLOGS("[Fuc_Sock_Create]::SUCC");
	return 0;
}
int  SockThd::Fuc_Sock_Bind() {
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	int enable = 1;
	if (setsockopt(sSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int)) < 0)
	{
 
		closesocket(sSocket);
		DISPANDLOGS("[Fuc_Sock_Bind]::setsockopt SO_REUSEADDR FALSE");
		return 2;
	}

	int iResult = ::bind((SOCKET)sSocket, (SOCKADDR*)&ServerAddr, (int)sizeof(ServerAddr));
	if (iResult != 0)
	{
		DISPANDLOGS("[Fuc_Sock_Bind]::FALSE");
		return 2;
	}
	else
	{
		DISPANDLOGS("[Fuc_Sock_Bind]::SUCC");
		return 0;

	}
}
int  SockThd::Fuc_Sock_Listen(){
	//Socket Listen

	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	int iResult = listen(sSocket, SOMAXCONN);
	if (iResult != 0)
	{
		DISPANDLOGS("[Fuc_Sock_Listen]::FALSE");
		return 3;
	}	
	else
	{
		DISPANDLOGS("[Fuc_Sock_Listen]::SUCC");
		return 0;
	}
		

}
int  SockThd::Fuc_Sock_Accept() {
	//���ݥL�H�s�u���禡 //�`�N!! �����_��
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	int	clientAddrLen = sizeof(ClientInfo);
	DISPANDLOGS("[Fuc_Sock_Accept]::���ݳs�u��");
	sAccept = accept(sSocket, (SOCKADDR*)&ClientInfo, &clientAddrLen);
	if (sAccept != INVALID_SOCKET)
	{
		string st_IP = inet_ntoa( ClientInfo.sin_addr );
		DISPANDLOGS("[Fuc_Sock_Accept]::���H�s�u�i��"+ st_IP);
		//Set_Sock_Connstatus(true);
		Set_Sock_Recvenable(true);
		return 4;// �� client �ݦ��\�s�u�L��
	}
	else
	{
		DISPANDLOGS("[Fuc_Sock_Accept]::�L�H�s�u");
		return 0;
	}

}

int  SockThd::Fuc_Sock_Connect()
{	//�s�u�ܦ���ServerAddr�ҰO����IP
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	int r = connect(sSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)); //������ sSocket�~��s�u
	if (r != SOCKET_ERROR) {
		DISPANDLOGS("[Fuc_Sock_Connect]::�s�u���\");
		Set_Sock_Connstatus(true);
		return 1;
	}
	else
	{

		DISPANDLOGS("[Fuc_Sock_Connect]::�s�u����");
		Set_Sock_Connstatus(false);
		return 0;
	}
	
}

void SockThd::Fuc_Sock_Send(SOCKET sSock){
	//Socket���o�eFUNC
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	string st = "";

		if (sSock == INVALID_SOCKET)
		{
			DISPANDLOGS("[Fuc_Sock_Send]�ǤJSOCKET���~,�����ǰe");
			return; 
		}
		string st_FName = st_TSFILE;
 
		FILE *pFile = fopen(st_FName.c_str(), "rb");
		if (pFile == NULL)
		{
			DISPANDLOGS("[Fuc_Sock_Send]�L�k�}���ɮ�,���ɮ׬O�_�s�b");
		}
		else
		{
			char * buffer;
			int nBlockSzie = 1000;
			long lSize;

			buffer = (char*)calloc((nBlockSzie),sizeof(char));
			fseek(pFile, 0, SEEK_END);
			lSize = ftell(pFile);
			rewind(pFile);
			int nLoop = lSize / nBlockSzie;
			if ((lSize % nBlockSzie) > 0) nLoop++;
			for (int i = 0; i < nLoop; i++)
			{
				int iRdnum = 0;
				if (buffer == NULL)	break; 						
				iRdnum = fread(buffer, sizeof(char), nBlockSzie, pFile); //read file to buffer

				/*
				if (1)//�o�{�ǿ�L�h����Ʀ����` �o����խ�a��X�@�ӶǤJbuffer���ɮ�
				{
					
					string FName = GetPathStr("samplefileB.txt");
					FILE *fp2 = fopen(FName.c_str(), "wb");
					fwrite(buffer, sizeof(char), nBlockSzie, fp2);
					fclose(fp2);
				}*/

				send(sSock, buffer, iRdnum, 0);
				memset(buffer, '0', nBlockSzie);

				char Sdisp[100];
				sprintf(Sdisp,"Send %d byte. Loop(%d/%d)..",iRdnum,i+1,nLoop);
				Fuc_DispTextToDlg(Sdisp);
			}
			free(buffer);
		}
		if(pFile != NULL)
			fclose(pFile);
		
}

void SockThd::Fuc_Sock_Recvfile(SOCKET sSock){
	//Socket������FUNC
	#ifdef use_CriticalSection
		su::AutoLocker<su::SCriticalSection> locker(m_ResultLocker);
	#else
		su::AutoLocker<su::SMutex> locker(m_ResultLocker);
	#endif

	if (sSock == INVALID_SOCKET)
	{
		DISPANDLOGS("[Fuc_Sock_Recvfile]�ǤJSOCKET���~,��������");
		return;
	}
		
	string st_FName = GetPathStr("ReceiveFile");
	
	//�NRecv�]�m���D���릡
	int timeout = 2000; //in milliseconds. this is 30 seconds
	setsockopt(sSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int)); //setting the receive timeout
	 
	int  nBlockSzie	= 0;
	int  count		= 0;
	int  sz_count	= 0;
	bool b_opened	= false;
	FILE *fp		= NULL;
		
	memset(buf_TSFILE, '0', BUFFER_SIZE);
	while ((nBlockSzie = recv(sSock, buf_TSFILE, sizeof(buf_TSFILE), 0)) > 0)
	{
		if (b_opened == false || fp == NULL)
		{
			fp = fopen(st_FName.c_str(), "wb");
			b_opened = true;
		}

		if (fp == NULL)
		{
			DISPANDLOGS("[Fuc_Sock_Recvfile]�L�k�}���ɮ׽��ˬd�ɮ׬O�_�s�b");
			break;
		}
		fwrite(buf_TSFILE, sizeof(char), nBlockSzie, fp);
		memset(buf_TSFILE, '0', BUFFER_SIZE);

		char Sdisp[100];
				
		count++;
		sz_count += nBlockSzie;
		sprintf(Sdisp, "Recv %d byte. Loop(%d)..total size:(%d)byte", nBlockSzie, count , sz_count);
		Fuc_DispTextToDlg(Sdisp);
	}

	if(fp!=NULL)
		fclose(fp);
			

 
}

bool SockThd::Fuc_DispTextToDlg(string st_Text)
{
	//�o�e��r��n��ܪ�����
	//�ݳz�L SET_REGIST_DISPDLG() �ӵ��U�n��ܪ���������HANDLE

	if (hWnd_DispText != NULL && IDB_DISPLBLSTATUS != NULL)
	{
		if(SendMessage(hWnd_DispText, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)s2ws(st_Text)))
			return true;
		else
			return false;
	}
	else
		return false;
}
void SockThd::DISPANDLOGS(std::string strLog)
{//�N�T����ܨðO���b�����ɤ�
	HostLogs(strLog);
	Fuc_DispTextToDlg(strLog);
}
