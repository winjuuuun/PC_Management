////////////////////////////////////////WINAPI����/////////////////////////////////////////
//�޺��ڽ��� ����Ʈ�ڽ����� lParam(���콺):���콺�� �����ϱ� ������
//�ƴ� ��� ��Ʈ�ѿ����� wParam(Ű����)

//WM_COMMAND�ȿ��� wParam�� LOWORD�� ������ �ĺ� HIWORD�� ������ �̺�Ʈ
//��ũ�ѿ����� lParam�� ��� �ٸ� �͵��� wParam

//���� �ѹ��� �� ���� static���� ����(���� 0���� �ʱ�ȭ�Ǿ� �ٷ� ��� �Ǿ� ����)
//WM_TIMER:�ϳ��� ������ ���ÿ� �ٸ� �۾��� �� ��
//WM_NOTIFY:Ʈ���䳪 ����Ʈ�信�� ���� �ٲ����� ����

//CreateWindow(��Ʈ������,��ġ,�θ�â,�����Ҹ��ҽ�,g_hinst,NULL);
//���ҽ� �����Ҷ�(���ҽ��� �ڵ鿡 ����)->GetDlgitem(�θ�â,���ҽ��ĺ���)
// 
/////////////////////////////////����������////////////////////////////
////chk�� �����ϴ� ����:���� üũ�ϱ����� ����
////tg�� �����ϴ� ����:����Ʈ �ڽ��� ���� ���� ����
// 
////////////////////////////////�ּ� ���///////////////////////////////////
//:���� ���� �� �ּ�
////:�����ϱ� ���� �ּ�
///////////////////////////////////////////////////////////����////////////////////////////////////////////////////////////
//������ PC����� ����(����ȣ��Ʈ)
// Ư¡
//-WINAPI�� ������ ���
//-��ȭ���ڱ��
//-P2P��� �ƴϹǷ� ����,Ŭ���̾�Ʈ ���� ����
//-ȸ���� ä���� �ش� �� ȸ���鿡�� ������ �߰�
//-��Ƽ��������(Ŭ���̾�Ʈ�κ��� ���Ʈ�ϴ� ������,�� Ŭ���̾�Ʈ�κ��� ���ú��ϴ� ������) (�ϳ��� ���Ʈ������,�� ���ú꽺����)
//-Ŭ���̾�Ʈ ����(ȸ������,�α���,����,ä��,Ż����� ��û)
//-��������(...���� ����)
//-DB�� ȸ�� ������ ����

//�Ұ�
//�α׾ƿ�ó��(���� �����ð��� ��� �����ϱ�)
//Ż���ϱ�

//Ÿ�̸ӷ� �������� �ϱ�
#include <Windows.h>
#include<stdio.h>
#include<CommCtrl.h>
#include <sql.h>
#include <sqlext.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK JoinLoginDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SelSeatDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OrderDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hinst;
LPCTSTR lpszClass = TEXT("��ȭ����");
////IP�� ��Ʈ��ȣ ����
const char g_szlpAddress[17] = "192.168.0.217";
const unsigned short g_uPort = 7878;
TCHAR tgIp[17] = "192.168.0.217", tgPort[10] = "7878";
TCHAR tgId[10], tgNick[10], tgPass[10];
int nip, nport;

HWND hChatEdit;

int chknickmsg = 0;

BOOL seljoinorloginordel = 0;
TCHAR selnum[10];

TCHAR nickandmsg[100];

DWORD ThreadID;
HANDLE hThread;

HWND hWndDlg;

TCHAR tgmessage[30];
TCHAR tgjoinloginmsg[100]="ID";
int nReturn;
WSADATA wsadata;
SOCKET clientsock;
sockaddr_in addr_client;
int addrlen_clt = sizeof(sockaddr);
char buf[1024];////�޽���
char strTemp[1024];

int chkseat=0;
int chkjoin = 0;
int chklogin = 0;
int i;
int pctime;
int chkexit;
int userexit;
TCHAR userid[10];
TCHAR exitmsg[20];

SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;
////DB�����Լ�
BOOL DBConnect() {
	SQLRETURN Ret;

	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS)return FALSE;
	if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)return FALSE;

	if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS)return FALSE;

	Ret = SQLConnect(hDbc, (SQLCHAR*)"mydb", SQL_NTS, (SQLCHAR*)"c##test", SQL_NTS, (SQLCHAR*)"C##TEST", SQL_NTS);

	if ((Ret != SQL_SUCCESS) && (Ret != SQL_SUCCESS_WITH_INFO))return FALSE;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)return FALSE;

	//MessageBox(hWndDlg, "OK", "str", MB_OK);


	return TRUE;
}
////DB�����Լ�
void DBDisConnect() {
	if (hStmt)SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if (hDbc)SQLDisconnect(hDbc);
	if (hDbc)SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if (hEnv)SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}
////��üȸ���� �ٷ� ����ü
typedef struct CLIENT {
	TCHAR id[10];
	TCHAR nick[10];
	TCHAR pw[10];
	int pctime;
};
CLIENT client;
////ȸ�������Լ�
void deleteclient() {
	SQLCHAR SQL[1000];
	SQLRETURN ret;
	wsprintf((LPTSTR)SQL, "delete from pcclient where id ='%s'",client.id);
	ret = SQLExecDirect(hStmt, SQL, SQL_NTS);
	SQLFetch(hStmt);
	if (hStmt)SQLCloseCursor(hStmt);
}
////ȸ�������Լ�
void addclient() {
	SQLCHAR SQL[1000];
	SQLRETURN ret;
	wsprintf((LPTSTR)SQL, "insert into pcclient (id, nick, pw,time) VALUES('%s','%s','%s',%d)",client.id,client.nick,client.pw,client.pctime);
	ret = SQLExecDirect(hStmt, SQL, SQL_NTS);
	SQLFetch(hStmt);
	if (hStmt)SQLCloseCursor(hStmt);
}
////ȸ�� ��� Į�� ��ȸ
void getclient() {
	SQLCHAR gid[10];
	SQLCHAR gpw[10];
	SQLCHAR gnick[10];
	SQLLEN gtime;
	TCHAR tgid[10];
	TCHAR tgnick[10];
	TCHAR tgpw[10];
	TCHAR tgtime[10];
	SQLCHAR SQL[1000];
	SQLRETURN ret;
	SQLLEN Igprivacy[4];
	SQLBindCol(hStmt, 1, SQL_C_CHAR, gid, sizeof(gid), &Igprivacy[0]);
	SQLBindCol(hStmt, 2, SQL_C_CHAR, gnick, sizeof(gnick), &Igprivacy[1]);
	SQLBindCol(hStmt, 3, SQL_C_CHAR, gpw, sizeof(gpw), &Igprivacy[2]);
	SQLBindCol(hStmt, 4, SQL_C_LONG, &gtime, sizeof(gtime), &Igprivacy[3]);
	wsprintf((LPTSTR)SQL, "select id,nick,pw,time from pcclient where id ='%s'",userid);
	ret = SQLExecDirect(hStmt, SQL, SQL_NTS);
	if (SQLFetch(hStmt) != SQL_NO_DATA) {
		wsprintf(tgid, TEXT("%s"), gid);
		wsprintf(tgnick, TEXT("%s"), gnick);
		wsprintf(tgpw, TEXT("%s"), gpw);
		wsprintf(tgtime, TEXT("%d"), gtime);
		lstrcpy(client.id, tgid);
		lstrcpy(client.nick, tgnick);
		lstrcpy(client.pw, tgpw);
		client.pctime = gtime;
	}
	//InvalidateRect(hDlg, NULL, TRUE);
	if (hStmt)SQLCloseCursor(hStmt);
}
void UpdateTime() {
	SQLCHAR SQL[1000];
	SQLRETURN ret;
	wsprintf((LPTSTR)SQL, "update pcclient set time= %d where id='%s'",pctime,client.id);
	ret = SQLExecDirect(hStmt, SQL, SQL_NTS);
	SQLFetch(hStmt);
	if (hStmt)SQLCloseCursor(hStmt);
}
////�����κ��� ���ú� �ϱ� ���� �������̰� ȸ������,�α��� �����ϸ� ����Ʈ�ڽ��� "�г���] "�� �߰� ����,������ �޼����� ����Ʈ�ڽ��� ����,Ż�𼺰��ϸ� â �ݱ�
DWORD WINAPI ThreadFunc(LPVOID Param) {
	HDC hdc;
	SOCKET* P = (SOCKET*)Param;
	for (;;) {
		nReturn = recv(*P, buf, 1024, 0);
		if (nReturn == 0 || nReturn == SOCKET_ERROR) {
			continue;
		}
		else {
			//MessageBox(hWndDlg, buf, "���� �޼���", MB_OK);
			if (lstrcmp(buf, TEXT("�ڸ���밡��")) == 0)chkseat = 1;				//�ڸ����Ұ�
			else if (lstrcmp(buf, TEXT("ȸ�����Լ���")) == 0) {//ȸ�������ϰ� ��� �����Ű��			//ȸ�����Խ���
				chkjoin = 1;
				lstrcpy(client.id, tgId);
				lstrcpy(client.pw, tgPass);
				lstrcpy(client.nick, tgNick);
				client.pctime = 0;
				DBConnect();
				addclient();
				DBDisConnect();
				DBConnect();
			}
			else if (lstrcmp(buf, TEXT("�α��μ���")) == 0) {					//�α��ν��� 1.���̵�/�������,2.���� �ð� 0�̻�
				pctime = 60;
				DBConnect();
				UpdateTime();
				DBDisConnect();
				DBConnect();
				chklogin = 1;
				lstrcpy(userid, tgId);
				DBConnect();
				getclient();
				DBDisConnect();
				DBConnect();
				pctime = client.pctime;
			}
			else if (lstrcmp(buf, TEXT("PC�ֹ�����")) == 0) {			//�ֹ�����
			}
			else if (lstrcmp(buf, "�����⼺��") == 0)SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			else if (lstrcmp(buf, "Ż�𼺰�") == 0) {
				deleteclient();
				SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			}
			if (lstrlen(buf) != 0) {
				sprintf_s(strTemp, "������ �޽���:%s", buf);
				hChatEdit = GetDlgItem(hWndDlg, IDC_CHATCLIENTLIST);
				int len = GetWindowTextLength(hChatEdit);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)strTemp);
				if (lstrcmp(strTemp, "������ �޽���:Ż�𼺰�") == 0)SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			}
		}
	}
	return 0;
}
////��ȭ���� ��Ʈ�� ����
BOOL CALLBACK ClientDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		hWndDlg = hDlg;
		InitCommonControls();
		SetDlgItemText(hDlg, IDC_IPCLIENTEDIT, tgIp);
		SetDlgItemText(hDlg, IDC_PORTCLIENTEDIT, tgPort);//192.168.0.13
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ENTERCLIENTBUTTON:////������� ������ ����Ʈ�ڽ��� ����
			GetDlgItemText(hDlg, IDC_IPCLIENTEDIT, tgIp, sizeof(tgIp));
			GetDlgItemText(hDlg, IDC_PORTCLIENTEDIT, tgPort, sizeof(tgPort));
			nip = atoi(tgIp);
			nport = atoi(tgPort);
			nReturn = WSAStartup(WORD(2.0), &wsadata);
			clientsock = socket(AF_INET, SOCK_STREAM, 0);
			addr_client.sin_family = AF_INET;
			addr_client.sin_addr.s_addr = inet_addr(tgIp);
			addr_client.sin_port = htons(nport);
			hThread = CreateThread(NULL, 0, ThreadFunc, &clientsock, 0, &ThreadID);
			nReturn = connect(clientsock, (sockaddr*)&addr_client, addrlen_clt);
			if (!nReturn) {
				hChatEdit = GetDlgItem(hWndDlg, IDC_CHATCLIENTLIST);
				int len = GetWindowTextLength(hChatEdit);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"�����Ͽ����ϴ�.");
			}
			break;
			return TRUE;
		case IDC_SENDMSGCLIENTBUTTON:////�޼����� ������ �����ϰ� ����Ʈ�ڽ��� ����
			if (chklogin == 1) {
				GetDlgItemText(hDlg, IDC_MSGCLIENTEDIT, tgmessage, sizeof(tgmessage));
				nReturn = send(clientsock, tgmessage, sizeof(tgmessage), 0);
				if (nReturn) {
					sprintf_s(buf, "�۽��� �޽���:%s", tgmessage);
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, LB_ADDSTRING, 0, (LPARAM)buf);
					hChatEdit = GetDlgItem(hWndDlg, IDC_CHATCLIENTLIST);
					int len = GetWindowTextLength(hChatEdit);
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)buf);
				}
			}
			else {
				MessageBox(hDlg, "�α����� ���� �ϼ���!!", "����", MB_OK);
			}
			break;
			return TRUE;
		case IDC_JOINBUTTON:////��� ������� �α���/�簡�� �����ϴ� ��ȭ���� ����
			if (chkseat == 1) {
				seljoinorloginordel = 0;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "�ڸ� ���� ���� �ϼ���!!", "����", MB_OK);
			}
			return TRUE;
		case IDC_LOGINBUTTON:////��� ������� �α���/�簡�� �����ϴ� ��ȭ���� ����
			if (chkseat == 1) {
				seljoinorloginordel = 1;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "�ڸ� ���� ���� �ϼ���!!", "����", MB_OK);
			}
			return TRUE;
		case IDC_ORDERBUTTON:
			if (chkseat == 1) {
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_ORDERDIALOG), HWND_DESKTOP, (DLGPROC)OrderDlgProc);
			}
			else {
				MessageBox(hDlg, "�ڸ� ���� ���� �ϼ���!!", "����", MB_OK);
			}
			return TRUE;
		case IDC_SEATBUTTON:
			DialogBox(g_hinst, MAKEINTRESOURCE(IDD_SELSEATDIALOG), HWND_DESKTOP, (DLGPROC)SelSeatDlgProc);
			return TRUE;
		case IDC_EXITBUTTON:
			if (chklogin == 1) {
				lstrcpy(exitmsg, "EXIT");
				lstrcat(exitmsg, userid);
				userexit = 1;
				UpdateTime();
				nReturn = send(clientsock, exitmsg, sizeof(exitmsg), 0);
			}
			else MessageBox(hDlg, "�α����ϼ���!!", "����", MB_OK);
			return TRUE;
		case IDC_DELBUTTON:
			if (chklogin == 1) {
				seljoinorloginordel = 2;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else MessageBox(hDlg, "�α����ϼ���!!", "����", MB_OK);
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
////������ �α��ΰ� ȸ������ ��û
BOOL CALLBACK JoinLoginDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_JOINORLOGINBUTTON:
			lstrcpy(tgjoinloginmsg, "");
			lstrcpy(tgId, "");
			lstrcpy(tgNick, "");
			lstrcpy(tgPass, "");
			GetDlgItemText(hDlg, IDC_IDEDIT, tgId, sizeof(tgId));
			GetDlgItemText(hDlg, IDC_NICKEDIT, tgNick, sizeof(tgNick));
			GetDlgItemText(hDlg, IDC_PASSEDIT, tgPass, sizeof(tgPass));
			lstrcpy(tgjoinloginmsg, "ID");
			lstrcat(tgjoinloginmsg, tgId);
			lstrcat(tgjoinloginmsg, "PW");
			lstrcat(tgjoinloginmsg, tgPass);
			if (seljoinorloginordel == 0) {
				lstrcat(tgjoinloginmsg, "NICK");
				lstrcat(tgjoinloginmsg, tgNick);
			}
			wsprintf(selnum, "%d", seljoinorloginordel);
			lstrcat(tgjoinloginmsg, selnum);
			nReturn = send(clientsock, tgjoinloginmsg, sizeof(tgjoinloginmsg), 0);
			break;
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
BOOL CALLBACK SelSeatDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	TCHAR tgSeatnum[10] = "SEAT";
	TCHAR tgnum[10];
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_SEATEDIT, tgnum, sizeof(tgnum));
			lstrcat(tgSeatnum, tgnum);
			nReturn = send(clientsock, tgSeatnum, sizeof(tgSeatnum), 0);
			break;
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
BOOL CALLBACK OrderDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	TCHAR ordermsg[20] = "ORDER";
	TCHAR tgitem[10]="";
	TCHAR tgitemcount[10]="";
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"PC");
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"�Ļ�");
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"����");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ITEMLIST:////ù��°��,�ι�°�濡 ������ ȸ�� ����Ʈ�ڽ��� ����
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE:
				i = SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETTEXT, i, (LPARAM)tgitem);
			}
			break;
			return TRUE;
		case IDOK:
			GetDlgItemText(hDlg, IDC_COUNTEDIT, tgitemcount, sizeof(tgitemcount));
			lstrcat(tgitem, tgitemcount);
			lstrcat(ordermsg, tgitem);
			nReturn = send(clientsock, ordermsg, sizeof(ordermsg), 0);
			break;
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
	DialogBox(g_hinst, MAKEINTRESOURCE(IDD_CLIENTDIALOG), HWND_DESKTOP, (DLGPROC)ClientDlgProc);
	return 0;
}
/*
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit) {
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;
	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit,sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
*/
/*
if (hBit == NULL) {
		hBit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
	}
	hMemDC = CreateCompatibleDC(hdc);
	OldBit = (HBITMAP)SelectObject(hMemDC, hBit);
	FillRect(hMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
	�׸���(hMemDC������)
SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);

WndProc�Լ�
case WM_PAINT:
	if (hBit)DrawBitmap(hdc, 0, 0, hBit);
*/
//�߿�!!!!!!!!!!!!!!!!!!!!!
//������۸� ó�� ���(ĸ�� �� �׸���)
//������۸�ó���� �Լ�ȭ�ؼ� Ÿ�̸ӿ��� ȣ���ϰ� �Լ��ȿ��� InvalidateRect �ϱ�

