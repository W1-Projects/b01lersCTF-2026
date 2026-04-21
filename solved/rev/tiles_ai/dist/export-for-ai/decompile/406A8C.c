/*
 * func-name: sub_406A8C
 * func-address: 0x406a8c
 * export-type: decompile
 * callers: 0x4033a0, 0x403cfb, 0x407b60
 * callees: none
 */

signed __int64 __fastcall sub_406A8C(
        volatile signed __int32 *uaddr,
        __int64 a2,
        __int64 a3,
        __int64 a4,
        u32 *uaddr2,
        u32 val3)
{
  struct timespec *utime; // r10
  signed __int64 result; // rax
  u32 val; // edx
  u32 *uaddr2_1; // r8
  u32 val3_1; // r9d
  struct timespec *utime_1; // r10

  result = *(unsigned int *)uaddr;
  if ( (int)result < 0 )
  {
    result = (unsigned int)_InterlockedExchangeAdd(uaddr, 0x7FFFFFFFu);
    if ( (_DWORD)result != -2147483647 )
    {
      result = sys_futex((u32 *)uaddr, 129, 1u, utime, uaddr2, val3);
      if ( result == -38 )
        return sys_futex((u32 *)uaddr, 1, val, utime_1, uaddr2_1, val3_1);
    }
  }
  return result;
}
