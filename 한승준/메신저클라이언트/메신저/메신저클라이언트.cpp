

//�Ұ�

#include <Windows.h>
#include<stdio.h>
#include<CommCtrl.h>
#include <sql.h>
#include <sqlext.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK JoinDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SelSeatDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OrderDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hinst;
LPCTSTR lpszClass = TEXT("��ȭ����");
////IP�� ��Ʈ��ȣ ����
const char g_szlpAddress[17] = "127.0.0.1";
const unsigned short g_uPort = 7878;
TCHAR tgIp[17] = "127.0.0.1", tgPort[10] = "7878";
TCHAR tgId[10], tgNick[10], tgPass[10], tgPn[10], tgAddr[10], tgBirth[10];
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
TCHAR tgcmdserver[200];
TCHAR tgjoinloginmsg[100]="ID";
int nReturn;
WSADATA wsadata;
SOCKET clientsock;
sockaddr_in addr_client;
int addrlen_clt = sizeof(sockaddr);
char buf[1024];////�޽���
char strTemp[1024];

//���� //�ֿ亯��
int turn = 0;
int ini;
TCHAR info[200];;
int infoi = 0;
TCHAR usersn[10];
TCHAR userpw[10];
int pctimei;
int ngitemcount;
TCHAR tgselitemi[10];
int selitemi;
int chkseat=0;
int chkjoin = 0;
int chklogin = 0;
int i;
int pctime;
int chkexit;
int userexit;
int chktimer;
int pci = 0;
TCHAR ordermsg[20];
TCHAR tgitem[10] = "";
TCHAR tgitemcount[10] = "";
TCHAR tgSeatnum[10];
TCHAR tgnum[10];
TCHAR userid[10];
TCHAR exitmsg[20];
TCHAR pcbuf[10];
TCHAR orderbuf[10];
TCHAR itembuf[10];
TCHAR pctimebuf[10];
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
		//MessageBox(hWndDlg, buf, "chkbuf", MB_OK);
		if (nReturn == 0 || nReturn == SOCKET_ERROR) {
			continue;
		}
		else {								
			orderi = 0;
			lstrcpy(orderbuf, "");
			for (int i = 0; i < 3; i++) {
				orderbuf[orderi++] = buf[i];
			}
			orderbuf[orderi] = '\0';												//����//�ֹ��� ���
			if (buf[0] == '1') {
				if (lstrcmp(buf, TEXT("11")) == 0) {
					chkseat = 1;																	//����//�ڸ�
					lstrcpy(tgSeatnum, tgnum);
					lstrcpy(usersn, tgSeatnum);
					wsprintf(buf, "%s", "�ڸ���밡��");
				}
				else wsprintf(buf, "%s", "�ڸ����Ұ�");
			}
			else if (buf[0] == '3') {
				 if (lstrcmp(buf, TEXT("31")) == 0) {								//����//ȸ������
					chkjoin = 1;
					wsprintf(buf, "%s", "ȸ�����Լ���");
				}
				 else wsprintf(buf, "%s", "ȸ�����Խ���");
			}
			else if (buf[0] == '4') {
				if (buf[1]=='1') {					//����//�α��εǸ� �����ð�ó���ǰ� ���̵�����			//�α��ν��� 1.���̵�/�������,2.���� �ð� 0�̻�
					pctimei = 0;
					for (int i = 2; i < 4; i++) {
						pctimebuf[pctimei++] = buf[i];	//����//������ ��ȣ�� 1�̾ƴ� 01����
					}
					pctimebuf[pctimei] = '\0';								//����//�����ð����
					pctime = atoi(pctimebuf);
					if (pctime >= 0) {
						lstrcpy(userid, tgId);									//����//���� ���̵�,��� ���
						lstrcpy(userpw, tgPass);
						i = 4;
						turn = -1;
						while (buf[i] != '\0') { //����//��ü CMD
							infoi = 0;
							if (buf[i] == ':') {
								ini = i + 1;
								turn++;
							}
							if (turn == 0) {
								while (buf[ini] != ':' && buf[ini] != '\0') {
									info[infoi++] = buf[ini++];
								}
								if (infoi >= 2) infoi -= 2;
								info[infoi] = '\0';
								lstrcpy(tgNick, info); //����//NICK������
							}
							else if (turn == 1) {
								while (buf[ini] != ':' && buf[ini] != '\0') {
									info[infoi++] = buf[ini++];
								}
								if (infoi >= 4) infoi -= 4;
								info[infoi] = '\0';
								lstrcpy(tgPn, info); //����//PN������
							}
							else if (turn == 2) {
								while (buf[ini] != ':' && buf[ini] != '\0') {
									info[infoi++] = buf[ini++];
								}
								if (infoi >= 5) infoi -= 5;
								info[infoi] = '\0';
								lstrcpy(tgAddr, info); //����//ADDR������
							}
							else if (turn == 3) {
								while (buf[ini] != '\0') {
									info[infoi++] = buf[ini++];
								}
								info[infoi] = '\0';
								lstrcpy(tgBirth, info); //����//BIRTH������
								turn = 0;
								break;
							}
							i++;
						}

						i = 0;
						wsprintf(tpctime, "%d", pctime);
						SetDlgItemText(hWndDlg, IDC_PCTIMEEDIT, tpctime);	//����//ȸ������ â��  ������ ���� ����
						SetDlgItemText(hWndDlg, IDC_IDJOINEDIT, userid);
						SetDlgItemText(hWndDlg, IDC_PASSJOINEDIT, userpw);
						SetDlgItemText(hWndDlg, IDC_NICKEDIT, tgNick);
						SetDlgItemText(hWndDlg, IDC_PNEDIT, tgPn);
						SetDlgItemText(hWndDlg, IDC_ADDREDIT, tgAddr);
						SetDlgItemText(hWndDlg, IDC_BIRTHEDIT, tgBirth);
						if (pctime > 0) {			//����//�α��ΰ� Ÿ�̸� ���� ����
							chklogin = 1;
							chktimer = 1;
							wsprintf(buf, "%s", "�α��μ���");
						}
						else wsprintf(buf, "%s", "�α��ν���(�ܿ��ð�����)");
					}
				}
				else wsprintf(buf, "%s", "�α��ν���");					//����//cmd�� 40���� ������ ����
			}
			else if (buf[0] == '2') {
				if (buf[1] == '1') {
					if (lstrcmp(orderbuf, TEXT("211")) == 0) {			//����//�ǽû�ǰ����ó��
						wsprintf(buf, "%s", "DRINK���Լ���");
					}
					else wsprintf(buf, "%s", "DRINK���Խ���");
				}
				else if (buf[1] == '2') {
					if (lstrcmp(orderbuf, TEXT("221")) == 0) {			//����//��ǰ����ó��
						wsprintf(buf, "%s", "FOOD���Լ���");
					}
					else wsprintf(buf, "%s", "FOOD���Խ���");
				}
				else if (buf[1] == '3') {
					if (lstrcmp(orderbuf, TEXT("231")) == 0) {			//����//�����ǰ����ó��
						wsprintf(buf, "%s", "PC���Լ���");
					}
					else wsprintf(buf, "%s", "PC���Խ���");
				}
			}
			else if (buf[0] == '5') {														//����//�α���
				if (lstrcmp(buf, TEXT("51")) == 0) {
					lstrcpy(userid, "");
					wsprintf(buf, "%s", "�α׾ƿ�����");
					chklogin = 0;
				}
				else 	wsprintf(buf, "%s", "�α׾ƿ�����");
			}
			else if (buf[0] == '6') {													//����//����
				if (lstrcmp(buf, TEXT("61")) == 0) {
					wsprintf(buf, "%s", "��������");
				}
				else wsprintf(buf, "%s", "��������");
			}
			else if (buf[0] == '7') {													//����//Ż��
				if (lstrcmp(buf, TEXT("71")) == 0) {
					wsprintf(buf, "%s", "Ż�𼺰�");
					SendMessage(hWndDlg, WM_CLOSE, 0, 0);
				}
				else wsprintf(buf, "%s", "Ż�����");
			}
			if (lstrlen(buf) != 0) {//����//��� �޼��� ��Ÿ����
				sprintf_s(strTemp, "������ �޽���:%s", buf);								//����//������ cmd�� ��ȯ�� �޼����� ����Ʈ�ڽ��� ����
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
				if (pctime == 0) {																				//����//�ð� 0�϶� �α׾ƿ�ó��
					wsprintf(tpctime, "%d", pctime);
					SetDlgItemText(hDlg, IDC_PCTIMEEDIT, tpctime);
					lstrcpy(tgcmdserver, "");
					lstrcpy(tgcmdserver, "5ID:");
					lstrcat(tgcmdserver, userid);
					lstrcat(tgcmdserver, "PW:");
					lstrcat(tgcmdserver, userpw);
					lstrcat(tgcmdserver, "SN:");
					lstrcat(tgcmdserver, usersn);
					chkendbtn = 1;
					chklogin = 0;
					nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
					chktimer = 0;
					break;
				}
			}
			break;
		case 2:																//����//�������ư ��������/�����ð� 0�϶� �����ð� Ÿ�̸� �����ϰ� �ٽ� Ÿ�̸� �����(�α����� ��� �ޱ� ����)
			if (chkendbtn == 1) {
				KillTimer(hDlg, 1);
				SetTimer(hDlg, 1, 1000, NULL);
				chkendbtn = 0;
			}
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
	
		case IDC_GOJOINBUTTON://����//ȸ������ �� �ڸ��������
			if (chkseat == 1) {
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINDIALOG), HWND_DESKTOP, (DLGPROC)JoinDlgProc);
			}
			else {
				MessageBox(hDlg, "�ڸ� ���� ���� �ϼ���!!", "����", MB_OK);
			}
			return TRUE;
		case IDC_GOLOGINBUTTON://����//�α��� �� �ڸ��� �α��� üũ�ϱ�
			if (chkseat == 1 && chklogin==0) {
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_LOGINDIALOG), HWND_DESKTOP, (DLGPROC)LoginDlgProc);
			}
			else {
				if(chkseat==0)MessageBox(hDlg, "�ڸ� ���� ���� �ϼ���!!", "����", MB_OK);
				if(chklogin==1)MessageBox(hDlg, "�α��� �Ǿ� �ֽ��ϴ�!!", "����", MB_OK);
			}
			return TRUE;
		case IDC_GOORDERBUTTON://����//�ֹ� �� �ڸ� ���
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
		case IDC_EXITBUTTON://����//������ �� �α��� üũ�ϱ�
			if (chklogin == 1) {
				lstrcpy(tgcmdserver, "");
				lstrcpy(tgcmdserver, "5ID:");
				lstrcat(tgcmdserver, userid);
				lstrcat(tgcmdserver, "PW:");
				lstrcat(tgcmdserver, userpw);
				lstrcat(tgcmdserver, "SN:");
				lstrcat(tgcmdserver, usersn);
				chkendbtn = 1;
				nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
			}
			else MessageBox(hDlg, "�α����ϼ���!!", "����", MB_OK);
			return TRUE;
		case IDC_DELBUTTON://����//Ż�� �� �α��� üũ�ϱ�
			if (chklogin == 1) {
				lstrcpy(tgcmdserver, "");
				lstrcpy(tgcmdserver, "7ID:");
				lstrcat(tgcmdserver, userid);
				lstrcat(tgcmdserver, "PW:");
				lstrcat(tgcmdserver, userpw);
				lstrcat(tgcmdserver, "SN:");
				lstrcat(tgcmdserver, usersn);
				chkendbtn = 1;
				nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
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
BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_LOGINBUTTON:		//����//�α���:���̵�,��� �Է�(�� �Է� ���������� ':'�� �Է� �� �ް� �ϱ�)
			lstrcpy(tgcmdserver, "");
			lstrcpy(tgId, "");
			lstrcpy(tgPass, "");
			GetDlgItemText(hDlg, IDC_IDLOGINEDIT, tgId, sizeof(tgId));
			GetDlgItemText(hDlg, IDC_PASSLOGINEDIT, tgPass, sizeof(tgPass));
			lstrcpy(tgcmdserver, "4ID:");
			lstrcat(tgcmdserver, tgId);
			lstrcat(tgcmdserver, "PW:");
			lstrcat(tgcmdserver, tgPass);
			nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
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
//����//������ �α��ΰ� ȸ������ ��û
BOOL CALLBACK JoinDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		SetDlgItemText(hWndDlg, IDC_PCTIMEEDIT, tpctime);		//����//�α����Ҷ� �ش� ���� ����
		SetDlgItemText(hWndDlg, IDC_IDJOINEDIT, userid);
		SetDlgItemText(hWndDlg, IDC_PASSJOINEDIT, userpw);
		SetDlgItemText(hWndDlg, IDC_NICKEDIT, tgNick);
		SetDlgItemText(hWndDlg, IDC_PNEDIT, tgPn);
		SetDlgItemText(hWndDlg, IDC_ADDREDIT, tgAddr);
		SetDlgItemText(hWndDlg, IDC_BIRTHEDIT, tgBirth);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_JOINBUTTON:		//����//ȸ������:���̵�,�г���,���,��ȭ��ȣ,�ּ�,���� �� �Է�/(�� �Է� ���������� ':'�� �Է� �� �ް� �ϱ�)
			lstrcpy(tgcmdserver, "");
			lstrcpy(tgId, "");
			lstrcpy(tgNick, "");
			lstrcpy(tgPass, "");
			lstrcpy(tgPn, "");
			lstrcpy(tgAddr, "");
			lstrcpy(tgBirth, "");
			GetDlgItemText(hDlg, IDC_IDJOINEDIT, tgId, sizeof(tgId));
			GetDlgItemText(hDlg, IDC_NICKEDIT, tgNick, sizeof(tgNick));
			GetDlgItemText(hDlg, IDC_PASSJOINEDIT, tgPass, sizeof(tgPass));
			GetDlgItemText(hDlg, IDC_PNEDIT, tgPn, sizeof(tgPn));
			GetDlgItemText(hDlg, IDC_ADDREDIT, tgAddr, sizeof(tgAddr));
			GetDlgItemText(hDlg, IDC_BIRTHEDIT, tgBirth, sizeof(tgBirth));
			lstrcpy(tgcmdserver, "3ID:");
			lstrcat(tgcmdserver, tgId);
			lstrcat(tgcmdserver, "PW:");
			lstrcat(tgcmdserver, tgPass);
			lstrcat(tgcmdserver, "NICK:");
			lstrcat(tgcmdserver, tgNick);
			lstrcat(tgcmdserver, "PN:");
			lstrcat(tgcmdserver, tgPn);
			lstrcat(tgcmdserver, "ADDR:");
			lstrcat(tgcmdserver, tgAddr);
			lstrcat(tgcmdserver, "BIRTH:");
			lstrcat(tgcmdserver, tgBirth);
			nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
			break;
			return TRUE;
		case IDC_UPDATEBUTTON:		//����//����:���̵�,�г���,���,��ȭ��ȣ,�ּ�,���� �� �Է�/(�� �Է� ���������� ':'�� �Է� �� �ް� �ϱ�)(�α��� ������ �����ϰ� �ϱ�(���ã�� ���))
			lstrcpy(tgcmdserver, "");
			lstrcpy(tgId, "");
			lstrcpy(tgNick, "");
			lstrcpy(tgPass, "");
			lstrcpy(tgPn, "");
			lstrcpy(tgAddr, "");
			lstrcpy(tgBirth, "");
			GetDlgItemText(hDlg, IDC_IDJOINEDIT, tgId, sizeof(tgId));
			GetDlgItemText(hDlg, IDC_NICKEDIT, tgNick, sizeof(tgNick));
			GetDlgItemText(hDlg, IDC_PASSJOINEDIT, tgPass, sizeof(tgPass));
			GetDlgItemText(hDlg, IDC_PNEDIT, tgPn, sizeof(tgPn));
			GetDlgItemText(hDlg, IDC_ADDREDIT, tgAddr, sizeof(tgAddr));
			GetDlgItemText(hDlg, IDC_BIRTHEDIT, tgBirth, sizeof(tgBirth));
			lstrcpy(tgcmdserver, "6ID:");
			lstrcat(tgcmdserver, tgId);
			lstrcat(tgcmdserver, "PW:");
			lstrcat(tgcmdserver, tgPass);
			lstrcat(tgcmdserver, "NICK:");
			lstrcat(tgcmdserver, tgNick);
			lstrcat(tgcmdserver, "PN:");
			lstrcat(tgcmdserver, tgPn);
			lstrcat(tgcmdserver, "ADDR:");
			lstrcat(tgcmdserver, tgAddr);
			lstrcat(tgcmdserver, "BIRTH:");
			lstrcat(tgcmdserver, tgBirth);
			nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
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
			lstrcpy(tgcmdserver, "1");
			GetDlgItemText(hDlg, IDC_SEATEDIT, tgnum, sizeof(tgnum));						//����//�ڸ� ����(02�� 2�ڸ� ��������)
			lstrcat(tgcmdserver, tgnum);
			nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
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
//����//�ֹ�
BOOL CALLBACK OrderDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"DRINK");
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"FOOD");
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"PC");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ITEMLIST:
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE:
				selitemi = SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETCURSEL, 0, 0);				//����//�ֹ� ��ǰ ����(����Ʈ�ڽ��� �ε����� 1�ڸ� ��������)
				selitemi += 1;
				wsprintf(tgselitemi, "%d", selitemi);
				//SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETTEXT, i, (LPARAM)tgitem);
			}
			break;
			return TRUE;
		case IDOK:																							//����//�ֹ� ���� ����(02�� 2�ڸ� ��������)
			lstrcpy(tgcmdserver, "2");
			lstrcat(tgcmdserver, tgselitemi);
			GetDlgItemText(hDlg, IDC_COUNTEDIT, tgitemcount, sizeof(tgitemcount));
			lstrcat(tgcmdserver, tgitemcount);
			lstrcat(tgcmdserver, "SN:");
			lstrcat(tgcmdserver, tgSeatnum);
			nReturn = send(clientsock, tgcmdserver, sizeof(tgcmdserver), 0);
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
