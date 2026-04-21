/*
 * func-name: sub_4029C0
 * func-address: 0x4029c0
 * export-type: decompile
 * callers: 0x4029c0, 0x4033a0
 * callees: 0x402680, 0x4029c0, 0x4039a8
 */

__int64 __fastcall sub_4029C0(int n8, __int64 n0x63_4)
{
  __int64 n8_2; // r12
  __int64 v4; // rcx
  __int64 v6; // rdx
  int v7; // eax
  __int64 v8; // rsi
  int v9; // edx
  __int64 v10; // rdi
  __int64 v11; // r9
  char v12; // cl
  int v13; // r10d
  signed __int32 v14; // edx
  __int64 v15; // rdx
  char v16; // cl
  int v17; // esi
  int v18; // r10d
  int i; // edx
  int v20; // eax
  int v21; // edx
  int v22; // ecx
  int j; // r10d
  __int64 j_1; // rdx
  unsigned __int64 n0x90; // rcx
  unsigned __int64 n4; // rsi
  int v28; // edx
  char v29; // di
  int v30; // r14d
  _BOOL4 v31; // r8d
  bool v32; // r9
  char v33; // r14
  char v34; // dl
  unsigned __int64 n4_2; // rax
  __int64 n5; // rcx
  bool v37; // si
  __int64 v38; // r14
  unsigned __int64 n0x3FFF; // rdx
  char *m; // rdx
  __int64 v41; // rax
  __int64 n0x63_2; // r11
  int _2; // r10d
  _QWORD *v44; // rsi
  int _2_3; // r8d
  int _2_1; // eax
  char _2_2; // di
  __int64 _1_1; // rdx
  int v49; // eax
  __int64 v50; // r12
  __int64 *v51; // rax
  __int64 v52; // rax
  __int64 *n0x63_7; // rax
  unsigned __int64 n0x63_6; // rsi
  unsigned __int64 v55; // rax
  unsigned __int64 v56; // rdi
  unsigned int v57; // eax
  unsigned int v58; // eax
  int v59; // ecx
  unsigned __int64 v60; // r8
  int v61; // eax
  unsigned int n8_1; // ecx
  int v63; // eax
  int v64; // edi
  __int64 v65; // rcx
  unsigned __int64 v66; // rax
  __int64 v67; // rax
  unsigned __int64 v68; // r8
  unsigned __int64 v69; // r9
  __int64 v70; // r14
  int v71; // eax
  unsigned __int64 v72; // r8
  char *v73; // rax
  char v74; // al
  char v75; // di
  int n4_1; // ecx
  _BYTE *v77; // rcx
  int k; // eax
  int v79; // ebx
  __int64 n0x63_5; // rax
  __int64 n0x63_1; // r11
  __m128i v82; // xmm1
  unsigned __int64 n0x90_1; // rbx
  unsigned __int8 *v84; // rdx
  __m128i v85; // [rsp+0h] [rbp-68h] BYREF
  __int64 n0x63_3; // [rsp+10h] [rbp-58h]
  __int64 n0x63; // [rsp+18h] [rbp-50h]
  int j_2; // [rsp+24h] [rbp-44h]
  __int64 _1; // [rsp+28h] [rbp-40h]

  n8_2 = n8;
  v4 = n8 + 10LL;
  v6 = qword_412920[v4];
  if ( !v6 )
    goto LABEL_134;
  v7 = *(_DWORD *)(v6 + 24);
  if ( v7 )
    goto LABEL_34;
  v8 = *(_QWORD *)(v6 + 8);
  if ( *(_DWORD *)(v6 + 28) )
  {
    qword_412920[v4] = v8;
  }
  else
  {
    if ( v6 == v8 )
    {
      qword_412920[v4] = 0LL;
    }
    else
    {
      *(_QWORD *)(*(_QWORD *)v6 + 8LL) = v8;
      *(_QWORD *)v8 = *(_QWORD *)v6;
      if ( v6 == qword_412920[v4] )
        qword_412920[v4] = *(_QWORD *)(v6 + 8);
    }
    *(_OWORD *)v6 = 0LL;
    v8 = qword_412920[n8 + 10];
    if ( !v8 )
      goto LABEL_134;
  }
  v9 = *(_DWORD *)(v8 + 28);
  if ( v9 != (2 << *(_BYTE *)(v8 + 32)) - 1 )
    goto LABEL_6;
  if ( (*(_BYTE *)(v8 + 32) & 0x20) != 0 )
  {
    v8 = *(_QWORD *)(v8 + 8);
    qword_412920[n8 + 10] = v8;
    v9 = *(_DWORD *)(v8 + 28);
LABEL_6:
    v10 = *(_QWORD *)(v8 + 16);
    if ( (v9 & ((2 << *(_BYTE *)(v10 + 8)) - 1)) == 0 )
    {
      v11 = *(_QWORD *)(v8 + 8);
      if ( v8 == v11 )
      {
        v17 = (*(_BYTE *)(v10 + 8) & 0x1F) + 2;
        v18 = 16 * word_410240[(*(_WORD *)(v11 + 32) >> 6) & 0x3F];
        for ( i = v18 * v17 + 16; (i ^ (i + v18 - 1)) <= 4095; i += v18 )
          ++v17;
        v20 = (*(_BYTE *)(v11 + 32) & 0x1F) + 1;
        if ( v20 > v17 )
          LOBYTE(v20) = v17;
        *(_BYTE *)(v10 + 8) = *(_BYTE *)(v10 + 8) & 0xE0 | (v20 - 1) & 0x1F;
      }
      else
      {
        qword_412920[n8_2 + 10] = v11;
      }
      v8 = v11;
    }
  }
  if ( *(_DWORD *)(v8 + 24) )
    __halt();
  v12 = *(_BYTE *)(*(_QWORD *)(v8 + 16) + 8LL);
  v13 = (2 << v12) - 1;
  do
    v14 = *(_DWORD *)(v8 + 28);
  while ( v14 != _InterlockedCompareExchange((volatile signed __int32 *)(v8 + 28), -(2 << v12) & v14, v14) );
  v7 = v14 & v13;
  *(_DWORD *)(v8 + 24) = v14 & v13;
  if ( (v14 & v13) == 0 )
    __halt();
  v21 = (*(_WORD *)(v8 + 32) >> 6) & 0x3F;
  if ( (unsigned int)(v21 - 7) <= 0x1F )
  {
    v15 = v21 - 7;
    v16 = *((_BYTE *)&qword_412920[110] + v15);
    if ( v16 )
      *((_BYTE *)&qword_412920[110] + v15) = v16 - 1;
  }
  v6 = v8;
LABEL_34:
  v22 = v7 & -v7;
  *(_DWORD *)(v6 + 24) = v7 - v22;
  if ( v22 )
    return (unsigned int)byte_4101E0[(124511785 * (v22 & (unsigned int)-v22)) >> 27];
LABEL_134:
  n0x63_3 = n0x63_4;
  v79 = word_410240[n8_2];
  n0x63_5 = sub_402680();
  n0x63_1 = n0x63_5;
  if ( !n0x63_5 )
    return 0xFFFFFFFFLL;
  v82 = (__m128i)(unsigned __int64)n0x63_5;
  n4_2 = qword_412920[n8_2 + 58];
  n0x90_1 = (16 * v79) & 0xFFFF0;
  v85 = _mm_unpacklo_epi64(v82, v82);
  if ( n8 > 8 )
  {
    for ( j = byte_410200[n8 & 3]; (j & 1) == 0 && n4_2 < 4 * j; j >>= 1 )
      ;
    while ( n0x90_1 * j > 0xFFFFF )
      j >>= 1;
  }
  else
  {
    v84 = (unsigned __int8 *)&unk_410210 + 3 * n8_2;
    j = *v84;
    if ( n4_2 < 4 * j )
    {
      j = v84[1];
      if ( n4_2 < 4 * j )
        j = v84[2];
    }
  }
  j_1 = j;
  if ( j == 1 )
  {
    if ( n0x90_1 + 16 > 0x800 )
    {
      n0x90 = n0x90_1;
      n4 = 4LL;
      goto LABEL_46;
    }
    j_1 = 2LL;
    j = 2;
  }
  n0x90 = j_1 * n0x90_1;
  if ( j_1 * n0x90_1 + 16 <= 0x800 )
  {
    n0x63_6 = n0x90 + 12;
    v55 = n0x90 >> 4;
    if ( n0x90 <= 0x90 )
    {
      n8_1 = n0x90 >> 4;
    }
    else
    {
      v56 = v55 + 1;
      v57 = ((unsigned int)(v55 + 1) >> 1) | ((unsigned int)(v55 + 1) >> 2);
      v58 = (((v57 >> 2) | v57) >> 4) | (v57 >> 2) | v57;
      v59 = 4 * byte_4101E0[(124511785 * ((v58 + 1) & ~v58)) >> 27] - 4;
      v60 = word_410240[4 * byte_4101E0[(124511785 * ((v58 + 1) & ~v58)) >> 27] - 3];
      v61 = 4 * byte_4101E0[(124511785 * ((v58 + 1) & ~v58)) >> 27] - 2;
      if ( v60 < v56 )
        v59 = v61;
      n8_1 = (word_410240[v59] < v56) + v59;
    }
    _1 = j_1;
    j_2 = j;
    n0x63 = n0x63_1;
    LODWORD(n0x63_3) = n8_1;
    v63 = sub_4029C0(n8_1, n0x63_6);
    n0x63_2 = n0x63;
    _2 = j_2;
    _1_1 = _1;
    v64 = v63;
    if ( v63 >= 0 )
    {
      v65 = qword_412920[(int)n0x63_3 + 10];
      LODWORD(n0x63_3) = 16 * word_410240[(int)n0x63_3] - 4;
      if ( (*(_BYTE *)(v65 + 32) & 0x1F) != 0 || *(_QWORD *)(v65 + 32) <= 0xFFFuLL )
        v66 = 16 * word_410240[(*(_WORD *)(v65 + 32) >> 6) & 0x3F];
      else
        v66 = (*(_QWORD *)(v65 + 32) & 0xFFFFFFFFFFFFF000LL) - 16;
      v68 = v66 - (int)n0x63_3 - 4;
      v69 = v68 >> 4;
      v44 = (_QWORD *)(*(_QWORD *)(v65 + 16) + v66 * v64 + 16);
      v70 = (__int64)v44 + v66 - 4;
      if ( *(_BYTE *)(*(_QWORD *)(v65 + 16) + v66 * v64 + 13) )
        v71 = (unsigned __int8)(*(_WORD *)(*(_QWORD *)(v65 + 16) + v66 * v64 + 14) + 1);
      else
        v71 = (unsigned __int8)dword_41292C;
      if ( *((_BYTE *)v44 - 4) )
        __halt();
      if ( v69 < v71 )
      {
        v72 = ((v69 | (v68 >> 5)) >> 2) | v69 | (v68 >> 5);
        v71 &= (v72 >> 4) | v72;
        if ( v69 < v71 )
        {
          v71 = v71 - v69 - 1;
          if ( v69 < v71 )
            __halt();
        }
      }
      if ( v71 )
      {
        *((_WORD *)v44 - 1) = v71;
        *((_BYTE *)v44 - 3) = -32;
        v44 += 2 * v71;
        *((_BYTE *)v44 - 4) = 0;
      }
      v73 = (char *)v44 - *(_QWORD *)(v65 + 16);
      *((_BYTE *)v44 - 3) = v64;
      *((_WORD *)v44 - 1) = (unsigned __int64)(v73 - 16) >> 4;
      v74 = v64;
      v75 = 0;
      n4_1 = v70 - (_DWORD)v44 - n0x63_3;
      if ( (_DWORD)v70 - (_DWORD)v44 != (_DWORD)n0x63_3 )
      {
        *(_BYTE *)(v70 + (int)n0x63_3 - ((int)v70 - (int)v44)) = 0;
        if ( n4_1 > 4 )
        {
          *(_DWORD *)(v70 - 4) = n4_1;
          v75 = -96;
          *(_BYTE *)(v70 - 5) = 0;
          v74 = *((_BYTE *)v44 - 3);
        }
        else
        {
          v74 = *((_BYTE *)v44 - 3);
          v75 = 32 * n4_1;
        }
      }
      v77 = (char *)v44 + 12;
      *((_BYTE *)v44 - 3) = v75 + (v74 & 0x1F);
      *(_QWORD *)(n0x63_2 + 32) &= 0xFFFuLL;
      *((_BYTE *)v44 - 3) = *((_BYTE *)v44 - 3) & 0x1F | 0xC0;
      for ( k = 0; _2 >= k; ++k )
      {
        *v77 = 0;
        v77 += n0x90_1;
      }
      _2_2 = _2 - 1;
      LOBYTE(_2_3) = _2 - 1;
      goto LABEL_92;
    }
    *(_QWORD *)(n0x63 + 32) = 0LL;
    *(_OWORD *)n0x63_2 = 0LL;
    *(_OWORD *)(n0x63_2 + 16) = 0LL;
    n0x63_7 = (__int64 *)n0x63;
    if ( !n0x63 )
      goto LABEL_100;
    goto LABEL_111;
  }
  n4 = 4 * j;
LABEL_46:
  v28 = n8 - 7;
  v29 = byte_412CB0;
  if ( (unsigned int)(n8 - 7) > 0x1F )
  {
    v32 = 1;
    v31 = 0;
  }
  else
  {
    v30 = *((unsigned __int8 *)&qword_412920[106] + v28);
    LOBYTE(n0x63) = *((_BYTE *)&qword_412920[110] + v28);
    v31 = (unsigned __int8)n0x63 > 0x63u;
    v32 = (unsigned __int8)n0x63 <= 0x63u;
    if ( v30 && (unsigned __int8)byte_412CB0 - v30 <= 9 )
    {
      v33 = n0x63 + 1;
      if ( (unsigned __int8)n0x63 >= 0x63u )
        v33 = -106;
      *((_BYTE *)&qword_412920[110] + v28) = v33;
    }
  }
  v34 = v29 + 1;
  if ( v29 == -1 )
  {
    for ( m = (char *)&unk_412C70; m != (char *)&unk_412C70 + 32; *(m - 1) = 0 )
    {
      *m = 0;
      m += 2;
    }
    v34 = 1;
  }
  byte_412CB0 = v34;
  if ( (n8 & 1) == 0 )
  {
    if ( n8 > 31 )
    {
      if ( n4_2 >= n4 )
        goto LABEL_61;
    }
    else
    {
      n4_2 += qword_412920[n8 + 59];
      if ( n4_2 >= n4 )
        goto LABEL_61;
    }
    if ( v32 )
    {
      if ( (n8 & 3) == 2 )
      {
        if ( n0x90 <= 0x4000 )
          goto LABEL_61;
      }
      else if ( n0x90 <= 0x8000 )
      {
        if ( n0x90 > 0x2000 )
        {
          v37 = 1;
          n5 = 5LL;
          j = 5;
          goto LABEL_62;
        }
        goto LABEL_61;
      }
      v37 = 1;
      n5 = 3LL;
      j = 3;
      goto LABEL_62;
    }
LABEL_61:
    n5 = j;
    v37 = j <= 7;
LABEL_62:
    v38 = n0x90_1 * n5 + ((-16 - (_WORD)n0x90_1 * (_WORD)n5) & 0xFFF) + 16;
    if ( v31 || !v37 )
      goto LABEL_87;
    goto LABEL_64;
  }
  if ( n4_2 >= n4 || !v32 || (n8 & 3) != 1 || n0x90 <= 0x8000 )
    goto LABEL_61;
  j = 2;
  v38 = 2 * n0x90_1 + ((-16 - 2 * (_WORD)n0x90_1) & 0xFFF) + 16;
LABEL_64:
  n0x3FFF = ((-20 - (_WORD)n0x63_3) & 0xFFF) + n0x63_3 + 20;
  if ( n0x3FFF < n0x90_1 + 16 )
  {
    v38 = ((-20 - (_WORD)n0x63_3) & 0xFFF) + n0x63_3 + 20;
    j = 1;
  }
  else if ( n0x3FFF > 0x3FFF && n4_2 < 2 * j )
  {
    v38 = ((-20 - (_WORD)n0x63_3) & 0xFFF) + n0x63_3 + 20;
    j = 1;
  }
LABEL_87:
  LODWORD(n0x63) = j;
  n0x63_3 = n0x63_1;
  v41 = sub_4039A8(0LL, v38, 3LL, 34LL, 0xFFFFFFFFLL, 0LL);
  n0x63_2 = n0x63_3;
  _2 = n0x63;
  v44 = (_QWORD *)v41;
  if ( v41 == -1 )
  {
    *(_QWORD *)(n0x63_3 + 32) = 0LL;
    *(_OWORD *)n0x63_2 = 0LL;
    *(_OWORD *)(n0x63_2 + 16) = 0LL;
    n0x63_7 = (__int64 *)n0x63;
    if ( !n0x63 )
    {
LABEL_100:
      *(__m128i *)n0x63_2 = _mm_load_si128(&v85);
      n0x63 = n0x63_2;
      return 0xFFFFFFFFLL;
    }
LABEL_111:
    *(_QWORD *)(n0x63_2 + 8) = n0x63_7;
    v67 = *n0x63_7;
    *(_QWORD *)n0x63_2 = v67;
    *(_QWORD *)(v67 + 8) = n0x63_2;
    **(_QWORD **)(n0x63_2 + 8) = n0x63_2;
    return 0xFFFFFFFFLL;
  }
  _2_3 = n0x63 - 1;
  *(_QWORD *)(n0x63_3 + 32) = *(_QWORD *)(n0x63_3 + 32) & 0xFFFLL | v38 & 0xFFFFFFFFFFFFF000LL;
  ++dword_41292C;
  _2_1 = 0xFF0 / n0x90_1 - 1;
  if ( _2_1 >= _2 )
    _2_1 = _2_3;
  _2_2 = 0;
  _1_1 = _2;
  if ( _2_1 >= 0 )
    _2_2 = _2_1;
LABEL_92:
  qword_412920[n8_2 + 58] += _1_1;
  *(_QWORD *)(n0x63_2 + 16) = v44;
  *(_DWORD *)(n0x63_2 + 24) = (2 << _2_2) - 1;
  *(_DWORD *)(n0x63_2 + 28) = (2 << _2_3) - *(_DWORD *)(n0x63_2 + 24) - 1;
  *v44 = n0x63_2;
  *(_BYTE *)(*(_QWORD *)(n0x63_2 + 16) + 8LL) = *(_BYTE *)(*(_QWORD *)(n0x63_2 + 16) + 8LL) & 0xE0 | _2_2 & 0x1F;
  v49 = *(_DWORD *)(n0x63_2 + 24);
  *(_WORD *)(n0x63_2 + 32) = *(_WORD *)(n0x63_2 + 32) & 0xF000 | ((n8 & 0x3F) << 6) | ((_BYTE)_2 - 1) & 0x1F | 0x20;
  *(_DWORD *)(n0x63_2 + 24) = v49 - 1;
  if ( *(_QWORD *)(n0x63_2 + 8) )
    __halt();
  if ( *(_QWORD *)n0x63_2 )
    __halt();
  v50 = n8_2 + 10;
  v51 = (__int64 *)qword_412920[v50];
  if ( v51 )
  {
    *(_QWORD *)(n0x63_2 + 8) = v51;
    v52 = *v51;
    *(_QWORD *)n0x63_2 = v52;
    *(_QWORD *)(v52 + 8) = n0x63_2;
    **(_QWORD **)(n0x63_2 + 8) = n0x63_2;
  }
  else
  {
    *(__m128i *)n0x63_2 = _mm_load_si128(&v85);
    qword_412920[v50] = n0x63_2;
  }
  return 0LL;
}
