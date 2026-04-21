/*
 * func-name: sub_406F39
 * func-address: 0x406f39
 * export-type: decompile
 * callers: 0x403c1e
 * callees: 0x40664b, 0x406917, 0x406d99, 0x407a09, 0x407ddc
 */

__int64 __fastcall sub_406F39(unsigned int fd, _BYTE *a2)
{
  __int64 v3; // rsi
  __int64 v4; // rdx
  __int64 v5; // rcx
  unsigned __int64 *buf; // rax
  __m128i *buf_1; // rbx
  unsigned __int64 arg_1; // rdx
  signed __int64 v9; // rax
  __int32 v10; // eax
  bool v11; // zf
  __m128i v12; // xmm0
  signed __int64 arg_2; // rax
  signed __int64 v15; // rax
  unsigned __int64 arg[4]; // [rsp+8h] [rbp-20h] BYREF

  v3 = (unsigned int)(char)*a2;
  if ( !sub_40664B((__int64)"rwa", v3) )
  {
    *(_DWORD *)_readfsqword_() = 22;
    return 0LL;
  }
  buf = (unsigned __int64 *)sub_407DDC(0x4F0uLL, v3, v4, v5);
  buf_1 = (__m128i *)buf;
  if ( !buf )
    return 0LL;
  sub_406917(buf, 0, 0xE8uLL);
  if ( !sub_40664B((__int64)a2, 43) )
    buf_1->m128i_i32[0] = 4 * (*a2 == 114) + 4;
  if ( sub_40664B((__int64)a2, 101) )
    v9 = sys_fcntl(fd, 2u, 1uLL);
  if ( *a2 == 97 )
  {
    arg_2 = sys_fcntl(fd, 3u, arg_1);
    if ( (arg_2 & 0x400) == 0 )
    {
      BYTE1(arg_2) |= 4u;
      v15 = sys_fcntl(fd, 4u, (int)arg_2);
    }
    v10 = buf_1->m128i_i32[0];
    LOBYTE(v10) = buf_1->m128i_i32[0] | 0x80;
    buf_1->m128i_i32[0] = v10;
  }
  else
  {
    v10 = buf_1->m128i_i32[0];
  }
  buf_1[7].m128i_i32[2] = fd;
  buf_1[5].m128i_i64[1] = (__int64)buf_1[15].m128i_i64;
  buf_1[6].m128i_i64[0] = 1024LL;
  buf_1[9].m128i_i32[0] = -1;
  if ( (v10 & 8) == 0 && !sys_ioctl(fd, 0x5413u, (unsigned __int64)arg) )
    buf_1[9].m128i_i32[0] = 10;
  v11 = byte_4128A1 == 0;
  v12 = _mm_loadl_epi64((const __m128i *)&off_411FC8);
  buf_1[5].m128i_i64[0] = (__int64)sub_407386;
  buf_1[1].m128i_i64[1] = (__int64)sub_407292;
  buf_1[4] = _mm_unpacklo_epi64(v12, (__m128i)(unsigned __int64)sub_40738E);
  if ( v11 )
    buf_1[8].m128i_i32[3] = -1;
  return sub_407A09(buf_1);
}
