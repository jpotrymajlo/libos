#include <libos.h>
#include <video/video.h>


UINT32 std_write(UINT8* buff, INT32 size)
{
  return write_video(buff, size);
}