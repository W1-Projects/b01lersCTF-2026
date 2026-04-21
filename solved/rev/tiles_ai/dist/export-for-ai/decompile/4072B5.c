/*
 * func-name: sub_4072B5
 * func-address: 0x4072b5
 * export-type: decompile
 * callers: none
 * callees: 0x406dfb
 */

signed __int64 __fastcall sub_4072B5(__int64 a1, char *iov_base, __int64 a3)
{
  __int64 v7; // rdx
  __int64 v8; // rsi
  unsigned __int64 fd; // rdi
  size_t iov_len; // rax
  signed __int64 iov_len_1; // rax
  signed __int64 iov_len_2; // rdx
  char *v13; // rdx
  struct iovec vec_; // [rsp+0h] [rbp-38h] BYREF
  __int64 v16; // [rsp+10h] [rbp-28h]
  __int64 v17; // [rsp+18h] [rbp-20h]

  v7 = *(_QWORD *)(a1 + 96);
  vec_.iov_base = iov_base;
  v8 = *(_QWORD *)(a1 + 88);
  v17 = v7;
  fd = *(int *)(a1 + 120);
  iov_len = (v7 == 0) + a3 - 1;
  v16 = v8;
  vec_.iov_len = iov_len;
  if ( iov_len )
  {
    iov_len_1 = sub_406DFB(sys_readv(fd, &vec_, 2uLL));
  }
  else
  {
    __asm { syscall; LINUX - }
    iov_len_1 = sub_406DFB((unsigned __int8)iov_len);
  }
  if ( iov_len_1 <= 0 )
  {
    iov_len_2 = 0LL;
    *(_DWORD *)a1 |= iov_len_1 == 0 ? 16 : 32;
  }
  else
  {
    iov_len_2 = iov_len_1;
    if ( vec_.iov_len < iov_len_1 )
    {
      v13 = *(char **)(a1 + 88);
      *(__m128i *)(a1 + 8) = _mm_unpacklo_epi64(
                               (__m128i)(unsigned __int64)v13,
                               (__m128i)(unsigned __int64)&v13[iov_len_1 - vec_.iov_len]);
      if ( *(_QWORD *)(a1 + 96) )
      {
        *(_QWORD *)(a1 + 8) = v13 + 1;
        iov_base[a3 - 1] = *v13;
      }
      return a3;
    }
  }
  return iov_len_2;
}
