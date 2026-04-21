/*
 * func-name: sub_406300
 * func-address: 0x406300
 * export-type: decompile
 * callers: 0x402680, 0x406b37, 0x407580, 0x407fbc, 0x408544
 * callees: none
 */

__m128i *__fastcall sub_406300(__m128i *a1, const __m128i *a2, unsigned __int64 n0x1F)
{
  __m128i *v3; // r8
  const __m128i *v5; // rax
  __m128i *v6; // rcx
  __int64 n3; // rdx
  __m128i v8; // xmm1
  __int8 v9; // al
  __int8 *v10; // rbx
  __int8 *v11; // r11
  unsigned __int64 v12; // r10
  char v13; // r14
  __m128i *v14; // rdx
  __int8 *v15; // rax
  unsigned __int64 v16; // r10
  const __m128i *v17; // rdx
  __m128i v18; // xmm0
  __int8 *v19; // rdx
  __int64 *v20; // rax
  char v21; // cl
  __int64 v22; // rsi
  int v23; // esi
  __int16 v24; // si
  __m128 v26; // xmm2
  __m128 v27; // xmm0
  __m128i v28; // xmm0
  __m128i *v29; // rax
  __int64 v30; // rdx
  __m128i *v31; // rdi
  __int32 v32; // eax
  __int16 v33; // ax
  __int8 *v34; // rax
  __m128i *v35; // rdx
  __m128 v36; // xmm2
  __m128 v37; // xmm0
  __int8 *v38; // rax
  __m128i *v39; // rdx
  __m128 v40; // xmm2
  __m128 v41; // xmm0

  v3 = a1;
  while ( 1 )
  {
    v5 = a2;
    if ( ((unsigned __int8)a2 & 3) == 0 )
      break;
    if ( !n0x1F )
    {
      v6 = a1;
      if ( ((unsigned __int8)a1 & 3) != 0 )
        return v3;
LABEL_12:
      v16 = n0x1F & 0xFFFFFFFFFFFFFFF0LL;
      v17 = (const __m128i *)((char *)a2 + (n0x1F & 0xFFFFFFFFFFFFFFF0LL));
      while ( a2 != v17 )
      {
        v18 = _mm_loadu_si128(a2);
        ++a1;
        ++a2;
        a1[-1] = v18;
      }
      v19 = &v6->m128i_i8[v16];
      v20 = (__int64 *)((char *)v5->m128i_i64 + v16);
      v21 = n0x1F - v16;
      if ( (n0x1F & 8) != 0 )
      {
        v22 = *v20;
        v19 += 8;
        ++v20;
        *((_QWORD *)v19 - 1) = v22;
      }
      if ( (v21 & 4) != 0 )
      {
        v23 = *(_DWORD *)v20;
        v19 += 4;
        v20 = (__int64 *)((char *)v20 + 4);
        *((_DWORD *)v19 - 1) = v23;
      }
      if ( (v21 & 2) != 0 )
      {
        v24 = *(_WORD *)v20;
        v19 += 2;
        v20 = (__int64 *)((char *)v20 + 2);
        *((_WORD *)v19 - 1) = v24;
      }
      if ( (((_BYTE)n0x1F - (_BYTE)v16) & 1) != 0 )
        *v19 = *(_BYTE *)v20;
      return v3;
    }
    a1->m128i_i8[0] = a2->m128i_i8[0];
    a2 = (const __m128i *)((char *)a2 + 1);
    a1 = (__m128i *)((char *)a1 + 1);
    --n0x1F;
  }
  v6 = a1;
  n3 = (unsigned __int8)a1 & 3;
  if ( ((unsigned __int8)a1 & 3) == 0 )
    goto LABEL_12;
  if ( n0x1F > 0x1F )
  {
    v8 = _mm_cvtsi32_si128(a2->m128i_i32[0]);
    if ( n3 == 2 )
    {
      v10 = &a2->m128i_i8[2];
      v11 = &a1->m128i_i8[2];
      v12 = (n0x1F - 4) & 0xFFFFFFFFFFFFFFF0LL;
      v13 = n0x1F - 2;
      a1->m128i_i16[0] = a2->m128i_i16[0];
      v34 = &a2->m128i_i8[2];
      v35 = (__m128i *)&a1->m128i_i16[1];
      while ( v34 != (char *)&a2->m128i_i16[1] + v12 )
      {
        v36 = (__m128)_mm_loadu_si128((const __m128i *)(v34 + 2));
        v37 = (__m128)_mm_shuffle_epi32(v8, 0);
        v34 += 16;
        v8 = _mm_cvtsi32_si128(*(_DWORD *)(v34 - 2));
        ++v35;
        v35[-1] = _mm_or_si128(
                    _mm_srli_epi32((__m128i)_mm_shuffle_ps(_mm_shuffle_ps(v37, v36, 0), v36, 152), 0x10u),
                    _mm_slli_epi32((__m128i)v36, 0x10u));
      }
    }
    else if ( n3 == 3 )
    {
      v10 = &a2->m128i_i8[1];
      a1->m128i_i8[0] = a2->m128i_i8[0];
      v11 = &a1->m128i_i8[1];
      v12 = (n0x1F - 4) & 0xFFFFFFFFFFFFFFF0LL;
      v13 = n0x1F - 1;
      v38 = &a2->m128i_i8[1];
      v39 = (__m128i *)&a1->m128i_i8[1];
      while ( v38 != &a2->m128i_i8[v12 + 1] )
      {
        v40 = (__m128)_mm_loadu_si128((const __m128i *)(v38 + 3));
        v41 = (__m128)_mm_shuffle_epi32(v8, 0);
        v38 += 16;
        v8 = _mm_cvtsi32_si128(*(_DWORD *)(v38 - 1));
        ++v39;
        v39[-1] = _mm_or_si128(
                    _mm_srli_epi32((__m128i)_mm_shuffle_ps(_mm_shuffle_ps(v41, v40, 0), v40, 152), 8u),
                    _mm_slli_epi32((__m128i)v40, 0x18u));
      }
    }
    else
    {
      v9 = a2->m128i_i8[2];
      v10 = &a2->m128i_i8[3];
      v11 = &a1->m128i_i8[3];
      v12 = (n0x1F - 4) & 0xFFFFFFFFFFFFFFF0LL;
      v13 = n0x1F - 3;
      a1->m128i_i16[0] = a2->m128i_i16[0];
      v14 = (__m128i *)((char *)a1->m128i_i32 + 3);
      a1->m128i_i8[2] = v9;
      v15 = &a2->m128i_i8[3];
      while ( v15 != (char *)a2->m128i_i32 + v12 + 3 )
      {
        v26 = (__m128)_mm_loadu_si128((const __m128i *)(v15 + 1));
        v27 = (__m128)_mm_shuffle_epi32(v8, 0);
        v15 += 16;
        v8 = _mm_cvtsi32_si128(*(_DWORD *)(v15 - 3));
        ++v14;
        v14[-1] = _mm_or_si128(
                    _mm_srli_epi32((__m128i)_mm_shuffle_ps(_mm_shuffle_ps(v27, v26, 0), v26, 152), 0x18u),
                    _mm_slli_epi32((__m128i)v26, 8u));
      }
    }
    a2 = (const __m128i *)&v10[v12];
    a1 = (__m128i *)&v11[v12];
    LOBYTE(n0x1F) = v13 - v12;
  }
  if ( (n0x1F & 0x10) != 0 )
  {
    v28 = _mm_loadu_si128(a2);
    v29 = a1 + 1;
    ++a2;
    *a1 = v28;
  }
  else
  {
    v29 = a1;
  }
  if ( (n0x1F & 8) != 0 )
  {
    v30 = a2->m128i_i64[0];
    v31 = (__m128i *)&v29->m128i_u64[1];
    a2 = (const __m128i *)((char *)a2 + 8);
    v29->m128i_i64[0] = v30;
  }
  else
  {
    v31 = v29;
  }
  if ( (n0x1F & 4) != 0 )
  {
    v32 = a2->m128i_i32[0];
    v31 = (__m128i *)((char *)v31 + 4);
    a2 = (const __m128i *)((char *)a2 + 4);
    v31[-1].m128i_i32[3] = v32;
  }
  if ( (n0x1F & 2) != 0 )
  {
    v33 = a2->m128i_i16[0];
    v31 = (__m128i *)((char *)v31 + 2);
    a2 = (const __m128i *)((char *)a2 + 2);
    v31[-1].m128i_i16[7] = v33;
  }
  if ( (n0x1F & 1) != 0 )
    v31->m128i_i8[0] = a2->m128i_i8[0];
  return v3;
}
