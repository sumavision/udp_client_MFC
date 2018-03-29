
// clientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "afxdialogex.h"
#include "ctrl_frame.h"
#include <share.h>

#define MAXSIZE 1024//最大读写缓存

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CclientDlg SockClnt;
sockaddr ServerAddr;
// CclientDlg 对话框
int ServerAddrLen = sizeof (sockaddr);
int protocol = SOCK_UDP;

//Exit :1;Close:0
int ExitCloseFlag = -1;

//模拟发送按键 :1;手动发送按键:0
int SndAutoFlag = -1;

int CclientDlg::threadcount = 0;
int CclientDlg::SndCount = 0;
int CclientDlg::RecvCount = 0;

CclientDlg::CclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CclientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_SERVER, m_IP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_list);
	//  DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_MESSAGEs, m_message);
	DDX_Control(pDX, IDC_STATIC_NAME, m_showname);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, send_msg);

	DDX_Control(pDX, IDC_RADIO_TCP, m_TcpCheck);
	DDX_Control(pDX, IDC_RADIO_UDP, m_UdpCheck);

	DDX_Control(pDX, IDC_EDIT_SND_COUNT, m_SndCount);
	DDX_Control(pDX, IDC_EDIT_INTERVAL_TIME, m_IntervalTime);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CclientDlg::OnBnClickedButtonConnect)
	ON_LBN_SELCHANGE(IDC_LIST_CLIENT, &CclientDlg::OnLbnSelchangeListClient)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CclientDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SND, &CclientDlg::OnBnClickedButtonSnd)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CclientDlg::OnBnClickedButtonExit)
	//ON_STN_CLICKED(IDC_STATIC_NAME, &CclientDlg::OnStnClickedStaticName)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SERVER, &CclientDlg::OnBnClickedButtonClearServer)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_CLNT, &CclientDlg::OnBnClickedButtonClearClnt)
END_MESSAGE_MAP()


// CclientDlg 消息处理程序

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_TcpCheck.SetCheck(0);         //默认tcp关闭
	m_UdpCheck.SetCheck(1);         //默认选择udp

	index = 0;

	m_IP.SetWindowTextW(_T("192.165.53.152"));
	m_Port.SetWindowTextW(_T("6180"));

	m_SndCount.SetWindowTextW(_T("1"));
	m_IntervalTime.SetWindowTextW(_T("1"));
	Sleep(500);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CclientDlg::OnPaint()
{
#if 1
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
#endif
}
#if 1
//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#endif 


