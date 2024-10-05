
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
LPCTSTR lpszClass = TEXT("대화상자");

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

////소켓통신 설정위한 변수 선언
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

////스레드 종료 위한 플래그
BOOL g_bThreadExitFlag = FALSE;
volatile BOOL g_bClientThreadExitFlags[10] = { FALSE };

int existsocktrue = 0, existsocktrue2 = 0;
int emptytrue = 0, idres = 0, pwres = 0;

int deltrue = 0;
int csi = 0;
int i, j;
int num;
////전원 버튼 눌렸는지 판별
BOOL serverpower = 0;
BOOL serveropt = 0;
////포트와 IP번호 설정
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
////메세지 길이 설정
const int buflen = 40960;
////메세지
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

////DB연동변수들
SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;


//변수들
TCHAR usertime[100];
TCHAR usernick[100];
TCHAR userpn[100];
TCHAR useraddr[100];
TCHAR userbirth[100];
TCHAR seatmsg[200];
TCHAR ordermsg[200];
TCHAR joinmsg[200];
TCHAR loginmsg[200];
TCHAR logoutmsg[200];
TCHAR updatemsg[200];
TCHAR deletemsg[200];
TCHAR tgSn[20];
TCHAR tgId[50];
TCHAR tgPw[50];
TCHAR tgNick[50];
TCHAR tgPn[50];
TCHAR tgAddr[50];
TCHAR tgBirth[50];
TCHAR info[200];
int turn;
int infoi;
int ini;
TCHAR itemn[10];
TCHAR itemcount[10];
int ici;
int itemni;
int itemcountn;
int itemi = 0;
int exituseri = 0;
TCHAR orderchk[30];
TCHAR seatnum[10];
int seati = 0;
int seatn;

