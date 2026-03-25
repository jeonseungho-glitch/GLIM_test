
// MFCCircleMakeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCCircleMake.h"
#include "MFCCircleMakeDlg.h"
#include "afxdialogex.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCCircleMakeDlg 대화 상자



CMFCCircleMakeDlg::CMFCCircleMakeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCCIRCLEMAKE_DIALOG, pParent)
	, m_nRadius(10)
	, m_nThickness(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_clickCount = 0;
	m_dragIndex = -1;
	m_pThread = nullptr;
	m_bThreadRunning = false;

	
	for (int i = 0; i < 3; i++) 
	{
		m_pts[i] = CPoint(0, 0);
	}
}

void CMFCCircleMakeDlg::DrawCustomCircle(CDC* pDC, double cx, double cy, double r, int thickness)
{
	// 선의 두께와 색상(검은색)을 적용할 펜 객체 생성
	CPen pen(PS_SOLID, thickness, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	const int segments = 360; // 360각형으로 쪼개어 정원에 한없이 가깝게 근사함
	for (int i = 0; i <= segments; i++)
	{
		double theta = 2.0 * PI * double(i) / double(segments);
		// 삼각함수를 이용한 원주 상의 X, Y 좌표 계산
		double x = cx + r * cos(theta);
		double y = cy + r * sin(theta);

		if (i == 0) {
			pDC->MoveTo((int)x, (int)y); // 첫 번째 점으로 이동
		}
		else {
			pDC->LineTo((int)x, (int)y); // 이후 점들을 촘촘히 선으로 연결
		}
	}
	pDC->SelectObject(pOldPen); // 기존 펜으로 복구 (메모리 누수 방어)
}

bool CMFCCircleMakeDlg::CalculateCircumcircle(double& cx, double& cy, double& r)
{
	double x1 = m_pts[0].x, y1 = m_pts[0].y;
	double x2 = m_pts[1].x, y2 = m_pts[1].y;
	double x3 = m_pts[2].x, y3 = m_pts[2].y;

	// 분모 D 계산 (세 점이 일직선상에 있는지 판별)
	double D = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));

	// 세 점이 일직선상에 있어 원을 만들 수 없는 경우 방어
	if (abs(D) < 0.000001) return false;

	// 외심의 X, Y 좌표 (C_x, C_y) 계산
	cx = ((x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) + (x3 * x3 + y3 * y3) * (y1 - y2)) / D;
	cy = ((x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) + (x3 * x3 + y3 * y3) * (x2 - x1)) / D;

	// 중심(cx, cy)에서 첫 번째 점(x1, y1)까지의 거리가 곧 반지름(r)
	r = sqrt((x1 - cx) * (x1 - cx) + (y1 - cy) * (y1 - cy));
	return true;
}

UINT CMFCCircleMakeDlg::RandomMoveThread(LPVOID pParam)
{
	// pParam으로 넘어온 'this'를 다이얼로그 포인터로 다시 변환합니다.
	CMFCCircleMakeDlg* pDlg = (CMFCCircleMakeDlg*)pParam;

	// 매번 다른 무작위 패턴이 나오도록 시간 기준 시드(Seed) 설정
	srand((unsigned int)time(NULL));

	// 화면 영역 밖으로 점이 도망가지 않도록 현재 다이얼로그의 크기를 구합니다.
	CRect rect;
	pDlg->GetClientRect(&rect);

	// 요구사항: "초당 2회, 총 10번 자동으로 반복"
	for (int step = 0; step < 10; step++)
	{
		// 3개의 점 좌표를 무작위 난수로 변경
		for (int i = 0; i < 3; i++)
		{
			int randomX = rand() % rect.Width();
			int randomY = rand() % rect.Height();
			pDlg->m_pts[i] = CPoint(randomX, randomY);
		}

		// 점 위치를 바꿨으니 화면을 갱신하라고 메인 UI에 명령을 내립니다.
		pDlg->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

		// 초당 2회 움직여야 하므로 0.5초(500ms) 동안 대기합니다.
		// 메인 스레드가 아니기 때문에 Sleep을 써도 화면이 하얗게 멈추지 않습니다!
		Sleep(500);
	}

	// 10번의 작업이 모두 끝나면 상태 플래그를 해제합니다.
	pDlg->m_bThreadRunning = false;

	return 0; // 스레드 정상 종료
}

void CMFCCircleMakeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_nRadius);
	DDX_Text(pDX, IDC_EDIT_THICKNESS, m_nThickness);
}

BEGIN_MESSAGE_MAP(CMFCCircleMakeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// ON_EN_CHANGE(IDC_EDIT1, &CMFCCircleMakeDlg::OnEnChangeEdit1)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_RESET, &CMFCCircleMakeDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CMFCCircleMakeDlg::OnBnClickedBtnRandom)
END_MESSAGE_MAP()


// CMFCCircleMakeDlg 메시지 처리기

BOOL CMFCCircleMakeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCCircleMakeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCCircleMakeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		UpdateData(TRUE);

		for (int i = 0; i < m_clickCount; i++)
		{
			DrawCustomCircle(&dc, m_pts[i].x, m_pts[i].y, m_nRadius, 1);

			CString strCoord;
			strCoord.Format(_T("(%d, %d)"), m_pts[i].x, m_pts[i].y);
			dc.TextOutW(m_pts[i].x + m_nRadius + 5, m_pts[i].y - m_nRadius - 15, strCoord);
		}

		if (m_clickCount >= 3)
		{
			double cx = 0, cy = 0, r = 0;

			if (CalculateCircumcircle(cx, cy, r))
			{
				DrawCustomCircle(&dc, cx, cy, r, m_nThickness);
			}
		}

		//CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCCircleMakeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCCircleMakeDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMFCCircleMakeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 마우스 버튼을 떼면 드래그 상태 해제
	m_dragIndex = -1;

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CMFCCircleMakeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	UpdateData(TRUE);

	if (m_clickCount >= 3)
	{
		for (int i = 0; i < 3; i++)
		{
			double distance = sqrt(pow(point.x - m_pts[i].x, 2) + pow(point.y - m_pts[i].y, 2));
			if (distance <= m_nRadius)
			{
				m_dragIndex = i;
				break;
			}
		}
	}
	else
	{
		m_pts[m_clickCount] = point;
		m_clickCount++;

		// [수정 타격 지점] FALSE를 TRUE로 변경하여 과거 잔상 삭제
		Invalidate(TRUE);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CMFCCircleMakeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_dragIndex != -1)
	{
		m_pts[m_dragIndex] = point;

		// [수정 타격 지점] FALSE를 TRUE로 변경하여 과거 잔상 삭제
		Invalidate(TRUE);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CMFCCircleMakeDlg::OnBnClickedBtnReset()
{
	// 스레드가 돌아가고 있는 도중에는 충돌 방지를 위해 초기화를 막습니다.
	if (m_bThreadRunning) return;

	m_clickCount = 0;
	m_dragIndex = -1;
	for (int i = 0; i < 3; i++)
	{
		m_pts[i] = CPoint(0, 0);
	}

	Invalidate(TRUE); // 화면 하얗게 지우기
}

void CMFCCircleMakeDlg::OnBnClickedBtnRandom()
{
	// 점이 3개 다 찍혀서 정원이 그려진 상태에서만 작동해야 합니다.
	if (m_clickCount < 3) return;

	// 이미 스레드가 작업 중이면 중복 실행을 막습니다. (안전 장치)
	if (m_bThreadRunning) return;

	m_bThreadRunning = true;

	// 메인 화면(UI)을 멈추지 않고, 뒤에서 몰래 일할 스레드(RandomMoveThread)를 출격시킵니다.
	// 'this'를 넘겨주어 스레드가 다이얼로그의 변수들에 접근할 수 있게 합니다.
	m_pThread = AfxBeginThread(RandomMoveThread, this);
}
