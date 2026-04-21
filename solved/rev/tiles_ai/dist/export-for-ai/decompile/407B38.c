/*
 * func-name: sub_407B38
 * func-address: 0x407b38
 * export-type: decompile
 * callers: 0x406ac7, 0x406bb6
 * callees: none
 */

__int64 __fastcall sub_407B38(int code, __int64 a2, unsigned __int64 *addr)
{
  return sys_arch_prctl((struct task_struct *)0x1002, code, addr);
}
