// #ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Virtual memory module mm/mm-vm.c
 */

 #include "string.h"
 #include "mm.h"
 #include <stdlib.h>
 #include <stdio.h>
 #include <pthread.h>
 
 /*get_vma_by_num - get vm area by numID
  *@mm: memory region
  *@vmaid: ID vm area to alloc memory region
  *
  */
 struct vm_area_struct *get_vma_by_num(struct mm_struct *mm, int vmaid)
 {
   struct vm_area_struct *pvma = mm->mmap;
 
   if (mm->mmap == NULL)
     return NULL;
 
   int vmait = pvma->vm_id;
 
   while (vmait < vmaid)
   {
     if (pvma == NULL)
       return NULL;
 
     pvma = pvma->vm_next;
     vmait = pvma->vm_id;
   }
 
   return pvma;
 }
 
 int __mm_swap_page(struct pcb_t *caller, int vicfpn , int swpfpn)
 {
     __swap_cp_page(caller->mram, vicfpn, caller->active_mswp, swpfpn);
     return 0;
 }
 
 /*get_vm_area_node - get vm area for a number of pages
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@incpgnum: number of page
  *@vmastart: vma end
  *@vmaend: vma end
  *
  */
 struct vm_rg_struct *get_vm_area_node_at_brk(struct pcb_t *caller, int vmaid, int size, int alignedsz)
 {
   struct vm_rg_struct *newrg;
   /* TODO retrive current vma to obtain newrg, current comment out due to compiler redundant warning*/
   //struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
 
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
   if (cur_vma == NULL) {
     return NULL;
   }
 
   int old_sbrk = cur_vma->sbrk;
   alignedsz = PAGING_PAGE_ALIGNSZ(size);
 
   newrg = malloc(sizeof(struct vm_rg_struct));
   if (newrg == NULL) {
     return NULL;
   }
 
   /* TODO: update the newrg boundary
   // newrg->rg_start = ...
   // newrg->rg_end = ...
   */

   cur_vma->sbrk = newrg->rg_end + 1; // Update sbrk to the next address after the allocated region
 
   return newrg;
 }
 
 /*validate_overlap_vm_area
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@vmastart: vma end
  *@vmaend: vma end
  *
  */
 int validate_overlap_vm_area(struct pcb_t *caller, int vmaid, int vmastart, int vmaend)
 {
   //struct vm_area_struct *vma = caller->mm->mmap;
 
   /* TODO validate the planned memory area is not overlapped */
 
   struct vm_area_struct *vma = caller->mm->mmap;
 
   while (vma != NULL) {
     if (vma->vm_id == vmaid) {
       vma = vma->vm_next;
       continue;
     }

     if ((vmastart - (int)vma->vm_end >= 0) && (vmastart - (int)vma->vm_start <= 0)) {
       return -1;
     }
 
     if ((vmastart - (int)vma->vm_end <= 0) && (vmastart - (int)vma->vm_start >= 0)) {
       return -1;
     }
 
     if ((vmaend - (int)vma->vm_end >= 0) && (vmaend - (int)vma->vm_start <= 0)) {
       return -1;
     }
 
     if ((vmaend - (int)vma->vm_end <= 0) && (vmaend - (int)vma->vm_start >= 0)) {
       return -1;
     }
 
     vma = vma->vm_next;
   }
 
   return 0;
 }
 
 /*inc_vma_limit - increase vm area limits to reserve space for new variable
  *@caller: caller
  *@vmaid: ID vm area to alloc memory region
  *@inc_sz: increment size
  *
  */
 int inc_vma_limit(struct pcb_t *caller, int vmaid, int inc_sz)
 {
   struct vm_rg_struct * newrg = malloc(sizeof(struct vm_rg_struct));
   int inc_amt = PAGING_PAGE_ALIGNSZ(inc_sz);
   int incnumpage =  inc_amt / PAGING_PAGESZ;
   struct vm_rg_struct *area = get_vm_area_node_at_brk(caller, vmaid, inc_sz, inc_amt);
   struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
 
   int old_end = cur_vma->vm_end;
 
   /*Validate overlap of obtained region */
   if (validate_overlap_vm_area(caller, vmaid, area->rg_start, area->rg_end) < 0)
     return -1; /*Overlap and failed allocation */
 
   /* TODO: Obtain the new vm area based on vmaid */
   //cur_vma->vm_end... 
   // inc_limit_ret...
 
   if (area == NULL || cur_vma == NULL) {
    return -1;
   }

   int old_sbrk = cur_vma->sbrk;
   if (vmaid == 0) {
     cur_vma->vm_end = PAGING_PAGE_ALIGNSZ(old_sbrk + inc_sz) - 1;
   } else {
     cur_vma->vm_end = ((old_sbrk - inc_sz) / PAGING_PAGESZ) * PAGING_PAGESZ;
   }
 
   struct vm_rg_struct *newrg = malloc(sizeof(struct vm_rg_struct));
   if (newrg == NULL) {
    return -1;
   }
   newrg->rg_start = area->rg_start;
   newrg->rg_end = area->rg_end;
   newrg->rg_next = NULL;
 
   if (vm_map_ram(caller, area->rg_start, area->rg_end, old_end, incnumpage , newrg) < 0)
     return -1; /* Map the memory to MEMRAM */
 
   return 0;
 }
 
 // #endif
 