/*
 * func-name: sub_4069DB
 * func-address: 0x4069db
 * export-type: decompile
 * callers: 0x4033a0, 0x403cdf, 0x407b60
 * callees: none
 */

void __fastcall sub_4069DB(volatile signed __int32 *uaddr, __int64 a2, __int64 a3, __int64 a4, __int64 a5, u32 val3)
{
  int v6; // ecx
  signed __int32 v7; // eax
  int v8; // edx
  int n11; // esi
  signed __int32 v10; // ecx
  signed __int32 v11; // r8d
  unsigned __int64 val_1; // r8
  signed __int32 val_3; // edx
  unsigned __int32 val_2; // eax
  u32 val; // edx
  u32 *uaddr2; // r8
  struct timespec *utime; // r10
  signed __int64 v18; // rax

  v6 = byte_4128A3;
  if ( byte_4128A3 )
  {
    v7 = _InterlockedCompareExchange(uaddr, -2147483647, 0);
    v8 = v7;
    if ( v6 < 0 )
      byte_4128A3 = 0;
    n11 = 11;
    if ( v7 )
    {
      while ( --n11 )
      {
        if ( v8 < 0 )
        {
          v10 = v8 + 0x7FFFFFFF;
          v11 = v8;
        }
        else
        {
          v11 = v8 - 0x7FFFFFFF;
          v10 = v8;
        }
        v8 = _InterlockedCompareExchange(uaddr, v11, v10);
        if ( v10 == v8 )
          return;
      }
      val_1 = (unsigned int)_InterlockedIncrement(uaddr);
      do
      {
        val_3 = val_1;
        if ( (val_1 & 0x80000000) != 0LL )
        {
          if ( sys_futex((u32 *)uaddr, 128, val_1, 0LL, (u32 *)val_1, val3) == -38 )
            v18 = sys_futex((u32 *)uaddr, 0, val, utime, uaddr2, val3);
          val_3 = (_DWORD)uaddr2 + 0x7FFFFFFF;
        }
        val_2 = _InterlockedCompareExchange(uaddr, val_3 + 0x80000000, val_3);
        val_1 = val_2;
      }
      while ( val_3 != val_2 );
    }
  }
}
