////////////////////////////////////////WINAPI지식/////////////////////////////////////////
//콤보박스나 리스트박스에선 lParam(마우스):마우스로 동작하기 때문에
//아닌 모든 컨트롤에서는 wParam(키보드)

//WM_COMMAND안에서 wParam을 LOWORD로 읽으면 식별 HIWORD로 읽으면 이벤트
//스크롤에서는 lParam을 사용 다른 것들은 wParam

//값을 한번만 쓸 경우는 static걸지 말기(값이 0으로 초기화되어 바로 사용 되어 있음)
//WM_TIMER:하나의 변수로 동시에 다른 작업을 할 때
//WM_NOTIFY:트리뷰나 리스트뷰에서 값이 바꼈는지 감지

//CreateWindow(컨트롤형식,위치,부모창,연결할리소스,g_hinst,NULL);
//리소스 연결할때(리소스를 핸들에 저장)->GetDlgitem(부모창,리소스식별자)
// 
/////////////////////////////////변수선언양식////////////////////////////
////chk로 시작하는 변수:값을 체크하기위한 변수
////tg로 시작하는 변수:에디트 박스에 띄우기 위한 변수
// 
////////////////////////////////주석 양식///////////////////////////////////
//:개발 수행 중 주석
////:설명하기 위한 주석
///////////////////////////////////////////////////////////개요////////////////////////////////////////////////////////////
//간단한 PC통신을 구현(로컬호스트)
// 특징
//-WINAPI로 가독성 향상
//-대화상자기반
//-P2P통신 아니므로 서버,클라이언트 나눠 구현
//-회원간 채팅은 해당 방 회원들에게 서버가 중계
//-멀티스레드방식(클라이언트로부터 어셉트하는 스레드,각 클라이언트로부터 리시브하는 스레드) (하나의 어셉트스레드,각 리시브스레드)
//-클라이언트 로직(회원가입,로그인,입장,채팅,탈퇴등을 요청)
//-서버로직(...등을 응답)
//-DB에 회원 정보를 저장

//할것
//로그아웃처리(현재 남은시간을 디비에 저장하기)
//탈퇴하기

//타이머로 꺼지도록 하기
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
char buf[1024];////메시지
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
////DB연결함수
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
////DB해제함수
void DBDisConnect() {
	if (hStmt)SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if (hDbc)SQLDisconnect(hDbc);
	if (hDbc)SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if (hEnv)SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}
