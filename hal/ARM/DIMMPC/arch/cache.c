
#include <types.h>
#include <cache.h>


// Enable the instruction cache
void CACHE_EnableICache(void)
{
    asm volatile (                                                      
        "mrc  p15,0,r1,c1,c0,0;"                                        
        "orr  r1,r1,#0x1000;" /* enable ICache */                       
        "mcr  p15,0,r1,c1,c0,0;"                                        
        :                                                               
        :                                                               
        : "r1" /* Clobber list */                                       
        );                                                              
}

// Disable the instruction cache (and invalidate it, required semanitcs)
void CACHE_DisableICache(void)
{
    asm volatile (                                                      
        "mrc    p15,0,r1,c1,c0,0;"                                      
        "bic    r1,r1,#0x1000;" /* disable Icache */                    
        "mcr    p15,0,r1,c1,c0,0;"                                      
        "mcr    p15,0,r1,c7,c5,0;"  /* invalidate instruction cache */  
        "nop;" /* next few instructions may be via cache */             
        "nop;"                                                          
        "nop;"                                                          
        "nop;"                                                          
        "nop;"                                                          
        "nop"                                                           
        :                                                               
        :                                                               
        : "r1" /* Clobber list */                                       
        );                                                              
}

// Query the state of the instruction cache
int CACHE_IsICacheEnable(void)
{
    register uint32 reg;                                             
    asm volatile ("mrc  p15,0,%0,c1,c0,0"                                
                  : "=r"(reg)                                            
                  :                                                      
        );                                                               
    return (0 != (0x1000 & reg)); /* Bit 12 is ICache enable */     
}

// Invalidate the entire cache
void CACHE_InvalidateIAll(void)
{
    asm volatile (                                                      
        "mcr    p15,0,r1,c7,c5,0;"  /* clear instruction cache */       
        "mcr    p15,0,r1,c8,c5,0;"  /* flush I TLB only */              
        /* cpuwait */                                                   
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              
        "mov    r1,r1;"                                                 
        "sub    pc,pc,#4;"                                              
        "nop;" /* next few instructions may be via cache */             
        "nop;"                                                          
        "nop;"                                                          
        "nop;"                                                          
        "nop;"                                                          
        "nop"                                                           
        :                                                               
        :                                                               
        : "r1" /* Clobber list */                                       
        );                                                              
}

// Synchronize the contents of the cache with memory.
// (which includes flushing out pending writes)
void CACHE_SyncICache(void)
{
    CACHE_SyncDCache();    /* ensure data gets to RAM */            
    CACHE_InvalidateIAll();/* forget all we know */       
}

// Enable the data cache
void CACHE_EnableDCache(void)
{                                           
    asm volatile (                                                      
        "mrc  p15,0,r1,c7,c10,4;"   /* drain write buffer */            
        "mrc  p15,0,r1,c1,c0,0;"                                        
        "orr  r1,r1,#0x0007;"  /* enable DCache (also ensures the */    
                               /* MMU and alignment faults are    */    
                               /* enabled)                        */    
        "mcr  p15,0,r1,c1,c0,0;"                                        
	"mrc  p15,0,r1,c1,c0,1;"                                        
	"bic  r1,r1,#1;"                                                
	"mcr  p15,0,r1,c1,c0,1;"                                        
        :                                                               
        :                                                               
        : "r1" /* Clobber list */                                       
        );                                                              
}

// Disable the data cache (and invalidate it, required semanitcs)
void CACHE_DisableDCache(void)
{
    asm volatile (                                                      
        "mrc  p15,0,r1,c1,c0,0;"    /* disable cache */                 
        "bic  r1,r1,#4;"                                                
        "mcr  p15,0,r1,c1,c0,0;"                                        
	"mrc  p15,0,r1,c1,c0,1;"    /* disable coalescing */            
	"orr  r1,r1,#1;"                                                
	"mcr  p15,0,r1,c1,c0,1;"                                        
        "mcr    p15,0,r1,c7,c6,0;"  /* invalidate data cache */         
        /* cpuwait */                                                   
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              
        "mov    r1,r1;"                                                 
        "sub    pc,pc,#4;"                                              
        :                                                               
        :                                                               
        : "r1" /* Clobber list */                                       
        );                                                              
}

// Query the state of the data cache
int CACHE_IsDCacheEnabled(void)
{
    register int reg;
    
    asm volatile ("mrc  p15,0,%0,c1,c0,0"                               
                  : "=r"(reg)                                           
                  :                                                     
                /*:*/                                                   
        );                                                              
    return (0 != (4 & reg)); /* Bit 2 is DCache enable */          
}

// Flush the entire dcache (and then both TLBs, just in case)
void CACHE_InvalidateDAll(void)
{
    /* this macro can discard dirty cache lines. */                     
    asm volatile (                                                      
        "mcr    p15,0,r1,c7,c6,0;"  /* invalidate data cache */         
        "mcr    p15,0,r1,c8,c7,0;"  /* flush I+D TLBs */                
        :                                                               
        :                                                               
        : "r1" /* Clobber list */                                       
        );                                                              
}


// Synchronize the contents of the cache with memory.
void CACHE_SyncDCache(void)
{

    /*HAL_DCACHE_EVICT();                                                 */
    asm volatile (                                                      
        "mcr    p15,0,r0,c7,c6,0;"  /* invalidate data cache */        
        /* cpuwait */                                                   
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              
        "mov    r1,r1;"                                                
        "sub    pc,pc,#4;"                                            
        "mcr    p15,0,r0,c7,c10,4;" /* and drain the write buffer */ 
        /* cpuwait */                                               
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */         
        "mov    r1,r1;"                                           
        "sub    pc,pc,#4;"                                       
        "nop"                                                   
        :                                                      
        :                                                     
        : "r0","r1"      /* Clobber list */                  
        );                                                  
}

