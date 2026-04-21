/*
 * func-name: sub_401FD7
 * func-address: 0x401fd7
 * export-type: decompile
 * callers: 0x401040
 * callees: none
 */

__int64 sub_401FD7()
{
  void (**i)(void); // rbx

  for ( i = (void (**)(void))&off_411FB0; &off_411FA8 < (__int64 (__fastcall **)())i; (*i)() )
    --i;
  return term_proc();
}