//팀플//요청 판별하고 처리해주기
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
		if (buf[0] == '1') {							//팀플//자리요청
			for (i = 1; i < 3; i++) {
				seatnum[seati++] = buf[i];
			}
			seatnum[seati] = '\0';
			seatn = atoi(seatnum);
			seati = 0;
			//팀플//좌석번호체크
			lstrcpy(seatmsg, "11");//팀플//정상일때
			//lstrcpy(seatmsg, "10");//팀플//아닐때
			nReturn = send(clientsock,seatmsg, sizeof(seatmsg), 0);
		}
		else if (buf[0] == '2') {							//팀플//주문요청
			itemni = 0;
			itemn[itemni++] = buf[1];
			itemn[itemni] = '\0';						//팀플//주문상품받기
			for (i = 2; i < 4; i++) {
				itemcount[ici++] = buf[i];
			}
			itemcount[ici] = '\0';
			itemcountn = atoi(itemcount);			//팀플//주문수량 받기
			for (i = 8; i < 10; i++) {
				seatnum[seati++] = buf[i];
			}
			seatnum[seati] = '\0';						//팀플//자리 받기
			lstrcpy(ordermsg, "2");
			lstrcat(ordermsg, itemn);
			lstrcat(ordermsg, "1");								//팀플// 상품,수량,자리 체크하고 맞으면 성공 메시지 전송
			//lstrcat(ordermsg, "0");//아닐때
			nReturn = send(clientsock,ordermsg, sizeof(ordermsg), 0);
			//좌석번호체크
		}
		else if (buf[0] == '3') {						//팀플//회원가입
			turn = -1;
			i = 1;
			while (buf[i] != '\0') {
				infoi = 0;
				if (buf[i] == ':') {
					ini = i + 1;
					turn++;
				}
				if (turn == 0) {												//팀플//아이디 받기
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 2) infoi -= 2;
					info[infoi] = '\0';
					lstrcpy(tgId, info);
				}
				else if (turn == 1) {									//팀플//비번받기
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 4) infoi -= 4;
					info[infoi] = '\0';
					lstrcpy(tgPw, info);
				}
				else if (turn == 2) {									//팀플//이름 받기
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 2) infoi -= 2;
					info[infoi] = '\0';
					lstrcpy(tgNick, info);
				}
				else if (turn == 3) {									//팀플//전화번호 받기
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 4) infoi -= 4;
					info[infoi] = '\0';
					lstrcpy(tgPn, info);
				}
				else if (turn == 4) {									//팀플//주소 받기
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 5) infoi -= 5;
					info[infoi] = '\0';
					lstrcpy(tgAddr, info);
				}
				else if (turn == 5) {									//팀플//생일 받기
					while (buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					info[infoi] = '\0';
					lstrcpy(tgBirth, info);
					turn = 0;
					break;
				}
				i++;
			}
			lstrcpy(joinmsg, "31");										//팀플//위에 받은 정보들 다 체크하고 이상 없으면 성공 메시지 보내기
			nReturn = send(clientsock, joinmsg, sizeof(joinmsg), 0);		
		}

		else if (buf[0] == '4') {										//팀플//로그인 요청
			i = 1;
			turn = -1;
			while (buf[i] != '\0') { //팀플//전체 CMD
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
					lstrcpy(tgId, info); //팀플//ID꺼내기
				}
				else if (turn == 1) {
					while (buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					info[infoi] = '\0';
					lstrcpy(tgPw, info); //팀플//PW꺼내기
					turn = 0;
					break;
				}
				i++;
			}

			lstrcpy(loginmsg, "41");
			//lstrcpy(loginmsg, "40");//팀플//아닐때
			lstrcpy(usertime, "10");			//팀플//DB에서 아이디와 비번으로 정보 가져오기(남은 시간)
			//lstrcpy(usertime, "00");	//팀플//남은시간 없을때
			lstrcpy(usernick, "nick01");				//팀플//계정 정보들...
			lstrcpy(userpn, "pn01");
			lstrcpy(useraddr, "addr01");
			lstrcpy(userbirth, "birth01");
			lstrcat(loginmsg, usertime);
			lstrcat(loginmsg, "NICK:");
			lstrcat(loginmsg, usernick);
			lstrcat(loginmsg, "PN:");
			lstrcat(loginmsg, userpn);
			lstrcat(loginmsg, "ADDR:");
			lstrcat(loginmsg, useraddr);
			lstrcat(loginmsg, "BIRTH:");
			lstrcat(loginmsg, userbirth);
			nReturn = send(clientsock, loginmsg, sizeof(loginmsg), 0);			//팀플//로그인 하면 계정의 해당 정보들을 클라이언트로 보내주기
		}
		else if (buf[0] == '5') {						//팀플//로그아웃 요청
			i = 1;
			turn = -1;
			while (buf[i] != '\0') { //팀플//전체 CMD
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
					lstrcpy(tgId, info); //팀플//ID꺼내기
				}
				else if (turn == 1) {
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 2) infoi -= 2;
					info[infoi] = '\0';
					lstrcpy(tgPw, info); //팀플//PW꺼내기
				}
				else if (turn == 2) {
					while (buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					info[infoi] = '\0';
					lstrcpy(tgSn, info); //팀플//자리 꺼내기
					turn = 0;
					break;
				}
				i++;
			}

			lstrcpy(logoutmsg, "51");
			//lstrcpy(logoutmsg, "50");		//아닐때
			nReturn = send(clientsock,logoutmsg, sizeof(logoutmsg), 0);			//팀플//아이디,비번,자리 맞으면 로그아웃 성공 메시지 보내기
		}
		else if (buf[0] == '6') {						//팀플//수정 요청
			i = 1;
			turn = -1;
			while (buf[i] != '\0') { //팀플//전체 CMD
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
					lstrcpy(tgId, info); //팀플//ID꺼내기
				}
				else if (turn == 1) {
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 4) infoi -= 4;
					info[infoi] = '\0';
					lstrcpy(tgPw, info); //팀플//PW꺼내기
				}
				else if (turn == 2) {
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 2) infoi -= 2;
					info[infoi] = '\0';
					lstrcpy(tgNick, info); //팀플//NICK꺼내기
				}
				else if (turn == 3) {
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 4) infoi -= 4;
					info[infoi] = '\0';
					lstrcpy(tgPn, info); //팀플//PN꺼내기
				}
				else if (turn == 4) {
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 5) infoi -= 5;
					info[infoi] = '\0';
					lstrcpy(tgAddr, info); //팀플//ADDR꺼내기
				}
				else if (turn == 5) {
					while (buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					info[infoi] = '\0';
					lstrcpy(tgBirth, info); //팀플//BIRTH꺼내기
					turn = 0;
					break;
				}
				i++;
			}

			lstrcpy(updatemsg, "61");
			//lstrcpy(updatemsg, "60");		//아닐떄
			nReturn = send(clientsock, updatemsg, sizeof(updatemsg), 0);			//팀플//계정의 모든 정보 이상 없으면 수정 성공메시지 보내기
		}
		else if (buf[0] == '7') {							//팀플//탈퇴 요청
			i = 1;
			turn = -1;
			while (buf[i] != '\0') { //팀플//전체 CMD
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
					lstrcpy(tgId, info); //팀플//ID꺼내기
				}
				else if (turn == 1) {
					while (buf[ini] != ':' && buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					if (infoi >= 2) infoi -= 2;
					info[infoi] = '\0';
					lstrcpy(tgPw, info); //팀플//PW꺼내기
				}
				else if (turn == 2) {
					while (buf[ini] != '\0') {
						info[infoi++] = buf[ini++];
					}
					info[infoi] = '\0';
					lstrcpy(tgSn, info); //팀플//자리 꺼내기
					turn = 0;
					break;
				}
				i++;
			}

			lstrcpy(deletemsg, "71");
			//lstrcpy(deletemsg, "70");		//아닐때
			nReturn = send(clientsock, deletemsg, sizeof(deletemsg), 0);				//팀플//아이디, 비번, 자리 맞으면 탈퇴 성공 메시지 보내기
		}
			sprintf_s(strTemp, "수신한 메시지:%s : %d", buf, clientsock);	//팀플//수신한 모든 메시지 에디트박스에 띄우기
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
			SendDlgItemMessage(hWndDlg, IDC_CHATSERVERLIST, EM_REPLACESEL, FALSE, (LPARAM)"손님이  입장하였습니다.");
			clientsockar[csi] = clientsock;
			g_bClientThreadExitFlags[csi] = FALSE;
			hClientThread[csi] = CreateThread(NULL, 0, ClientThreadFunc, (LPVOID)csi, 0, &ClientThreadID);
			csi++;
		}
	}
	return 0;
}
////대화상자 컨트롤들의 동작 수행
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
				MessageBox(hDlg, "영업시작", "server", MB_OK);
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
