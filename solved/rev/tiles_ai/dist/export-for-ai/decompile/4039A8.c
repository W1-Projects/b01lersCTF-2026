/*
 * func-name: sub_4039A8
 * func-address: 0x4039a8
 * export-type: decompile
 * callers: 0x402680, 0x4029c0, 0x4033a0, 0x407b60
 * callees: 0x4039a7, 0x406d99
 */

__int64 __fastcall sub_4039A8(
        unsigned __int64 addr,
        unsigned __int64 len,
        __int64 prot,
        __int64 flags_1,
        int fd,
        unsigned __int64 off)
{
  int flags; // ebx
  signed __int64 v7; // rax
  __int64 v8; // rdx
  __int64 v9; // rcx
  __int64 v10; // r8
  __int64 v11; // r9
  int prot_1; // [rsp+10h] [rbp-18h]
  int fd_1; // [rsp+14h] [rbp-14h]
  unsigned __int64 off_1; // [rsp+18h] [rbp-10h]

  if ( (off & 0xFFF) != 0 )
  {
    *(_DWORD *)sub_406D99(addr, len, prot, flags_1) = 22;
    return -1LL;
  }
  if ( len > 0x7FFFFFFFFFFFFFFELL )
  {
    *(_DWORD *)sub_406D99(addr, len, prot, flags_1) = 12;
    return -1LL;
  }
  flags = flags_1;
  if ( (flags_1 & 0x10) != 0 )
  {
    off_1 = off;
    fd_1 = fd;
    prot_1 = prot;
    nullsub_2();
    off = off_1;
    fd = fd_1;
    LODWORD(prot) = prot_1;
  }
  v7 = sys_mmap(addr, len, (int)prot, flags, fd, off);
  if ( v7 == -1 && !addr )
  {
    v8 = -12LL;
    if ( (flags & 0x30) == 0x20 )
      v7 = -12LL;
  }
  return sub_406DFB(v7, len, v8, v9, v10, v11);
}
