

//할것

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
LPCTSTR lpszClass = TEXT("대화상자");
////IP와 포트번호 설정
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
char buf[1024];////메시지
char strTemp[1024];

//팀플 //주요변수
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


DWORD WINAPI ThreadFunc(LPVOID Param) {//팀플//리시브스레드로써 요청의 결과를 다룸
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
			if (lstrcmp(buf, TEXT("자리사용가능")) == 0)chkseat = 1;					//팀플//자리
			else if (lstrcmp(buf, TEXT("회원가입성공")) == 0) {								//팀플//회원가입
				chkjoin = 1;
			}
			else if (lstrcmp(buf, TEXT("로그인성공")) == 0) {					//팀플//로그인되면 남은시간처리되고 아이디저장			//로그인실패 1.아이디/비번오류,2.남은 시간 0이상
				chktimer = 1;
				lstrcpy(userid, tgId);
				lstrcpy(exitclient, "EXIT");
				lstrcat(exitclient, userid);
				chklogin = 1;
				wsprintf(tpctime, "%d", pctime);
				SetDlgItemText(hWndDlg, IDC_PCTIMEEDIT, tpctime);
			}
			else if (lstrcmp(orderbuf, TEXT("OC")) == 0) {			//팀플//피시상품구매처리
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
			else if (lstrcmp(buf, "나가기성공") == 0) {//팀플//나가기
				lstrcpy(userid, "");
			}
			else if (lstrcmp(buf, "탈퇴성공") == 0) {//팀플//탈퇴
				SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			}
			if (lstrlen(buf) != 0) {//팀플//모든 메세지 나타내기
				sprintf_s(strTemp, "수신한 메시지:%s", buf);
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
//팀플//대화상자 컨트롤 동작//모든 요청다루기
BOOL CALLBACK ClientDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		hWndDlg = hDlg;
		InitCommonControls();
		SetDlgItemText(hDlg, IDC_IPCLIENTEDIT, tgIp);
		SetDlgItemText(hDlg, IDC_PORTCLIENTEDIT, tgPort);//팀플//192.168.0.13
		//lstrcpy(tpctime, "");
		SetTimer(hDlg, 1, 1000, NULL);
		SetTimer(hDlg, 2, 1000, NULL);
		return TRUE;
	case WM_TIMER:
		switch (wParam) {
		case 1:												//팀플//남은 시간 타이머
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
		case 2:																//팀플//나가기버튼 눌렸을때 남은시간 타이머 종료
			if (chkendbtn == 1)KillTimer(hDlg, 1);
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ENTERCLIENTBUTTON://팀플//소켓연결
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
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"입장하였습니다.");
			}
			break;
			return TRUE;
		case IDC_SENDMSGCLIENTBUTTON:////메세지를 서버로 전송하고 에디트박스에 띄우기(팀플에 아직 구현하지 않음)
			if (chklogin == 1) {
				GetDlgItemText(hDlg, IDC_MSGCLIENTEDIT, tgmessage, sizeof(tgmessage));
				nReturn = send(clientsock, tgmessage, sizeof(tgmessage), 0);
				if (nReturn) {
					sprintf_s(buf, "송신한 메시지:%s", tgmessage);
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, LB_ADDSTRING, 0, (LPARAM)buf);
					hChatEdit = GetDlgItem(hWndDlg, IDC_CHATCLIENTLIST);
					int len = GetWindowTextLength(hChatEdit);
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
					SendDlgItemMessage(hDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)buf);
				}
			}
			else {
				MessageBox(hDlg, "로그인을 먼저 하세요!!", "오류", MB_OK);
			}
			break;
			return TRUE;
		case IDC_JOINBUTTON://팀플//회원가입
			if (chkseat == 1) {
				seljoinorloginordel = 0;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "자리 선택 먼저 하세요!!", "오류", MB_OK);
			}
			return TRUE;
		case IDC_LOGINBUTTON://팀플//로그인
			if (chkseat == 1) {
				seljoinorloginordel = 1;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "자리 선택 먼저 하세요!!", "오류", MB_OK);
			}
			return TRUE;
		case IDC_ORDERBUTTON://팀플//주문
			if (chkseat == 1) {
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_ORDERDIALOG), HWND_DESKTOP, (DLGPROC)OrderDlgProc);
			}
			else {
				MessageBox(hDlg, "자리 선택 먼저 하세요!!", "오류", MB_OK);
			}
			return TRUE;
		case IDC_SEATBUTTON:
			DialogBox(g_hinst, MAKEINTRESOURCE(IDD_SELSEATDIALOG), HWND_DESKTOP, (DLGPROC)SelSeatDlgProc);
			return TRUE;
		case IDC_EXITBUTTON://팀플//나가기
			if (chklogin == 1) {
				lstrcpy(exitmsg, "EXIT");
				lstrcat(exitmsg, userid);
				chkendbtn = 1;
				nReturn = send(clientsock, exitmsg, sizeof(exitmsg), 0);
			}
			else MessageBox(hDlg, "로그인하세요!!", "오류", MB_OK);
			return TRUE;
		case IDC_DELBUTTON://팀플//탈퇴
			if (chklogin == 1) {
				seljoinorloginordel = 2;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else MessageBox(hDlg, "로그인하세요!!", "오류", MB_OK);
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
//팀플//서버에 로그인과 회원가입 요청
BOOL CALLBACK JoinLoginDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_JOINORLOGINBUTTON:		//팀플//회원가입:아이디,닉네임,비번 다 입력/로그인:아이디,비번 입력/탈퇴:아이디,비번 입력//id01/nick01/pw01
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
			lstrcat(tgjoinloginmsg, selnum);					//팀플//값:0회원가입,1로그인,2탈퇴
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
//팀플//자리 선택
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
//팀플//주문 수량은 일의 자리로
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
