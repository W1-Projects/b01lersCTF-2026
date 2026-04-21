/*
 * func-name: sub_407DE6
 * func-address: 0x407de6
 * export-type: decompile
 * callers: 0x406f21
 * callees: 0x406d99
 */

__int64 __fastcall sub_407DE6(_BYTE *a1, int n0x7F)
{
  __int64 n2; // rax

  if ( !a1 )
    return 1LL;
  if ( (unsigned int)n0x7F <= 0x7F )
  {
    *a1 = n0x7F;
    return 1LL;
  }
  if ( **(_QWORD **)(__readfsqword(0) + 168) )
  {
    if ( (unsigned int)n0x7F <= 0x7FF )
    {
      a1[1] = n0x7F & 0x3F | 0x80;
      n2 = 2LL;
      *a1 = (n0x7F >> 6) | 0xC0;
    }
    else if ( (unsigned int)(n0x7F - 57344) <= 0x1FFF || (unsigned int)n0x7F <= 0xD7FF )
    {
      *a1 = (n0x7F >> 12) | 0xE0;
      a1[2] = n0x7F & 0x3F | 0x80;
      n2 = 3LL;
      a1[1] = (n0x7F >> 6) & 0x3F | 0x80;
    }
    else
    {
      if ( (unsigned int)(n0x7F - 0x10000) > 0xFFFFF )
      {
LABEL_8:
        *(_DWORD *)_readfsqword_() = 84;
        return -1LL;
      }
      *(_DWORD *)a1 = (unsigned __int8)(n0x7F >> 18) | (((n0x7F >> 12) & 0x3F | (((n0x7F >> 6) & 0x3F | ((n0x7F & 0x3F) << 8)) << 8)) << 8) | 0x808080F0;
      return 4LL;
    }
  }
  else
  {
    if ( (unsigned int)(n0x7F - 57216) > 0x7F )
      goto LABEL_8;
    *a1 = n0x7F;
    return 1LL;
  }
  return n2;
}
