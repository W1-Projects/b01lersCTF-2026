/*
 * func-name: sub_40714D
 * func-address: 0x40714d
 * export-type: decompile
 * callers: 0x403ac9, 0x403da5, 0x40611d, 0x407659, 0x407ef2
 * callees: none
 */

__int64 __fastcall sub_40714D(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, u32 val3)
{
  signed __int32 v6; // r8d
  __int64 result; // rax
  volatile signed __int32 *uaddr; // rdi
  signed __int32 v10; // eax
  u32 *uaddr2; // r8
  signed __int32 v12; // ecx
  int val_1; // edx
  u32 val; // edx
  signed __int64 v15; // rax

  v6 = *(_DWORD *)(__readfsqword(0) + 48);
  result = 0LL;
  if ( (*(_DWORD *)(a1 + 140) & 0xBFFFFFFF) != v6 )
  {
    uaddr = (volatile signed __int32 *)(a1 + 140);
    v10 = _InterlockedCompareExchange((volatile signed __int32 *)(a1 + 140), v6, 0);
    uaddr2 = (u32 *)(v6 | 0x40000000u);
    if ( v10 )
    {
      while ( 1 )
      {
        v12 = _InterlockedCompareExchange(uaddr, (signed __int32)uaddr2, 0);
        if ( !v12 )
          break;
        val_1 = v12 | 0x40000000;
        if ( ((v12 & 0x40000000) != 0 || _InterlockedCompareExchange(uaddr, val_1, v12) == v12)
          && sys_futex((u32 *)uaddr, 128, val_1, 0LL, uaddr2, val3) == -38 )
        {
          v15 = sys_futex((u32 *)uaddr, 0, val, 0LL, uaddr2, val3);
        }
      }
    }
    return 1LL;
  }
  return result;
}
