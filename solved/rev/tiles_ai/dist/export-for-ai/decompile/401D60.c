/*
 * func-name: sub_401D60
 * func-address: 0x401d60
 * export-type: decompile
 * callers: 0x401f6a
 * callees: 0x406bb6, 0x408544
 */

__int64 __fastcall sub_401D60(__int64 a1, char *a2)
{
  char *v2; // rdx
  __int64 v3; // rax
  unsigned __int64 *v5; // rax
  unsigned __int64 n0x25; // rcx
  char *v7; // rax
  char n47; // dl
  __int64 result; // rax
  __int128 *mode; // rdx
  char *mode_1; // r8
  __int128 ufds_; // [rsp+0h] [rbp-158h] BYREF
  __int64 n2; // [rsp+10h] [rbp-148h]
  char v14; // [rsp+18h] [rbp-140h] BYREF
  _QWORD buf[39]; // [rsp+20h] [rbp-138h] BYREF

  v2 = a2;
  memset(buf, 0, 0x130uLL);
  unk_413508 = a1;
  v3 = 0LL;
  while ( *(_QWORD *)(a1 + 8 * v3++) )
    ;
  v5 = (unsigned __int64 *)(a1 + 8 * v3);
  qword_4128A8 = (__int64)v5;
  while ( 1 )
  {
    n0x25 = *v5;
    if ( !*v5 )
      break;
    if ( n0x25 <= 0x25 )
      buf[n0x25] = v5[1];
    v5 += 2;
  }
  qword_412888 = buf[16];
  if ( buf[32] )
    qword_412870 = buf[32];
  qword_4128D0 = buf[6];
  if ( !a2 )
  {
    v2 = "";
    if ( buf[31] )
      v2 = (char *)buf[31];
  }
  unk_412878 = v2;
  v7 = v2 + 1;
  unk_412880 = v2;
  while ( 1 )
  {
    n47 = *(v7 - 1);
    if ( !n47 )
      break;
    if ( n47 == 47 )
      unk_412880 = v7;
    ++v7;
  }
  sub_406BB6(buf);
  sub_408544(buf[25]);
  if ( buf[11] != buf[12] || (result = buf[14], buf[13] != buf[14]) || buf[23] )
  {
    n2 = 2LL;
    ufds_ = 0LL;
    DWORD2(ufds_) = 1;
    result = sys_poll((struct poll_fd *)&ufds_, 3u, 0LL);
    if ( (int)result < 0 )
      __halt();
    mode = &ufds_;
    mode_1 = &v14;
    do
    {
      if ( (*((_BYTE *)mode + 6) & 0x20) != 0 )
      {
        result = sys_open("/dev/null", 32770, (int)mode);
        if ( result < 0 )
          __halt();
      }
      mode = (__int128 *)((char *)mode + 8);
    }
    while ( mode_1 != (char *)mode );
    byte_4128A2 = 1;
  }
  return result;
}
