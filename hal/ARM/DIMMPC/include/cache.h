
#ifndef __CACHE_H__
#define __CACHE_H__



void CACHE_EnableICache    (void);
void CACHE_DisableICache   (void);
int  CACHE_IsICacheEnable  (void);
void CACHE_InvalidateIAll  (void);
void CACHE_SyncICache      (void);
void CACHE_EnableDCache    (void);
void CACHE_DisableDCache   (void);
int  CACHE_IsDCacheEnabled (void);
void CACHE_InvalidateDAll  (void);
void CACHE_SyncDCache      (void);
void CACHE_FlushDCache     (int _base_, int _size_ );
void CACHE_InvalidateDCache(int _base_ , int _size_ );
void CACHE_StoreDCache     (int _base_, int _size_);

#endif
