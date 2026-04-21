/*
 * func-name: sub_406AC7
 * func-address: 0x406ac7
 * export-type: decompile
 * callers: none
 * callees: 0x407b38
 */

__int64 __fastcall sub_406AC7(__m128i *code)
{
  int v1; // eax
  __m128i v2; // xmm0
  signed __int64 v3; // rax
  __int64 v4; // rax

  code->m128i_i64[0] = (__int64)code;
  v1 = sub_407B38((int)code);
  if ( v1 < 0 )
    return 0xFFFFFFFFLL;
  v2 = _mm_unpacklo_epi64((__m128i)(unsigned __int64)code, (__m128i)(unsigned __int64)code);
  if ( !v1 )
    byte_4128A0 = 1;
  code[3].m128i_i32[2] = 2;
  v3 = sys_set_tid_address(&tidptr);
  code[3].m128i_i32[0] = v3;
  code[10].m128i_i64[1] = (__int64)&unk_4128D8;
  code[8].m128i_i64[1] = (__int64)&code[8].m128i_i64[1];
  v4 = qword_412870;
  code[1] = v2;
  code[2].m128i_i64[0] = v4;
  return 0LL;
}
