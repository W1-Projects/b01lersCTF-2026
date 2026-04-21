/*
 * func-name: sub_406780
 * func-address: 0x406780
 * export-type: decompile
 * callers: none
 * callees: 0x406680, 0x406917
 */

char *__fastcall sub_406780(char *j_1, unsigned __int8 *i_2)
{
  unsigned __int8 v3; // si
  unsigned __int8 *i; // r8
  unsigned __int8 v6; // cl
  char *j; // rax
  unsigned __int64 v8; // rdx
  __int64 v9; // rcx
  unsigned __int8 *i_1; // [rsp+8h] [rbp-30h]
  _QWORD v11[5]; // [rsp+10h] [rbp-28h] BYREF

  v3 = *i_2;
  if ( !v3 || !i_2[1] )
    return (char *)((char *)sub_406680(j_1, v3) - j_1);
  i_1 = i_2;
  sub_406917(v11, 0LL, 32LL);
  for ( i = i_1; ; ++i )
  {
    v6 = *i;
    if ( !*i )
      break;
    v11[v6 >> 6] |= 1LL << v6;
  }
  for ( j = j_1; ; ++j )
  {
    v8 = (unsigned __int8)*j;
    if ( !(_BYTE)v8 )
      break;
    v9 = v11[(unsigned __int8)v8 >> 6];
    if ( _bittest64(&v9, v8) )
      break;
  }
  return (char *)(j - j_1);
}
