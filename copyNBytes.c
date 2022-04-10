#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

// error code
#define ERR_NONE 0
#define ERR_DST_NULL -1
#define ERR_SRC_NULL -2
#define ERR_LOW_DST_MEM -3
#define ERR_ZEROBYTES_CP -4
#define ERR_SRC_DST_OVERLAP -5

/* Function:  copyNBytes
 * ------------------------------
 * copies n bytes from source to destination
 *
 * dest: pointer for destination buffer/memory to write to
 * src: pointer for source buffer/memory to read from
 * n_bytes: number of bytes to copy from src to dest
 *
 * returns:
 *   ERR_NONE if copy operation completed successfully
 *   ERR_DST_NULL if dest is not valid i.e NULL pointer
 *   ERR_SRC_NULL if src is not valid i.e NULL pointer
 *   ERR_LOW_DST_MEM is destination size is not enough to hold/write n_bytes
 *   ERR_ZEROBYTES_CP nothing/zero bytes to copy
 *   ERR_SRC_DST_OVERLAP src and dest chunks overlap each other
 */
int copyNBytes(void *dest, size_t  dsize, const void *src, size_t n_bytes) {
    if (dest == NULL)
        return ERR_DST_NULL;

    if(src == NULL)
        return ERR_SRC_NULL;

    if (dsize < n_bytes) // covers dsize == 0 case too
        return ERR_LOW_DST_MEM;

    if (n_bytes == 0)
        return ERR_ZEROBYTES_CP;

    // check for overlapping
    // src lies before dest and after n_bytes src reaches in dest chunk ||
    // dest lies befor src and after dsize, dest reaches in src chunk
    if( ((dest > src) && (dest < (src+n_bytes))) || ((src > dest) && (src < (dest+dsize))) )
        return ERR_SRC_DST_OVERLAP;

    while(n_bytes) {
        // if ptrs and size is aligned w.r.t long, copy chunks of long
        if ((uintptr_t)dest % sizeof(long) == 0 && (uintptr_t)src % sizeof(long) == 0 && n_bytes % sizeof(long) == 0) {
            *((long *)dest++) = *((const long *)src++);
            n_bytes -= sizeof(long);
        }
        else { // else copy byte by byte
            *((char *)dest++) = *((const char *)src++);
            --n_bytes;
        }
    };

    return ERR_NONE;
}

/* Function: testSrcNull
 *  test copyNBytes api with an invalid src pointer
 */
void testSrcNull() {
    char * src = NULL;
    char dest[10];
    if(copyNBytes(dest,10, src, 10) != ERR_SRC_NULL) {
        printf("testSrcNull failed.\n");
    } else {
        printf("testSrcNull passed.\n");
    }
}

/* Function: testDestNull
 *  test copyNBytes api with an invalid dest pointer
 */
void testDestNull() {
    char * src = "hello string to be copied 12 3 4 5 3 3 23 2 3 3 4 4 5 65 6 5 5 4 4 4 34 34 4 5 6 6 5 5 4 4 5 5 56 6 ";
    char dest = NULL;
    if(copyNBytes(dest, 0, src, 10) != ERR_DST_NULL) {
        printf("testDestNull failed.\n");
    } else {
        printf("testDestNull passed.\n");
    }
}

/* Function: testNBytesCopy1
 *  test copyNBytes api with full array data copy
 */
void testNBytesCopy1() {
    char *src = (char*)malloc(128);
    strcpy(src, "hello string to be copied 12 3 4 5 3 3 23 2 3 3 4 4 5 65 6 5 5 4 4 4 34 34 4 5 6 6 5 5 4 4 5 5 56 6");
    char *dest = (char*)malloc(128);

    int ret = copyNBytes(dest, 128, src, strlen(src) + 1);

    if( ret != ERR_NONE || strlen(src) != strlen(dest) || strncmp(src, dest, strlen(src)) != 0) {
        printf("testNBytesCopy1 failed.\n");
    } else {
        printf("testNBytesCopy1 passed.\n");
    }
    free(dest);
}

/* Function: testNBytesCopy2
 *  test copyNBytes api with partial array data copy
 */
void testNBytesCopy2() {
    char * src = "hello string to be copied";
    char dest[100] = {0};

    int ret = copyNBytes(dest, 100, src, 10);

    if( ret != ERR_NONE || strncmp(src, dest, 10) != 0) {
        printf("testNBytesCopy2 failed.\n");
    } else {
        printf("testNBytesCopy2 passed.\n");
    }
}

/* Function: testOverlapData
 *  test copyNBytes api with src overlapping over dest
 */
void testOverlapData() {
    char *src = (char *)malloc(128);
    char *dest = src + 20;
    strcpy(src, "Hello this is string for overlapped src and dest");
    int ret = copyNBytes(dest, 100, src, 32);
    if( ret != ERR_SRC_DST_OVERLAP ) {
        printf("testOverlapData failed.\n");
    } else {
        printf("testOverlapData passed.\n");
    }
}

/* Function: testZerosBytesCopy
 *  test copyNBytes api with with 0 bytes
 */
void testZerosBytesCopy() {
    char * src = "hello string to be copied";
    char dest[100] = {0};

    int ret = copyNBytes(dest, 100, src, 0);

    if( ret != ERR_ZEROBYTES_CP) {
        printf("testZerosBytesCopy failed.\n");
    } else {
        printf("testZerosBytesCopy passed.\n");
    }
}

/* Function: testSmallerDestSize
 *  test copyNBytes api with not enough dest to cpy n_bytes
 */
void testSmallerDestSize() {
    char * src = "hello string to be copied";
    char dest[10] = {0};

    int ret = copyNBytes(dest, 10, src, 20);

    if( ret != ERR_LOW_DST_MEM) {
        printf("testSmallerDestSize failed.\n");
    } else {
        printf("testSmallerDestSize passed.\n");
    }
}

int main() {
    printf("Running test cases ...\n");
    testSrcNull();
    testDestNull();
    testNBytesCopy1();
    testNBytesCopy2();
    testOverlapData();
    testZerosBytesCopy();
    testSmallerDestSize();
    return 0;
}