////전체회원을 다룰 구조체
typedef struct CLIENT {
	TCHAR id[10];
	TCHAR nick[10];
	TCHAR pw[10];
	int pctime;
};
CLIENT client;
////회원삭제함수
void deleteclient() {
	SQLCHAR SQL[1000];
	SQLRETURN ret;
	wsprintf((LPTSTR)SQL, "delete from pcclient where id ='%s'",client.id);
	ret = SQLExecDirect(hStmt, SQL, SQL_NTS);
	SQLFetch(hStmt);
	if (hStmt)SQLCloseCursor(hStmt);
}
////회원가입함수
void addclient() {
	SQLCHAR SQL[1000];
	SQLRETURN ret;
	wsprintf((LPTSTR)SQL, "insert into pcclient (id, nick, pw,time) VALUES('%s','%s','%s',%d)",client.id,client.nick,client.pw,client.pctime);
	ret = SQLExecDirect(hStmt, SQL, SQL_NTS);
	SQLFetch(hStmt);
	if (hStmt)SQLCloseCursor(hStmt);
}
////회원 모든 칼럼 조회
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
////서버로부터 리시브 하기 위한 스레드이고 회원가입,로그인 성공하면 에디트박스에 "닉네임] "이 뜨게 설정,수신한 메세지를 에디트박스에 띄우고,탈퇴성공하면 창 닫기
DWORD WINAPI ThreadFunc(LPVOID Param) {
	HDC hdc;
	SOCKET* P = (SOCKET*)Param;
	for (;;) {
		nReturn = recv(*P, buf, 1024, 0);
		if (nReturn == 0 || nReturn == SOCKET_ERROR) {
			continue;
		}
		else {
			//MessageBox(hWndDlg, buf, "받은 메세지", MB_OK);
			if (lstrcmp(buf, TEXT("자리사용가능")) == 0)chkseat = 1;				//자리사용불가
			else if (lstrcmp(buf, TEXT("회원가입성공")) == 0) {//회원가입하고 디비에 저장시키기			//회원가입실패
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
			else if (lstrcmp(buf, TEXT("로그인성공")) == 0) {					//로그인실패 1.아이디/비번오류,2.남은 시간 0이상
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
			else if (lstrcmp(buf, TEXT("PC주문성공")) == 0) {			//주문실패
			}
			else if (lstrcmp(buf, "나가기성공") == 0)SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			else if (lstrcmp(buf, "탈퇴성공") == 0) {
				deleteclient();
				SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			}
			if (lstrlen(buf) != 0) {
				sprintf_s(strTemp, "수신한 메시지:%s", buf);
				hChatEdit = GetDlgItem(hWndDlg, IDC_CHATCLIENTLIST);
				int len = GetWindowTextLength(hChatEdit);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_SETSEL, (WPARAM)len, (LPARAM)len);
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
				SendDlgItemMessage(hWndDlg, IDC_CHATCLIENTLIST, EM_REPLACESEL, FALSE, (LPARAM)strTemp);
				if (lstrcmp(strTemp, "수신한 메시지:탈퇴성공") == 0)SendMessage(hWndDlg, WM_CLOSE, 0, 0);
			}
		}
	}
	return 0;
}
////대화상자 컨트롤 동작
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
		case IDC_ENTERCLIENTBUTTON:////소켓통신 설정후 에디트박스에 띄우기
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
		case IDC_SENDMSGCLIENTBUTTON:////메세지를 서버로 전송하고 에디트박스에 띄우기
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
		case IDC_JOINBUTTON:////토글 방식으로 로그인/재가입 수행하는 대화상자 띄우기
			if (chkseat == 1) {
				seljoinorloginordel = 0;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "자리 선택 먼저 하세요!!", "오류", MB_OK);
			}
			return TRUE;
		case IDC_LOGINBUTTON:////토글 방식으로 로그인/재가입 수행하는 대화상자 띄우기
			if (chkseat == 1) {
				seljoinorloginordel = 1;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else {
				MessageBox(hDlg, "자리 선택 먼저 하세요!!", "오류", MB_OK);
			}
			return TRUE;
		case IDC_ORDERBUTTON:
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
		case IDC_EXITBUTTON:
			if (chklogin == 1) {
				lstrcpy(exitmsg, "EXIT");
				lstrcat(exitmsg, userid);
				userexit = 1;
				UpdateTime();
				nReturn = send(clientsock, exitmsg, sizeof(exitmsg), 0);
			}
			else MessageBox(hDlg, "로그인하세요!!", "오류", MB_OK);
			return TRUE;
		case IDC_DELBUTTON:
			if (chklogin == 1) {
				seljoinorloginordel = 2;
				DialogBox(g_hinst, MAKEINTRESOURCE(IDD_JOINLOGINDIALOG), HWND_DESKTOP, (DLGPROC)JoinLoginDlgProc);
			}
			else MessageBox(hDlg, "로그인하세요!!", "오류", MB_OK);
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
////서버에 로그인과 회원가입 요청
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
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"식사");
		SendDlgItemMessage(hDlg, IDC_ITEMLIST, LB_ADDSTRING, 0, (LPARAM)"음료");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ITEMLIST:////첫번째방,두번째방에 접속한 회원 리스트박스에 띄우기
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
	그리기(hMemDC영역에)
SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);

WndProc함수
case WM_PAINT:
	if (hBit)DrawBitmap(hdc, 0, 0, hBit);
*/
//중요!!!!!!!!!!!!!!!!!!!!!
//더블버퍼링 처리 방법(캡쳐 후 그리기)
//더블버퍼링처리를 함수화해서 타이머에서 호출하고 함수안에서 InvalidateRect 하기

//더블버퍼링할때 함수에서 MemDC에다가 그릴내용 넣어주기

//더블버퍼링할때 함수에다가 InvalidateRect를 FALSE로 주기->그래야 기존잔상이 안남는다
//페인트에서는 캡쳐한영역(핸들) 그려주기

//SQLBindCol:변수에다 DB속성을 연결(바인딩)
//SQLExecDirect:SQL실행
//SQLFetch:반복문을 돌며 레코드가 끝날때까지 바인딩된 변수에 값이 하나씩 들어옴

//SendMessage: 컨트롤에게 데이터를 전달
//SendMessage(hList, LB_ADDSTRING,0,(LPARAM)tbid);
//->리스트박스에 tbid를 LPARAM으로 전달
//SetDlgItemText(hWndDlg, IDC_LIST1, tbid);
//->리스트박스에 값 띄우기
//질의문마다 DB연결끊고 다시 연결하기(SQL상태값은 포인터이기때문에 질의문 실행하면 값 바뀜)

