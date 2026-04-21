/*
 * func-name: sub_4074E6
 * func-address: 0x4074e6
 * export-type: decompile
 * callers: 0x40611d, 0x40721e, 0x407580
 * callees: none
 */

__int64 __fastcall sub_4074E6(__int64 a1)
{
  unsigned __int64 v1; // rax
  __int64 v2; // rcx
  unsigned __int64 v3; // rcx
  __m128i v4; // xmm1
  __int64 result; // rax

  *(_DWORD *)(a1 + 136) |= *(_DWORD *)(a1 + 136) - 1;
  if ( (*(_DWORD *)a1 & 8) != 0 )
  {
    *(_DWORD *)a1 |= 0x20u;
    return 0xFFFFFFFFLL;
  }
  else
  {
    v1 = *(_QWORD *)(a1 + 88);
    v2 = *(_QWORD *)(a1 + 96);
    *(_OWORD *)(a1 + 8) = 0LL;
    v3 = v1 + v2;
    v4 = (__m128i)v1;
    *(_QWORD *)(a1 + 56) = v1;
    result = 0LL;
    *(__m128i *)(a1 + 32) = _mm_unpacklo_epi64((__m128i)v3, v4);
  }
  return result;
}
