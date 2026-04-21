/*
 * func-name: sub_407580
 * func-address: 0x407580
 * export-type: decompile
 * callers: 0x403fe0, 0x404080, 0x405300, 0x407659
 * callees: 0x406300, 0x4074e6
 */

unsigned __int64 __fastcall sub_407580(const __m128i *a1, unsigned __int64 n0x1F_1, __int64 a3)
{
  const __m128i *v3; // rcx
  __int64 v5; // rax
  __m128i *v6; // rdi
  unsigned __int64 n0x1F_3; // rdx
  unsigned __int64 n0x1F; // r8
  unsigned __int64 n0x1F_5; // rax
  unsigned __int64 n0x1F_2; // [rsp+0h] [rbp-28h]
  unsigned __int64 n0x1F_4; // [rsp+0h] [rbp-28h]
  const __m128i *v12; // [rsp+8h] [rbp-20h]

  v3 = a1;
  v5 = *(_QWORD *)(a3 + 32);
  if ( !v5 )
  {
    if ( (unsigned int)sub_4074E6(a3) )
      return 0LL;
    v5 = *(_QWORD *)(a3 + 32);
    v3 = a1;
  }
  v6 = *(__m128i **)(a3 + 40);
  if ( v5 - (__int64)v6 < n0x1F_1 )
    return (*(__int64 (__fastcall **)(__int64, const __m128i *, unsigned __int64))(a3 + 72))(a3, v3, n0x1F_1);
  n0x1F_3 = n0x1F_1;
  if ( *(int *)(a3 + 144) >= 0 )
  {
    while ( n0x1F_3 )
    {
      if ( v3->m128i_i8[n0x1F_3 - 1] == 10 )
      {
        n0x1F_4 = n0x1F_3;
        v12 = v3;
        n0x1F_5 = (*(__int64 (__fastcall **)(__int64, const __m128i *))(a3 + 72))(a3, v3);
        if ( n0x1F_5 < n0x1F_4 )
          return n0x1F_5;
        v6 = *(__m128i **)(a3 + 40);
        n0x1F = n0x1F_1 - n0x1F_4;
        v3 = (const __m128i *)((char *)v12 + n0x1F_4);
        goto LABEL_5;
      }
      --n0x1F_3;
    }
  }
  n0x1F = n0x1F_1;
LABEL_5:
  n0x1F_2 = n0x1F;
  sub_406300(v6, v3, n0x1F);
  *(_QWORD *)(a3 + 40) += n0x1F_2;
  return n0x1F_1;
}