//������۸��Ҷ� �Լ����� MemDC���ٰ� �׸����� �־��ֱ�

//������۸��Ҷ� �Լ����ٰ� InvalidateRect�� FALSE�� �ֱ�->�׷��� �����ܻ��� �ȳ��´�
//����Ʈ������ ĸ���ѿ���(�ڵ�) �׷��ֱ�

//SQLBindCol:�������� DB�Ӽ��� ����(���ε�)
//SQLExecDirect:SQL����
//SQLFetch:�ݺ����� ���� ���ڵ尡 ���������� ���ε��� ������ ���� �ϳ��� ����

//SendMessage: ��Ʈ�ѿ��� �����͸� ����
//SendMessage(hList, LB_ADDSTRING,0,(LPARAM)tbid);
//->����Ʈ�ڽ��� tbid�� LPARAM���� ����
//SetDlgItemText(hWndDlg, IDC_LIST1, tbid);
//->����Ʈ�ڽ��� �� ����
//���ǹ����� DB������� �ٽ� �����ϱ�(SQL���°��� �������̱⶧���� ���ǹ� �����ϸ� �� �ٲ�)

//SQLBindCol�� ����ȣ�� �ش� ������ ��ȯ�ϴ� ������տ����� �� ����(�������� �����ϰ����ϴ� ������ ����)

