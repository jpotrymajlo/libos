#include <libos.h>
#include <arch.h>
#include <stdio.h>


void bad_mode (void)
{

}

void show_regs (struct lb_registers *regs)
{
  printk(" cpsr - 0x%.8x", regs->cpsr); 
  printk(" r0   - 0x%.8x", regs->r0);
  printk(" r1   - 0x%.8x\n", regs->r1);
  printk(" r2   - 0x%.8x", regs->r2);
  printk(" r3   - 0x%.8x", regs->r3);
  printk(" r4   - 0x%.8x\n", regs->r4);
  printk(" r5   - 0x%.8x", regs->r5);
  printk(" r6   - 0x%.8x", regs->r6);
  printk(" r7   - 0x%.8x\n", regs->r7);
  printk(" r8   - 0x%.8x", regs->r8);
  printk(" r9   - 0x%.8x", regs->r9);
  printk(" r10  - 0x%.8x\n", regs->r10);
  printk(" r11  - 0x%.8x", regs->r11);
  printk(" r12  - 0x%.8x", regs->r12);
  printk(" lr   - 0x%.8x\n", regs->lr);
  printk(" pc   - 0x%.8x", regs->pc);

  return;
}

void do_undefined_instruction (struct lb_registers *pt_regs)
{
  const char* str_ptr="undefined instruction"; 
  printk("%s\n", str_ptr);	
  show_regs (pt_regs);
  printk("[PC] = 0x%x\n", *(UINT32*)(pt_regs->pc));
  printk("current task = %s\n", pCurrTask->bTaskName);
  bad_mode ();
}

void do_software_interrupt (struct lb_registers *pt_regs)
{
  const char* str_ptr="software interrupt"; 
  printk("%s\n", str_ptr);  
  show_regs (pt_regs);
  printk("current task = %s\n", pCurrTask->bTaskName);
  bad_mode ();
}

void do_prefetch_abort (struct lb_registers *pt_regs)
{
  const char* str_ptr="prefetch abort"; 
  printk("%s\n", str_ptr);  
  show_regs (pt_regs);
  printk("current task = %s\n", pCurrTask->bTaskName);
  bad_mode ();
}

void do_data_abort (struct lb_registers *pt_regs)
{
  const char* str_ptr="data abort"; 
  printk("%s\n", str_ptr);	
  show_regs (pt_regs);
  printk("current task = %s\n", pCurrTask->bTaskName);
  bad_mode ();
}

void do_not_used (struct lb_registers *pt_regs)
{
  const char* str_ptr="not used"; 
  printk("%s\n", str_ptr);  
  show_regs (pt_regs);
  printk("current task = %s\n", pCurrTask->bTaskName);
  bad_mode ();
}

void do_fiq (struct lb_registers *pt_regs)
{
  const char* str_ptr="do fiq"; 
  printk("%s\n", str_ptr);  
  show_regs (pt_regs);
  printk("current task = %s\n", pCurrTask->bTaskName);
  bad_mode ();
}

/*void do_irq (struct pt_regs *pt_regs)
{
  puts ("irq");	
  show_regs (pt_regs);
  bad_mode ();
}

*/