//SQLBindCol의 열번호는 해당 쿼리가 반환하는 결과집합에서의 열 순서(쿼리에서 추출하고자하는 열들의 순서)

//콤보박스내용가져오기
//SendDlgItemMessage:부모창의 컨트롤의 내용을 가져와라
//SendMessage는 부모창대신 컨트롤의 핸들이고 컨트롤 식별자가 없음
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

//리스트뷰에서 내용 선택하기
//ListView_GetItemText(리스트뷰핸들,nlv->iITem,서브아이템(열)인덱스,저장할문자열,저장할문자열크기); 
//case WM_NOTIFY:
//	LPNMHDR hdr;
//	LPNMLISTVIEW nlv;
//	hdr = (LPNMHDR)lParam;
//	nlv = (LPNMLISTVIEW)lParam;
//	if (hdr->hwndFrom == 리스트뷰핸들) {
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

//콤보박스 그릴땐 내용이 보이게 밑으로 크게 그리기

//외래키를 받을때 원본 테이블의 기본키만 받을수 있음

//문자열 전역변수를 대화상자함수에서 GetDlgItemText로 담는데 사용할때 잘리는 문제  

//오라클에서 테이블의 외래키 제약조건명 검색
//SELECT CONSTRAINT_NAME
//FROM USER_CONSTRAINTS
//WHERE TABLE_NAME = 'EMP' AND CONSTRAINT_TYPE = 'R';

//리스트뷰는 ListView_InsertItem(리스트뷰핸들,&Li);으로 아이템 추가하고 , ListView_SetItem(리스트뷰핸들,&Li); 으로 행 추가하기

//GetDlgItemText(hDlg, IDC_DEPTRETIREECOMBO, tgdept, sizeof(tgdept)); 이미 바인딩 되어있는 값은  노티파이로 먹히지 않기 때문에 GetDlgItemText(hDlg,식별자,문자열,sizeof(문자열));

//SQL 실행:SQLExecDirect(hStmt,SQL,SQL_NTS)
//SQL 한번 동작(여러 레코드 수가 있는 select문): SQLFetch(hStmt)

//커맨드에서 컨트롤 동작은 사용자가 직접 컨트롤을 조작했을때이고  다른곳에서  컨트롤을 바인딩시키는것과 다르게 인식한다.

//라디오버튼 누르기: SendDlgItemMessage(hDlg, IDC_NONMARRIAGERADIO, BM_SETCHECK, BST_CHECKED, 0); BST가 바꾸고 싶은 값

////////////////////////////////////////////////////////소켓통신/////////////////////////////////////////////
//스레드는 무한반복문으로 독립적으로 계속 수행
//서버설정
//nReturn = WSAStartup(WORD(2.0), &wsadata);
//listensock = socket(AF_INET, SOCK_STREAM, 0);
//addr_server.sin_family = AF_INET;
//addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
//addr_server.sin_port = htons(g_uPort);
//nReturn = bind(listensock, (sockaddr*)&addr_server, sizeof(sockaddr));
//nReturn = listen(listensock, 1);
//클라이언트설정
//nReturn = WSAStartup(WORD(2.0), &wsadata);
//clientsock = socket(AF_INET, SOCK_STREAM, 0);
//addr_client.sin_family = AF_INET;
//addr_client.sin_addr.s_addr = inet_addr(g_szlpAddress);
//addr_client.sin_port = htons(g_uPort);
//클라이언트에서 connect하면 서버에서 accept로 받음
//클라이언트에서 send하면 서버에서 recv로 받음
//외부스레드역할: 계속 어셉트하고 해당 소켓을 리시브해주는 내부스레드 만들기
//내부스레드 역할: 계속 각 소켓마다 리시브받기
//연결받기랑 리시브랑 독립적으로 계속 수행
//클라이언트수만큼 내부스레드 만들어짐
//어셉트는 새로운 연결 들어올때까지 대기된다. 즉 스레드가 멈춤 그렇기에 어셉트와 리시브는 같이 있으면 기존 클라이언트를 리시브 못한다.
//배열에 담는걸 왜 외부스레드에서 해야하는지? 내부스레드에서 계속 돌아가서 그러는건지? 외부스레드에서 배열인덱스가 증가하는데 내부에선 그걸 이용해서 그런건지? 
//커멘드안의 컨트롤식별자는 break로 끝내고 이벤트처리는 return 0으로 끝내기
