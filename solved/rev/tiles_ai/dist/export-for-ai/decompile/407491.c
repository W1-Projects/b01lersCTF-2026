/*
 * func-name: sub_407491
 * func-address: 0x407491
 * export-type: decompile
 * callers: none
 * callees: 0x40738e
 */

size_t __fastcall sub_407491(__m128i *a1, __int64 a2, size_t a3)
{
  __m128i *v3; // r8
  __int64 v4; // r9
  size_t v5; // r10
  unsigned __int64 arg; // [rsp+8h] [rbp-10h] BYREF

  v3 = a1;
  v4 = a2;
  v5 = a3;
  a1[4].m128i_i64[1] = (__int64)sub_40738E;
  if ( (a1->m128i_i8[0] & 0x40) == 0 && sys_ioctl(a1[7].m128i_u32[2], 0x5413u, (unsigned __int64)&arg) )
    v3[9].m128i_i32[0] = -1;
  return sub_40738E(v3, v4, v5);
}
