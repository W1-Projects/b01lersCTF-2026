/*
 * func-name: sub_40611D
 * func-address: 0x40611d
 * export-type: decompile
 * callers: 0x403d07
 * callees: 0x405300, 0x40714d, 0x4071e0, 0x4074e6
 */

__int64 __fastcall sub_40611D(__int64 n75913, unsigned __int64 n42, const __m128i *a3)
{
  int v3; // ebx
  int v4; // ecx
  __int64 v5; // rax
  unsigned int v6; // r15d
  bool v7; // zf
  int v9; // [rsp+4h] [rbp-18Ch]
  __int64 v10; // [rsp+10h] [rbp-180h]
  __m128i m_; // [rsp+20h] [rbp-170h] BYREF
  __int64 v12; // [rsp+30h] [rbp-160h]
  _OWORD n75913_1[2]; // [rsp+38h] [rbp-158h] BYREF
  __int64 v14; // [rsp+58h] [rbp-138h]
  char v15; // [rsp+68h] [rbp-128h] BYREF
  _BYTE n0x1F[216]; // [rsp+B8h] [rbp-D8h] BYREF

  v14 = 0LL;
  memset(n75913_1, 0, sizeof(n75913_1));
  m_ = _mm_loadu_si128(a3);
  v12 = a3[1].m128i_i64[0];
  if ( (int)sub_405300(0LL, n42, (unsigned __int64)&m_, (unsigned __int64)n0x1F, (__int64)n75913_1) < 0 )
  {
    return (unsigned int)-1;
  }
  else
  {
    v3 = 0;
    if ( *(int *)(n75913 + 140) >= 0 )
      v3 = sub_40714D(n75913);
    v4 = *(_DWORD *)n75913 & 0x20;
    *(_DWORD *)n75913 &= ~0x20u;
    v9 = v4;
    if ( *(_QWORD *)(n75913 + 96) )
    {
      if ( *(_QWORD *)(n75913 + 32) || (v6 = -1, !(unsigned int)sub_4074E6(n75913)) )
        v6 = sub_405300(n75913, n42, (unsigned __int64)&m_, (unsigned __int64)n0x1F, (__int64)n75913_1);
    }
    else
    {
      v5 = *(_QWORD *)(n75913 + 88);
      *(_QWORD *)(n75913 + 96) = 80LL;
      *(_QWORD *)(n75913 + 56) = 0LL;
      v6 = -1;
      v10 = v5;
      *(_QWORD *)(n75913 + 88) = &v15;
      *(_OWORD *)(n75913 + 32) = 0LL;
      if ( !(unsigned int)sub_4074E6(n75913) )
        v6 = sub_405300(n75913, n42, (unsigned __int64)&m_, (unsigned __int64)n0x1F, (__int64)n75913_1);
      if ( v10 )
      {
        (*(void (__fastcall **)(__int64, _QWORD, _QWORD))(n75913 + 72))(n75913, 0LL, 0LL);
        v7 = *(_QWORD *)(n75913 + 40) == 0LL;
        *(_QWORD *)(n75913 + 88) = v10;
        *(_QWORD *)(n75913 + 96) = 0LL;
        *(_QWORD *)(n75913 + 56) = 0LL;
        if ( v7 )
          v6 = -1;
        *(_OWORD *)(n75913 + 32) = 0LL;
      }
    }
    if ( (*(_DWORD *)n75913 & 0x20) != 0 )
      v6 = -1;
    *(_DWORD *)n75913 |= v9;
    if ( v3 )
      sub_4071E0(n75913);
  }
  return v6;
}
