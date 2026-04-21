/*
 * func-name: sub_403DA5
 * func-address: 0x403da5
 * export-type: decompile
 * callers: none
 * callees: 0x40714d, 0x4071e0, 0x40721e, 0x407539
 */

__int64 __fastcall sub_403DA5(__int64 a1)
{
  bool v1; // sf
  __int64 result; // rax
  int v3; // ebx
  _BYTE *v4; // rax
  unsigned int v5; // [rsp+8h] [rbp-10h]

  if ( dword_4121AC < 0 )
  {
    v1 = (int)sub_407539(a1, &unk_412120) < 0;
    result = 0xFFFFFFFFLL;
    if ( v1 )
      return result;
    v3 = 0;
    goto LABEL_4;
  }
  v3 = sub_40714D(&unk_412120);
  v1 = (int)sub_407539(a1, &unk_412120) < 0;
  result = 0xFFFFFFFFLL;
  if ( !v1 )
  {
LABEL_4:
    if ( n10 == 10 || (v4 = (_BYTE *)qword_412148, qword_412148 == qword_412140) )
    {
      result = (unsigned int)((int)sub_40721E(&unk_412120, 10LL) >> 31);
    }
    else
    {
      ++qword_412148;
      *v4 = 10;
      result = 0LL;
    }
  }
  if ( v3 )
  {
    v5 = result;
    sub_4071E0(&unk_412120);
    return v5;
  }
  return result;
}
