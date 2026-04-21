/*
 * func-name: sub_406B37
 * func-address: 0x406b37
 * export-type: decompile
 * callers: 0x406bb6
 * callees: 0x406300
 */

__int64 __fastcall sub_406B37(_QWORD *a1)
{
  _QWORD *v1; // rbp
  __int64 v2; // rbx
  __int64 v3; // r12
  __int64 result; // rax

  v1 = a1 + 1;
  v2 = qword_4128B0;
  v3 = ((unsigned __int64)a1 + qword_4128B8 - 200) & -qword_4128C0;
  while ( v2 )
  {
    *v1++ = v3 - *(_QWORD *)(v2 + 40);
    sub_406300((__m128i *)(v3 - *(_QWORD *)(v2 + 40)), *(const __m128i **)(v2 + 8), *(_QWORD *)(v2 + 16));
    v2 = *(_QWORD *)v2;
  }
  *a1 = qword_4128C8;
  result = v3;
  *(_QWORD *)(v3 + 8) = a1;
  return result;
}
