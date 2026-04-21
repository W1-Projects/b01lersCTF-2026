/*
 * func-name: sub_401F3D
 * func-address: 0x401f3d
 * export-type: decompile
 * callers: 0x401f6a
 * callees: 0x401000
 */

void sub_401F3D()
{
  void (**i)(void); // rbx

  init_proc();
  for ( i = (void (**)(void))&off_411FA0; i < (void (**)(void))&off_411FA8; ++i )
    (*i)();
}
