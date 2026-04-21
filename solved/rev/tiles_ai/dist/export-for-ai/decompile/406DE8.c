/*
 * func-name: sub_406DE8
 * func-address: 0x406de8
 * export-type: decompile
 * callers: 0x401040
 * callees: none
 */

void __fastcall __noreturn sub_406DE8(int error_code)
{
  signed __int64 v1; // rax
  signed __int64 v2; // rax

  v1 = sys_exit_group(error_code);
  while ( 1 )
    v2 = sys_exit(error_code);
}
