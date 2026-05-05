---
description: Visual C++ MFC 코딩 스타일, JSON 메시지 형식, 명명 규칙
alwaysApply: true
---

# 코딩 컨벤션

## C++/MFC 스타일

- **명명**: MFC 헝가리언 표기법 준수
  - 멤버 변수: `m_` 접두사 (`m_Bridge`, `m_lstbxMsg`, `m_SECSCommunicationOK`)
  - 지역 변수: 타입 접두사 (`l`=LONG, `cs`=CString, `b`=BOOL)
  - 클래스: `C` 접두사 (`CEqSampleDlg`, `CBridgeClient`)
- **문자 인코딩**: MultiByte(ANSI). JSON 내부는 항상 `std::string`(narrow). UI 표시용 `CString`은 `_T()` 매크로 사용.
- 주석은 WHY가 비명백할 때만. WHAT 설명 금지.

---

## JSON 메시지 형식

릴레이(SecsBridge)와의 통신은 **newline-delimited JSON** (`\n` 구분). 소켓으로 한 줄씩 송수신.

### 요청 (MFC → 릴레이)

```json
{
    "id":       "mfc-1",
    "stream":   1,
    "function": 13,
    "reply":    true,
    "data":     <type-tagged 값 또는 배열>
}
```

- `id`: `"mfc-<카운터>"` 형식 (자동 생성, `MakeRequest` 사용)
- `reply`: true면 릴레이가 짝수 Function 응답 수신까지 대기

### 응답 (MFC → 릴레이, reply=true 메시지에 대한 답)

```json
{
    "id":       "<수신한 id 그대로>",
    "stream":   1,
    "function": 14,
    "reply":    false,
    "data":     <type-tagged 값>
}
```

- `id`는 수신된 메시지의 id를 그대로 복사 (`MakeReply` 사용)

---

## type-tagged 값 형식

```json
{"type": "I4",      "value": 655360}
{"type": "A",       "value": "XComV2"}
{"type": "Binary",  "value": "AA=="}
{"type": "Boolean", "value": true}
```

- `Binary`는 base64 인코딩
- 배열: `[{type-tagged}, {type-tagged}, ...]`
- 중첩 배열: `[[...], [...]]`

지원 타입: `I1 I2 I4 I8 U1 U2 U4 U8 F4 F8 A JIS8 Binary Boolean`

---

## JSON 파싱 헬퍼 규칙

`BridgeGetInt`, `BridgeGetStr`, `BridgeGetBool`은 단순 문자열 탐색 기반 파서다.  
JSON 라이브러리가 없으므로, 필드명이 중복되거나 중첩 구조에서는 잘못된 값을 반환할 수 있다.  
단순한 최상위 필드 파싱 용도로만 사용하고, 복잡한 구조 파싱에는 쓰지 않는다.

---

## SECS-II 데이터 타입 상수

`EqSampleDlg.cpp` 상단 `#define` 값들이 각 타입별 테스트 샘플.  
배열 크기: `MAX_ARRAY = 7` (변경 시 `VALUE_*_ARR` 초기화 루프도 수정).

---

## Git 커밋 규칙

```
<type>: <변경 내용 요약>
```

| 타입 | 용도 |
|------|------|
| `feat` | 기능 추가 |
| `fix` | 버그 수정 |
| `design` | UI/리소스 수정 |
| `rename` | 이름 변경 |
| `remove` | 코드 삭제 |
| `docs` | 문서 수정 |
| `refactor` | 코드 개선 |

**예시**
```
feat: S2F33 송신 UI 콤보 항목 추가
fix: OnBridgeMsg에서 pLine delete 누락 수정
refactor: MakeRequest id 생성 방식 UUID로 변경
```
