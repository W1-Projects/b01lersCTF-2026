/*
 * func-name: sub_403A9A
 * func-address: 0x403a9a
 * export-type: decompile
 * callers: 0x4033a0
 * callees: 0x4039a7, 0x406dfb
 */

__int64 __fastcall sub_403A9A(unsigned __int64 addr, size_t len)
{
  signed __int64 v2; // rdi
  __int64 v3; // rdx
  __int64 v4; // rcx
  __int64 v5; // r8
  __int64 v6; // r9

  nullsub_2();
  v2 = sys_munmap(addr, len);
  return sub_406DFB(v2, len, v3, v4, v5, v6);
}
