/*
 * func-name: sub_406BB6
 * func-address: 0x406bb6
 * export-type: decompile
 * callers: 0x401d60
 * callees: 0x406b37, 0x407b38
 */

__int64 __fastcall sub_406BB6(_QWORD *a1)
{
  __int64 v1; // rcx
  __int64 v2; // rdx
  __int64 v3; // rdi
  _QWORD *v4; // rax
  _QWORD *v5; // r8
  int n1685382481; // ecx
  unsigned __int64 n0x800000; // rcx
  __int64 v8; // rax
  __int64 v9; // rsi
  __int64 v10; // rdx
  unsigned __int64 n8; // rax
  __int64 v12; // rsi
  __int64 n231; // rdx
  _QWORD *v14; // rdi
  unsigned __int64 len; // rsi
  __m128i *code; // rax
  __m128i *code_1; // rbx
  int v18; // eax
  __m128i v19; // xmm0
  signed __int64 v20; // rax
  __int64 result; // rax

  v1 = a1[7];
  if ( v1 )
  {
    v2 = *(unsigned __int16 *)(v1 + 56);
    v3 = *(unsigned __int16 *)(v1 + 54);
    v4 = (_QWORD *)(v1 + *(_QWORD *)(v1 + 32));
  }
  else
  {
    v4 = (_QWORD *)a1[3];
    v2 = a1[5];
    v3 = a1[4];
  }
  v5 = 0LL;
  while ( v2 )
  {
    n1685382481 = *(_DWORD *)v4;
    if ( *(_DWORD *)v4 != 2 )
    {
      if ( n1685382481 == 7 )
      {
        v5 = v4;
      }
      else if ( n1685382481 == 1685382481 )
      {
        n0x800000 = v4[5];
        if ( (unsigned int)n0x800000 < n0x800000 )
        {
          if ( n0x800000 > 0x800000 )
            LODWORD(n0x800000) = 0x800000;
          n0x800000 = n0x800000;
        }
      }
    }
    --v2;
    v4 = (_QWORD *)((char *)v4 + v3);
  }
  if ( v5 )
  {
    v8 = v5[4];
    v9 = v5[2];
    v10 = v5[5];
    qword_413528 = v9;
    qword_413530 = v8;
    n8 = v5[6];
    qword_4128C8 = 1LL;
    n8 = n8;
    qword_4128B0 = (__int64)&unk_413520;
  }
  else
  {
    v10 = qword_413538;
    v9 = qword_413528;
    n8 = n8;
  }
  v12 = v10 + ((n8 - 1) & -(v10 + v9));
  n231 = n8 + 223;
  qword_413538 = v12;
  qword_413548 = v12;
  if ( n8 <= 7 )
  {
    n8 = 8LL;
    n231 = 231LL;
    n8 = 8LL;
  }
  n8_0 = n8;
  v14 = &unk_413560;
  len = (n231 + v12) & 0xFFFFFFFFFFFFFFF8LL;
  len = len;
  if ( len > 0x150 )
    v14 = (_QWORD *)sys_mmap(0LL, len, 3uLL, 0x22uLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  code = (__m128i *)sub_406B37(v14);
  code->m128i_i64[0] = (__int64)code;
  code_1 = code;
  v18 = sub_407B38((int)code);
  if ( v18 < 0 )
    __halt();
  v19 = _mm_unpacklo_epi64((__m128i)(unsigned __int64)code_1, (__m128i)(unsigned __int64)code_1);
  if ( !v18 )
    byte_4128A0 = 1;
  code_1[3].m128i_i32[2] = 2;
  v20 = sys_set_tid_address(&tidptr);
  code_1[3].m128i_i32[0] = v20;
  code_1[10].m128i_i64[1] = (__int64)&unk_4128D8;
  code_1[8].m128i_i64[1] = (__int64)&code_1[8].m128i_i64[1];
  result = qword_412870;
  code_1[1] = v19;
  code_1[2].m128i_i64[0] = result;
  return result;
}
