/*
 * func-name: sub_407292
 * func-address: 0x407292
 * export-type: decompile
 * callers: 0x406f39
 * callees: 0x406dfb, 0x40728f
 */

__int64 __fastcall sub_407292(__int64 a1)
{
  unsigned int fd; // eax

  fd = sub_40728F(*(_DWORD *)(a1 + 120));
  return sub_406DFB(sys_close(fd));
}