//�޺��ڽ����밡������
//SendDlgItemMessage:�θ�â�� ��Ʈ���� ������ �����Ͷ�
//SendMessage�� �θ�â��� ��Ʈ���� �ڵ��̰� ��Ʈ�� �ĺ��ڰ� ����
//case WM_COMMAND:
//	switch (LOWORD(wParam)) {
//	case IDC_DEPTCOMBO:
//		switch (HIWORD(wParam)) {
//		case CBN_SELCHANGE:
//			deptcbi = SendDlgItemMessage(hDlg, IDC_DEPTCOMBO, CB_GETCURSEL, 0, 0);	
//			SendDlgItemMessage(hDlg, IDC_DEPTCOMBO, CB_GETLBTEXT, deptcbi, (LPARAM)tgdept);
//			break;
//		}
//		return TRUE;

//����Ʈ�信�� ���� �����ϱ�
//ListView_GetItemText(����Ʈ���ڵ�,nlv->iITem,���������(��)�ε���,�����ҹ��ڿ�,�����ҹ��ڿ�ũ��); 
//case WM_NOTIFY:
//	LPNMHDR hdr;
//	LPNMLISTVIEW nlv;
//	hdr = (LPNMHDR)lParam;
//	nlv = (LPNMLISTVIEW)lParam;
//	if (hdr->hwndFrom == ����Ʈ���ڵ�) {
//		switch (hdr->code) {
//		case LVN_ITEMCHANGED:
//			if (nlv->uChanged == LVIF_STATE && nlv->uNewState == (LVIS_SELECTED | LVIS_FOCUSED)) {
//				ListView_GetItemText(hLEmp, nlv->iItem, 14, tempemp, 10);
//				GetPrivacy(hDlg);
//				SetDlgItemText(hDlg, IDC_ENEDIT, tempemp);
//				DBDisConnect();
//				DBConnect();
//				GetEinfo(hDlg);
//				DBDisConnect();
//				DBConnect();
//				InvalidateRect(hDlg, NULL, TRUE);
//			}
//		}
//	}
//	return TRUE;

