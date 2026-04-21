/*
 * func-name: sub_40738E
 * func-address: 0x40738e
 * export-type: decompile
 * callers: 0x406f39, 0x407491
 * callees: 0x406dfb
 */

size_t __fastcall sub_40738E(__m128i *a1, __int64 a2, size_t a3)
{
  _BYTE *iov_base_1; // rax
  _BYTE *iov_base; // rdx
  size_t iov_len_2; // r14
  int n2; // ebp
  iovec *p_vec; // r15
  unsigned __int64 n2_1; // r12
  __int64 v10; // rax
  __int64 v11; // rcx
  __m128i v12; // xmm1
  size_t result; // rax
  __int64 iov_len_1; // rax
  size_t iov_len; // rdx
  iovec vec; // [rsp+10h] [rbp-58h] BYREF
  _QWORD p_vec_1[9]; // [rsp+20h] [rbp-48h] BYREF

  iov_base_1 = (_BYTE *)a1[2].m128i_i64[1];
  iov_base = (_BYTE *)a1[3].m128i_i64[1];
  p_vec_1[0] = a2;
  vec.iov_base = iov_base;
  vec.iov_len = iov_base_1 - iov_base;
  iov_len_2 = iov_base_1 - iov_base + a3;
  p_vec_1[1] = a3;
  if ( iov_base_1 == iov_base )
  {
    n2 = 1;
    p_vec = (iovec *)p_vec_1;
  }
  else
  {
    n2 = 2;
    p_vec = &vec;
  }
  n2_1 = n2;
  while ( 1 )
  {
    iov_len_1 = sub_406DFB(sys_writev(a1[7].m128i_i32[2], p_vec, n2_1));
    if ( iov_len_2 == iov_len_1 )
    {
      v10 = a1[5].m128i_i64[1];
      v11 = v10 + a1[6].m128i_i64[0];
      v12 = (__m128i)(unsigned __int64)v10;
      a1[3].m128i_i64[1] = v10;
      result = a3;
      a1[2] = _mm_unpacklo_epi64((__m128i)(unsigned __int64)v11, v12);
      return result;
    }
    if ( iov_len_1 < 0 )
      break;
    iov_len = p_vec->iov_len;
    iov_len_2 -= iov_len_1;
    if ( iov_len < iov_len_1 )
    {
      --n2;
      iov_len_1 -= iov_len;
      ++p_vec;
      iov_len = p_vec->iov_len;
      n2_1 = n2;
    }
    p_vec->iov_base = (char *)p_vec->iov_base + iov_len_1;
    p_vec->iov_len = iov_len - iov_len_1;
  }
  a1->m128i_i32[0] |= 0x20u;
  result = 0LL;
  a1[3].m128i_i64[1] = 0LL;
  a1[2] = 0LL;
  if ( n2 != 2 )
    return a3 - p_vec->iov_len;
  return result;
}
