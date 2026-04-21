/*
 * func-name: sub_401F6A
 * func-address: 0x401f6a
 * export-type: decompile
 * callers: 0x401f6a
 * callees: 0x401040, 0x401d60, 0x401f3d
 */

void __fastcall __noreturn sub_401F6A(__int64 (__fastcall *a1)(_QWORD, __int64, __int64), unsigned int a2, __int64 a3)
{
  __int64 v3; // r13
  unsigned int v5; // eax

  v3 = a3 + 8LL * (int)a2 + 8;
  sub_401F3D();
  v5 = a1(a2, a3, v3);
  sub_401040(v5);
}
