/*
 * func-name: sub_40664B
 * func-address: 0x40664b
 * export-type: decompile
 * callers: 0x403c1e, 0x406f39, 0x4070c1
 * callees: 0x406680
 */

_BYTE *__fastcall sub_40664B(__int64 a1, char a2)
{
  _BYTE *result; // rax

  result = (_BYTE *)sub_406680();
  if ( *result != a2 )
    return 0LL;
  return result;
}
