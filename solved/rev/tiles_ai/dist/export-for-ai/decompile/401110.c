/*
 * func-name: sub_401110
 * func-address: 0x401110
 * export-type: decompile
 * callers: none
 * callees: 0x4010a0
 */

void *sub_401110()
{
  void *result; // rax

  if ( !byte_412220 )
  {
    result = sub_4010A0();
    byte_412220 = 1;
  }
  return result;
}
