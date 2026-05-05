<XCom 2.4>
<S1F1 P Are You There
>

<S1F2 S Online Data
  <LIST 2 
    <ASCII 10 MDLN>
    <ASCII 10 SOFTREV>
  >
>

<S1F13 P Establish Communications Request
  <LIST 0 >
>

<S1F13 P Establish Communications Request
  <LIST 2 
    <ASCII 10 MDLN>
    <ASCII 10 SOFTREV>
  >
>

<S1F14 S Establish Communications Request ACK
  <LIST 2 
    <BINARY 1 COMMACK>
    <LIST 0 >
  >
>

<S1F14 S Establish Communications Request ACK
  <LIST 2 
    <BINARY 1 COMMACK>
    <LIST 2 
      <ASCII 10 MDLN>
      <ASCII 10 SOFTREV>
    >
  >
>

<S2F13 P Equipment Constant Request
  <LIST n 
    <UINT2 1 ECID>
  >
>

<S2F14 S Equipment Constant Data
    UNDEFINED STRUCTURE
> 

<S2F33 P Report Data
  <LIST 2 
    <LIST 10 
      <INT1 1 Int 1>
      <INT2 1 Int 2>
      <INT4 1 Int 4>
      <INT8 1 Int 8>
      <UINT1 1 Uint 1>
      <UINT2 1 Uint 2>
      <UINT4 1 Uint 4>
      <UINT8 1 Uint 8>
      <FLOAT4 1 Float 4>
      <FLOAT8 1 Float 8>
    >
    <LIST 4 
      <ASCII 100 Ascii>
      <JIS8 100 Jis 8>
      <BINARY 1 Binary>
      <BOOL 1 Bool>
    >
  >
>

<S2F34 S Report Data Ack
  <BINARY 1 RDACK>
>

<S5F23 P 
  <LIST 12 
    <BOOL n BoolArray>
    <BINARY n BinaryArray>
    <UINT1 n U1Array>
    <UINT2 n U2Array>
    <UINT4 n U4Array>
    <UINT8 n U8Array>
    <INT1 n I1Array>
    <INT2 n I2Array>
    <INT4 n I4Array>
    <INT8 n I8Array>
    <FLOAT4 n F4Array>
    <FLOAT8 n F8Array>
  >
>

<S5F24 S 
  <UINT1 1 ACK5>
>

