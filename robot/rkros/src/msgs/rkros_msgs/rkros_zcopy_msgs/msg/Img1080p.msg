# img1080p

uint64 frame_no         # Frame Number

builtin_interfaces/Time timestamp   # timestamp should be acquisition time of image

int32 width             # image width, that is, number of columns
int32 height            # image height, that is, number of rows
uint32 stride           # Full row length in bytes
uint32 format           # Format of pixels -- channel meaning, ordering, size
uint32 data_size        # the actual number of bytes occupied by the image data

# Maximum allowed data size: 1920 x 1088 x 3 = 6266880 bytes
uint32 MAX_SIZE=6266880

uint8[6266880] data