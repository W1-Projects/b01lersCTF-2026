/*
 * func-name: sub_407F42
 * func-address: 0x407f42
 * export-type: decompile
 * callers: 0x401040, 0x407534, 0x40853f
 * callees: 0x403cdf, 0x407ef2
 */

void __fastcall sub_407F42(__int64 a1, __int64 a2)
{
  __int64 v2; // rdx
  __int64 v3; // rcx
  __int64 i; // rbx
  __int64 v5; // r8
  u32 val3; // r9d
  __int64 v7; // rdx
  __int64 v8; // rcx
  __int64 v9; // r8
  u32 val3_1; // r9d
  __int64 v11; // rdx
  __int64 v12; // rcx
  __int64 v13; // r8
  u32 val3_2; // r9d

  for ( i = *(_QWORD *)sub_403CDF(); i; i = *(_QWORD *)(i + 112) )
    sub_407EF2(i, a2, v2, v3, v5, val3);
  sub_407EF2((__int64)::i, a2, v2, v3, v5, val3);
  sub_407EF2((__int64)i_0, a2, v7, v8, v9, val3_1);
  sub_407EF2(i_1, a2, v11, v12, v13, val3_2);
}
