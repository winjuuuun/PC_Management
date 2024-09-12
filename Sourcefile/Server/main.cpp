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
LPCTSTR lpszClass = TEXT("ī����");


CS* create();
void SVR_Open();
DWORD WINAPI ThreadFunc1(LPVOID);
DWORD WINAPI ThreadFunc2(LPVOID);
void addsock(SOCKET);
void delsock(SOCKET);


HWND hWndMain;
BOOL CONN_ST = FALSE;	// ���� ���� ����
const unsigned short g_uPort = 7878;
HWND hToolBar;		// ���� �ڵ�

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
	};	// ���� ��������

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		InitCommonControls();

		// ���� ����ü ����
		C_S = create();

		// ���� �ڵ� ����
		hToolBar = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, ID_TOOLBAR, 2, g_hInst, IDB_OPEN, ToolBtn, 2, 64, 20, 64, 20, sizeof(TBBUTTON));
		
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 10:
			// ��������
			SVR_Open();
			break;
		case 11:
			// ����� Ŭ���̾�Ʈ ���� �˷���
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

// ���� �����Լ�
void SVR_Open() {
	static BOOL state = TRUE;
	HANDLE hThread;
	DWORD ThreadID;

	if (state) {
		// ���� �ʱ�ȭ (���� ���̺귯�� ����, ���� �ý��� ���� ����)
		nReturn = WSAStartup(WORD(2.0), &wsadata);

		// ���� ���� (IPv4: AF_INET | IPv6: AF_INET6 , ���� ��� Ÿ��, �������� ����)
		listensock = socket(AF_INET, SOCK_STREAM, 0);

		addr_server.sin_family = AF_INET;
		addr_server.sin_addr.s_addr = htons(INADDR_ANY);
		addr_server.sin_port = htons(g_uPort);

		// ���� ���ε� (���� ��ü, �Ұ� ��ü�� �ο��� �ּ� ���� ����ü, ����ü ����)
		nReturn = bind(listensock, (sockaddr*)&addr_server, sizeof(sockaddr));

		// ���� ��� (���� ��ü, ���� ��⿭ ũ��)
		nReturn = listen(listensock, 1);

		// accept ������ ����
		CloseHandle(hThread = CreateThread(NULL, 0, ThreadFunc1, &listensock, 0, &ThreadID));

		CONN_ST = TRUE;

		state = FALSE;
	}
	InvalidateRect(hWndMain, NULL, FALSE);
	MessageBox(hWndMain, "�������¿Ϸ�","�˸�", MB_OK);
}

DWORD WINAPI ThreadFunc1(LPVOID Param) {
	SOCKET* P = (SOCKET*)Param;
	TCHAR Thread_N[30];
	TCHAR strEdit[128];
	HANDLE hThread;
	DWORD ThreadID;
	CS* S;

	// ���� ����ϱ� (���� ��ü, Ŭ���̾�Ʈ �ּ� ����, �ּ� ���� ���� ũ��)
	for (;;) {
		clientsock = accept(*P, (sockaddr*)&addr_client, &addrlen_clt);

		if (clientsock != INVALID_SOCKET) {
			MessageBox(hWndMain, "Ŭ���̾�Ʈ�� �����Ͽ����ϴ�.", "server", MB_OK);

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
		// ������ ���� (��Ĺ ��ü, ���� ���ڿ�, ���ڿ� ũ��, �ɼ�);
		nReturn = recv(*P, buf, buflen, 0);

		if (nReturn == 0 || nReturn == SOCKET_ERROR) {
			MessageBox(hWndMain, "���ſ���", "server", MB_OK);
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
