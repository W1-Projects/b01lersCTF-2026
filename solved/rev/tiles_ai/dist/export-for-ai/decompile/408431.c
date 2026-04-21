/*
 * func-name: sub_408431
 * func-address: 0x408431
 * export-type: decompile
 * callers: 0x407fbc
 * callees: 0x4039a7, 0x406d99, 0x406dfb
 */

__int64 __fastcall sub_408431(
        unsigned __int64 addr,
        unsigned __int64 n0x63,
        unsigned __int64 new_len,
        int flags,
        unsigned __int64 new_addr_1,
        int a6)
{
  unsigned __int64 new_addr; // r8
  unsigned __int64 new_len_1; // [rsp+10h] [rbp-68h]
  int flags_1; // [rsp+1Ch] [rbp-5Ch]

  if ( new_len > 0x7FFFFFFFFFFFFFFELL )
  {
    *(_DWORD *)_readfsqword_() = 12;
    return -1LL;
  }
  else
  {
    new_addr = 0LL;
    if ( (flags & 2) != 0 )
    {
      flags_1 = flags;
      new_len_1 = new_len;
      nullsub_2();
      new_addr = new_addr_1;
      flags = flags_1;
      new_len = new_len_1;
    }
    return sub_406DFB(sys_mremap(addr, n0x63, new_len, flags, new_addr));
  }
}
