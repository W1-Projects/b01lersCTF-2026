/*
 * func-name: sub_406880
 * func-address: 0x406880
 * export-type: decompile
 * callers: 0x406680, 0x407539
 * callees: none
 */

_BYTE *__fastcall sub_406880(_BYTE *i_1)
{
  _BYTE *i; // rax

  for ( i = i_1; ((unsigned __int8)i & 7) != 0; ++i )
  {
    if ( !*i )
      return (_BYTE *)(i - i_1);
  }
  while ( ((*(_QWORD *)i - 0x101010101010101LL) & ~*(_QWORD *)i & 0x8080808080808080LL) == 0 )
    i += 8;
  while ( *i )
    ++i;
  return (_BYTE *)(i - i_1);
}
