/*
 * func-name: sub_407EF2
 * func-address: 0x407ef2
 * export-type: decompile
 * callers: 0x407f42
 * callees: 0x40714d
 */

void __fastcall sub_407EF2(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, u32 val3)
{
  __int64 v6; // rsi
  __int64 v7; // rax

  if ( a1 )
  {
    if ( *(int *)(a1 + 140) >= 0 )
      sub_40714D(a1, a2, a3, a4, a5, val3);
    if ( *(_QWORD *)(a1 + 40) != *(_QWORD *)(a1 + 56) )
      (*(void (__fastcall **)(__int64, _QWORD, _QWORD))(a1 + 72))(a1, 0LL, 0LL);
    v6 = *(_QWORD *)(a1 + 8);
    v7 = *(_QWORD *)(a1 + 16);
    if ( v6 != v7 )
      (*(void (__fastcall **)(__int64, __int64, __int64))(a1 + 80))(a1, v6 - v7, 1LL);
  }
}
