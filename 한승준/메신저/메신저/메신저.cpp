
#include <Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<CommCtrl.h>
#include <sql.h>
#include <sqlext.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int isEmpty();
HINSTANCE g_hinst;
LPCTSTR lpszClass = TEXT("��ȭ����");

HANDLE hSemClientSend[10];
HANDLE hSemClientRecv[10];

HWND hChatEdit;

TCHAR tgdelmsg[100];

TCHAR chknonexistsock[10];

TCHAR chkoneroosockar[10];

TCHAR getdelnick[10], getdel[10];
int getdelnicki = 0, getdeli = 0;

int delclienti = 0;

int nonexistsock;

////������� �������� ���� ����
int nReturn;
WSADATA wsadata;
SOCKET listensock;
SOCKET clientsock = 0;

TCHAR chkclientsockar[10];

DWORD ThreadID;
HANDLE hThread;
HANDLE hClientThread[10];
DWORD ClientThreadID;

SOCKET clientsockar[10];

////������ ���� ���� �÷���
BOOL g_bThreadExitFlag = FALSE;
volatile BOOL g_bClientThreadExitFlags[10] = { FALSE };

int existsocktrue = 0, existsocktrue2 = 0;
int emptytrue = 0, idres = 0, pwres = 0;

int deltrue = 0;
int csi = 0;
int i, j;
int num;
////���� ��ư ���ȴ��� �Ǻ�
BOOL serverpower = 0;
BOOL serveropt = 0;
////��Ʈ�� IP��ȣ ����
const char g_szlpAddress[17] = "127.0.0.1";
const unsigned short g_uPort = 7878;
TCHAR tgIp[10] = "127.0.0.1", tgPort[10] = "7878";
int nip, nport;

BOOL seljoinorlogin = 0;

BOOL chkloginexist = 0;

TCHAR tgroom[10];

TCHAR tgmessage[10];
TCHAR tgjoinloginmsg[100];
int selnamear[10];

unsigned long argp;
sockaddr_in addr_server;
sockaddr_in addr_client;
int addrlen_srv = sizeof(sockaddr);
int addrlen_clt = sizeof(sockaddr);
in_addr in;
hostent* ht;
////�޼��� ���� ����
const int buflen = 40960;
////�޼���
char buf[buflen];
char getbuf[100];
char getcutbuf[100];
char getcutID[100];
char getcutPw[100];
char getcutNick[100];

TCHAR getcutEnter[100];
TCHAR getcutRoom[100];
int cutidi = 0;
int cutpwi = 0;
int cutnicki = 0;
int cutenteri = 0;
int cutroomi = 0;
char strTemp[buflen];

int selnamesocknumar[10];
int selnamesocknumari = 0;

HWND hWndDlg;

////DB����������
SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;


int itemi = 0;
int exituseri = 0;
TCHAR orderchk[30];

