# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

**EqSample**은 Linkgenesis **XComPro** 장비 통신 프레임워크의 샘플 애플리케이션이다. HSMS(High-Speed SECS Message Services) 위에서 SECS-II 프로토콜을 이용해 반도체 장비와 통신하는 방법을 시연한다.

- **언어/환경**: Visual C++ (MFC), Visual Studio 2010, Win32 x86
- **GUI**: MFC 다이얼로그 기반 애플리케이션 (Dynamic MFC DLL)
- **핵심 의존성**: XComPro ActiveX COM 컨트롤 (CLSID: `560F8BBF-A69A-4320-82A3-EE167D16559E`)
- **문자 인코딩**: MultiByte (ANSI), 소스 내 한국어 주석 포함

## 빌드 방법

Visual Studio 2010 이상에서 솔루션 파일을 열어 빌드한다.

```
EqSample.sln 파일을 Visual Studio로 열기
→ 빌드 구성 선택: Debug|Win32 또는 Release|Win32
→ 빌드(F7)
```

- **출력 디렉터리**: `EqSample/` (프로젝트 폴더와 동일)
- **실행 파일**: `EqSample/EqSample.exe`
- **프리컴파일 헤더**: `stdafx.h` / `stdafx.cpp`
- 별도의 CMake, Makefile, CLI 빌드 스크립트는 없다.

## 아키텍처

```
CEqSampleApp (CWinApp)
└── CEqSampleDlg (CDialogEx)  ← 메인 다이얼로그
    └── CXcomproctrl1           ← XComPro ActiveX 래퍼 (IDispatch)
```

### 핵심 클래스

| 파일 | 클래스 | 역할 |
|------|--------|------|
| `EqSample.h/cpp` | `CEqSampleApp` | `CWinApp` 파생. 앱 초기화 및 다이얼로그 진입점 |
| `EqSampleDlg.h/cpp` | `CEqSampleDlg` | UI 로직 전담. 메시지 구성·송수신·로그 표시 |
| `xcomproctrl1.h/cpp` | `CXcomproctrl1` | XComPro ActiveX IDispatch 래퍼 |

### 데이터 흐름

1. **초기화**: `CEqSampleDlg::OnInitDialog()` → `m_XComPro.Initialize("EqSample.cfg")` 호출
2. **통신 시작**: "Start XCom" 버튼 → `m_XComPro.Start()`
3. **메시지 송신**:
   - `MakeSecsMsg(lMsgId)` 로 메시지 객체 생성
   - `SetXxxItem()` 계열 메서드로 필드 값 채움 (I1/I2/I4/I8, U1-U8, F4/F8, Bool, Binary, ASCII, JIS8)
   - `Send(lMsgId)` 로 전송, 결과를 리스트박스에 기록
4. **메시지 수신**: `LoadSecsMsg()` → `GetXxxItem()` 으로 필드 읽기
5. **종료**: `m_XComPro.Stop()` → `m_XComPro.Close()`

### 설정 파일

| 파일 | 용도 |
|------|------|
| `EqSample/EqSample.cfg` | XComPro 설정 (HSMS IP/Port, 타임아웃, 로그 옵션) |
| `EqSample/Sample.sml` | SECS-II 메시지 정의 (S1F1, S1F13, S2F13, S2F33, S5F23 등) |

기본 설정: `Host=false`, `HSMS=true`, `IP=127.0.0.1`, `Port=2001`, `Active=false` (Passive 모드)

### 지원 SECS-II 메시지

`EqSampleDlg.cpp`의 콤보박스 초기화 부분에서 지원 메시지 목록을 확인할 수 있다. S1F1/F13, S2F13/F33, S5F23 등이 포함된다.

### 테스트 데이터 상수

`EqSampleDlg.cpp` 상단의 `#define` 값들이 각 데이터 타입별 테스트 샘플 값이다. 배열 크기는 `MAX_ARRAY=7`로 고정되어 있다.