BOOL CclientDlg::PreTranslateMessage(MSG* pMsg)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PORT);
	CString str;
    ASSERT(pEdit && pEdit->GetSafeHwnd());

    if((WM_KEYDOWN == pMsg->message) && (pEdit->GetSafeHwnd() == pMsg->hwnd))
    {
		if((pMsg->wParam >= _T('0') && pMsg->wParam <= _T('9'))
            || ((pMsg->wParam >= VK_NUMPAD0) && (pMsg->wParam <= VK_NUMPAD9)))
		{
			if(pEdit->GetWindowTextLengthW()<5){
				return CDialog::PreTranslateMessage(pMsg);
			}
			else
			{
				return TRUE;
			}
		}
		else if((_T('*') == pMsg->wParam) || (_T('#') == pMsg->wParam)
             || (VK_LEFT == pMsg->wParam) || (VK_RIGHT == pMsg->wParam)
             || (VK_UP == pMsg->wParam) || (VK_DOWN == pMsg->wParam)
             || (VK_DELETE == pMsg->wParam) || (VK_BACK == pMsg->wParam))
        {
            return CDialog::PreTranslateMessage(pMsg);
        }
        else
        {
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}


void CclientDlg::GetServerAddr(data *d)
{
	//CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PORT);

	CString sPort;

	//pEdit->GetWindowTextW(sPort);//获取端口字符串
	m_Port.GetWindowTextW(sPort);
	int dPort = _ttoi(sPort);//获取端口整形

	DWORD dwIP;
	m_IP.GetAddress(dwIP);//获取ip

	sockaddr_in *serAddr;
	memset((char*)&(d->ServerAddr),0,sizeof(sockaddr));
	serAddr= (sockaddr_in *)&(d->ServerAddr);
    serAddr->sin_family = AF_INET;
    serAddr->sin_port = htons(dPort);
    serAddr->sin_addr.S_un.S_addr = htonl(dwIP); 

	d->ServerAddrLen = sizeof(sockaddr);
}
int CclientDlg::DisplayCurrConnectAddr(data *d)
{
	CString sPort;
	m_Port.GetWindowTextW(sPort);

	DWORD dwIP;
	m_IP.GetAddress(dwIP);//获取ip

	unsigned char* pDwIP = (unsigned char*)&dwIP;

	CString name;
	name.Format(_T("%u.%u.%u.%u"),*(pDwIP+3),*(pDwIP+2),*(pDwIP+1),*pDwIP);
	name += _T(":") + sPort;
	CString Proto;
	if(d->protocol == SOCK_UDP)
	{
		Proto = _T("(UDP)");
	}
	else if (d->protocol == SOCK_TCP)
	{
		Proto = _T("(TCP)");
	}
	name += Proto;
	d->ListName = name;

	//搜索该地址是否已经连接
	if (m_list.FindString(-1,d->ListName) >= 0)
	{
		return -1;//已连接返回
	}
	
	m_list.SetItemData(m_list.AddString(name),(DWORD_PTR)d);

	return 0;
}
int /*CclientDlg::*/SockTcpRecv(SOCKET       Sock_Fd,char *buff,int len)
{
	int RecvLen = 0;
	RecvLen = recv(Sock_Fd, buff, len, 0);

	return RecvLen;
}
int /*CclientDlg::*/SockTcpSend(SOCKET       Sock_Fd,char *buff,int len)
{
	int SndLen = 0;
	SndLen =send(Sock_Fd,buff,len,0);

	return SndLen;
}
SOCKET CclientDlg::SockTcpStart(data *d)
{
	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
        return sclient;
    }
	sockaddr ServerA;
	ServerA = d->ServerAddr;
	//创建连接
	if (connect(sclient, &ServerA, sizeof(ServerA)) == SOCKET_ERROR)
    {
		closesocket(sclient);
		AfxMessageBox(_T("connect error !"));
        printf("connect error !");
        return INVALID_SOCKET;
    }
	return sclient;
}
int /*CclientDlg::*/SockUdpRecv(SOCKET       Sock_Fd,char *buff,data *d,int len)
{
	int RecvLen = 0;
	d->ServerAddrLen = sizeof(sockaddr);
	RecvLen = recvfrom(Sock_Fd, buff, len, 0,&(d->ServerAddr),&(d->ServerAddrLen));

	return RecvLen;
}
int /*CclientDlg::*/SockUdpSend(SOCKET       Sock_Fd,char *buff,data *d,int len)
{
	int SndLen = 0;

	d->ServerAddrLen = sizeof(sockaddr);
	SndLen =sendto(Sock_Fd,buff,len,0,&(d->ServerAddr),(d->ServerAddrLen));

	return SndLen;
}

SOCKET CclientDlg::SockUdpStart()
{
	SOCKET sclient=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
       
    }
	return sclient;
}

void CclientDlg::OnBnClickedButtonConnect()
{
	SOCKET sclient = 0;
	data *d = new data;

	//获取输入的服务器IP地址与端口
	GetServerAddr(d);

	if (m_TcpCheck.GetCheck() == 1)
	{
		sclient = SockTcpStart(d);

		d->protocol = SOCK_TCP;
	}
	else if (m_UdpCheck.GetCheck() == 1)
	{
		sclient = SockUdpStart();
		d->protocol = SOCK_UDP;
	}
	else 
	{
		AfxMessageBox(_T("请选择连接协议"));
		return;
	}
	if (sclient == INVALID_SOCKET)
	{
		AfxMessageBox(_T("套接字无效"));
		return;
	}
	
	d->socket = sclient;
	d->index = index;

	//当前IP和端口显示
	if (DisplayCurrConnectAddr(d)< 0)
	{
		AfxMessageBox(_T("该连接已存在"));
		return;
	}
	
	DWORD dwThreadID;
	//HWND hwnd = GetSafeHwnd();
	HANDLE hThread = CreateThread(NULL, 10*1024, (LPTHREAD_START_ROUTINE)CclientDlg::GetServerData, (LPVOID)d, 0, &dwThreadID);
	
    if(hThread == NULL)   
    {   
         TerminateThread(hThread, 0);   
         return;   
    }

	index++;
	
	//关闭句柄，关闭句柄不代表关闭线程，如果不关闭句柄，会导致系统的句柄越来越少，最终不够用的情况。
	CloseHandle(hThread);
	Sleep(10);
}


