/*
 * func-name: sub_405300
 * func-address: 0x405300
 * export-type: decompile
 * callers: 0x40611d
 * callees: 0x403e59, 0x403fe0, 0x404080, 0x4068ed, 0x406d99, 0x406dd3, 0x406f21, 0x407580
 */

__int64 __fastcall sub_405300(
        __int64 n75913,
        unsigned __int64 n42,
        unsigned __int64 m,
        unsigned __int64 n0x1F,
        __int64 n75913_1)
{
  unsigned __int8 *n75913_8; // r10
  unsigned int *n75913_5; // r15
  int n214748364_3; // r11d
  unsigned int v8; // r12d
  unsigned __int8 n37; // al
  unsigned __int8 *i_1; // r13
  unsigned __int8 *i; // rbx
  __int64 n214748364_12; // r13
  int n0x7FFFFFFF; // r8d
  unsigned int n42_3; // edx
  int n75913_14; // eax
  char *i_2; // rax
  unsigned int v17; // ebp
  int n16; // r9d
  __int64 v19; // rdx
  unsigned int v20; // eax
  int *v21; // rdx
  unsigned int v22; // eax
  int v23; // edx
  unsigned int v24; // eax
  _DWORD *v25; // rdx
  char *i_3; // rbx
  int v27; // eax
  __int64 v28; // rax
  char *j; // rax
  int n9; // edx
  int v31; // edx
  unsigned int k; // eax
  char v33; // al
  __int64 v34; // rax
  unsigned __int64 n214748364_1; // r13
  int v36; // eax
  int n16_6; // eax
  char m_5; // cl
  int n2; // r8d
  unsigned __int64 v40; // rax
  char v41; // dl
  const char *m_8; // rax
  unsigned __int64 m_6; // rcx
  char m_9; // al
  _BYTE *n214748364_4; // rax
  bool v46; // r13
  unsigned int *v47; // rax
  __int64 (null)_3; // rax
  const char *(null); // r10
  __int64 n2_6; // rax
  int n2_4; // r8d
  const char *n2_3; // rax
  __int64 n2_5; // rax
  int n42_2; // eax
  int n214748364_2; // eax
  int n214748364_13; // eax
  __int64 n10; // r8
  unsigned __int64 n0x1F_1; // [rsp-10h] [rbp-E8h]
  unsigned __int64 n42_1; // [rsp-8h] [rbp-E0h]
  unsigned __int8 *n75913_13; // [rsp+0h] [rbp-D8h]
  unsigned __int64 n214748364; // [rsp+0h] [rbp-D8h]
  const char *(null)_2; // [rsp+0h] [rbp-D8h]
  const char *(null)_1; // [rsp+0h] [rbp-D8h]
  int n16_3; // [rsp+0h] [rbp-D8h]
  int n16_2; // [rsp+0h] [rbp-D8h]
  int n16_4; // [rsp+0h] [rbp-D8h]
  int n214748364_11; // [rsp+0h] [rbp-D8h]
  int n214748364_10; // [rsp+0h] [rbp-D8h]
  int n214748364_5; // [rsp+8h] [rbp-D0h]
  const char *m_3; // [rsp+8h] [rbp-D0h]
  int n214748364_6; // [rsp+8h] [rbp-D0h]
  int n16_9; // [rsp+8h] [rbp-D0h]
  int n214748364_9; // [rsp+8h] [rbp-D0h]
  int n214748364_7; // [rsp+8h] [rbp-D0h]
  unsigned int m_2; // [rsp+14h] [rbp-C4h]
  char *m_1; // [rsp+18h] [rbp-C0h]
  int n75913_3; // [rsp+20h] [rbp-B8h]
  unsigned int k_1; // [rsp+20h] [rbp-B8h]
  int n16_1; // [rsp+20h] [rbp-B8h]
  int n2_1; // [rsp+20h] [rbp-B8h]
  int n16_5; // [rsp+20h] [rbp-B8h]
  int n16_7; // [rsp+20h] [rbp-B8h]
  unsigned __int64 n0x1F_2; // [rsp+20h] [rbp-B8h]
  unsigned __int64 n0x1F_3; // [rsp+20h] [rbp-B8h]
  _BOOL4 v86; // [rsp+28h] [rbp-B0h]
  const char *m_4; // [rsp+28h] [rbp-B0h]
  __int64 n0x1F_4; // [rsp+28h] [rbp-B0h]
  __int64 n75913_2; // [rsp+38h] [rbp-A0h]
  unsigned __int64 k_2; // [rsp+40h] [rbp-98h]
  char *_____0X0x; // [rsp+40h] [rbp-98h]
  bool v93; // [rsp+40h] [rbp-98h]
  unsigned __int8 *n75913_12; // [rsp+40h] [rbp-98h]
  unsigned __int8 *n75913_10; // [rsp+40h] [rbp-98h]
  int n16_8; // [rsp+48h] [rbp-90h]
  int n214748364_8; // [rsp+48h] [rbp-90h]
  int n2_2; // [rsp+50h] [rbp-88h]
  unsigned __int8 *n75913_11; // [rsp+50h] [rbp-88h]
  unsigned __int8 *n75913_9; // [rsp+58h] [rbp-80h]
  _BYTE n75913_6[4]; // [rsp+64h] [rbp-74h] BYREF
  _DWORD m_7[2]; // [rsp+68h] [rbp-70h] BYREF
  __m128i n75913_4[2]; // [rsp+70h] [rbp-68h] BYREF
  char v104; // [rsp+97h] [rbp-41h] BYREF
  _BYTE n75913_7[64]; // [rsp+98h] [rbp-40h] BYREF

  n75913_8 = (unsigned __int8 *)n42;
  n75913_5 = (unsigned int *)n75913;
  n214748364_3 = 0;
  v8 = 0;
  m_1 = (char *)m;
  n75913_2 = n0x1F;
  m_2 = 0;
LABEL_15:
  if ( (int)(0x7FFFFFFF - v8) < n214748364_3 )
    goto LABEL_63;
  n0x7FFFFFFF = 0x7FFFFFFF;
  while ( 1 )
  {
    n37 = *n75913_8;
    v8 += n214748364_3;
    if ( !*n75913_8 )
    {
      if ( n75913_5 )
        return v8;
      m = m_2;
      n10 = 1LL;
      n75913 = n75913_2 + 16;
      if ( m_2 )
      {
        while ( 1 )
        {
          n42 = *(unsigned int *)(n75913_1 + 4 * n10);
          if ( !(_DWORD)n42 )
            break;
          sub_403E59((long double *)n75913, n42, (int *)m_1);
          n75913 += 16LL;
          if ( n10 == 10 )
            return 1;
        }
        while ( n10 != 10 )
        {
          if ( *(_DWORD *)(n75913_1 + 4 * n10) )
            goto LABEL_230;
          ++n10;
        }
        return 1;
      }
      return 0;
    }
    i_1 = n75913_8;
    while ( n37 && n37 != 37 )
      n37 = *++i_1;
    for ( i = i_1; *i == 37 && i[1] == 37; i += 2 )
      ++i_1;
    n214748364_12 = i_1 - n75913_8;
    n214748364_5 = n0x7FFFFFFF - v8;
    if ( n214748364_12 > (int)(n0x7FFFFFFF - v8) )
      goto LABEL_63;
    n214748364_3 = n214748364_12;
    if ( n75913_5 && (*(_BYTE *)n75913_5 & 0x20) == 0 )
    {
      n75913 = (__int64)n75913_8;
      n42 = (int)n214748364_12;
      n75913_13 = n75913_8;
      sub_407580(n75913_8, (int)n214748364_12, n75913_5);
      n214748364_3 = n214748364_12;
      n75913_8 = n75913_13;
      n0x7FFFFFFF = 0x7FFFFFFF;
    }
    if ( (_DWORD)n214748364_12 )
      goto LABEL_14;
    n42_3 = (char)i[1];
    n75913_14 = n42_3 - 48;
    n42 = n42_3;
    if ( n42_3 - 48 <= 9 && i[2] == 36 )
    {
      n42_3 = (char)i[3];
      n75913_3 = n75913_14;
      i_2 = (char *)(i + 3);
      m_2 = 1;
      n42 = n42_3;
    }
    else
    {
      n75913_3 = -1;
      i_2 = (char *)(i + 1);
    }
    v17 = 0;
    n75913 = 75913LL;
    while ( 1 )
    {
      n0x1F = n42_3 - 32;
      if ( (unsigned int)n0x1F > 0x1F )
        goto LABEL_64;
      if ( !_bittest((const int *)&n75913, n0x1F) )
        break;
      ++i_2;
      v17 |= 1 << (n42_3 - 32);
      n42_3 = *i_2;
      n42 = n42_3;
    }
    if ( (_BYTE)n42 != 42 )
      break;
    n42 = (unsigned int)i_2[1];
    m = (unsigned int)(n42 - 48);
    if ( (unsigned int)m <= 9 )
    {
      if ( i_2[2] == 36 )
      {
        i = (unsigned __int8 *)(i_2 + 3);
        v19 = i_2[1] - 48LL;
        if ( !n75913_5 )
        {
          n75913 = n75913_1;
          m_2 = 1;
          n16 = 0;
          *(_DWORD *)(n75913_1 + 4 * v19) = 10;
          n42 = (unsigned __int8)i_2[3];
          goto LABEL_43;
        }
        m_2 = 1;
        n16 = *(_DWORD *)(n75913_2 + 16 * v19);
      }
      else
      {
        if ( m_2 )
          goto LABEL_230;
        i = (unsigned __int8 *)(i_2 + 1);
        if ( !n75913_5 )
        {
          v86 = 0;
          n16 = 0;
          LODWORD(n214748364) = -1;
          goto LABEL_84;
        }
LABEL_38:
        n75913 = (__int64)m_1;
        v20 = *(_DWORD *)m_1;
        if ( *(_DWORD *)m_1 > 0x2Fu )
        {
          n75913 = (__int64)m_1;
          v21 = (int *)*((_QWORD *)m_1 + 1);
          *((_QWORD *)m_1 + 1) = v21 + 2;
        }
        else
        {
          v21 = (int *)(*((_QWORD *)m_1 + 2) + v20);
          *(_DWORD *)m_1 = v20 + 8;
        }
        m_2 = 0;
        n16 = *v21;
      }
      v22 = v17;
      n42 = *i;
      if ( n16 < 0 )
      {
        BYTE1(v22) = BYTE1(v17) | 0x20;
        v17 = v22;
        n16 = -n16;
      }
      goto LABEL_43;
    }
    n75913 = m_2;
    if ( m_2 )
      goto LABEL_230;
    i = (unsigned __int8 *)(i_2 + 1);
    n16 = 0;
    if ( n75913_5 )
      goto LABEL_38;
LABEL_43:
    if ( (_BYTE)n42 == 46 )
    {
      v23 = (char)i[1];
      if ( i[1] == 42 )
      {
        m = (unsigned int)(char)i[2];
        if ( (unsigned int)(m - 48) <= 9 && i[3] == 36 )
        {
          v28 = (char)i[2] - 48LL;
          if ( n75913_5 )
          {
            n75913 = n75913_2;
            LODWORD(n214748364) = *(_QWORD *)(n75913_2 + 16 * v28);
            v86 = (n214748364 & 0x80000000) == 0LL;
          }
          else
          {
            n75913 = n75913_1;
            LODWORD(n214748364) = 0;
            v86 = 1;
            *(_DWORD *)(n75913_1 + 4 * v28) = 10;
          }
          i += 4;
        }
        else
        {
          n42 = m_2;
          if ( m_2 )
            goto LABEL_230;
          if ( n75913_5 )
          {
            n75913 = (__int64)m_1;
            v24 = *(_DWORD *)m_1;
            if ( *(_DWORD *)m_1 > 0x2Fu )
            {
              n75913 = (__int64)m_1;
              v25 = (_DWORD *)*((_QWORD *)m_1 + 1);
              *((_QWORD *)m_1 + 1) = v25 + 2;
            }
            else
            {
              v25 = (_DWORD *)(*((_QWORD *)m_1 + 2) + v24);
              *(_DWORD *)m_1 = v24 + 8;
            }
            LODWORD(n214748364) = *v25;
            v86 = *v25 >= 0;
          }
          else
          {
            LODWORD(n214748364) = 0;
            v86 = 1;
          }
          i += 2;
        }
      }
      else
      {
        LODWORD(n214748364) = 0;
        for ( j = (char *)(i + 1); ; v23 = *j )
        {
          n9 = v23 - 48;
          if ( (unsigned int)n9 > 9 )
          {
            v86 = 1;
            i = (unsigned __int8 *)j;
            goto LABEL_84;
          }
          if ( (int)n214748364 > 214748364 )
            break;
          n0x1F = (unsigned int)(-10 * n214748364);
          n42 = (unsigned int)(n0x1F + 0x7FFFFFFF);
          if ( n9 > (int)n42 )
            break;
          ++j;
          LODWORD(n214748364) = n9 - n0x1F;
        }
        while ( 1 )
        {
          i = (unsigned __int8 *)(j + 1);
          if ( (unsigned int)(j[1] - 48) > 9 )
            break;
          v31 = j[2];
          j += 2;
          if ( (unsigned int)(v31 - 48) > 9 )
          {
            i = (unsigned __int8 *)j;
            break;
          }
        }
        v86 = 1;
        LODWORD(n214748364) = -1;
      }
    }
    else
    {
      v86 = 0;
      LODWORD(n214748364) = -1;
    }
LABEL_84:
    for ( k = 0; ; k = n42 )
    {
      m = (unsigned int)((char)*i - 65);
      if ( (unsigned int)m > 0x39 )
        goto LABEL_230;
      n0x1F = k;
      ++i;
      n42 = byte_410400[58 * k + (int)m];
      m = (unsigned int)(n42 - 1);
      if ( (unsigned int)m > 7 )
        break;
    }
    if ( !(_DWORD)n42 )
    {
LABEL_230:
      *(_DWORD *)sub_406D99(n75913, n42, m, n0x1F) = 22;
      return (unsigned int)-1;
    }
    if ( (_DWORD)n42 == 27 )
    {
      if ( n75913_3 != -1 )
        goto LABEL_230;
      if ( n75913_5 )
        goto LABEL_93;
    }
    else
    {
      n75913 = (unsigned int)n75913_3;
      if ( n75913_3 == -1 )
      {
        if ( !n75913_5 )
          return 0;
        n75913 = (__int64)n75913_4;
        k_1 = k;
        k_2 = k;
        sub_403E59((long double *)n75913_4[0].m128i_i8, n42, (int *)m_1);
        k = k_1;
        n0x1F = k_2;
LABEL_93:
        if ( (*(_BYTE *)n75913_5 & 0x20) != 0 )
          return (unsigned int)-1;
        n42 = (unsigned int)(char)*(i - 1);
        if ( k )
        {
          n75913 = *(i - 1) & 0xF;
          if ( (_BYTE)n75913 == 3 )
            n42 = (unsigned int)(char)(*(i - 1) & 0xDF);
        }
        if ( (v17 & 0x2000) != 0 )
          v17 &= ~0x10000u;
        m = (unsigned int)(n42 - 65);
        switch ( (int)n42 )
        {
          case 'A':
          case 'E':
          case 'F':
          case 'G':
          case 'a':
          case 'e':
          case 'f':
          case 'g':
            n75913 = (unsigned int)n214748364;
            if ( (n214748364 & 0x80000000) != 0LL && v86 )
              goto LABEL_63;
            n75913 = (__int64)n75913_5;
            n214748364_13 = sub_404080(n75913_5, n16, n214748364, v17, n42, n16, *(long double *)n75913_4);
            n0x1F = n0x1F_1;
            n42 = n42_1;
            n214748364_3 = n214748364_13;
            if ( n214748364_13 == -1 )
              goto LABEL_63;
            goto LABEL_14;
          case 'C':
            v33 = n75913_4[0].m128i_i8[0];
            if ( !n75913_4[0].m128i_i64[0] )
              goto LABEL_179;
            m_7[0] = n75913_4[0].m128i_i32[0];
            m_7[1] = 0;
            n75913_4[0].m128i_i64[0] = (__int64)m_7;
            m_3 = (const char *)m_7;
            n214748364 = -1LL;
LABEL_111:
            m = (unsigned __int64)m_3;
            n214748364_1 = 0LL;
            while ( n214748364_1 < n214748364 )
            {
              n42 = *(unsigned int *)m;
              if ( !(_DWORD)n42 )
                break;
              n75913 = (__int64)n75913_6;
              n16_1 = n16;
              m_4 = (const char *)m;
              v36 = sub_406F21(n75913_6);
              if ( v36 < 0 )
                return (unsigned int)-1;
              n16 = n16_1;
              m = (unsigned __int64)m_4;
              n0x1F = n214748364 - n214748364_1;
              if ( n214748364 - n214748364_1 >= v36 )
              {
                m = (unsigned __int64)(m_4 + 4);
                n214748364_1 += v36;
                continue;
              }
              break;
            }
            if ( n214748364_1 > 0x7FFFFFFF )
              goto LABEL_63;
            v93 = n16 <= (int)n214748364_1;
            if ( (v17 & 0x12000) == 0 && n16 > (int)n214748364_1 )
            {
              n42 = 32LL;
              n75913 = (__int64)n75913_5;
              n16_2 = n16;
              sub_403FE0(n75913_5, 32, n16, n214748364_1);
              n16 = n16_2;
            }
            n0x1F = 0LL;
            while ( n0x1F < n214748364_1 )
            {
              n42 = *(unsigned int *)m_3;
              if ( !(_DWORD)n42 )
                break;
              n75913 = (__int64)n75913_6;
              n16_3 = n16;
              n0x1F_2 = n0x1F;
              n42_2 = sub_406F21(n75913_6);
              n16 = n16_3;
              n42 = n42_2;
              n0x1F = n42_2 + n0x1F_2;
              if ( n214748364_1 < n0x1F )
                break;
              m_3 += 4;
              if ( (*(_BYTE *)n75913_5 & 0x20) == 0 )
              {
                n75913 = (__int64)n75913_6;
                n0x1F_3 = n42_2 + n0x1F_2;
                sub_407580(n75913_6, n42_2, n75913_5);
                n0x1F = n0x1F_3;
                n16 = n16_3;
              }
            }
            if ( ((v17 ^ 0x2000) & 0x12000) == 0 && !v93 )
            {
              n42 = 32LL;
              n75913 = (__int64)n75913_5;
              n16_4 = n16;
              sub_403FE0(n75913_5, 32, n16, n214748364_1);
              n16 = n16_4;
            }
            n214748364_2 = n214748364_1;
            if ( n16 >= (int)n214748364_1 )
              n214748364_2 = n16;
            n214748364_3 = n214748364_2;
            goto LABEL_14;
          case 'S':
            n214748364 = (int)n214748364;
            m_3 = (const char *)n75913_4[0].m128i_i64[0];
            goto LABEL_111;
          case 'X':
          case 'x':
            n75913 = n42 & 0x20;
            goto LABEL_109;
          case 'c':
            v33 = n75913_4[0].m128i_i8[0];
LABEL_179:
            v104 = v33;
            n2 = 0;
            v17 &= ~0x10000u;
            n2_1 = 1;
            n75913_8 = (unsigned __int8 *)&v104;
            _____0X0x = "-+   0X0x";
            LODWORD(n214748364) = 1;
            n0x1F_4 = 1LL;
            goto LABEL_161;
          case 'd':
          case 'i':
            m_8 = (const char *)n75913_4[0].m128i_i64[0];
            if ( n75913_4[0].m128i_i64[0] < 0 )
            {
              m_8 = (const char *)-n75913_4[0].m128i_i64[0];
              n75913 = (__int64)"-+   0X0x";
              n2 = 1;
              n75913_4[0].m128i_i64[0] = -n75913_4[0].m128i_i64[0];
              _____0X0x = "-+   0X0x";
            }
            else if ( (v17 & 0x800) != 0 )
            {
              n75913 = (__int64)"+   0X0x";
              n2 = 1;
              _____0X0x = "+   0X0x";
            }
            else if ( (v17 & 1) != 0 )
            {
              n75913 = (__int64)"   0X0x";
              n2 = 1;
              _____0X0x = "   0X0x";
            }
            else
            {
              n75913 = (__int64)"-+   0X0x";
              n2 = 0;
              _____0X0x = "-+   0X0x";
            }
            goto LABEL_141;
          case 'm':
            n16_5 = n16;
            v47 = (unsigned int *)sub_406D99(n75913, n42, m, n0x1F);
            (null)_3 = sub_406DD3(*v47);
            n16 = n16_5;
            (null) = (const char *)(null)_3;
            goto LABEL_170;
          case 'n':
            switch ( k )
            {
              case 0u:
                *(_DWORD *)n75913_4[0].m128i_i64[0] = v8;
                break;
              case 1u:
              case 2u:
              case 6u:
              case 7u:
                m = n75913_4[0].m128i_i64[0];
                *(_QWORD *)n75913_4[0].m128i_i64[0] = (int)v8;
                break;
              case 3u:
                *(_WORD *)n75913_4[0].m128i_i64[0] = v8;
                break;
              case 4u:
                *(_BYTE *)n75913_4[0].m128i_i64[0] = v8;
                break;
              default:
                goto LABEL_104;
            }
            goto LABEL_104;
          case 'o':
            n75913 = (__int64)n75913_7;
            v40 = n75913_4[0].m128i_i64[0];
            n75913_8 = n75913_7;
            while ( v40 )
            {
              v41 = v40;
              --n75913_8;
              v40 >>= 3;
              m = (v41 & 7u) + 48;
              *n75913_8 = m;
            }
            if ( (v17 & 8) == 0 )
              goto LABEL_148;
            m = (int)n214748364;
            if ( n75913_7 - n75913_8 < (int)n214748364 )
              goto LABEL_148;
            n2 = 0;
            LODWORD(n214748364) = (unsigned int)n75913_7 - (_DWORD)n75913_8 + 1;
            _____0X0x = "-+   0X0x";
            goto LABEL_149;
          case 'p':
            n16_6 = 16;
            n42 = 120LL;
            if ( (unsigned int)n214748364 >= 0x10 )
              n16_6 = n214748364;
            v17 |= 8u;
            n75913 = 32LL;
            LODWORD(n214748364) = n16_6;
LABEL_109:
            v34 = n75913_4[0].m128i_i64[0];
            n75913_8 = n75913_7;
            for ( m = n75913_4[0].m128i_i64[0]; m; m >>= 4 )
            {
              m_5 = m;
              *--n75913_8 = n75913 | byte_4103F0[m_5 & 0xF];
            }
            if ( v34 && (v17 & 8) != 0 )
            {
              m = (unsigned __int64)"-+   0X0x";
              n2 = 2;
              _____0X0x = &a0x0x[(int)n42 >> 4];
            }
            else
            {
LABEL_148:
              n2 = 0;
              _____0X0x = "-+   0X0x";
            }
            goto LABEL_149;
          case 's':
            (null) = (const char *)n75913_4[0].m128i_i64[0];
            if ( !n75913_4[0].m128i_i64[0] )
              (null) = "(null)";
LABEL_170:
            n16_7 = n16;
            if ( (n214748364 & 0x80000000) != 0LL )
            {
              n75913 = (__int64)(null);
              n42 = 0x7FFFFFFFLL;
              (null)_1 = (null);
              n2_5 = sub_4068ED((null), 0x7FFFFFFFLL);
              n75913_8 = (unsigned __int8 *)(null)_1;
              n16 = n16_7;
              n2_4 = n2_5;
              n2_3 = &(null)_1[n2_5];
              if ( *n2_3 )
                goto LABEL_63;
            }
            else
            {
              n42 = (int)n214748364;
              (null)_2 = (null);
              n2_6 = sub_4068ED((null), n42);
              n75913_8 = (unsigned __int8 *)(null)_2;
              n16 = n16_7;
              n2_4 = n2_6;
              n2_3 = &(null)_2[n2_6];
            }
            LODWORD(n214748364) = n2_4;
            v17 &= ~0x10000u;
            n0x1F_4 = n2_3 - (const char *)n75913_8;
            n75913 = n2_3 - (const char *)n75913_8;
            if ( n2_3 - (const char *)n75913_8 > n2_4 )
            {
              LODWORD(n2_3) = (_DWORD)n2_3 - (_DWORD)n75913_8;
LABEL_202:
              LODWORD(n214748364) = (_DWORD)n2_3;
              n2 = 0;
              n2_1 = (int)n2_3;
              _____0X0x = "-+   0X0x";
            }
            else
            {
              n2_1 = n2_4;
              n2 = 0;
              _____0X0x = "-+   0X0x";
            }
            goto LABEL_161;
          case 'u':
            n2 = 0;
            _____0X0x = "-+   0X0x";
            m_8 = (const char *)n75913_4[0].m128i_i64[0];
LABEL_141:
            n42 = 0xCCCCCCCCCCCCCCCDLL;
            m_6 = (unsigned __int64)m_8;
            n75913_8 = n75913_7;
            while ( m_6 )
            {
              --n75913_8;
              m_9 = m_6;
              m_6 /= 0xAuLL;
              m = m_6;
              n75913 = 10 * m_6;
              *n75913_8 = m_9 - 10 * m_6 + 48;
            }
LABEL_149:
            n0x1F = (unsigned int)n214748364;
            if ( (n214748364 & 0x80000000) != 0LL && v86 )
              goto LABEL_63;
            m = (unsigned int)n214748364;
            if ( v86 )
              v17 &= ~0x10000u;
            if ( (_DWORD)n214748364 || n75913_4[0].m128i_i64[0] )
            {
              n75913 = n75913_7 - n75913_8;
              n214748364_4 = (_BYTE *)(n75913_7 - n75913_8 + (n75913_4[0].m128i_i64[0] == 0));
              n0x1F_4 = n75913_7 - n75913_8;
              if ( (__int64)n214748364_4 < (int)n214748364 )
                LODWORD(n214748364_4) = n214748364;
              m = (int)n214748364_4;
              if ( n75913 <= (int)n214748364_4 )
                n75913 = (unsigned int)n214748364_4;
              LODWORD(n214748364) = n75913;
              if ( 0x7FFFFFFF - n2 < (int)n75913 )
                goto LABEL_63;
              n2_1 = n2 + n75913;
            }
            else
            {
              n2_1 = n2;
              n75913_8 = n75913_7;
              LODWORD(n214748364) = 0;
              n0x1F_4 = 0LL;
            }
LABEL_161:
            n0x1F = (unsigned int)n2_1;
            n214748364_3 = n2_1;
            if ( n16 >= n2_1 )
              n214748364_3 = n16;
            if ( n214748364_5 < n214748364_3 )
              goto LABEL_63;
            if ( (v17 & 0x12000) != 0 )
            {
              if ( (*(_BYTE *)n75913_5 & 0x20) != 0 )
                goto LABEL_211;
            }
            else if ( n16 > n2_1 )
            {
              n42 = 32LL;
              n75913 = (__int64)n75913_5;
              n75913_9 = n75913_8;
              n2_2 = n2;
              n16_8 = n16;
              n214748364_6 = n214748364_3;
              sub_403FE0(n75913_5, 32, n214748364_3, n2_1);
              n214748364_3 = n214748364_6;
              n16 = n16_8;
              n2 = n2_2;
              n75913_8 = n75913_9;
              if ( (*(_BYTE *)n75913_5 & 0x20) != 0 )
              {
LABEL_167:
                n0x1F = (unsigned int)n0x1F_4;
                if ( (int)n0x1F_4 >= (int)n214748364 )
                  goto LABEL_14;
                v46 = n16 <= n2_1;
LABEL_215:
                n42 = 48LL;
                n75913 = (__int64)n75913_5;
                n75913_10 = n75913_8;
                n214748364_7 = n214748364_3;
                sub_403FE0(n75913_5, 48, n214748364, n0x1F);
                n75913_8 = n75913_10;
                n214748364_3 = n214748364_7;
                goto LABEL_216;
              }
            }
            else if ( (*(_BYTE *)n75913_5 & 0x20) != 0 )
            {
              goto LABEL_167;
            }
            n75913 = (__int64)_____0X0x;
            n42 = n2;
            n75913_11 = n75913_8;
            n214748364_8 = n214748364_3;
            n16_9 = n16;
            sub_407580(_____0X0x, n2, n75913_5);
            n16 = n16_9;
            n214748364_3 = n214748364_8;
            n75913_8 = n75913_11;
LABEL_211:
            v46 = n16 <= n2_1;
            if ( ((v17 ^ 0x10000) & 0x12000) == 0 && n16 > n2_1 )
            {
              n42 = 48LL;
              n75913 = (__int64)n75913_5;
              n75913_12 = n75913_8;
              n214748364_9 = n214748364_3;
              sub_403FE0(n75913_5, 48, n214748364_3, n2_1);
              n75913_8 = n75913_12;
              n214748364_3 = n214748364_9;
            }
            n0x1F = (unsigned int)n0x1F_4;
            if ( (int)n0x1F_4 < (int)n214748364 )
              goto LABEL_215;
LABEL_216:
            if ( (*(_BYTE *)n75913_5 & 0x20) == 0 )
            {
              n42 = n0x1F_4;
              n75913 = (__int64)n75913_8;
              n214748364_10 = n214748364_3;
              sub_407580(n75913_8, n0x1F_4, n75913_5);
              n214748364_3 = n214748364_10;
            }
            if ( ((v17 ^ 0x2000) & 0x12000) == 0 && !v46 )
            {
              n42 = 32LL;
              n75913 = (__int64)n75913_5;
              n214748364_11 = n214748364_3;
              sub_403FE0(n75913_5, 32, n214748364_3, n2_1);
              n214748364_3 = n214748364_11;
            }
LABEL_14:
            n75913_8 = i;
            break;
          default:
            n0x1F_4 = n75913_7 - n75913_8;
            n75913 = n75913_7 - n75913_8;
            if ( n75913_7 - n75913_8 > (int)n214748364 )
            {
              n2_3 = (const char *)(n75913_7 - n75913_8);
              goto LABEL_202;
            }
            n2 = 0;
            n2_1 = n214748364;
            _____0X0x = "-+   0X0x";
            goto LABEL_161;
        }
        goto LABEL_15;
      }
      m = n75913_3;
      if ( n75913_5 )
      {
        n75913 = n75913_2;
        n75913_4[0] = _mm_load_si128((const __m128i *)(n75913_2 + 16LL * n75913_3));
        goto LABEL_93;
      }
      *(_DWORD *)(n75913_1 + 4LL * n75913_3) = n42;
    }
LABEL_104:
    n75913_8 = i;
  }
LABEL_64:
  n16 = 0;
  while ( 1 )
  {
    m = n42_3 - 48;
    if ( (unsigned int)m > 9 )
    {
      i = (unsigned __int8 *)i_2;
      if ( n16 == -1 )
        goto LABEL_63;
      goto LABEL_43;
    }
    if ( n16 > 214748364 )
      break;
    n0x1F = (unsigned int)(-10 * n16);
    n42 = (unsigned int)(n0x1F + 0x7FFFFFFF);
    if ( (int)m > (int)n42 )
    {
      m = (unsigned int)(i_2[1] - 48);
      if ( (unsigned int)m <= 9 )
      {
        i_3 = i_2 + 2;
        if ( (unsigned int)(i_2[2] - 48) <= 9 )
          goto LABEL_61;
      }
      goto LABEL_63;
    }
    ++i_2;
    n16 = m - n0x1F;
    n42_3 = *i_2;
    n42 = n42_3;
  }
  i_3 = i_2;
  do
  {
LABEL_61:
    if ( (unsigned int)(i_3[1] - 48) > 9 )
      break;
    v27 = i_3[2];
    i_3 += 2;
  }
  while ( (unsigned int)(v27 - 48) <= 9 );
LABEL_63:
  *(_DWORD *)sub_406D99(n75913, n42, m, n0x1F) = 75;
  return (unsigned int)-1;
}
