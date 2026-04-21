/*
 * func-name: sub_403C1E
 * func-address: 0x403c1e
 * export-type: decompile
 * callers: none
 * callees: 0x40664b, 0x406d99, 0x406dfb, 0x406f39, 0x4070c1
 */

__int64 __fastcall sub_403C1E(const char *filename, char *a2)
{
  __int64 v3; // rsi
  __int64 v4; // rdx
  __int64 v5; // rcx
  int v6; // r12d
  __int64 flags; // rsi
  signed __int64 v8; // rdi
  __int64 v9; // rdx
  __int64 v10; // rcx
  __int64 v11; // r8
  __int64 v12; // r9
  signed int fd; // eax
  unsigned int fd_1; // ebp
  unsigned int fd_2; // r8d
  signed __int64 v16; // rax
  __int64 v17; // rdx
  signed __int64 v19; // rax

  v3 = (unsigned int)*a2;
  if ( !sub_40664B("rwa", v3) )
  {
    *(_DWORD *)sub_406D99("rwa", v3, v4, v5) = 22;
    return 0LL;
  }
  v6 = sub_4070C1(a2);
  flags = v6 | 0x8000;
  v8 = sys_open(filename, flags, 438);
  fd = sub_406DFB(v8, flags, v9, v10, v11, v12);
  fd_1 = fd;
  fd_2 = fd;
  if ( fd < 0 )
    return 0LL;
  if ( (v6 & 0x80000) != 0 )
    v16 = sys_fcntl(fd, 2u, 1uLL);
  v17 = sub_406F39(fd_2, a2);
  if ( !v17 )
    v19 = sys_close(fd_1);
  return v17;
}
