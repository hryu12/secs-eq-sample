---
description: CBridgeClient TCP 통신, SECS-II JSON 메시지 흐름, MFC 스레드 모델을 다룰 때 적용
alwaysApply: true
---

# 구현 규칙 — 아키텍처 및 메시지 흐름

## 1. 클래스 계층 구조

```
CEqSampleApp (CWinApp)
└── CEqSampleDlg (CDialogEx)    ← 모든 UI·통신 로직
    └── CBridgeClient m_Bridge  ← TCP 소켓으로 SecsBridge relay에 연결
```

XComPro ActiveX는 제거됨. `xcomproctrl1.h/cpp`도 삭제됨.

---

## 2. 스레드 모델

| 스레드 | 역할 |
|--------|------|
| MFC UI 스레드 | 모든 UI 조작, 메시지 송신, 수신 메시지 처리 |
| `CBridgeClient::RecvThread` | `recv()` 블로킹 루프. `PostMessage`로만 UI 스레드에 전달 |

**RecvThread는 UI 위젯을 직접 건드리지 않는다.** 수신 데이터는 반드시 `PostMessage`로 전달한다.

```
RecvThread (Win32 스레드)
  └─ recv() 수신
       └─ new std::string*  →  PostMessage(WM_BRIDGE_MSG)
       └─ 연결/단절         →  PostMessage(WM_BRIDGE_CONN)

UI 스레드
  └─ OnBridgeMsg(WPARAM, LPARAM)   ← LPARAM = std::string*, 처리 후 반드시 delete
  └─ OnBridgeConn(WPARAM, LPARAM)  ← WPARAM: 1=연결, 0=단절
```

---

## 3. 연결 흐름

```
OnBnClickedBtnStart()
  └─ m_Bridge.Connect("127.0.0.1", 19001, GetSafeHwnd())
       └─ TCP connect → RecvThread 시작
            └─ WM_BRIDGE_CONN (WPARAM=1) → OnBridgeConn
```

- 릴레이 주소/포트: `127.0.0.1:19001` (현재 하드코딩)
- `m_SECSCommunicationOK = true`는 S1F13 수신 시 설정됨. 이 플래그가 false면 Send 버튼이 동작하지 않는다.

---

## 4. 메시지 송신 흐름

```
OnBnClickedBtnSendMsg()
  └─ MakeRequest(stream, func, wbit, data)  // JSON 문자열 생성
       └─ m_Bridge.SendJson(json)            // TCP send, '\n' 구분자 포함
```

- `MakeRequest`는 내부 카운터(`mfc-1`, `mfc-2`, ...)로 고유 id를 생성한다.
- `wbit=true`이면 릴레이가 응답(짝수 Function)을 기다린다.

### 지원 송신 메시지

| 콤보 항목 | S/F | wbit |
|-----------|-----|------|
| S1F1 | S1F1 | true |
| S1F13_1 | S1F13 | true (data: `[]`) |
| S1F13_2 | S1F13 | true (data: ModelName+SoftRev) |
| S2F13 | S2F13 | true |
| S2F33 | S2F33 | true (전 타입 샘플) |
| S5F23 | S5F23 | true |

---

## 5. 메시지 수신 및 자동 응답 흐름

```
OnBridgeMsg(WPARAM, LPARAM lParam)
  └─ std::string* pLine = (std::string*)lParam
       └─ BridgeGetInt/Str/Bool 로 stream, function, reply, id 파싱
            └─ reply=true → MakeReply() + m_Bridge.SendJson()
            └─ reply=false → 로그 출력만
  └─ delete pLine  ← 반드시 호출
```

### 자동 응답 규칙

| 수신 | 응답 |
|------|------|
| S1F1 | S1F2 (ModelName, SoftRev) |
| S1F13 | S1F14 + `m_SECSCommunicationOK=true` |
| S2F13 | S2F14 |
| S2F33 | S2F34 |
| S5F23 | S5F24 |

---

## 6. 종료 흐름

```
OnBnClickedBtnStop() 또는 OnBnClickedOk()
  └─ m_Bridge.Disconnect()
       └─ m_running=false → closesocket → WaitForSingleObject(5000)
```

`Disconnect()`는 소켓을 닫아 RecvThread의 `recv()`를 강제 종료시킨다.
