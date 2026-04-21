/*
 * func-name: sub_403E59
 * func-address: 0x403e59
 * export-type: decompile
 * callers: 0x405300
 * callees: none
 */

void __fastcall sub_403E59(long double *a1, int a2, int *a3)
{
  unsigned int v3; // eax
  int *v4; // rcx
  unsigned int v5; // eax
  unsigned int *v6; // rcx
  unsigned int v7; // eax
  __int16 *v8; // rcx
  unsigned int v9; // eax
  unsigned __int16 *v10; // rcx
  unsigned int v11; // eax
  char *v12; // rcx
  unsigned int v13; // eax
  unsigned __int8 *v14; // rcx
  unsigned int v15; // eax
  _QWORD *v16; // rcx
  unsigned int n0xAF; // eax
  double *v18; // rcx
  long double *v19; // rax

  switch ( a2 )
  {
    case 9:
    case 12:
    case 13:
    case 14:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
      v15 = *a3;
      if ( (unsigned int)*a3 > 0x2F )
      {
        v16 = (_QWORD *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v16 + 1;
      }
      else
      {
        v16 = (_QWORD *)(*((_QWORD *)a3 + 2) + v15);
        *a3 = v15 + 8;
      }
      *(_QWORD *)a1 = *v16;
      break;
    case 10:
      v3 = *a3;
      if ( (unsigned int)*a3 > 0x2F )
      {
        v4 = (int *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v4 + 2;
      }
      else
      {
        v4 = (int *)(*((_QWORD *)a3 + 2) + v3);
        *a3 = v3 + 8;
      }
      *(_QWORD *)a1 = *v4;
      break;
    case 11:
      v5 = *a3;
      if ( (unsigned int)*a3 > 0x2F )
      {
        v6 = (unsigned int *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v6 + 2;
      }
      else
      {
        v6 = (unsigned int *)(*((_QWORD *)a3 + 2) + v5);
        *a3 = v5 + 8;
      }
      *(_QWORD *)a1 = *v6;
      break;
    case 15:
      v7 = *a3;
      if ( (unsigned int)*a3 > 0x2F )
      {
        v8 = (__int16 *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v8 + 4;
      }
      else
      {
        v8 = (__int16 *)(*((_QWORD *)a3 + 2) + v7);
        *a3 = v7 + 8;
      }
      *(_QWORD *)a1 = *v8;
      break;
    case 16:
      v9 = *a3;
      if ( (unsigned int)*a3 > 0x2F )
      {
        v10 = (unsigned __int16 *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v10 + 4;
      }
      else
      {
        v10 = (unsigned __int16 *)(*((_QWORD *)a3 + 2) + v9);
        *a3 = v9 + 8;
      }
      *(_QWORD *)a1 = *v10;
      break;
    case 17:
      v11 = *a3;
      if ( (unsigned int)*a3 > 0x2F )
      {
        v12 = (char *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v12 + 8;
      }
      else
      {
        v12 = (char *)(*((_QWORD *)a3 + 2) + v11);
        *a3 = v11 + 8;
      }
      *(_QWORD *)a1 = *v12;
      break;
    case 18:
      v13 = *a3;
      if ( (unsigned int)*a3 > 0x2F )
      {
        v14 = (unsigned __int8 *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v14 + 8;
      }
      else
      {
        v14 = (unsigned __int8 *)(*((_QWORD *)a3 + 2) + v13);
        *a3 = v13 + 8;
      }
      *(_QWORD *)a1 = *v14;
      break;
    case 25:
      n0xAF = a3[1];
      if ( n0xAF > 0xAF )
      {
        v18 = (double *)*((_QWORD *)a3 + 1);
        *((_QWORD *)a3 + 1) = v18 + 1;
      }
      else
      {
        v18 = (double *)(*((_QWORD *)a3 + 2) + n0xAF);
        a3[1] = n0xAF + 16;
      }
      *a1 = *v18;
      break;
    case 26:
      v19 = (long double *)((*((_QWORD *)a3 + 1) + 15LL) & 0xFFFFFFFFFFFFFFF0LL);
      *((_QWORD *)a3 + 1) = v19 + 1;
      *a1 = *v19;
      break;
    default:
      return;
  }
}
