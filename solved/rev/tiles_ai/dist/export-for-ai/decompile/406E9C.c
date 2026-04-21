/*
 * func-name: sub_406E9C
 * func-address: 0x406e9c
 * export-type: decompile
 * callers: 0x404080, 0x406e9c
 * callees: 0x406e9c
 */

long double __fastcall sub_406E9C(_DWORD *a1, long double a2)
{
  unsigned __int16 n0x7FFF; // dx
  long double result; // fst7
  long double v9; // [rsp+0h] [rbp-38h]
  long double v10; // [rsp+20h] [rbp-18h]

  v10 = a2;
  n0x7FFF = WORD4(a2) & 0x7FFF;
  if ( (WORD4(a2) & 0x7FFF) != 0 )
  {
    if ( n0x7FFF != 0x7FFF )
    {
      WORD4(v10) = WORD4(a2) & 0x8000 | 0x3FFE;
      result = v10;
      *a1 = n0x7FFF - 16382;
      return result;
    }
    return a2;
  }
  if ( a2 == 0.0 )
  {
    *a1 = 0;
    return a2;
  }
  result = a2 * 1.329228e36;
  v9 = a2 * 1.329228e36;
  sub_406E9C(*(_TBYTE *)&v9);
  *a1 -= 120;
  return result;
}
