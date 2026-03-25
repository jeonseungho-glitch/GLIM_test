
// MFCCircleMakeDlg.h: 헤더 파일
//
#include <vector>

#pragma once

// CMFCCircleMakeDlg 대화 상자
class CMFCCircleMakeDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCCircleMakeDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

public:
    CPoint m_pts[3];
    int m_clickCount; // 클릭한 횟수
    int m_dragIndex;  // 드래그 점
    CWinThread* m_pThread; // 랜덤 이동 스레드 포인터
    bool m_bThreadRunning; // 스레드 실행 확인 용
    int m_nRadius; // 반지름
    int m_nThickness; // 두께

    void DrawCustomCircle(CDC* pDC, double cx, double cy, double r, int thickness); // 원을 그리는 함수
    bool CalculateCircumcircle(double& cx, double& cy, double& r); // 외접원의 중심과 반지름을 도출하는 함수

    static UINT RandomMoveThread(LPVOID pParam); // 스레드 제어 스테틱함수

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCIRCLEMAKE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnRandom();
};
