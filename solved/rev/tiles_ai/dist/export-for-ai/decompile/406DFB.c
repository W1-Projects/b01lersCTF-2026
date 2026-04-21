/*
 * func-name: sub_406DFB
 * func-address: 0x406dfb
 * export-type: decompile
 * callers: 0x403942, 0x4039a8, 0x403a62, 0x403a9a, 0x403c1e, 0x407292, 0x4072b5, 0x40738e, 0x408431
 * callees: 0x406d99
 */

__int64 __fastcall sub_406DFB(unsigned __int64 a1)
{
  if ( a1 <= 0xFFFFFFFFFFFFF000LL )
    return a1;
  *(_DWORD *)_readfsqword_() = -(int)a1;
  return -1LL;
}
