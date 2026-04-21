/*
 * func-name: sub_4068ED
 * func-address: 0x4068ed
 * export-type: decompile
 * callers: 0x405300
 * callees: 0x407a40
 */

__int64 __fastcall sub_4068ED(__int64 a1, __int64 a2)
{
  __int64 v2; // rdx
  __int64 result; // rax

  v2 = sub_407A40(a1, 0LL, a2);
  result = v2 - a1;
  if ( !v2 )
    return a2;
  return result;
}
