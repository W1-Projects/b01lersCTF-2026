/*
 * func-name: sub_407A40
 * func-address: 0x407a40
 * export-type: decompile
 * callers: 0x4068ed
 * callees: none
 */

unsigned __int8 *__fastcall sub_407A40(unsigned __int8 *a1, unsigned __int8 a2, __int64 a3)
{
  char *v4; // r10
  unsigned __int8 *result; // rax
  __int64 v6; // rdx

  while ( ((unsigned __int8)a1 & 7) != 0 )
  {
    if ( !a3 )
      return 0LL;
    if ( *a1 == a2 )
      goto LABEL_6;
    ++a1;
    --a3;
  }
  result = 0LL;
  if ( !a3 )
    return result;
LABEL_6:
  if ( *a1 == a2 )
  {
    result = a1;
    while ( a3 )
    {
      if ( *result == a2 )
        return result;
      ++result;
      --a3;
    }
    return 0LL;
  }
  v4 = (char *)&a1[a3];
  result = &a1[a3 & 0xFFFFFFFFFFFFFFF8LL];
  while ( 1 )
  {
    v6 = v4 - (char *)a1;
    if ( a1 == result )
      goto LABEL_16;
    if ( ((((0x101010101010101LL * a2) ^ *(_QWORD *)a1) - 0x101010101010101LL) & ~((0x101010101010101LL * a2) ^ *(_QWORD *)a1) & 0x8080808080808080LL) != 0 )
      break;
    a1 += 8;
  }
  result = a1;
LABEL_16:
  while ( v6 )
  {
    if ( *result == a2 )
      return result;
    ++result;
    --v6;
  }
  return 0LL;
}
