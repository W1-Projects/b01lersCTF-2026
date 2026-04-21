/*
 * func-name: sub_403A62
 * func-address: 0x403a62
 * export-type: decompile
 * callers: 0x402680
 * callees: 0x406dfb
 */

__int64 __fastcall sub_403A62(__int64 a1, __int64 a2, int prot)
{
  unsigned __int64 start_1; // rdi
  size_t len; // rsi
  signed __int64 v6; // rdi
  __int64 v7; // rdx
  __int64 v8; // rcx
  __int64 v9; // r8
  __int64 v10; // r9

  start_1 = a1 & 0xFFFFFFFFFFFFF000LL;
  len = ((a1 + a2 + 4095) & 0xFFFFFFFFFFFFF000LL) - start_1;
  v6 = sys_mprotect(start_1, len, prot);
  return sub_406DFB(v6, len, v7, v8, v9, v10);
}
