/* file malloc2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 void *malloc_safe(size_t size, char *calling_proc)
 /********************************************************/
 /*  Like malloc(size) except fatal err if malloc fails. */
 /********************************************************/
 {char *proc = "malloc_safe";
  void *x;
if (db_level > 3) printf("%sdoing %s\n",pdb,proc);
  x = malloc(size);
  if (x == NULL) fatal_error(calling_proc,"MEM ALLOC",-1);
  if (memory_trace) printf("%s MALLOC %x\n",pdb,(int)x);
  return x;
 }
/* end malloc2.c */
