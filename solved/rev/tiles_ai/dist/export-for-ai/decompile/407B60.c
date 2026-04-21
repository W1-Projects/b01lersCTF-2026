/*
 * func-name: sub_407B60
 * func-address: 0x407b60
 * export-type: decompile
 * callers: none
 * callees: 0x4039a8, 0x4069db, 0x406a8c, 0x406d99
 */

__int64 __fastcall sub_407B60(__int64 n0xF_2, _BYTE *n0x800000_2, __int64 a3, __int64 a4, __int64 a5, u32 val3)
{
  unsigned __int64 n0xF_1; // rbp
  unsigned __int64 n0xF; // rbx
  __int64 n0x800000_5; // rbx
  __int64 v9; // rcx
  u32 *uaddr2_1; // r8
  u32 val3_2; // r9d
  __int64 n0x800000_4; // rdx
  __int64 n0x800000_3; // rdi
  signed __int64 v14; // rax
  unsigned __int64 n0x800000_1; // rax
  unsigned __int64 v16; // rdx
  unsigned __int64 brk; // rdi
  unsigned __int64 brk_2; // rcx
  __int64 *brk_1; // rcx
  signed __int64 v20; // rax
  __int64 v21; // rdx
  unsigned __int64 len; // rsi
  unsigned __int64 len_1; // rax
  __int64 n0x800000_6; // rax
  __int64 v26; // rdx
  __int64 v27; // rcx
  u32 *uaddr2; // r8
  u32 val3_1; // r9d
  __int64 v30; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int64 len_2; // [rsp+8h] [rbp-30h]
  _BYTE n0x800000[32]; // [rsp+18h] [rbp-20h] BYREF

  n0xF_1 = 1LL;
  n0xF = 1LL;
  if ( n0xF_2 )
    n0xF_1 = n0xF_2;
  if ( n0xF_2 >= 0 )
  {
    while ( n0xF < n0xF_1 && n0xF <= 0xF )
      n0xF *= 2LL;
    sub_4069DB(uaddr_0, (__int64)n0x800000_2, a3, a4, a5, val3);
    n0x800000_4 = n0x800000_2;
    n0x800000_3 += (n0xF - 1) & -n0x800000_3;
    n0x800000_3 = n0x800000_3;
    if ( n0x800000_2 - n0x800000_3 >= n0xF_1 )
      goto LABEL_23;
    if ( n0x800000_3 )
    {
      n0x800000_1 = n0x800000_1;
      if ( n0x800000_2 != n0x800000_1 )
        goto LABEL_25;
    }
    else
    {
      v14 = sys_brk(0LL);
      n0x800000_1 = (-(int)v14 & 0xFFF) + v14;
      n0x800000_1 = n0x800000_1;
      n0x800000_2 = n0x800000_1;
      n0x800000_3 = n0x800000_1;
    }
    v16 = (n0x800000_3 + n0xF_1 - n0x800000_4 + 4095) & 0xFFFFFFFFFFFFF000LL;
    if ( v16 < ~n0x800000_1 )
    {
      brk = v16 + n0x800000_1;
      brk_2 = n0x800000_0 - 0x800000;
      if ( (unsigned __int64)n0x800000_0 < 0x800000 )
        brk_2 = 0LL;
      if ( brk_2 >= brk || n0x800000_1 >= n0x800000_0 )
      {
        n0x800000_2 = n0x800000;
        brk_1 = &v30 - 1048573;
        if ( (unsigned __int64)n0x800000 < 0x800000 )
          brk_1 = 0LL;
        if ( (unsigned __int64)brk_1 >= brk || n0x800000_1 >= (unsigned __int64)n0x800000 )
        {
          v20 = sys_brk(brk);
          v9 = v21 + n0x800000_1;
          if ( v21 + n0x800000_1 == v20 )
          {
            n0x800000_4 = n0x800000_2 + v21;
            n0x800000_3 = n0x800000_3;
            n0x800000_2 = n0x800000_4;
            n0x800000_1 = n0x800000_4;
LABEL_23:
            n0x800000_5 = n0x800000_3;
            goto LABEL_33;
          }
        }
      }
    }
LABEL_25:
    len = (n0xF_1 + 4095) & 0xFFFFFFFFFFFFF000LL;
    if ( len >> 3 >= len - n0xF_1
      || (len_1 = 4096 << ((unsigned int)n0xB >> 1), n0x800000_2 - n0x800000_3 >= len_1 - n0xF_1) )
    {
      n0x800000_5 = sub_4039A8(0LL, len, 3LL, 34LL, -1, 0LL);
      sub_406A8C(uaddr_0, len, v26, v27, uaddr2, val3_1);
      if ( n0x800000_5 != -1 )
        return n0x800000_5;
      return 0LL;
    }
    if ( len < len_1 )
    {
      if ( (unsigned int)n0xB <= 0xB )
        ++n0xB;
      len = len_1;
    }
    len_2 = len;
    n0x800000_6 = sub_4039A8(0LL, len, 3LL, 34LL, -1, 0LL);
    n0x800000_5 = n0x800000_6;
    if ( n0x800000_6 == -1 )
    {
      sub_406A8C(uaddr_0, len_2, n0x800000_4, v9, uaddr2_1, val3_2);
      return 0LL;
    }
    n0x800000_2 = (_BYTE *)(n0x800000_6 + len_2);
    n0x800000_3 = n0x800000_6;
    n0x800000_2 = n0x800000_6 + len_2;
LABEL_33:
    n0x800000_3 = n0x800000_3 + n0xF_1;
    sub_406A8C(uaddr_0, (__int64)n0x800000_2, n0x800000_4, v9, uaddr2_1, val3_2);
    return n0x800000_5;
  }
  *(_DWORD *)_readfsqword_() = 12;
  return 0LL;
}
