

//�Ұ�

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
const char g_szlpAddress[17] = "127.0.0.1";
const unsigned short g_uPort = 7878;
TCHAR tgIp[17] = "127.0.0.1", tgPort[10] = "7878";
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

//���� //�ֿ亯��
int chkseat=0;
int chkjoin = 0;
int chklogin = 0;
int i;
int pctime;
int chkexit;
int userexit;
int chktimer;
int pci = 0;
TCHAR ordermsg[20] = "ORDER";
TCHAR tgitem[10] = "";
TCHAR tgitemcount[10] = "";
TCHAR tgSeatnum[10] = "SEAT";
TCHAR tgnum[10];
TCHAR userid[10];
TCHAR exitmsg[20];
TCHAR pcbuf[10];
TCHAR orderbuf[10];
TCHAR itembuf[10];
TCHAR pctimebuf[2];
TCHAR exitclient[10];
TCHAR tpctime[10];
int pbi = 0;
int orderi = 0;
int itemi = 0;
int chkendbtn = 0;

SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;


DWORD WINAPI ThreadFunc(LPVOID Param) {//����//���ú꽺����ν� ��û�� ����� �ٷ�
	HDC hdc;
	SOCKET* P = (SOCKET*)Param;
	for (;;) {
		lstrcpy(buf, "");
		nReturn = recv(*P, buf, 1024, 0);
		if (nReturn == 0 || nReturn == SOCKET_ERROR) {
			continue;
		}
		else {								
			orderi = 0;
			lstrcpy(orderbuf, "");
			for (int i = 0; i < 2; i++)orderbuf[orderi++] = buf[i];
			orderbuf[orderi] = '\0';
			if (lstrcmp(buf, TEXT("�ڸ���밡��")) == 0)chkseat = 1;					//����//�ڸ�
			else if (lstrcmp(buf, TEXT("ȸ�����Լ���")) == 0) {								//����//ȸ������
				chkjoin = 1;
			}
			else if (lstrcmp(buf, TEXT("�α��μ���")) == 0) {					//����//�α��εǸ� �����ð�ó���ǰ� ���̵�����			//�α��ν��� 1.���̵�/�������,2.���� �ð� 0�̻�
				chktimer = 1;
				lstrcpy(userid, tgId);
				lstrcpy(exitclient, "EXIT");
				lstrcat(exitclient, userid);
				chklogin = 1;
				wsprintf(tpctime, "%d", pctime);
				SetDlgItemText(hWndDlg, IDC_PCTIMEEDIT, tpctime);
			}
			else if (lstrcmp(orderbuf, TEXT("OC")) == 0) {			//����//�ǽû�ǰ����ó��
				if (lstrlen(buf) >= 5) {
					pbi = 0;
					for (int i = 2; i < 4; i++)pcbuf[pbi++] = buf[i];
					pcbuf[pbi] = '\0';
					if (lstrcmp(pcbuf, TEXT("PC")) == 0) {
						pctimebuf[0] = buf[4];
						pctimebuf[1] = '\0';
						pctime = atoi(pctimebuf);
					}
				}
			}
			else if (lstrcmp(buf, "�����⼺��") == 0) {//����//������
				lstrcpy(userid, "");
			}
			else if (lstrcmp(buf, "Ż�𼺰�") == 0) {//����//Ż��
				SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			}
			if (lstrlen(buf) != 0) {//����//��� �޼��� ��Ÿ����
				sprintf_s(strTemp, "������ �޽���:%s", buf);
				hChatEdit = GetDlgItem(hWndDlg, IDC_CHATCLIENTLIST);
				int len = GetWindowTextLength(hChatEdit);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)strTemp);
			}
		}
	}
	return 0;
}
//����//��ȭ���� ��Ʈ�� ����//��� ��û�ٷ��
BOOL CALLBACK ClientDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		hWndDlg = hDlg;
		InitCommonControls();
		SetDlgItemText(hDlg, IDC_IPCLIENTEDIT, tgIp);
		SetDlgItemText(hDlg, IDC_PORTCLIENTEDIT, tgPort);//����//192.168.0.13
		//lstrcpy(tpctime, "");
		SetTimer(hDlg, 1, 1000, NULL);
		SetTimer(hDlg, 2, 1000, NULL);
		return TRUE;
	case WM_TIMER:
		switch (wParam) {
		case 1:												//����//���� �ð� Ÿ�̸�
			if (chktimer == 1) {
				pctime -= 1;
				wsprintf(tpctime, "%d", pctime);
				SetDlgItemText(hDlg, IDC_PCTIMEEDIT, tpctime);
				if (pctime == 0) {
					wsprintf(tpctime, "%d", pctime);
					SetDlgItemText(hDlg, IDC_PCTIMEEDIT, tpctime);
					nReturn = send(clientsock, exitclient, sizeof(exitclient), 0);
					chktimer = 0;
					break;
				}
			}
			break;
		case 2:																//����//�������ư �������� �����ð� Ÿ�̸� ����
			if (chkendbtn == 1)KillTimer(hDlg, 1);
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ENTERCLIENTBUTTON://����//���Ͽ���
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
		case IDC_SENDMSGCLIENTBUTTON:////�޼����� ������ �����ϰ� ����Ʈ�ڽ��� ����(���ÿ� ���� �������� ����)
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
		case IDC_JOINBUTTON://����//ȸ������
			if (chkseat == 1) {
				seljoinorloginordel = 0;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "�ڸ� ���� ���� �ϼ���!!", "����", MB_OK);
			}
			return TRUE;
		case IDC_LOGINBUTTON://����//�α���
			if (chkseat == 1) {
				seljoinorloginordel = 1;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "�ڸ� ���� ���� �ϼ���!!", "����", MB_OK);
			}
			return TRUE;
		case IDC_ORDERBUTTON://����//�ֹ�
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
		case IDC_EXITBUTTON://����//������
			if (chklogin == 1) {
				lstrcpy(exitmsg, "EXIT");
				lstrcat(exitmsg, userid);
				chkendbtn = 1;
				nReturn = send(clientsock, exitmsg, sizeof(exitmsg), 0);
			}
			else MessageBox(hDlg, "�α����ϼ���!!", "����", MB_OK);
			return TRUE;
		case IDC_DELBUTTON://����//Ż��
			if (chklogin == 1) {
				seljoinorloginordel = 2;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else MessageBox(hDlg, "�α����ϼ���!!", "����", MB_OK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			//KillTimer(hWndDlg, 1);
			InvalidateRect(hWndDlg, NULL, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//����//������ �α��ΰ� ȸ������ ��û
BOOL CALLBACK JoinLoginDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_JOINORLOGINBUTTON:		//����//ȸ������:���̵�,�г���,��� �� �Է�/�α���:���̵�,��� �Է�/Ż��:���̵�,��� �Է�//id01/nick01/pw01
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
			lstrcat(tgjoinloginmsg, selnum);					//����//��:0ȸ������,1�α���,2Ż��
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
//����//�ڸ� ����
BOOL CALLBACK SelSeatDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			lstrcpy(tgSeatnum, "SEAT");
			GetDlgItemText(hDlg, IDC_SEATEDIT, tgnum, sizeof(tgnum));
			lstrcat(tgSeatnum, tgnum);
			nReturn = send(clientsock, tgSeatnum, sizeof(tgSeatnum), 0);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			InvalidateRect(hWndDlg, NULL, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//����//�ֹ� ������ ���� �ڸ���
BOOL CALLBACK OrderDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"PC");
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"DRINK");
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"FOOD");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ITEMLIST:
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE:
				i = SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETTEXT, i, (LPARAM)tgitem);
			}
			break;
			return TRUE;
		case IDOK:
			lstrcpy(ordermsg, "ORDER");
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
