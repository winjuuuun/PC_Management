

//할것

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
LPCTSTR lpszClass = TEXT("대화상자");
////IP와 포트번호 설정
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
char buf[1024];////메시지
char strTemp[1024];

//팀플 //주요변수
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


DWORD WINAPI ThreadFunc(LPVOID Param) {//팀플//리시브스레드로써 요청의 결과를 다룸
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
			orderbuf[orderi] = '\0';												//팀플//주문을 담기
			if (buf[0] == '1') {
				if (lstrcmp(buf, TEXT("11")) == 0) {
					chkseat = 1;																	//팀플//자리
					lstrcpy(tgSeatnum, tgnum);
					lstrcpy(usersn, tgSeatnum);
					wsprintf(buf, "%s", "자리사용가능");
				}
				else wsprintf(buf, "%s", "자리사용불가");
			}
			else if (buf[0] == '3') {
				 if (lstrcmp(buf, TEXT("31")) == 0) {								//팀플//회원가입
					chkjoin = 1;
					wsprintf(buf, "%s", "회원가입성공");
				}
				 else wsprintf(buf, "%s", "회원가입실패");
			}
			else if (buf[0] == '4') {
				if (buf[1]=='1') {					//팀플//로그인되면 남은시간처리되고 아이디저장			//로그인실패 1.아이디/비번오류,2.남은 시간 0이상
					pctimei = 0;
					for (int i = 2; i < 4; i++) {
						pctimebuf[pctimei++] = buf[i];	//팀플//갯수나 번호는 1이아닌 01형식
					}
					pctimebuf[pctimei] = '\0';								//팀플//남은시간담기
					pctime = atoi(pctimebuf);
					if (pctime >= 0) {
						lstrcpy(userid, tgId);									//팀플//계정 아이디,비번 담기
						lstrcpy(userpw, tgPass);
						i = 4;
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
								lstrcpy(tgNick, info); //팀플//NICK꺼내기
							}
							else if (turn == 1) {
								while (buf[ini] != ':' && buf[ini] != '\0') {
									info[infoi++] = buf[ini++];
								}
								if (infoi >= 4) infoi -= 4;
								info[infoi] = '\0';
								lstrcpy(tgPn, info); //팀플//PN꺼내기
							}
							else if (turn == 2) {
								while (buf[ini] != ':' && buf[ini] != '\0') {
									info[infoi++] = buf[ini++];
								}
								if (infoi >= 5) infoi -= 5;
								info[infoi] = '\0';
								lstrcpy(tgAddr, info); //팀플//ADDR꺼내기
							}
							else if (turn == 3) {
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

						i = 0;
						wsprintf(tpctime, "%d", pctime);
						SetDlgItemText(hWndDlg, IDC_PCTIMEEDIT, tpctime);	//팀플//회원가입 창에  계정의 정보 띄우기
						SetDlgItemText(hWndDlg, IDC_IDJOINEDIT, userid);
						SetDlgItemText(hWndDlg, IDC_PASSJOINEDIT, userpw);
						SetDlgItemText(hWndDlg, IDC_NICKEDIT, tgNick);
						SetDlgItemText(hWndDlg, IDC_PNEDIT, tgPn);
						SetDlgItemText(hWndDlg, IDC_ADDREDIT, tgAddr);
						SetDlgItemText(hWndDlg, IDC_BIRTHEDIT, tgBirth);
						if (pctime > 0) {			//팀플//로그인과 타이머 동작 시작
							chklogin = 1;
							chktimer = 1;
							wsprintf(buf, "%s", "로그인성공");
						}
						else wsprintf(buf, "%s", "로그인실패(잔여시간없음)");
					}
				}
				else wsprintf(buf, "%s", "로그인실패");					//팀플//cmd를 40으로 받으면 실패
			}
			else if (buf[0] == '2') {
				if (buf[1] == '1') {
					if (lstrcmp(orderbuf, TEXT("211")) == 0) {			//팀플//피시상품구매처리
						wsprintf(buf, "%s", "DRINK구입성공");
					}
					else wsprintf(buf, "%s", "DRINK구입실패");
				}
				else if (buf[1] == '2') {
					if (lstrcmp(orderbuf, TEXT("221")) == 0) {			//팀플//식품구매처리
						wsprintf(buf, "%s", "FOOD구입성공");
					}
					else wsprintf(buf, "%s", "FOOD구입실패");
				}
				else if (buf[1] == '3') {
					if (lstrcmp(orderbuf, TEXT("231")) == 0) {			//팀플//음료상품구매처리
						wsprintf(buf, "%s", "PC구입성공");
					}
					else wsprintf(buf, "%s", "PC구입실패");
				}
			}
			else if (buf[0] == '5') {														//팀플//로그인
				if (lstrcmp(buf, TEXT("51")) == 0) {
					lstrcpy(userid, "");
					wsprintf(buf, "%s", "로그아웃성공");
					chklogin = 0;
				}
				else 	wsprintf(buf, "%s", "로그아웃실패");
			}
			else if (buf[0] == '6') {													//팀플//수정
				if (lstrcmp(buf, TEXT("61")) == 0) {
					wsprintf(buf, "%s", "수정성공");
				}
				else wsprintf(buf, "%s", "수정실패");
			}
			else if (buf[0] == '7') {													//팀플//탈퇴
				if (lstrcmp(buf, TEXT("71")) == 0) {
					wsprintf(buf, "%s", "탈퇴성공");
					SendMessage(hWndDlg, WM_CLOSE, 0, 0);
				}
				else wsprintf(buf, "%s", "탈퇴실패");
			}
			if (lstrlen(buf) != 0) {//팀플//모든 메세지 나타내기
				sprintf_s(strTemp, "수신한 메시지:%s", buf);								//팀플//위에서 cmd를 변환한 메세지를 리스트박스에 띄우기
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
				if (pctime == 0) {																				//팀플//시간 0일때 로그아웃처리
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
		case 2:																//팀플//나가기버튼 눌렸을때/남은시간 0일때 남은시간 타이머 종료하고 다시 타이머 만들기(로그인을 계속 받기 위해)
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
	
		case IDC_GOJOINBUTTON://팀플//회원가입 전 자리먼저잡기
			if (chkseat == 1) {
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINDIALOG), HWND_DESKTOP, (DLGPROC)JoinDlgProc);
			}
			else {
				MessageBox(hDlg, "자리 선택 먼저 하세요!!", "오류", MB_OK);
			}
			return TRUE;
		case IDC_GOLOGINBUTTON://팀플//로그인 전 자리와 로그인 체크하기
			if (chkseat == 1 && chklogin==0) {
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_LOGINDIALOG), HWND_DESKTOP, (DLGPROC)LoginDlgProc);
			}
			else {
				if(chkseat==0)MessageBox(hDlg, "자리 선택 먼저 하세요!!", "오류", MB_OK);
				if(chklogin==1)MessageBox(hDlg, "로그인 되어 있습니다!!", "오류", MB_OK);
			}
			return TRUE;
		case IDC_GOORDERBUTTON://팀플//주문 전 자리 잡기
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
		case IDC_EXITBUTTON://팀플//나가기 전 로그인 체크하기
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
			else MessageBox(hDlg, "로그인하세요!!", "오류", MB_OK);
			return TRUE;
		case IDC_DELBUTTON://팀플//탈퇴 전 로그인 체크하기
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
BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_LOGINBUTTON:		//팀플//로그인:아이디,비번 입력(다 입력 가능하지만 ':'은 입력 못 받게 하기)
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
//팀플//서버에 로그인과 회원가입 요청
BOOL CALLBACK JoinDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		//DBConnect();
		InitCommonControls();
		SetDlgItemText(hWndDlg, IDC_PCTIMEEDIT, tpctime);		//팀플//로그인할때 해당 정보 띄우기
		SetDlgItemText(hWndDlg, IDC_IDJOINEDIT, userid);
		SetDlgItemText(hWndDlg, IDC_PASSJOINEDIT, userpw);
		SetDlgItemText(hWndDlg, IDC_NICKEDIT, tgNick);
		SetDlgItemText(hWndDlg, IDC_PNEDIT, tgPn);
		SetDlgItemText(hWndDlg, IDC_ADDREDIT, tgAddr);
		SetDlgItemText(hWndDlg, IDC_BIRTHEDIT, tgBirth);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_JOINBUTTON:		//팀플//회원가입:아이디,닉네임,비번,전화번호,주소,생일 다 입력/(다 입력 가능하지만 ':'은 입력 못 받게 하기)
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
		case IDC_UPDATEBUTTON:		//팀플//수정:아이디,닉네임,비번,전화번호,주소,생일 다 입력/(다 입력 가능하지만 ':'은 입력 못 받게 하기)(로그인 전에도 가능하게 하기(비번찾기 기능))
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
			lstrcpy(tgcmdserver, "1");
			GetDlgItemText(hDlg, IDC_SEATEDIT, tgnum, sizeof(tgnum));						//팀플//자리 선택(02등 2자리 형식으로)
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
//팀플//주문
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
				selitemi = SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETCURSEL, 0, 0);				//팀플//주문 상품 선택(리스트박스의 인덱스로 1자리 형식으로)
				selitemi += 1;
				wsprintf(tgselitemi, "%d", selitemi);
				//SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_GETTEXT, i, (LPARAM)tgitem);
			}
			break;
			return TRUE;
		case IDOK:																							//팀플//주문 수량 선택(02등 2자리 형식으로)
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
