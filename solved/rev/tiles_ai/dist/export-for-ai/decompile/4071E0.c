/*
 * func-name: sub_4071E0
 * func-address: 0x4071e0
 * export-type: decompile
 * callers: 0x403ac9, 0x403da5, 0x40611d, 0x407659
 * callees: none
 */

signed __int64 __fastcall sub_4071E0(__int64 a1, __int64 a2, __int64 a3, __int64 a4, u32 *uaddr2, u32 val3)
{
  struct timespec *utime; // r10
  u32 *uaddr; // rdi
  signed __int64 result; // rax
  u32 val; // edx
  u32 *uaddr2_1; // r8
  u32 val3_1; // r9d
  struct timespec *utime_1; // r10

  uaddr = (u32 *)(a1 + 140);
  result = (unsigned int)_InterlockedExchange((volatile __int32 *)(a1 + 140), 0);
  if ( (result & 0x40000000) != 0 )
  {
    result = sys_futex(uaddr, 129, 1u, utime, uaddr2, val3);
    if ( result == -38 )
      return sys_futex(uaddr, 1, val, utime_1, uaddr2_1, val3_1);
  }
  return result;
}
