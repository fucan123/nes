
// nes_mfc.cpp : 定义应用程序的类行为。
//
#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "nes_mfc.h"
#include "MainFrm.h"
#include "CDebugDlg.h"
#include "CCpuMemDlg.h"
#include "CPpuMemDlg.h"
#include "CNesInfoDlg.h"
#include <iostream>

#pragma comment(lib, "d2d1.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CPU全局变量
extern ROM g_ROM;
//CPU全局变量
extern CPU g_CPU;
//CPU全局变量
extern PPU g_PPU;

typedef struct s_nes_header {
	byte id[4];       //
	byte rom_count;   //
	byte vrom_count;  //
	byte control1;    //
	byte control2;    //
	byte reserved[8]; //
} nes_header;

// Cnes_mfcApp

BEGIN_MESSAGE_MAP(Cnes_mfcApp, CWinApp)
	ON_BN_CLICKED(ID_DEBUG, &Cnes_mfcApp::OnDebug)
	ON_COMMAND(ID_CPU_MEM, &Cnes_mfcApp::OnCPUMem)
	ON_COMMAND(ID_PPU_MEM, &Cnes_mfcApp::OnPPUMem)
	ON_COMMAND(ID_NES_INFO, &Cnes_mfcApp::OnNesInfo)
END_MESSAGE_MAP()

// Cnes_mfcApp 构造

Cnes_mfcApp::Cnes_mfcApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("nes_mfc.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 Cnes_mfcApp 对象

Cnes_mfcApp theApp;


// Cnes_mfcApp 初始化

BOOL Cnes_mfcApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	try {
		if (!g_ROM.open("tk90.nes"))
			throw "文件打开失败";
	}
	catch (char* msg) {
		CString str(msg);
		::MessageBox(NULL, str, L"错误提示", MB_OK);
	}
	

	struct stat st;
	FILE* fp = fopen("tk90.nes", "r");
	if (!fp) {
	}
	fstat(_fileno(fp), &st);
	_off_t size = st.st_size;
	char* tmp = new char[size];
	nes_header header;
	memset(tmp, 0, sizeof(nes_header));
	fread(tmp, size, 1, fp);
	memcpy(&header, tmp, sizeof(nes_header));

	int mapper = (header.control1 >> 4) | (header.control2 & 0xf0);
	int rom_size = header.rom_count * 0x4000 + header.vrom_count * 0x2000;
	char* roms = new char[rom_size];
	memcpy(roms, tmp + 16, rom_size);

	char* m = new char[0xffff + 1];
	memset(m, 0, 0xffff + 1);
	if (header.rom_count == 1) {
		memcpy(&m[0x8000], roms, 0x4000); 
		memcpy(&m[0xc000], roms, 0x4000); 
	
	}
	if (header.rom_count == 2) {
		memcpy(&m[0x8000], roms, 0x8000);
	}
	word x = 0x01, dt = 0x0d;
	word r = x - dt;

	g_CPU.load(m, 0xffff + 1, m + header.rom_count * 0x4000, 0x2000);
	g_PPU.load(roms + header.rom_count * 0x4000, 0x2000, header.control1 & 0x01);
	//g_CPU.write(0x2000, 0x10);
	CString t;
	t.Format(L"%d", header.control1  &0x01);
	//::MessageBox(NULL, t, L"t", MB_OK);
	//AfxBeginThread(CPURun, this);
	//AfxBeginThread(PPURun, this);
	return TRUE;
}

int Cnes_mfcApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// Cnes_mfcApp 消息处理程序
void Cnes_mfcApp::OnDebug()
{
	// TODO: 在此添加命令处理程序代码
	/*CDebugDlg aboutDlg(&cpu);
	aboutDlg.DoModal();
	return;*/
	//IDD_ABOUTBOX;
	dlgdbg = new CDebugDlg();
	((CDebugDlg*)dlgdbg)->Create(IDD_ABOUTBOX);//创建一个非模态对话框    IDD_DIALOG2是我创建的一对话框ID
	((CDebugDlg*)dlgdbg)->ShowWindow(SW_SHOWNORMAL);//显示非模态对话框
}

void Cnes_mfcApp::OnCPUMem()
{
	dlgcpumem = new CCpuMemDlg();
	((CCpuMemDlg*)dlgcpumem)->Create(IDD_DIALOG_CPUMEM);//创建一个非模态对话框    IDD_DIALOG2是我创建的一对话框ID
	((CCpuMemDlg*)dlgcpumem)->ShowWindow(SW_SHOWNORMAL);//显示非模态对话框
}


void Cnes_mfcApp::OnPPUMem()
{
	dlgppumem = new CPpuMemDlg();
	((CCpuMemDlg*)dlgppumem)->Create(IDD_DIALOG_PPUMEM);//创建一个非模态对话框    IDD_DIALOG2是我创建的一对话框ID
	((CCpuMemDlg*)dlgppumem)->ShowWindow(SW_SHOWNORMAL);//显示非模态对话框
}

void Cnes_mfcApp::OnNesInfo()
{
	CNesInfoDlg dlg;
	dlg.DoModal();
}