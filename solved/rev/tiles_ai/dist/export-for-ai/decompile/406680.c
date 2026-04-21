/*
 * func-name: sub_406680
 * func-address: 0x406680
 * export-type: decompile
 * callers: 0x40664b, 0x406780
 * callees: 0x406880
 */

_QWORD *__fastcall sub_406680(_QWORD *a1, unsigned __int8 a2)
{
  _QWORD *v2; // rdx
  int v3; // eax
  int v5; // eax

  v2 = a1;
  if ( !a2 )
    return (_QWORD *)((char *)a1 + sub_406880(a1, a2, a1));
  while ( ((unsigned __int8)v2 & 7) != 0 )
  {
    v3 = *(unsigned __int8 *)v2;
    if ( !(_BYTE)v3 || v3 == a2 )
      return v2;
    v2 = (_QWORD *)((char *)v2 + 1);
  }
  while ( (((((0x101010101010101LL * a2) ^ *v2) - 0x101010101010101LL) & ~((0x101010101010101LL * a2) ^ *v2) | ~*v2 & (*v2 - 0x101010101010101LL)) & 0x8080808080808080LL) == 0 )
    ++v2;
  while ( 1 )
  {
    v5 = *(unsigned __int8 *)v2;
    if ( !(_BYTE)v5 || v5 == a2 )
      break;
    v2 = (_QWORD *)((char *)v2 + 1);
  }
  return v2;
}
