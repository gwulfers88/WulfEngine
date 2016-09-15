#ifndef PLATFORM_H
#define PLATFORM_H

typedef char		i8;
typedef short		i16;
typedef int			i32;
typedef long long	i64;

typedef i32			b32;
typedef float		f32;
typedef double		f64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef size_t mem_size;

#define ArrayCount(a) (sizeof(a) / sizeof(a[0]))
#define Assert(expression) if(!expression){ *(int*)0 = 0; }

#define Kilobytes(size) (size * 1024LL)
#define Megabytes(size) (Kilobytes(size) * 1024LL)
#define Gigabytes(size) (Megabytes(size) * 1024LL)
#define Terabytes(size) (Gigabytes(size) * 1024LL)

struct FileResult
{
	void* data;
	i64 fileSize;
};

#define PLATFORM_READ_ENTIRE_FILE(name) FileResult name(i8* filename)
#define PLATFORM_WRITE_ENTIRE_FILE(name) void name(i8* filename, FileResult fileData)

#endif
