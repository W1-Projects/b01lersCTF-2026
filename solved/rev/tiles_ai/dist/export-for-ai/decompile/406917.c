/*
 * func-name: sub_406917
 * func-address: 0x406917
 * export-type: decompile
 * callers: 0x403fe0, 0x406780, 0x406f39
 * callees: none
 */

unsigned __int64 *__fastcall sub_406917(unsigned __int64 *buf, unsigned __int8 a2, unsigned __int64 n0x3E)
{
  unsigned __int64 value; // rax
  unsigned __int64 *buf_1; // r8
  unsigned __int64 n0x3E_1; // rcx
  __int64 v7; // rdx

  value = 0x101010101010101LL * a2;
  if ( n0x3E > 0x7E )
  {
    buf_1 = buf;
    *(unsigned __int64 *)((char *)buf + n0x3E - 8) = value;
    n0x3E_1 = n0x3E;
    if ( ((unsigned __int8)buf & 0xF) != 0 )
    {
      v7 = -(char)buf & 0xF;
      *buf = value;
      buf[1] = value;
      n0x3E_1 -= v7;
      buf = (unsigned __int64 *)((char *)buf + v7);
    }
    memset64(buf, value, n0x3E_1 >> 3);
    return buf_1;
  }
  else
  {
    if ( (_DWORD)n0x3E )
    {
      *(_BYTE *)buf = a2;
      *((_BYTE *)buf + n0x3E - 1) = a2;
      if ( (unsigned int)n0x3E > 2 )
      {
        *(_WORD *)((char *)buf + 1) = value;
        *(_WORD *)((char *)buf + n0x3E - 3) = value;
        if ( (unsigned int)n0x3E > 6 )
        {
          *(_DWORD *)((char *)buf + 3) = value;
          *(_DWORD *)((char *)buf + n0x3E - 7) = value;
          if ( (unsigned int)n0x3E > 0xE )
          {
            *(unsigned __int64 *)((char *)buf + 7) = value;
            *(unsigned __int64 *)((char *)buf + n0x3E - 15) = value;
            if ( (unsigned int)n0x3E > 0x1E )
            {
              *(unsigned __int64 *)((char *)buf + 15) = value;
              *(unsigned __int64 *)((char *)buf + 23) = value;
              *(unsigned __int64 *)((char *)buf + n0x3E - 31) = value;
              *(unsigned __int64 *)((char *)buf + n0x3E - 23) = value;
              if ( (unsigned int)n0x3E > 0x3E )
              {
                *(unsigned __int64 *)((char *)buf + 31) = value;
                *(unsigned __int64 *)((char *)buf + 39) = value;
                *(unsigned __int64 *)((char *)buf + 47) = value;
                *(unsigned __int64 *)((char *)buf + 55) = value;
                *(unsigned __int64 *)((char *)buf + n0x3E - 63) = value;
                *(unsigned __int64 *)((char *)buf + n0x3E - 55) = value;
                *(unsigned __int64 *)((char *)buf + n0x3E - 47) = value;
                *(unsigned __int64 *)((char *)buf + n0x3E - 39) = value;
              }
            }
          }
        }
      }
    }
    return buf;
  }
}
