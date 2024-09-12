#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#define ID_TOOLBAR 100


typedef struct Client_Socket {
	TCHAR ID[128];
	SOCKET Client_Sock;
	struct Client_Socket* link;
}CS;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("카운터");


CS* create();
void SVR_Open();
DWORD WINAPI ThreadFunc1(LPVOID);
DWORD WINAPI ThreadFunc2(LPVOID);
void addsock(SOCKET);
void delsock(SOCKET);


HWND hWndMain;
BOOL CONN_ST = FALSE;	// 서버 오픈 상태
const unsigned short g_uPort = 7878;
HWND hToolBar;		// 툴바 핸들

int nReturn;
WSADATA wsadata;
SOCKET listensock;
SOCKET clientsock = 0;
sockaddr_in addr_server;
sockaddr_in addr_client;
int addrlen_clt = sizeof(sockaddr);
CS* C_S;
const int buflen = 4096;
TCHAR buf[buflen];



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_ASTERISK);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	TBBUTTON ToolBtn[2] = {
		{0,10,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
		{1,11,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
	};	// 툴바 정보세팅

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		InitCommonControls();

		// 소켓 구조체 생성
		C_S = create();

		// 툴바 핸들 생성
		hToolBar = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, ID_TOOLBAR, 2, g_hInst, IDB_OPEN, ToolBtn, 2, 64, 20, 64, 20, sizeof(TBBUTTON));
		
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 10:
			// 서버오픈
			SVR_Open();
			break;
		case 11:
			// 연결된 클라이언트 정보 알려줌
			break;
		}
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}


CS* create() {
	CS* N;

	N = (CS*)malloc(sizeof(CS));

	lstrcpy(N->ID, TEXT(""));
	N->Client_Sock = 0;
	N->link = NULL;

	return N;
}

// 서버 오픈함수
void SVR_Open() {
	static BOOL state = TRUE;
	HANDLE hThread;
	DWORD ThreadID;

	if (state) {
		// 소켓 초기화 (윈속 라이브러리 버전, 윈속 시스템 관련 정보)
		nReturn = WSAStartup(WORD(2.0), &wsadata);

		// 소켓 생성 (IPv4: AF_INET | IPv6: AF_INET6 , 소켓 통신 타입, 프로토콜 결정)
		listensock = socket(AF_INET, SOCK_STREAM, 0);

		addr_server.sin_family = AF_INET;
		addr_server.sin_addr.s_addr = htons(INADDR_ANY);
		addr_server.sin_port = htons(g_uPort);

		// 소켓 바인드 (소켓 객체, 소객 객체에 부여할 주소 정보 구조체, 구조체 길이)
		nReturn = bind(listensock, (sockaddr*)&addr_server, sizeof(sockaddr));

		// 접속 대기 (소켓 객체, 연결 대기열 크기)
		nReturn = listen(listensock, 1);

		// accept 스레드 생성
		CloseHandle(hThread = CreateThread(NULL, 0, ThreadFunc1, &listensock, 0, &ThreadID));

		CONN_ST = TRUE;

		state = FALSE;
	}
	InvalidateRect(hWndMain, NULL, FALSE);
	MessageBox(hWndMain, "서버오픈완료","알림", MB_OK);
}

DWORD WINAPI ThreadFunc1(LPVOID Param) {
	SOCKET* P = (SOCKET*)Param;
	TCHAR Thread_N[30];
	TCHAR strEdit[128];
	HANDLE hThread;
	DWORD ThreadID;
	CS* S;

	// 접속 허용하기 (소켓 객체, 클라이언트 주소 정보, 주소 정보 구조 크기)
	for (;;) {
		clientsock = accept(*P, (sockaddr*)&addr_client, &addrlen_clt);

		if (clientsock != INVALID_SOCKET) {
			MessageBox(hWndMain, "클라이언트가 접속하였습니다.", "server", MB_OK);

			addsock(clientsock);

			S = C_S;
			while (S->link != NULL) {
				S = S->link;
			}

			CloseHandle(hThread = CreateThread(NULL, 0, ThreadFunc2, &(S->Client_Sock), 0, &ThreadID));
		}

	}
	return 0;
}

DWORD WINAPI ThreadFunc2(LPVOID Param) {
	SOCKET* P = (SOCKET*)Param;


	static int i;

	while (1) {
		// 데이터 수신 (소캣 객체, 받을 문자열, 문자열 크기, 옵션);
		nReturn = recv(*P, buf, buflen, 0);

		if (nReturn == 0 || nReturn == SOCKET_ERROR) {
			MessageBox(hWndMain, "수신에러", "server", MB_OK);
			return 0;
		}
		else {
		}
	}
	return 0;
}


void addsock(SOCKET S) {
	CS* N, * P;

	N = create();
	N->Client_Sock = S;

	P = C_S;

	while (P->link != NULL) {
		P = P->link;
	}

	P->link = N;

}

void delsock(SOCKET S) {
	if (C_S->link != NULL) {
		CS* E, * P;
		P = C_S;

		while (P->link != NULL) {
			E = P;
			P = P->link;
			if (P->Client_Sock == S) {
				E->link = P->link;
				break;
			}
		}
		free(P);
	}
}