DWORD WINAPI CclientDlg::GetServerData(LPVOID lpParam)
{
	threadcount++;//线程数加1

	char buffer[MAXSIZE] = {0};
	fd_set fds;
	int RecvLen = 0;
	data d = *(data *)lpParam;
	
	SOCKET client = *((SOCKET *)(lpParam));

	CListBox* listbox = (CListBox *)AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_LIST_CLIENT);
#if 0
	CString name;
	listbox->GetText(d.index,name);//获取名称,用于文件读写

	CString filename = CString(".txt");
	filename = name + filename;
	
	CFile file; //定义文件变量   
   
	//CString filename=L"D://test.txt";   
   
	/*
	modeCreate,指定构造函数创建一个新文件，如果该文件已经存在，那么将它的长度截断为0   
   
	modeWrite,打开文件，该文件仅用于写入操作    
   
	shareDenyNone,允许其他程序共享访问文件
	*/   
	if(file.Open(filename,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))    
	{    
   
		file.SeekToBegin(); //到达文件开头    
	   
		//file.Write("/xff/xfe", 2); //Unicode编码的txt文件以0xff、0xfe两个字节作为开头标记。
	}
	else
	{
		closesocket(client);
		file.Close();
		return 0;
	}
#endif
	while(1)
	{
		RecvLen = 0;
		memset(buffer, 0, MAXSIZE);
		FD_ZERO( & fds);
        FD_SET(client, &fds);
		if (select(client + 1, &fds, NULL, NULL, NULL) == SOCKET_ERROR)
		{
			if (errno == WSAEINTR) 
				continue;
			
			break;
		}
		if (FD_ISSET(client, &fds))
		{
			if (d.protocol == SOCK_TCP)
			{
				RecvLen =SockTcpRecv(client, buffer,MAXSIZE-1);
			}
			else if (d.protocol == SOCK_UDP)
			{
				RecvLen = SockUdpRecv(client, buffer,&d,MAXSIZE-1);
			}
			if(RecvLen <= 0)
			{
				break;
			}

			RecvLen = get_server_msg(buffer,RecvLen);
			if (RecvLen < 0)
			{
				break;
			}
			RecvCount++;

			//服务器消息框内显示
			CString RecvData(buffer);
			RecvData += _T("\r\n");
			RecvLen = RecvData.GetLength();
			
			CEdit* ServerMsg = (CEdit *)AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_EDIT_MESSAGEs);
			CString ServerOpt;
			ServerMsg->GetWindowTextW(ServerOpt);
			CString Count;
			Count.Format(_T("第 %d 次 :\r\n"),RecvCount);
			ServerOpt += (Count + RecvData);
			ServerMsg->SetWindowTextW(ServerOpt);

			//延时
			CString StrTime;
			CEdit* TimeMsg = (CEdit *)AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_EDIT_INTERVAL_TIME);
		    TimeMsg->GetWindowTextW(StrTime);
		    int time = _ttoi(StrTime)*1000;//时间单位转化为 s
	
			if (SndCount == 0)
			{
				AfxMessageBox(_T("success"));
				RecvCount = 0;
				SndAutoFlag = -1;
			} 
			else 
			{
				Sleep(time);
				CWnd* pWnd;
				pWnd=((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
				HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
				::PostMessage(hwnd,WM_COMMAND,MAKEWPARAM(IDC_BUTTON_SND, BN_CLICKED),NULL);
				SndAutoFlag = 1;
			}
			
			//文本显示
			//file.Write(RecvData,RecvLen);
        }
		
        Sleep(5);
	}
	//file.Close();
	if (client>0)
		closesocket(client);
	
	//退出不需弹出
	if (ExitCloseFlag == 0)
	{
		AfxMessageBox(_T("连接断开"));
	//}
		//线程中只能通过AfxGetApp来获取窗口并操作控件
		//获取列表操作指针
		int findindex = listbox->FindString(-1,d.ListName);
		if(findindex>=0)
		{
			delete &d;
			listbox->DeleteString(findindex);
		}
	}

	SndAutoFlag = -1;
	RecvCount = 0;
	//线程数-1
	threadcount--;
	return 0;
}