//�޺��ڽ� �׸��� ������ ���̰� ������ ũ�� �׸���

//�ܷ�Ű�� ������ ���� ���̺��� �⺻Ű�� ������ ����

//���ڿ� ���������� ��ȭ�����Լ����� GetDlgItemText�� ��µ� ����Ҷ� �߸��� ����  

//����Ŭ���� ���̺��� �ܷ�Ű �������Ǹ� �˻�
//SELECT CONSTRAINT_NAME
//FROM USER_CONSTRAINTS
//WHERE TABLE_NAME = 'EMP' AND CONSTRAINT_TYPE = 'R';

//����Ʈ��� ListView_InsertItem(����Ʈ���ڵ�,&Li);���� ������ �߰��ϰ� , ListView_SetItem(����Ʈ���ڵ�,&Li); ���� �� �߰��ϱ�

//GetDlgItemText(hDlg, IDC_DEPTRETIREECOMBO, tgdept, sizeof(tgdept)); �̹� ���ε� �Ǿ��ִ� ����  ��Ƽ���̷� ������ �ʱ� ������ GetDlgItemText(hDlg,�ĺ���,���ڿ�,sizeof(���ڿ�));

//SQL ����:SQLExecDirect(hStmt,SQL,SQL_NTS)
//SQL �ѹ� ����(���� ���ڵ� ���� �ִ� select��): SQLFetch(hStmt)

