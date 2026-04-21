/*
 * func-name: sub_408544
 * func-address: 0x408544
 * export-type: decompile
 * callers: 0x401d60
 * callees: 0x406300
 */

unsigned __int64 __fastcall sub_408544(const __m128i *a1)
{
  unsigned __int64 result; // rax

  if ( a1 )
    sub_406300((__m128i *)&unk_4136E0, a1, 8uLL);
  else
    unk_4136E0 = 1103515245LL * (_QWORD)&unk_4136E0;
  *((_BYTE *)&unk_4136E0 + 1) = 0;
  result = __readfsqword(0);
  *(_QWORD *)(result + 40) = unk_4136E0;
  return result;
}
