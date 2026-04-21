/*
 * func-name: sub_4084CB
 * func-address: 0x4084cb
 * export-type: decompile
 * callers: 0x407f83
 * callees: none
 */

__int64 __fastcall sub_4084CB(unsigned int *a1)
{
  unsigned int v1; // eax
  unsigned __int64 v2; // rdx
  __int64 result; // rax

  a1[34] |= a1[34] - 1;
  if ( *((_QWORD *)a1 + 5) != *((_QWORD *)a1 + 7) )
    (*((void (__fastcall **)(unsigned int *, _QWORD, _QWORD))a1 + 9))(a1, 0LL, 0LL);
  v1 = *a1;
  *((_QWORD *)a1 + 7) = 0LL;
  *((_OWORD *)a1 + 2) = 0LL;
  if ( (v1 & 4) != 0 )
  {
    *a1 = v1 | 0x20;
    return 0xFFFFFFFFLL;
  }
  else
  {
    v2 = *((_QWORD *)a1 + 11) + *((_QWORD *)a1 + 12);
    result = -((v1 >> 4) & 1);
    *(__m128i *)(a1 + 2) = _mm_unpacklo_epi64((__m128i)v2, (__m128i)v2);
  }
  return result;
}