//Ŀ�ǵ忡�� ��Ʈ�� ������ ����ڰ� ���� ��Ʈ���� �����������̰�  �ٸ�������  ��Ʈ���� ���ε���Ű�°Ͱ� �ٸ��� �ν��Ѵ�.

//������ư ������: SendDlgItemMessage(hDlg, IDC_NONMARRIAGERADIO, BM_SETCHECK, BST_CHECKED, 0); BST�� �ٲٰ� ���� ��

////////////////////////////////////////////////////////�������/////////////////////////////////////////////
//������� ���ѹݺ������� ���������� ��� ����
//��������
//nReturn = WSAStartup(WORD(2.0), &wsadata);
//listensock = socket(AF_INET, SOCK_STREAM, 0);
//addr_server.sin_family = AF_INET;
//addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
//addr_server.sin_port = htons(g_uPort);
//nReturn = bind(listensock, (sockaddr*)&addr_server, sizeof(sockaddr));
//nReturn = listen(listensock, 1);
//Ŭ���̾�Ʈ����
//nReturn = WSAStartup(WORD(2.0), &wsadata);
//clientsock = socket(AF_INET, SOCK_STREAM, 0);
//addr_client.sin_family = AF_INET;
//addr_client.sin_addr.s_addr = inet_addr(g_szlpAddress);
//addr_client.sin_port = htons(g_uPort);
//Ŭ���̾�Ʈ���� connect�ϸ� �������� accept�� ����
//Ŭ���̾�Ʈ���� send�ϸ� �������� recv�� ����
//�ܺν����忪��: ��� ���Ʈ�ϰ� �ش� ������ ���ú����ִ� ���ν����� �����
//���ν����� ����: ��� �� ���ϸ��� ���ú�ޱ�
//����ޱ�� ���ú�� ���������� ��� ����
//Ŭ���̾�Ʈ����ŭ ���ν����� �������
//���Ʈ�� ���ο� ���� ���ö����� ���ȴ�. �� �����尡 ���� �׷��⿡ ���Ʈ�� ���ú�� ���� ������ ���� Ŭ���̾�Ʈ�� ���ú� ���Ѵ�.
//�迭�� ��°� �� �ܺν����忡�� �ؾ��ϴ���? ���ν����忡�� ��� ���ư��� �׷��°���? �ܺν����忡�� �迭�ε����� �����ϴµ� ���ο��� �װ� �̿��ؼ� �׷�����? 
//Ŀ������ ��Ʈ�ѽĺ��ڴ� break�� ������ �̺�Ʈó���� return 0���� ������
