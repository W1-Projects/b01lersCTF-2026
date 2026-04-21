/*
 * func-name: sub_407659
 * func-address: 0x407659
 * export-type: decompile
 * callers: 0x407539
 * callees: 0x40714d, 0x4071e0, 0x407580
 */

unsigned __int64 __fastcall sub_407659(
        const __m128i *a1,
        unsigned __int64 a2,
        __int64 a3,
        __int64 a4,
        __int64 a5,
        u32 val3)
{
  __int64 v6; // rax
  unsigned __int64 n0x1F; // r13
  __int64 v9; // r12
  unsigned __int64 n0x1F_1; // rax
  int v12; // r14d
  __int64 v13; // rdx
  __int64 v14; // rcx
  u32 *uaddr2; // r8
  u32 val3_1; // r9d
  unsigned __int64 n0x1F_2; // [rsp+8h] [rbp-30h]

  v6 = 0LL;
  n0x1F = a3 * a2;
  if ( a2 )
    v6 = a3;
  v9 = v6;
  if ( *(int *)(a4 + 140) >= 0 )
  {
    v12 = sub_40714D(a4, a2, a3, a4, a5, val3);
    n0x1F_1 = sub_407580(a1, n0x1F, a4);
    if ( v12 )
    {
      n0x1F_2 = n0x1F_1;
      sub_4071E0(a4, n0x1F, v13, v14, uaddr2, val3_1);
      n0x1F_1 = n0x1F_2;
    }
  }
  else
  {
    n0x1F_1 = sub_407580(a1, n0x1F, a4);
  }
  if ( n0x1F != n0x1F_1 )
    return n0x1F_1 / a2;
  return v9;
}
