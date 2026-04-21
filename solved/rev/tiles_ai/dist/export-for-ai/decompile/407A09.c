/*
 * func-name: sub_407A09
 * func-address: 0x407a09
 * export-type: decompile
 * callers: 0x406f39
 * callees: 0x403cdf, 0x403cfb
 */

__int64 __fastcall sub_407A09(__int64 a1)
{
  __int64 *v1; // rdx
  __int64 v2; // rax

  v1 = (__int64 *)sub_403CDF();
  v2 = *v1;
  *(_QWORD *)(a1 + 112) = *v1;
  if ( v2 )
    *(_QWORD *)(v2 + 104) = a1;
  *v1 = a1;
  sub_403CFB();
  return a1;
}
