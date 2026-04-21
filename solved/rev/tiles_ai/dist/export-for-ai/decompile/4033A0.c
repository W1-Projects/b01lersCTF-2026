/*
 * func-name: sub_4033A0
 * func-address: 0x4033a0
 * export-type: decompile
 * callers: 0x407dd7, 0x407ddc, 0x407fbc
 * callees: 0x402680, 0x4029c0, 0x4039a8, 0x403a9a, 0x4069db, 0x406a8c, 0x406d99
 */

__int64 __fastcall sub_4033A0(unsigned __int64 n0x63, __int64 a2, __int64 a3, __int64 a4)
{
  int n0x63_1; // r13d
  unsigned __int64 n3_2; // rax
  __int64 n3_4; // rbx
  int n3; // r12d
  unsigned __int64 v8; // rcx
  unsigned int v9; // eax
  unsigned int v10; // eax
  unsigned __int64 v11; // rsi
  int n3_3; // eax
  __int64 v13; // rbp
  unsigned __int64 n0xC; // rdx
  int v15; // edx
  __int64 n3_1; // rax
  unsigned __int8 v17; // bl
  char v18; // r12
  __int64 v19; // r15
  __int64 result; // rax
  __int64 *v21; // r12
  char v22; // dl
  __int64 v23; // rax
  char *i; // rax
  int v25; // ecx
  int v26; // edx
  unsigned int v27; // eax
  unsigned __int64 v28; // rdx
  unsigned __int64 v29; // r15
  unsigned __int64 v30; // rcx
  unsigned __int64 v31; // rdi
  __int64 v32; // rsi
  int v33; // edx
  unsigned __int64 v34; // rcx
  __int64 v35; // rdx
  char v36; // cl
  int n4; // edx

  if ( n0x63 > 0x7FFFFFFFFFFFEFFELL )
  {
    *(_DWORD *)sub_406D99(n0x63, a2, a3, a4) = 12;
    return 0LL;
  }
  n0x63_1 = n0x63;
  if ( n0x63 > 0x1FFEB )
  {
    v21 = (__int64 *)sub_4039A8(0LL, n0x63 + 20, 3LL, 34LL, 0xFFFFFFFFLL, 0LL);
    if ( v21 == (__int64 *)-1LL )
      return 0LL;
    if ( byte_4128A3 )
      sub_4069DB(&unk_412CC0);
    v22 = byte_412CB0 + 1;
    if ( byte_412CB0 == -1 )
    {
      for ( i = (char *)&unk_412C70; (char *)&unk_412C70 + 32 != i; *(i - 1) = 0 )
      {
        *i = 0;
        i += 2;
      }
      v22 = 1;
    }
    byte_412CB0 = v22;
    v23 = sub_402680();
    v13 = v23;
    if ( !v23 )
    {
      sub_406A8C(&unk_412CC0);
      sub_403A9A(v21, n0x63 + 20);
      return 0LL;
    }
    *(_QWORD *)(v23 + 16) = v21;
    v19 = 0LL;
    *v21 = v23;
    v18 = 0;
    *(_DWORD *)(v23 + 28) = 0;
    *(_DWORD *)(v23 + 24) = 0;
    *(_QWORD *)(v23 + 32) = (n0x63 + 4115) & 0xFFFFFFFFFFFFF000LL | 0xFE0;
    v17 = ++dword_41292C;
  }
  else
  {
    n3_2 = (n0x63 + 3) >> 4;
    n3_4 = (int)n3_2;
    n3 = n3_2;
    if ( n0x63 + 3 > 0x9F )
    {
      v8 = n3_2 + 1;
      v9 = ((unsigned int)(n3_2 + 1) >> 1) | ((unsigned int)(n3_2 + 1) >> 2);
      v10 = (((v9 >> 2) | v9) >> 4) | (v9 >> 2) | v9;
      n3 = 4 * byte_4101E0[(124511785 * ((((v10 >> 8) | v10) + 1) & ~((v10 >> 8) | v10))) >> 27] - 4;
      v11 = word_410240[4 * byte_4101E0[(124511785 * ((((v10 >> 8) | v10) + 1) & ~((v10 >> 8) | v10))) >> 27] - 3];
      n3_3 = 4 * byte_4101E0[(124511785 * ((((v10 >> 8) | v10) + 1) & ~((v10 >> 8) | v10))) >> 27] - 2;
      if ( v11 < v8 )
        n3 = n3_3;
      n3_4 = n3;
      if ( word_410240[n3] < v8 )
        n3_4 = ++n3;
    }
    if ( byte_4128A3 )
      sub_4069DB(&unk_412CC0);
    v13 = qword_412920[n3_4 + 10];
    if ( !v13 )
    {
      if ( n3 <= 3 || n3 > 31 || n3 == 6 || (n3 & 1) != 0 || qword_412920[n3_4 + 58] )
        goto LABEL_19;
      v13 = qword_412920[(n3 | 1) + 10];
      n0xC = qword_412920[(n3 | 1) + 58];
      if ( !v13 )
      {
        if ( n0xC + 3 <= 0xC )
          n3 |= 1u;
        goto LABEL_19;
      }
      if ( !*(_DWORD *)(v13 + 24) && !*(_DWORD *)(v13 + 28) )
        n0xC += 3LL;
      if ( n0xC > 0xC )
      {
LABEL_19:
        v15 = sub_4029C0(n3, n0x63);
        if ( v15 >= 0 )
        {
          n3_1 = n3;
          v17 = dword_41292C;
          v18 = v15;
          v13 = qword_412920[n3_1 + 10];
          v19 = v15;
          goto LABEL_40;
        }
        sub_406A8C(&unk_412CC0);
        return 0LL;
      }
      n3 |= 1u;
    }
    v25 = *(_DWORD *)(v13 + 24);
    v26 = v25 & -v25;
    if ( !v26 )
      goto LABEL_19;
    v17 = dword_41292C;
    *(_DWORD *)(v13 + 24) = v25 - v26;
    v27 = (124511785 * (v26 & (unsigned int)-v26)) >> 27;
    v19 = byte_4101E0[v27];
    v18 = byte_4101E0[v27];
  }
LABEL_40:
  sub_406A8C(&unk_412CC0);
  if ( (*(_BYTE *)(v13 + 32) & 0x1F) != 0 || *(_QWORD *)(v13 + 32) <= 0xFFFuLL )
    v28 = 16 * word_410240[(*(_WORD *)(v13 + 32) >> 6) & 0x3F];
  else
    v28 = (*(_QWORD *)(v13 + 32) & 0xFFFFFFFFFFFFF000LL) - 16;
  v29 = v28 * v19;
  v30 = v28 - n0x63 - 4;
  result = *(_QWORD *)(v13 + 16) + v29 + 16;
  v31 = v30 >> 4;
  v32 = result + v28 - 4;
  v33 = v17;
  if ( *(_BYTE *)(*(_QWORD *)(v13 + 16) + v29 + 13) )
    v33 = (unsigned __int8)(*(_WORD *)(*(_QWORD *)(v13 + 16) + v29 + 14) + 1);
  if ( *(_BYTE *)(*(_QWORD *)(v13 + 16) + v29 + 12) )
    __halt();
  if ( v31 < v33 )
  {
    v34 = ((v31 | (v30 >> 5)) >> 2) | v31 | (v30 >> 5);
    v33 &= (v34 >> 4) | v34;
    if ( v31 < v33 )
    {
      v33 = v33 - v31 - 1;
      if ( v31 < v33 )
        __halt();
    }
  }
  if ( v33 )
  {
    *(_WORD *)(*(_QWORD *)(v13 + 16) + v29 + 14) = v33;
    *(_BYTE *)(result - 3) = -32;
    result += 16 * v33;
    *(_BYTE *)(result - 4) = 0;
  }
  v35 = result - *(_QWORD *)(v13 + 16);
  v36 = 0;
  *(_BYTE *)(result - 3) = v18;
  *(_WORD *)(result - 2) = (unsigned __int64)(v35 - 16) >> 4;
  n4 = v32 - result - n0x63_1;
  if ( (_DWORD)v32 - (_DWORD)result != n0x63_1 )
  {
    *(_BYTE *)(v32 + n0x63_1 - ((int)v32 - (int)result)) = 0;
    if ( n4 > 4 )
    {
      *(_DWORD *)(v32 - 4) = n4;
      v36 = -96;
      *(_BYTE *)(v32 - 5) = 0;
      v18 = *(_BYTE *)(result - 3);
    }
    else
    {
      v18 = *(_BYTE *)(result - 3);
      v36 = 32 * n4;
    }
  }
  *(_BYTE *)(result - 3) = v36 + (v18 & 0x1F);
  return result;
}
