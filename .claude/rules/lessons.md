---
description: CBridgeClient, MFC 스레드 경계, SECS-II JSON 통신에서 발견된 알려진 함정과 주의사항
alwaysApply: true
---

# 교훈 — 알려진 함정과 주의사항

## 1. WM_BRIDGE_MSG의 std::string* 소유권

`RecvThread`가 `new std::string*`을 생성해 `PostMessage(WM_BRIDGE_MSG, 0, (LPARAM)p)`로 전달한다.  
`OnBridgeMsg`에서 **반드시 `delete pLine`을 호출**해야 한다. 빠트리면 메모리 누수가 누적된다.

```cpp
LRESULT CEqSampleDlg::OnBridgeMsg(WPARAM, LPARAM lParam)
{
    std::string* pLine = reinterpret_cast<std::string*>(lParam);
    std::string  line  = *pLine;
    delete pLine;   // ← 반드시
    ...
}
```

---

## 2. m_SECSCommunicationOK 가드

`Send` 버튼 핸들러는 `m_SECSCommunicationOK`가 `false`이면 동작하지 않는다.  
이 플래그는 S1F13 수신(`OnBridgeMsg` 내 S1F13 처리 블록)에서만 `true`로 설정된다.  
Bridge 연결 직후(`WM_BRIDGE_CONN`)에는 아직 false다 — S1F13 교환이 완료되어야 송신이 가능하다.

---

## 3. RecvThread에서 UI 직접 조작 금지

`RecvThread`는 Win32 스레드로 MFC UI 스레드가 아니다.  
`m_lstbxMsg.AddString(...)` 같은 UI 호출을 RecvThread 내에서 하면 크래시 또는 예측 불가 동작이 발생한다.  
수신 데이터는 반드시 `PostMessage`를 통해 UI 스레드로 전달한다.

---

## 4. Disconnect()가 RecvThread를 종료시키는 방식

`Disconnect()`는 `m_running=false`로 설정한 후 `closesocket()`을 호출한다.  
소켓이 닫히면 RecvThread의 `recv()`가 0 또는 음수를 반환해 루프를 탈출한다.  
`WaitForSingleObject(m_hThread, 5000)`으로 스레드 종료를 기다린다.  
5초 내에 종료되지 않으면 핸들만 닫고 계속 진행된다 — `TerminateThread`를 추가하지 않는다.

---

## 5. MakeReply에서 id를 반드시 수신된 값 그대로 사용

릴레이는 `id` 필드로 request와 reply를 매핑한다.  
`OnBridgeMsg`에서 응답을 보낼 때 `BridgeGetStr(line, "id")`로 추출한 id를 `MakeReply`에 그대로 전달해야 한다.  
새 id를 생성하거나 빈 문자열을 전달하면 릴레이가 응답을 매핑하지 못한다.

---

## 6. 리스트박스 200개 상한

`OnBridgeMsg`, `OnBridgeConn`, `OnBnClickedBtnSendMsg` 말미에서 항목 수가 200을 넘으면 `DeleteString(0)`으로 제거한다.  
새로 로그를 추가하는 코드를 작성할 때는 이 패턴을 함께 포함한다.

```cpp
while (m_lstbxMsg.GetCount() > 200) m_lstbxMsg.DeleteString(0);
```

---

## 7. JSON 전송 시 '\n' 구분자

`CBridgeClient::SendJson`은 내부에서 `json + "\n"`을 전송한다.  
호출하는 쪽에서 `\n`을 직접 붙이지 않는다 — 중복으로 빈 줄이 전송되어 릴레이 파서가 오동작한다.

---

## 8. reply=false 수신 메시지는 응답 불필요

`OnBridgeMsg`에서 `BridgeGetBool(line, "reply")`가 `false`이면  
이건 우리가 먼저 보낸 요청에 대한 릴레이의 응답이다 — 추가 응답을 보내지 않고 로그만 출력한다.
