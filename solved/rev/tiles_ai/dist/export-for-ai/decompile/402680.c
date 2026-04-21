/*
 * func-name: sub_402680
 * func-address: 0x402680
 * export-type: decompile
 * callers: 0x4029c0, 0x4033a0
 * callees: 0x4039a8, 0x403a62, 0x406300, 0x406d99
 */

__int64 sub_402680()
{
  __int64 v0; // r10
  _QWORD *v1; // rax
  __int64 v3; // rcx
  __int64 v4; // rdx
  __int64 n25; // rax
  __int64 n100; // rax
  unsigned __int64 brk; // rax
  unsigned __int64 brk_1; // rbx
  _QWORD *v9; // rcx
  __int64 v10; // rdi
  __int64 v11; // r14
  __int64 v12; // rax
  signed __int64 v13; // rax
  _QWORD *v14; // rdi
  int v15; // eax
  __int64 v16; // rdx
  _DWORD *v17; // rax
  __int64 v18; // [rsp+0h] [rbp-38h]
  __int64 v19; // [rsp+0h] [rbp-38h]
  _QWORD *v20; // [rsp+0h] [rbp-38h]
  __int64 v21; // [rsp+8h] [rbp-30h]
  __int64 v22; // [rsp+8h] [rbp-30h]
  _QWORD v23[4]; // [rsp+18h] [rbp-20h] BYREF

  if ( !dword_412928 )
  {
    v3 = 0LL;
    v23[0] = 1103515245LL * (_QWORD)v23;
    while ( 1 )
    {
      v4 = *((_QWORD *)&byte_4128A0 + 1);
      n25 = *(_QWORD *)(v4 + v3);
      if ( !n25 )
        break;
      if ( n25 == 25 )
      {
        v18 = v3;
        sub_406300(v23, *(_QWORD *)(v4 + v3 + 8) + 8LL, 8LL, v3, &byte_4128A0);
        v3 = v18;
      }
      v3 += 16LL;
    }
    dword_412928 = 1;
    qword_412920 = v23[0];
  }
  v0 = qword_412930;
  if ( !qword_412930 )
  {
    if ( n100 )
    {
      v0 = qword_412938;
      n100 = n100 - 1;
LABEL_16:
      n100 = n100;
      qword_412938 = v0 + 40;
      *(_OWORD *)v0 = 0LL;
      return v0;
    }
    brk = brk;
    brk_1 = brk;
    if ( brk )
    {
LABEL_18:
      v9 = (_QWORD *)qword_412968;
      if ( (qword_412968 & 0xFFF) != 0 )
      {
LABEL_19:
        brk_1 = brk - 1;
LABEL_20:
        brk = brk_1;
        qword_412968 = (__int64)(v9 + 512);
        if ( qword_412960 )
          *(_QWORD *)(qword_412960 + 8) = v9;
        else
          qword_412958 = (__int64)v9;
        qword_412960 = (__int64)v9;
        *v9 = qword_412920;
        *(_DWORD *)(qword_412960 + 16) = 101;
        v0 = qword_412960 + 24;
        n100 = 100LL;
        goto LABEL_16;
      }
LABEL_33:
      v14 = v9;
      v20 = v9;
      v22 = v0;
      v15 = sub_403A62(v9, 4096LL, 3LL);
      v9 = v20;
      if ( v15 )
      {
        v17 = (_DWORD *)sub_406D99(v14, 4096LL, v16, v20);
        v9 = v20;
        v0 = v22;
        if ( *v17 != 38 )
          return v0;
      }
      brk = brk;
      goto LABEL_19;
    }
    if ( qword_412CB8 == -1 )
    {
LABEL_27:
      v19 = v0;
      v21 = 2LL << qword_412950;
      v12 = sub_4039A8(0LL, 2LL << qword_412950 << 12, 0LL, 34LL, 0xFFFFFFFFLL, 0LL);
      v0 = v19;
      if ( v12 == -1 )
        return v0;
      v9 = (_QWORD *)(v12 + 4096);
      ++qword_412950;
      qword_412968 = v12 + 4096;
      brk = v21 - 1;
      if ( (((_DWORD)v12 + 4096) & 0xFFF) != 0 )
      {
        brk_1 = v21 - 2;
        goto LABEL_20;
      }
      goto LABEL_33;
    }
    if ( qword_412CB8 )
    {
      v10 = qword_412CB8 + 4096;
      v11 = sys_brk(qword_412CB8 + 4096);
      if ( v10 != v11 )
        goto LABEL_26;
    }
    else
    {
      v13 = sys_brk(brk);
      qword_412CB8 = v13 + (-(int)v13 & 0xFFF);
      v11 = qword_412CB8 + 0x2000;
      if ( v11 != sys_brk(qword_412CB8 + 0x2000) )
      {
LABEL_26:
        brk = brk;
        qword_412CB8 = -1LL;
        if ( brk )
          goto LABEL_18;
        goto LABEL_27;
      }
      sub_4039A8(qword_412CB8, 4096LL, 0LL, 50LL, 0xFFFFFFFFLL, 0LL);
    }
    qword_412CB8 = v11;
    v9 = (_QWORD *)(v11 - 4096);
    goto LABEL_20;
  }
  v1 = *(_QWORD **)(qword_412930 + 8);
  if ( (_QWORD *)qword_412930 == v1 )
  {
    qword_412930 = 0LL;
  }
  else
  {
    *(_QWORD *)(*(_QWORD *)qword_412930 + 8LL) = v1;
    *v1 = *(_QWORD *)v0;
    if ( v0 == qword_412930 )
      qword_412930 = *(_QWORD *)(v0 + 8);
  }
  *(_OWORD *)v0 = 0LL;
  return v0;
}
