/*
 * func-name: sub_407F83
 * func-address: 0x407f83
 * export-type: decompile
 * callers: none
 * callees: 0x4084cb
 */

__int64 __fastcall sub_407F83(__int64 a1)
{
  _BYTE v2[9]; // [rsp+Fh] [rbp-9h] BYREF

  if ( (unsigned int)sub_4084CB() || (*(__int64 (__fastcall **)(__int64, _BYTE *, __int64))(a1 + 64))(a1, v2, 1LL) != 1 )
    return 0xFFFFFFFFLL;
  else
    return v2[0];
}
