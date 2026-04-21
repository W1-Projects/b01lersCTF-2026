/*
 * func-name: sub_407FBC
 * func-address: 0x407fbc
 * export-type: decompile
 * callers: 0x407de1
 * callees: 0x4033a0, 0x406300, 0x406d99, 0x408431
 */

__int64 __fastcall sub_407FBC(const __m128i *a1, unsigned __int64 n0x63, __int64 n0x63_9, __int64 n0x1FFEB_1)
{
  unsigned __int64 n0x63_1; // r9
  unsigned __int64 n0xFFFF; // rdx
  signed int v7; // esi
  const __m128i *v8; // rax
  __int64 v9; // r8
  int v10; // eax
  int v11; // eax
  int v12; // ecx
  unsigned __int64 n5; // rax
  _DWORD *v15; // rax
  __int64 v16; // rcx
  unsigned __int64 v17; // rax
  unsigned __int64 v18; // r8
  unsigned int v19; // eax
  unsigned int v20; // eax
  int v21; // edi
  int n0x63_7; // eax
  int n0x63_10; // ebp
  __int64 v24; // r15
  int n0x63_11; // edi
  __int64 n0x63_4; // rsi
  __m128i *v27; // rax
  unsigned __int64 n0x63_5; // r9
  unsigned __int64 n0x63_6; // rdx
  char v30; // si
  int n4; // ecx
  __int64 v32; // rax
  char v33; // si
  unsigned __int8 n0x9F; // cl
  unsigned __int8 v35; // dl
  __int64 v36; // r11
  unsigned __int64 n0x63_8; // rdx
  unsigned __int64 n0x1FFEB; // rcx
  unsigned __int64 v39; // r10
  unsigned __int64 n131051; // rax
  __int64 v41; // rdi
  __int8 *v42; // r14
  __int64 v43; // rax
  unsigned __int64 v44; // rax
  unsigned __int64 v45; // rdx
  _BYTE *v46; // rax
  int n4_1; // edx
  unsigned __int64 n0x63_3; // [rsp+0h] [rbp-48h]
  __m128i *v49; // [rsp+0h] [rbp-48h]
  unsigned __int64 n0x63_12; // [rsp+0h] [rbp-48h]
  unsigned __int64 v51; // [rsp+8h] [rbp-40h]
  unsigned __int64 v52; // [rsp+8h] [rbp-40h]
  __int64 v53; // [rsp+10h] [rbp-38h]
  unsigned __int64 n0x63_2; // [rsp+18h] [rbp-30h]

  if ( !a1 )
    return sub_4033A0(n0x63, n0x63, n0x63_9, n0x1FFEB_1);
  n0x63_1 = n0x63;
  if ( n0x63 <= 0x7FFFFFFFFFFFEFFELL )
  {
    if ( ((unsigned __int8)a1 & 0xF) != 0 )
      __halt();
    n0xFFFF = a1[-1].m128i_u16[7];
    v7 = a1[-1].m128i_i8[13] & 0x1F;
    if ( a1[-1].m128i_i8[12] )
    {
      if ( a1[-1].m128i_i16[7] )
        __halt();
      n0xFFFF = a1[-1].m128i_i32[2];
      if ( (int)n0xFFFF <= 0xFFFF )
        __halt();
    }
    v8 = &a1[-(int)n0xFFFF];
    v9 = v8[-1].m128i_i64[0];
    if ( &v8[-1] != *(const __m128i **)(v9 + 16) )
      __halt();
    if ( v7 > (*(_BYTE *)(v9 + 32) & 0x1F) )
      __halt();
    v10 = *(_DWORD *)(v9 + 24);
    if ( _bittest(&v10, v7) )
      __halt();
    v11 = *(_DWORD *)(v9 + 28);
    if ( _bittest(&v11, v7) )
      __halt();
    if ( *(_QWORD *)(v9 & 0xFFFFFFFFFFFFF000LL) != qword_412920[0] )
      __halt();
    if ( ((*(_WORD *)(v9 + 32) >> 6) & 0x3Fu) > 0x2F )
    {
      if ( (~*(_WORD *)(v9 + 32) & 0xFC0) != 0 )
        goto LABEL_27;
    }
    else
    {
      v12 = word_410240[(*(_WORD *)(v9 + 32) >> 6) & 0x3F];
      if ( (int)n0xFFFF < v12 * v7 )
        __halt();
      if ( (int)n0xFFFF >= v12 * (v7 + 1) )
LABEL_27:
        __halt();
    }
    if ( *(_QWORD *)(v9 + 32) <= 0xFFFuLL )
    {
      n0x9F = a1[-1].m128i_u8[13];
      v35 = n0x9F & 0x1F;
    }
    else
    {
      if ( ((*(_QWORD *)(v9 + 32) & 0xFFFFFFFFFFFFF000LL) >> 4) - 1 < n0xFFFF )
        __halt();
      n0x9F = a1[-1].m128i_u8[13];
      v35 = n0x9F & 0x1F;
      if ( (*(_BYTE *)(v9 + 32) & 0x1F) == 0 )
      {
        n5 = (*(_QWORD *)(v9 + 32) & 0xFFFFFFFFFFFFF000LL) - 16;
        goto LABEL_59;
      }
    }
    n5 = 16 * word_410240[(*(_WORD *)(v9 + 32) >> 6) & 0x3F];
LABEL_59:
    v36 = *(_QWORD *)(v9 + 16) + n5 * v35 + 16;
    n0x63_8 = v36 + n5 - 4;
    LOBYTE(n5) = n0x9F >> 5;
    if ( n0x9F > 0x9Fu )
    {
      if ( (_BYTE)n5 != 5 )
        __halt();
      n5 = *(unsigned int *)(n0x63_8 - 4);
      if ( n5 <= 4 )
        __halt();
      if ( *(_BYTE *)(n0x63_8 - 5) )
        __halt();
    }
    else
    {
      n5 = (unsigned __int8)n5;
    }
    n0x1FFEB = n0x63_8 - (_QWORD)a1;
    if ( n0x63_8 - (unsigned __int64)a1 < n5 )
      __halt();
    v39 = n0x63_8 - n5;
    if ( *(_BYTE *)(n0x63_8 - n5) )
      __halt();
    if ( *(_BYTE *)n0x63_8 )
      __halt();
    n131051 = 131051LL;
    n0x63_4 = (*(_WORD *)(v9 + 32) >> 6) & 0x3F;
    if ( n0x1FFEB <= 0x1FFEB )
      n131051 = n0x63_8 - (_QWORD)a1;
    if ( n131051 >= n0x63_1 )
    {
      v17 = (n0x63_1 + 3) >> 4;
      if ( n0x63_1 + 3 <= 0x9F )
      {
        n0x63_7 = v17 + 1;
      }
      else
      {
        v18 = v17 + 1;
        v19 = ((unsigned int)(v17 + 1) >> 1) | ((unsigned int)(v17 + 1) >> 2);
        v20 = (((v19 >> 2) | v19) >> 4) | (v19 >> 2) | v19;
        v21 = 4 * byte_410EC0[(124511785 * ((((v20 >> 8) | v20) + 1) & ~((v20 >> 8) | v20))) >> 27] - 4;
        n0x63_7 = 4 * byte_410EC0[(124511785 * ((((v20 >> 8) | v20) + 1) & ~((v20 >> 8) | v20))) >> 27] - 3;
        n0x63_10 = v21 + 3;
        if ( word_410240[n0x63_7] < v18 )
        {
          v21 += 2;
          n0x63_7 = n0x63_10;
        }
        v24 = v21;
        n0x63_11 = v21 + 2;
        if ( word_410240[v24] < v18 )
          n0x63_7 = n0x63_11;
      }
      n0x63_4 = (unsigned __int8)n0x63_4;
      if ( (unsigned __int8)n0x63_4 <= n0x63_7 )
      {
        v30 = 0;
        n4 = n0x1FFEB - n0x63_1;
        if ( n4 )
        {
          *(_BYTE *)(n0x63_8 + -n4) = 0;
          if ( n4 > 4 )
          {
            *(_DWORD *)(n0x63_8 - 4) = n4;
            v30 = -96;
            *(_BYTE *)(n0x63_8 - 5) = 0;
          }
          else
          {
            v30 = 32 * n4;
          }
        }
        v16 = (__int64)a1;
        a1[-1].m128i_i8[13] = v30 + (a1[-1].m128i_i8[13] & 0x1F);
        return v16;
      }
    }
    else if ( n0x63_1 > 0x1FFEB && ((*(_WORD *)(v9 + 32) >> 6) & 0x3Fu) > 0x2F )
    {
      if ( (~*(_WORD *)(v9 + 32) & 0xFC0) != 0 )
        __halt();
      v41 = *(_QWORD *)(v9 + 16);
      n0x63_4 = *(_QWORD *)(v9 + 32) & 0xFFFFFFFFFFFFF000LL;
      v42 = &a1->m128i_i8[-v36];
      n0x63_8 = ((unsigned __int64)a1[257].m128i_u64 + n0x63_1 - v36 + 3) & 0xFFFFFFFFFFFFF000LL;
      if ( n0x63_4 != n0x63_8 )
      {
        n0x63_2 = n0x63_1;
        v53 = v9;
        v52 = v39;
        v32 = sub_408431(v41, n0x63_4, n0x63_8, 1, v9, n0x63_1, 0);
        n0x63_1 = n0x63_2;
        v9 = v53;
        v39 = v52;
        n0x63_8 = n0x63_12;
        v41 = v32;
      }
      if ( v41 != -1 )
      {
        v43 = *(_QWORD *)(v9 + 32);
        *(_QWORD *)(v9 + 16) = v41;
        v16 = (__int64)&v42[v41 + 16];
        v33 = 0;
        v44 = n0x63_8 | v43 & 0xFFF;
        v45 = n0x63_8 - 20;
        *(_QWORD *)(v9 + 32) = v44;
        v46 = (_BYTE *)(v41 + v45 + 16);
        *v46 = 0;
        n4_1 = v45 - (_DWORD)v42 - n0x63_1;
        if ( n4_1 )
        {
          v46[-n4_1] = 0;
          if ( n4_1 > 4 )
          {
            *((_DWORD *)v46 - 1) = n4_1;
            v33 = -96;
            *(v46 - 5) = 0;
          }
          else
          {
            v33 = 32 * n4_1;
          }
        }
        v42[v41 + 13] = v33 + (v42[v41 + 13] & 0x1F);
        return v16;
      }
    }
    v51 = v39;
    n0x63_3 = n0x63_1;
    v27 = (__m128i *)sub_4033A0(n0x63_1, n0x63_4, n0x63_8, n0x1FFEB);
    v16 = (__int64)v27;
    if ( v27 )
    {
      n0x63_5 = n0x63_3;
      v49 = v27;
      n0x63_6 = n0x63_5;
      if ( v51 - (unsigned __int64)a1 <= n0x63_5 )
        n0x63_6 = v51 - (_QWORD)a1;
      sub_406300(v27, a1, n0x63_6);
      ((void (__fastcall *)(const __m128i *))((char *)&qword_402008[148] + 5))(a1);
      return (__int64)v49;
    }
    return v16;
  }
  v15 = (_DWORD *)_readfsqword_();
  v16 = 0LL;
  *v15 = 12;
  return v16;
}
