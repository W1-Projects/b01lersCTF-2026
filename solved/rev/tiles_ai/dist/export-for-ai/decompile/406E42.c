/*
 * func-name: sub_406E42
 * func-address: 0x406e42
 * export-type: decompile
 * callers: 0x404080
 * callees: none
 */

__int64 __fastcall sub_406E42(_DWORD a1, int n16, int a3, _DWORD a4, int a5, int a6, unsigned __int64 a7, __int64 a8)
{
  __int64 result; // rax

  if ( !((a7 >> 63) | a8 & 0x7FFF) )
    return 2 - ((unsigned int)(a7 == 0) - 1);
  result = 4 * (unsigned int)(a7 >> 63);
  if ( (a8 & 0x7FFF) == 0x7FFF )
  {
    result = 0LL;
    if ( (a7 & 0x8000000000000000LL) != 0LL )
      return 2 * a7 == 0;
  }
  return result;
}
