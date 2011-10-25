/**
 * \defgroup sterwniki Sterowniki
 * @{
 *  Modu³ sterowników definiuje funkcje s³u¿¹ce operacjom na urz¹dzeniach
 *  w celu ujednoliecenia interfejsu 
 *
 */

#include <arch.h>
/*#include <drv.h>*/
/** 
 * @brief 
 * 
 * @param path 
 * @param flags 
 * 
 * @return 
 */

int open(const char *path, int flags)
{
  return 0;
}
/** 
 * @brief 
 * 
 * @param fd 
 * @param buff 
 * @param count 
 * 
 * @return 
 */
ssize_t write(int fd, const char* buff, size_t count)
{
  return 0;
}
/** 
 * @brief 
 * 
 * @param fd 
 * @param buff 
 * @param count 
 * 
 * @return 
 */

ssize_t read(int fd, void* buff, size_t count)
{
  return 0;
}

/** 
 * @brief 
 * 
 * @param fd 
 * @param offset 
 * @param whence 
 * 
 * @return 
 */
off_t lseek(int fd, off_t offset, int whence)
{
  return 0;
}

int ioctl(int fd, int command, ...)
{
  return 0;
}
