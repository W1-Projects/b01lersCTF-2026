/*
 * func-name: sub_407539
 * func-address: 0x407539
 * export-type: decompile
 * callers: 0x403da5
 * callees: 0x406880, 0x407659
 */

__int64 __fastcall sub_407539(_BYTE *i, __int64 a2)
{
  _BYTE *v2; // rbx

  v2 = sub_406880(i);
  return (unsigned int)-(sub_407659(i, 1LL, v2, a2) != (_QWORD)v2);
}
