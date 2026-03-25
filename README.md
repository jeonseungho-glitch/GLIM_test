🔴 MFC 3-Point Circumcircle & Multi-threading Test
본 프로젝트는 **MFC(Microsoft Foundation Class)**를 기반으로 한 다이얼로그 애플리케이션으로, 사용자가 클릭한 세 지점을 정확히 관통하는 외접원(Circumcircle)의 수학적 구현과 멀티스레드 기반의 비동기 애니메이션 제어 능력을 검증하기 위한 코딩 테스트 결과물입니다.

🛠 핵심 구현 기술 (Key Features)
1. API 독립형 수동 렌더링 엔진
   - 제약 사항 준수: Ellipse, DrawPolygon, GDI+ 등 표준 원 그리기 API를 일절 사용하지 않았습니다.
   - 수학적 모사: 원주율($\pi$)과 삼각함수($\sin$, $\cos$)를 활용한 파라메트릭 방정식(Parametric Equation)을 통해 360개의 정점을 산출하고, 이를 직선(LineTo)으로 촘촘히 연결하여 정원을 렌더링했습니다.
2. 외접원(Circumcircle) 도출 알고리즘
  - 세 점의 좌표를 바탕으로 두 선분의 수직이등분선이 만나는 외심(Circumcenter) 좌표를 하드코딩된 공식으로 계산합니다.
  - 분모가 0이 되는 경우(세 점이 일직선상에 위치)를 방어하는 예외 처리 로직이 포함되어 있습니다.
3. 멀티스레드 기반 비동기 애니메이션
  - UI 프리징 방어: 초당 2회씩 10번 진행되는 '랜덤 이동' 기능을 메인 UI 스레드가 아닌 워커 스레드(Worker Thread)에서 실행하도록 설계했습니다.
  - 강제 동기화 렌더링: 스레드 환경에서 배경 지우기(WM_ERASEBKGND)가 누락되어 발생하는 잔상 문제를 해결하기 위해 RedrawWindow와 RDW_UPDATENOW 플래그를 사용하여 무결점 애니메이션을 구현했습니다.
4. 실시간 인터랙션 및 드래그
  - Hit Test: 피타고라스 정리를 이용한 거리 계산 로직으로 특정 점을 선택하고 드래그할 수 있는 실시간 인터페이스를 제공합니다.
  - 실시간 갱신: 마우스 이동(WM_MOUSEMOVE) 시 Invalidate(TRUE) 호출을 통해 좌표 변화에 따른 정원을 즉각적으로 재계산 및 렌더링합니다.

🚀 실행 환경 (Environment)
OS: Windows 10 / 11
IDE: Visual Studio 2022 / 2026
Language: C++ (MFC)
Platform: x64 / x86

📂 주요 파일 구성
  - MFCCircleMakeDlg.cpp: 메인 비즈니스 로직, 수학 엔진, 마우스 이벤트 처리.
  - MFCCircleMakeDlg.h: 멤버 변수 선언 및 스레드 함수 정의.
  - DrawCustomCircle(): API 없이 원을 그리는 핵심 렌더링 함수.
  - RandomMoveThread(): 비동기 랜덤 이동을 수행하는 워커 스레드 함수.

💡 개발 시 고려 사항
  - 메모리 최적화: GDI Object(CPen 등) 사용 후 SelectObject를 통한 복원 및 적절한 소멸 처리를 통해 리소스 누수를 차단했습니다.
  - 가독성 및 유지보수: 각 기능별로 함수를 모듈화하여 확장성을 고려했습니다.
