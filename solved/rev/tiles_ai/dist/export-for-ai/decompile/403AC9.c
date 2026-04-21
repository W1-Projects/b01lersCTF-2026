/*
 * func-name: sub_403AC9
 * func-address: 0x403ac9
 * export-type: decompile
 * callers: 0x403ac9
 * callees: 0x403ac9, 0x403cdf, 0x403cfb, 0x40714d, 0x4071e0
 */

__int64 __fastcall sub_403AC9(__int64 a1)
{
  int v1; // ebp
  __int64 v2; // rsi
  __int64 v3; // rax
  unsigned int v4; // ebp
  __int64 i; // rbx
  int v7; // edx
  int v8; // eax
  int v9; // [rsp+Ch] [rbp-1Ch]

  if ( a1 )
  {
    v1 = 0;
    if ( *(int *)(a1 + 140) >= 0 )
      v1 = sub_40714D(a1);
    if ( *(_QWORD *)(a1 + 40) == *(_QWORD *)(a1 + 56)
      || ((*(void (__fastcall **)(__int64, _QWORD, _QWORD))(a1 + 72))(a1, 0LL, 0LL), *(_QWORD *)(a1 + 40)) )
    {
      v2 = *(_QWORD *)(a1 + 8);
      v3 = *(_QWORD *)(a1 + 16);
      if ( v2 != v3 )
        (*(void (__fastcall **)(__int64, __int64, __int64))(a1 + 80))(a1, v2 - v3, 1LL);
      *(_QWORD *)(a1 + 56) = 0LL;
      *(_OWORD *)(a1 + 32) = 0LL;
      *(_OWORD *)(a1 + 8) = 0LL;
      if ( v1 )
        sub_4071E0(a1);
      return 0;
    }
    else
    {
      if ( v1 )
        sub_4071E0(a1);
      return (unsigned int)-1;
    }
  }
  else
  {
    v4 = 0;
    if ( off_412108 )
      v4 = sub_403AC9(off_412108);
    if ( qword_412CC8 )
      v4 |= sub_403AC9(qword_412CC8);
    for ( i = *(_QWORD *)sub_403CDF(); i; i = *(_QWORD *)(i + 112) )
    {
      if ( *(int *)(i + 140) >= 0 )
      {
        v7 = sub_40714D(i);
        if ( *(_QWORD *)(i + 40) != *(_QWORD *)(i + 56) )
        {
          v9 = v7;
          v8 = sub_403AC9(i);
          v7 = v9;
          v4 |= v8;
        }
        if ( v7 )
          sub_4071E0(i);
      }
      else if ( *(_QWORD *)(i + 40) != *(_QWORD *)(i + 56) )
      {
        v4 |= sub_403AC9(i);
      }
    }
    sub_403CFB();
  }
  return v4;
}
