#include<chunk.h>

/* bin的数量 */
#define NBINS 128   

/* 62个small bin */
#define NSMALLBINS 62

/* 32位架构8bytes对齐，64位架构16bytes对齐*/
#define SMALLBIN_WIDTH MALLOC_ALIGN

/* 最小large bin字节数 */
#define MIN_LARGE_SIZE (NSMALLBINS * SMALLBIN_WIDTH)

/*
 特别注意：0和1索引暂时保留！
 ----------small bin(64bit machine) ---------------
 start    end    index
 32       47     2
 48       63     3
 ....
 1008     1023   63

 ----------small bin(32bit machine) ---------------
 start    end    index
 16       23     2
 24       31     3
 ....
 504      511    63
 */ 

/* 判断是否是属于small bin */
#define  in_smallbin_range(sz) ((unsigned long)(sz) < (unsigned long)MIN_LARGE_SIZE)
/* 定位small bin的索引 */
#define  smallbin_index(sz) \
(SMALLBIN_WIDTH == 16 ? (((unsigned)(sz)) >> 4) : (((unsigned)(sz)) >> 3))

/* 
 large bin表示的内存大小
Large bins 一共包括 63 个 bin，
每个 bin 中的 chunk 大小不是一个固定公差的等差数列，而是分成 6 组 bin，每组 bin 是一个
固定公差的等差数列，每组的 bin 数量依次为 32、16、8、4、2、1，公差依次为 64B、512B、
4096B、32768B、262144B 等
以 SIZE_SZ 为 4B 的平台为例
第一组 large bin 的起始 chunk 大小为 512B，共 32 个 bin，
公差为 64B，等差数列满足如下关系：（index表示第index个large bin）
Chunk_size=512 + 64 * index
第二组 large bin 的起始 chunk 大小为第一组 bin 的结束 chunk 大小，满足如下关系：
Chunk_size=512 + 64 * 32 + 512 * index
*/

/* 定位large bin的索引 */
/* 32bit machine */
#define  largebin_index_32(sz) \
(((((unsigned long)(sz)) >> 6) <= 39)? 56 + (((unsigned long)(sz)) >> 6): \
((((unsigned long)(sz)) >> 9) <= 20)? 91 + (((unsigned long)(sz)) >> 9): \
((((unsigned long)(sz)) >> 12) <= 10)? 110 + (((unsigned long)(sz)) >> 12): \
((((unsigned long)(sz)) >> 15) <= 4)? 119 + (((unsigned long)(sz)) >> 15): \
((((unsigned long)(sz)) >> 18) <= 2)? 124 + (((unsigned long)(sz)) >> 18): \
126)
/* 64bit machine */
#define  largebin_index_64(sz) \
(((((unsigned long)(sz)) >> 6) <= 47)? 48 + (((unsigned long)(sz)) >> 6): \
((((unsigned long)(sz)) >> 9) <= 20)? 91 + (((unsigned long)(sz)) >> 9): \
((((unsigned long)(sz)) >> 12) <= 10)? 110 + (((unsigned long)(sz)) >> 12): \
((((unsigned long)(sz)) >> 15) <= 4)? 119 + (((unsigned long)(sz)) >> 15): \
((((unsigned long)(sz)) >> 18) <= 2)? 124 + (((unsigned long)(sz)) >> 18): \
126)
/*注意：这样求得的index并不是严格由前面的公式规定，例如：
  32bit系统43520B满足第3个判断条件，但实际上规定在第四组large bin中，
  但保证是单射函数即可，位运算减小计算开销
*/

#define  largebin_index(sz) \
(SIZE_SZ == 8 ?  largebin_index_64 (sz) :  largebin_ind ex_32 (sz))
#define  bin_index(sz) \
(( in_smallbin_range(sz)) ?  smallbin_index(sz) :  largebin_index(sz))

