// TransferSocket.cpp : 定義應用程式的進入點。
//
#pragma warning(disable:C2760)
#include "stdafx.h"
#include "TransferSocket.h"

#include <windows.h>
#include <commdlg.h>


#define MAX_LOADSTRING 100

// 全域變數: 
HINSTANCE hInst;                                // 目前執行個體
HWND		g_hWnd;   // main window
HWND		hWndButtonSelect;					//
HWND		hWndButtonTransfer;					//
HWND		hWndButtonSocketCreate;				//
HWND		hWndButtonSocketAccept;				//
HWND		hWndButtonSocketConnect;			//

HWND		hWndLBLSTATUS;						//

WCHAR szTitle[MAX_LOADSTRING];                  // 標題列文字
WCHAR szWindowClass[MAX_LOADSTRING];            // 主視窗類別名稱



SockThd *					SktThd;

// 這個程式碼模組中所包含之函式的向前宣告: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此置入程式碼。

    // 初始化全域字串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TRANSFERSOCKET, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 執行應用程式初始設定: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRANSFERSOCKET));


    MSG msg;

    // 主訊息迴圈: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函式: MyRegisterClass()
//
//  用途: 註冊視窗類別。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRANSFERSOCKET));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TRANSFERSOCKET);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函式: InitInstance(HINSTANCE, int)
//
//   用途: 儲存執行個體控制代碼並且建立主視窗
//
//   註解: 
//
//        在這個函式中，我們會將執行個體控制代碼儲存在全域變數中，
//        並且建立和顯示主程式視窗。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 將執行個體控制代碼儲存在全域變數中

   //主視窗大小
   int MainFormWidth  = 250;
   int MainFormHeight = 350;
   // Create WinForm
   g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0, MainFormWidth, MainFormHeight, nullptr, nullptr, hInstance, nullptr);

   
   //按鈕大小
   int ibtHeight = 25; 
   int ibtWidth = 140;
   

   //顯示欄大小
   int lbl_Height	 = 80;
   int lbl_Width	 = ibtWidth;

   //按鈕列起始座標
   int Y_cood		 = lbl_Height +25;
   int X_cood		= MainFormWidth/2- ibtWidth /2; //置中
   
   
   //label L"Static"
   hWndLBLSTATUS = CreateWindowW(TEXT("STATIC"), L"Status", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_EDITCONTROL,
	   X_cood, 10  , lbl_Width, lbl_Height, g_hWnd, (HMENU)IDB_LBLSTATUS, NULL, NULL);

	//   x,	   y,nWidth, nHeight,hWndParent, hMenu,hInstance,lpParam
   hWndButtonSocketCreate = CreateWindowW(L"BUTTON", L"初始/重製連線", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   X_cood, Y_cood	  , ibtWidth, ibtHeight, g_hWnd, (HMENU)IDB_BSOCKETCTEATE, NULL, NULL);
   hWndButtonSocketAccept = CreateWindowW(L"BUTTON", L"等待連接SERVER", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   X_cood, Y_cood + 25, ibtWidth, ibtHeight, g_hWnd, (HMENU)IDB_BSOCKETACCEPT, NULL, NULL);
   hWndButtonSocketConnect= CreateWindowW(L"BUTTON", L"主動連線CLIENT", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   X_cood, Y_cood + 50, ibtWidth, ibtHeight, g_hWnd, (HMENU)IDB_BSOCKETCONNECT, NULL, NULL);
   hWndButtonSelect = CreateWindowW(L"BUTTON", L"選檔", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   X_cood, Y_cood + 75, ibtWidth, ibtHeight, g_hWnd, (HMENU)IDB_BSELECTFILE, NULL, NULL);
   hWndButtonTransfer = CreateWindowW(L"BUTTON", L"傳送", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   X_cood, Y_cood + 100, ibtWidth, ibtHeight, g_hWnd, (HMENU)IDB_TRANSFER, NULL, NULL);



   //Socket初始化
   HostLogs("======Create SockThd=======");
   SktThd = new SockThd(g_hWnd);
   if (SktThd)
   {
	   	 
	   SktThd->SET_REGIST_DISPDLG(hWndLBLSTATUS, IDB_LBLSTATUS);//註冊用來顯示的元件
	   ReadConnInfo(&Local_IP, &Local_Port);
	   HostLogs("[TransferSocket]::讀取IP:" + Local_IP + " Port" + Local_Port);
	   SktThd->SET_SOCK_INFO(Local_IP, Local_Port);

	   SktThd->Run();//創立線程	
	   SktThd->SET_SOCK_CREATE();
   }

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