void CclientDlg::OnLbnSelchangeListClient()
{
	int index = m_list.GetCurSel();
	if(index != LB_ERR)
	{
	#if 0
		CString name;
		m_list.GetText(index,name);//获取名称,用于文件读写

		CString filename = CString(".txt");
		filename = name + filename;

		CStdioFile m_file(filename,CFile::modeRead|CFile::shareDenyNone);
		CString contents;
		m_file.ReadString(contents);
		m_message.SetWindowTextW(contents);
	#endif
		CString title;
		m_list.GetText(index,title);
		m_showname.SetWindowTextW(title);
		//m_file.Close();
	}
}


void CclientDlg::OnBnClickedButtonClose()
{
	int index = m_list.GetCurSel();
	ExitCloseFlag = 0;
	if(index!=LB_ERR)
	{
		data *d = (data *)m_list.GetItemData(index);
		closesocket(d->socket);
		delete d;
		m_list.DeleteString(index);

		//清除当前连接地址显示
		CString clear("");
		m_showname.SetWindowTextW(clear);
	}
}


void CclientDlg::OnBnClickedButtonSnd()
{
	char buffer[MAXSIZE] = {0};
	int len = 0;

	int SndLen = 0;
	
	CString msg;
	
	send_msg.GetWindowTextW(msg);
	int index = m_list.GetCurSel();

	if(index !=LB_ERR)
	{
		data *d = (data *)m_list.GetItemData(index);
#if 1		
		//宽字符转换char数组
		int n = msg.GetLength(); 

		int len = WideCharToMultiByte(CP_ACP,0,msg,msg.GetLength(),NULL,0,NULL,NULL);
		char * cmsg = new char[len+1];  
		memset(cmsg,0,len+1);
		WideCharToMultiByte(CP_ACP,0,msg,msg.GetLength(),cmsg,len,NULL,NULL);
		//cmsg[len] = '/0';//这里作为字符串的时候应该写入字符串结尾，但是服务端没有过滤/0，所以不发送了
#endif
		//len = convert_string_to_ctrl_frame(cmsg,buffer,len+1);
		//if(ctrl_frame_convert_result(len) != CTRL_ANL_OK)
		len = get_snd_msg(cmsg,buffer,len+1);
		if (len <= 0)
		{
			return;
		}

		if (SndAutoFlag != 1)
		{
			m_message.SetWindowTextW(_T(""));

			//发送次数
			CString StrCount;
			m_SndCount.GetWindowTextW(StrCount);
			SndCount = _ttoi(StrCount);
		}
		
		if (d->protocol == SOCK_TCP)
		{
			SndLen = SockTcpSend(d->socket, buffer,len);
		}
		else if (d->protocol == SOCK_UDP)
		{
			SndLen = SockUdpSend(d->socket, buffer,d,len);
		}
		if(SndLen <= 0)
		{
			return;
		}
		
		//if (SndCount != 0)
		{
			SndCount --;
		}
	
		Sleep(500);
		delete[] cmsg;//使用完毕删除空间
	}
}


void CclientDlg::OnBnClickedButtonExit()
{
	ExitCloseFlag = 1;
	int ListCount = m_list.GetCount();

	for (int i = ListCount - 1;i >= 0;i--)
	//for(int i=0;i<ListCount;i++)
	{
		data *d = (data *)m_list.GetItemData(i);
		closesocket(d->socket);//循环关闭socket
		m_list.DeleteString(i);
		//Sleep(500);
		delete d;
	}
	//清除当前连接地址显示
	CString clear("");
	m_showname.SetWindowTextW(clear);
	
	while(threadcount>0)//由于socket关闭后，线程不一定关闭，所以等待线程结束。
	{
		Sleep(100);
	}
	CclientDlg::OnCancel();
}



void CclientDlg::OnBnClickedButtonClearServer()
{
	CString clear("");

	m_message.SetWindowTextW(clear);
}


void CclientDlg::OnBnClickedButtonClearClnt()
{
	send_msg.SetWindowTextW(_T(""));
}
