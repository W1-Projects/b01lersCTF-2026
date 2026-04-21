/*
 * func-name: sub_40721E
 * func-address: 0x40721e
 * export-type: decompile
 * callers: 0x403da5
 * callees: 0x4074e6
 */

__int64 __fastcall sub_40721E(__int64 n75913, char a2)
{
  _BYTE *v2; // rax
  _BYTE *v3; // rdx
  __int64 result; // rax
  _BYTE v5[9]; // [rsp+1Fh] [rbp-9h] BYREF

  v2 = *(_BYTE **)(n75913 + 32);
  v5[0] = a2;
  if ( !v2 )
  {
    if ( (unsigned int)sub_4074E6(n75913) )
      return 0xFFFFFFFFLL;
    v2 = *(_BYTE **)(n75913 + 32);
  }
  v3 = *(_BYTE **)(n75913 + 40);
  if ( v3 != v2 )
  {
    result = v5[0];
    if ( v5[0] != *(_DWORD *)(n75913 + 144) )
    {
      *(_QWORD *)(n75913 + 40) = v3 + 1;
      *v3 = result;
      return result;
    }
  }
  if ( (*(__int64 (__fastcall **)(__int64, _BYTE *, __int64))(n75913 + 72))(n75913, v5, 1LL) != 1 )
    return 0xFFFFFFFFLL;
  return v5[0];
}