//
//  函式: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  用途:     處理主視窗的訊息。
//
//  WM_COMMAND  - 處理應用程式功能表
//  WM_PAINT    - 繪製主視窗
//  WM_DESTROY  - 顯示結束訊息然後返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 剖析功能表選取項目: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDB_BSELECTFILE:{
				string st_FilePath = "";
				st_FilePath = OpenFileDialog();
				if (st_FilePath == "")
				{
					SktThd->DISPANDLOGS("[TransferSocket]::未選擇檔案");
				}
				else
				{
					SktThd->DISPANDLOGS("[TransferSocket]::選擇了"+ st_FilePath);
					SktThd->SET_TRANSFER_FILENAME(st_FilePath);
				}
			}
				break;
			case IDB_TRANSFER:{
				SktThd->SET_SOCK_SENDFILE();
			}
				break;
			case IDB_BSOCKETCTEATE:{
				HostLogs("[TransferSocket]::按下初始化按鈕");								
				ReadConnInfo(&Local_IP, &Local_Port);				
				SktThd->SET_SOCK_INFO(Local_IP, Local_Port);
				SktThd->SET_SOCK_CREATE();				
				EnableWindow(hWndButtonSocketAccept, true);
				EnableWindow(hWndButtonSocketConnect, true);
			}
				break;
			case IDB_BSOCKETACCEPT: {
				HostLogs("[TransferSocket]::按下接收按鈕");
				EnableWindow(hWndButtonSocketConnect, false);			 
				SktThd->SET_SOCK_ACCEPT();
			}
				break;
			case IDB_BSOCKETCONNECT: {
				HostLogs("[TransferSocket]::按下連線按鈕");
		 
				EnableWindow(hWndButtonSocketAccept, false);
				SktThd->SET_SOCK_CONNECT();
			}
				break;
			case IDB_LBLSTATUS:{
				
				
				
			}
			default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此加入任何使用 hdc 的繪圖程式碼...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// [關於] 方塊的訊息處理常式。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


bool ReadConnInfo(string* IP,string* Port) 
{
	//說明:從設定檔中取出IP、Port字串
	//輸入:要返回的字串位址
	//輸出:開檔失敗返回false 開檔成功返回true並將字串填入point中 

	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0; // 刪除檔名，只獲得路徑字串
	char filename[100] ;   
	sprintf(filename , "%SNetConfig.txt", (LPCTSTR)szFilePath);

	fstream fp;
	char line[128];
	fp.open(filename, ios::in);//開啟檔案
	
	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		HostLogs("[ReadConnInfo]::開檔失敗");
		return false;
	}

 
	HostLogs("[ReadConnInfo]::讀取資料");
	int count = 0;
	while (fp.getline(line, sizeof(line), ',')) {
		HostLogs(line);
		if (count == 1)
			*IP = line;		//第一取Ip
		else if (count == 3)
			*Port = line;	//第二取Port

		count++;
	}

	fp.close();//關閉檔案
	return true;
}

string OpenFileDialog()
{
	string			st_rtn = "";
	TCHAR			szFile[MAX_PATH];      
	ZeroMemory(szFile, MAX_PATH);	
	OPENFILENAME	ofn;      // 公共對話結構
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"All(*.*)\0*.*\0Text(*.txt)\0*.TXT\0\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (::GetOpenFileName(&ofn))
	{
		//顯示選擇的文件   
		OutputDebugString(szFile);
		OutputDebugString((LPCWSTR)"\r\n");
	}


	if (szFile[0] == NULL)
		st_rtn = "";
	else
	{
		
		/*只能轉英文檔名
		UINT len = wcslen(szFile) * 2;
		char *buf = (char *)malloc(len);
		wcstombs(buf, szFile, len);
		*/
		char	file_path[FILE_NAME_MAX_SIZE];
		sprintf(file_path, "%S", (LPCTSTR)szFile);
		
		st_rtn = file_path;
		//free(buf);
	}
	return st_rtn;
}
