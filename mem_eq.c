#include <stdio.h>
#include <unistd.h>

int mem_cq(const void *m1, const void *m2, size_t n) {
    const unsigned char *us1 = m1;
    const unsigned char *us2 = m2;
    unsigned int result = 0;

    for (int i = 0; i < n; i++)
         result += us1[i] ^ us2[i];

    return result;
}
