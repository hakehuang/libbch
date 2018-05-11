#ifndef _LINUX_COMPAT_H_
#define _LINUX_COMPAT_H_

#define KERN_ERR

#define GFP_KERNEL             0
#define printk                 printf
#define kmalloc(size, flag)    malloc((size))
#define kzalloc(size, flag)    calloc(1, (size))
#define kfree(ptr)             free((ptr))
#define DIV_ROUND_UP(n, d)     (((n) + (d) - 1) / (d))
#define ARRAY_SIZE(arr)        (sizeof(arr) / sizeof((arr)[0]))

#define cpu_to_be32(x)         __swab32((x))
#define __swab32(x) (uint32_t)__builtin_bswap32((uint32_t)(x))

static inline int fls(int x)
{
	return x ? sizeof(x) * 8 - __builtin_clz(x) : 0;
}

#endif /* _LINUX_COMPAT_H_ */
