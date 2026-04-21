/*
 * func-name: sub_403FE0
 * func-address: 0x403fe0
 * export-type: decompile
 * callers: 0x404080, 0x405300
 * callees: 0x406917, 0x407580
 */

__int64 __fastcall sub_403FE0(unsigned int *a1, char a2, int a3, int a4)
{
  __int64 v4; // rsi
  __int64 n256_1; // r12
  __int64 n256; // rdx
  unsigned int n0xFF; // ebx
  __int64 result; // rax
  _BYTE v9[296]; // [rsp+0h] [rbp-128h] BYREF

  v4 = (unsigned int)a2;
  n256_1 = a3 - a4;
  n256 = 256LL;
  n0xFF = n256_1;
  if ( (int)n256_1 <= 256 )
    n256 = n256_1;
  sub_406917(v9, v4, n256);
  result = *a1;
  while ( n0xFF > 0xFF )
  {
    if ( (result & 0x20) == 0 )
    {
      sub_407580(v9, 256LL, a1);
      result = *a1;
    }
    n0xFF -= 256;
  }
  if ( (result & 0x20) == 0 )
    return sub_407580(v9, (unsigned __int8)n256_1, a1);
  return result;
}