DWORD WINAPI ClientThreadFunc(LPVOID Param) {
	int i;
	int csi = (int)Param;
	SOCKET clientsock = clientsockar[csi];
	while (!g_bThreadExitFlag && !g_bClientThreadExitFlags[csi]) {
		if (clientsock == nonexistsock) break;
		TCHAR enterclientnick[100];
		lstrcpy(buf, "");
		nReturn = recv(clientsock, buf, buflen, 0);
		existsocktrue = 0, existsocktrue2 = 0;
		emptytrue = 0, idres = 0, pwres = 0;
		cutidi = 0;
		cutpwi = 0;
		cutnicki = 0;
		cutenteri = 0;
		cutroomi = 0;
		TCHAR chkseatbuf[10];

		TCHAR chkseatmsg[20];
		TCHAR chkjoinmsg[20];
		TCHAR chkloginmsg[20];
		TCHAR chkexitmsg[20];
		TCHAR chkdelmsg[20];
		TCHAR chkordermsg[20];
		TCHAR chkjoinlogin[3];
		TCHAR chkorder[6];
		TCHAR chkitem[20];
		TCHAR chkexit[5];
		TCHAR chkexituser[5];
		lstrcpy(getbuf, "");
		lstrcpy(getbuf, buf);
		if (lstrlen(getbuf) == 5|| lstrlen(getbuf) == 6) {				//����//�¼���ɾ� �Ǻ�
			for (i = 0; i < 4; i++)chkseatbuf[i] = getbuf[i];
			chkseatbuf[i] = '\0';
		}
		else if (lstrlen(getbuf) == 13|| lstrlen(getbuf) == 23) {		//����//�α������� ȸ�����Ը�ɾ����� �Ǻ�(��� ȸ������)
			for (i = 0; i < 2; i++)chkjoinlogin[i] = getbuf[i];
			chkjoinlogin[i] = '\0';
		}
		else if (lstrlen(getbuf) >=8) {														//����//�ֹ����� ���������� �Ǻ�						
			for (i = 0; i < 5; i++)chkorder[i] = getbuf[i];	
			chkorder[i] = '\0';
			for (i = 0; i < 4; i++)chkexit[i] = getbuf[i];
			chkexit[i] = '\0';
			exituseri = 0;
			if (lstrcmp(chkexit, "EXIT") == 0) {										//����//������ ����
				for (i = 4; i < 8; i++)chkexituser[exituseri++] = getbuf[i];
				chkexituser[exituseri] = '\0';
			}
		}
		lstrcpy(chkseatmsg, "�ڸ���밡��");
		lstrcpy(chkjoinmsg, "ȸ�����Լ���");
		lstrcpy(chkloginmsg, "�α��μ���");
		lstrcpy(chkexitmsg, "�����⼺��");
		lstrcpy(chkdelmsg, "Ż�𼺰�");
		if (lstrcmp(chkseatbuf, "SEAT") == 0) {									//����//�ڸ�
			nReturn = send(clientsock, chkseatmsg, sizeof(chkseatmsg), 0);
			lstrcpy(chkseatbuf, "");
		}
		else if (lstrcmp(chkorder, "ORDER") == 0) {														//����//�ֹ�
			itemi = 0;
			lstrcpy(orderchk, "OC");
			for (int i = 5; i < lstrlen(getbuf); i++)chkitem[itemi++] = getbuf[i];
			chkitem[itemi] = '\0';
			lstrcat(orderchk, chkitem);
			nReturn = send(clientsock, orderchk, sizeof(orderchk), 0);
			lstrcpy(chkorder, "");
		}																												//����//�Ʒ� ������ �α�
		else if (lstrcmp(chkjoinlogin, "ID") == 0 && getbuf[12] == '1') {		//����//�α���
			lstrcpy(chkjoinlogin, "");
			nReturn = send(clientsock, chkloginmsg, sizeof(chkloginmsg), 0);
		}
		else if (lstrcmp(chkjoinlogin, "ID") == 0 && getbuf[12] == '2')nReturn = send(clientsock, chkdelmsg, sizeof(chkdelmsg), 0);//����//Ż��
		else if (lstrcmp(chkjoinlogin, "ID") == 0 && getbuf[22] == '0') {//����//ȸ������
			nReturn = send(clientsock, chkjoinmsg, sizeof(chkjoinmsg), 0);
			lstrcpy(chkjoinlogin, "");
		}
		else if (lstrcmp(chkexit, "EXIT") == 0)nReturn = send(clientsock, chkexitmsg, sizeof(chkexitmsg), 0);
		
			sprintf_s(strTemp, "������ �޽���:%s : %d", buf, clientsock);	//����//������ ��� �޽��� ����Ʈ�ڽ��� ����
			hChatEdit = GetDlgItem(hWndDlg, IDC_CHATSERVERLIST);
			int len = GetWindowTextLength(hChatEdit);
			SendDlgItemMessage(hWndDlg, IDC_CHATSERVERLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
			SendDlgItemMessage(hWndDlg, IDC_CHATSERVERLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
			SendDlgItemMessage(hWndDlg, IDC_CHATSERVERLIST, EM_REPLACESEL, FALSE, (LPARAM)strTemp);
	}
	return 0;
}
DWORD WINAPI ThreadFunc(LPVOID Param) {
	TCHAR chksockar[10];
	HDC hdc;
	SOCKET* P = (SOCKET*)Param;
	SOCKET popdata;
	TCHAR chkcsi[10];
	int* tp = (int*)Param;
	int ti = *tp;
	int qsize;
	int i;
	int poptrue = 0;
	for (;;) {
		clientsock = accept(*P, (sockaddr*)&addr_client, &addrlen_clt);
		if (clientsock != INVALID_SOCKET && clientsock != 0) {
			hChatEdit = GetDlgItem(hWndDlg, IDC_CHATSERVERLIST);
			int len = GetWindowTextLength(hChatEdit);
			SendDlgItemMessage(hWndDlg, IDC_CHATSERVERLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
			SendDlgItemMessage(hWndDlg, IDC_CHATSERVERLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
			SendDlgItemMessage(hWndDlg, IDC_CHATSERVERLIST, EM_REPLACESEL, FALSE, (LPARAM)"�մ���  �����Ͽ����ϴ�.");
			clientsockar[csi] = clientsock;
			g_bClientThreadExitFlags[csi] = FALSE;
			hClientThread[csi] = CreateThread(NULL, 0, ClientThreadFunc, (LPVOID)csi, 0, &ClientThreadID);
			csi++;
		}
	}
	return 0;
}
////��ȭ���� ��Ʈ�ѵ��� ���� ����
BOOL CALLBACK ServerDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		hWndDlg = hDlg;
		InitCommonControls();
		SetDlgItemText(hDlg, IDC_IPSERVEREDIT, tgIp);
		SetDlgItemText(hDlg, IDC_PORTSERVEREDIT, tgPort);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_SERVERPOWERBUTTON:
			if (serverpower == 0) {
				serverpower = 1;
				GetDlgItemText(hDlg, IDC_IPSERVEREDIT, tgIp, sizeof(tgIp));
				GetDlgItemText(hDlg, IDC_PORTSERVEREDIT, tgPort, sizeof(tgPort));
				nip = atoi(tgIp);
				nport = atoi(tgPort);
				nReturn = WSAStartup(WORD(2.0), &wsadata);
				listensock = socket(AF_INET, SOCK_STREAM, 0);
				addr_server.sin_family = AF_INET;
				addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
				addr_server.sin_port = htons(nport);
				nReturn = bind(listensock, (sockaddr*)&addr_server, sizeof(sockaddr));
				nReturn = listen(listensock, 1);
				MessageBox(hDlg, "��������", "server", MB_OK);
				hThread = CreateThread(NULL, 0, ThreadFunc, &listensock, 0, &ThreadID);
			}
			else {
				serverpower = 0;
				MessageBox(hDlg, "end", "server", MB_OK);
				for (i = 0; i < csi; i++)TerminateThread(hClientThread[i], 0);
				TerminateThread(hThread, 0);
				for (i = 0; i < csi; i++) {
					clientsockar[i] = 0;
				}
				csi = 0;
				closesocket(clientsock);
				WSACleanup();
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			InvalidateRect(hWndDlg, NULL, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int APIENTRY WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, LPSTR lpszCmdParam, int nCmdShow)
{
	g_hinst = hinstance;
	DialogBox(g_hinst, MAKEINTRESOURCE(IDD_SERVERDIALOG), HWND_DESKTOP, (DLGPROC)ServerDlgProc);
}
