# EqSample

SecsBridge 릴레이에 연결하는 SECS-II 장비(Equipment) 시뮬레이터 샘플 애플리케이션.

## 개발 환경

- Visual C++ (MFC), Visual Studio 2010
- Win32 x86
- 별도 패키지 관리자 없음 (Windows SDK / MFC 기본 포함)

## 아키텍처

```
┌─────────────────┐    JSON/TCP      ┌──────────────┐    HSMS    ┌─────────┐
│    EqSample     │ ◄──────────────► │  SecsBridge  │ ◄────────► │   MES   │
│  (MFC, 장비 측) │  127.0.0.1:19001 │   (릴레이)   │            │         │
└─────────────────┘                  └──────────────┘            └─────────┘

CEqSampleDlg
└── CBridgeClient m_Bridge  ← TCP 소켓, newline-delimited JSON
    └── RecvThread           ← 수신 전용 Win32 스레드
```

## 주요 기능

- SecsBridge 릴레이에 TCP 연결/해제
- SECS-II 메시지 수동 송신 (콤보박스 선택)
- 수신 메시지 자동 응답

| 송신 | 수신 시 자동 응답 |
|------|-----------------|
| S1F1 | S1F2 |
| S1F13 | S1F14 (이후 송신 활성화) |
| S2F13 | S2F14 |
| S2F33 | S2F34 |
| S5F23 | S5F24 |

## 시작하기

```powershell
# Visual Studio 2010 이상으로 솔루션 열기
EqSample.sln

# 빌드 구성 선택 후 빌드 (F7)
# Debug|Win32 또는 Release|Win32

# SecsBridge 릴레이 실행 후 EqSample.exe 실행
EqSample/EqSample.exe
```

## 설정

릴레이 주소는 `EqSampleDlg.cpp`의 `OnBnClickedBtnStart()`에서 변경한다.

| 항목 | 기본값 | 설명 |
|------|--------|------|
| 릴레이 IP | `127.0.0.1` | SecsBridge 릴레이 주소 |
| 릴레이 Port | `19001` | SecsBridge 릴레이 포트 |
