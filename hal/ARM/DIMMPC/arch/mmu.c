

#include <mmu.h>
#include <cache.h>
#include <stdlib.h>


static unsigned char  mmu_PageTable[0x4000]; 

void MMU_Enable(void)
{

}

void MMU_Disable(void)
{

}

void MMU_InitPageTable(void)
{
  CACHE_DisableDCache();
  CACHE_DisableICache();
  /* wyczys tablice stron */
  memset(mmu_PageTable, 0, sizeof(mmu_PageTable));
  


  // create Page Table

  /*                                 Actual    Virtual    Size    Attribute                                                  Function    */
  /*                                  Base       Base      MB      cached?          buffered?         access permissions                      */
  /*                                 xxx00000   xxx00000                                                                                     */
  X_ARM_MMU_SECTION(mmu_PageTable, 0x000,     0x500,       32,  ARM_CACHEABLE,   ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Boot flash ROMspace */
  X_ARM_MMU_SECTION(mmu_PageTable, 0x400,     0x400,       128, ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* PXA2x0 Register     */
  X_ARM_MMU_SECTION(mmu_PageTable, 0xA00,     0x000,       64,  ARM_UNCACHEABLE,   ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* SDRAM Bank 0 */
  X_ARM_MMU_SECTION(mmu_PageTable, 0xA00,     0xA00,       64,  ARM_CACHEABLE,   ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Unmapped Memory     */
  /* wlacz mmu */

  /* ustaw tablice stron */
  asm volatile ("mcr p15,0,%0,c2,c0,0" : : "r"(mmu_PageTable));
  CACHE_EnableDCache();
  CACHE_EnableICache();

  CACHE_InvalidateIAll();
  CACHE_InvalidateDAll();
}
