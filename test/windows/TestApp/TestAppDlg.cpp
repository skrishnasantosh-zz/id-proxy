
// TestAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TestApp.h"
#include "TestAppDlg.h"
#include "afxdialogex.h"

#include "../../../platform/platform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestAppDlg dialog



CTestAppDlg::CTestAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTAPP_DIALOG, pParent)
	, m_accessTokenText(_T(""))
	, m_tokenSecretText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT5, m_accessToken);
	DDX_Control(pDX, IDC_EDIT6, m_tokenSecret);
	DDX_Text(pDX, IDC_EDIT5, m_accessTokenText);
	DDX_Text(pDX, IDC_EDIT6, m_tokenSecretText);
}

BEGIN_MESSAGE_MAP(CTestAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNWEBBROWSER, &CTestAppDlg::OnBnClickedBtnwebbrowser)
END_MESSAGE_MAP()


// CTestAppDlg message handlers

BOOL CTestAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestAppDlg::OnBnClickedBtnwebbrowser()
{

	Platform_t platform;

	LoadPlatform(&platform, GetModuleHandle(NULL));

	platform.browserFrame.events.onAfterPageLoad = NULL;
	platform.browserFrame.events.onError = NULL;

	platform.browserFrame.ShowBrowserFrame(&platform, GetModuleHandle(NULL), m_hWnd, TEXT("http://accounts-dev.autodesk.com/Logon"));

	platform.Unload(&platform);
}