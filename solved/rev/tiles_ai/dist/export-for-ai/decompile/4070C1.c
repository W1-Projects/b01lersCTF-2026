/*
 * func-name: sub_4070C1
 * func-address: 0x4070c1
 * export-type: decompile
 * callers: 0x403c1e
 * callees: 0x40664b
 */

__int64 __fastcall sub_4070C1(char *a1)
{
  unsigned int n2; // ebx
  _BYTE *v2; // rdx
  unsigned int n2_1; // eax
  char n97; // al
  unsigned int n2_2; // edx
  unsigned int v7; // edx

  n2 = 2;
  if ( !sub_40664B((__int64)a1, 43) )
    n2 = *a1 != 114;
  v2 = sub_40664B((__int64)a1, 120);
  n2_1 = n2;
  if ( v2 )
  {
    LOBYTE(n2_1) = n2 | 0x80;
    n2 = n2_1;
  }
  if ( sub_40664B((__int64)a1, 101) )
    n2 |= 0x80000u;
  n97 = *a1;
  if ( *a1 != 114 )
  {
    if ( n97 == 119 )
    {
      n2 |= 0x240u;
    }
    else
    {
      n2_2 = n2;
      n2 |= 0x440u;
      v7 = n2_2 | 0x40;
      if ( n97 != 97 )
        return v7;
    }
  }
  return n2;
}
