
#include <assert.h>
#include "matmath.hpp"


const mat4f mat4f::unit(vec4f(1,0,0,0),vec4f(0,1,0,0),vec4f(0,0,1,0),vec4f(0,0,0,1));



// 4x4 matrix inversion implementation below

// CONVERTED from niswegmann/small-matrix-inverse/invert4x4_c.h UNLICENSED

namespace
{
static inline void invert4x4(const data_t * src, data_t * dst)
{
    data_t det;

    /* Compute adjoint: */

    dst[0] =
        + src[ 5] * src[10] * src[15]
        - src[ 5] * src[11] * src[14]
        - src[ 9] * src[ 6] * src[15]
        + src[ 9] * src[ 7] * src[14]
        + src[13] * src[ 6] * src[11]
        - src[13] * src[ 7] * src[10];

    dst[1] =
        - src[ 1] * src[10] * src[15]
        + src[ 1] * src[11] * src[14]
        + src[ 9] * src[ 2] * src[15]
        - src[ 9] * src[ 3] * src[14]
        - src[13] * src[ 2] * src[11]
        + src[13] * src[ 3] * src[10];

    dst[2] =
        + src[ 1] * src[ 6] * src[15]
        - src[ 1] * src[ 7] * src[14]
        - src[ 5] * src[ 2] * src[15]
        + src[ 5] * src[ 3] * src[14]
        + src[13] * src[ 2] * src[ 7]
        - src[13] * src[ 3] * src[ 6];

    dst[3] =
        - src[ 1] * src[ 6] * src[11]
        + src[ 1] * src[ 7] * src[10]
        + src[ 5] * src[ 2] * src[11]
        - src[ 5] * src[ 3] * src[10]
        - src[ 9] * src[ 2] * src[ 7]
        + src[ 9] * src[ 3] * src[ 6];

    dst[4] =
        - src[ 4] * src[10] * src[15]
        + src[ 4] * src[11] * src[14]
        + src[ 8] * src[ 6] * src[15]
        - src[ 8] * src[ 7] * src[14]
        - src[12] * src[ 6] * src[11]
        + src[12] * src[ 7] * src[10];

    dst[5] =
        + src[ 0] * src[10] * src[15]
        - src[ 0] * src[11] * src[14]
        - src[ 8] * src[ 2] * src[15]
        + src[ 8] * src[ 3] * src[14]
        + src[12] * src[ 2] * src[11]
        - src[12] * src[ 3] * src[10];

    dst[6] =
        - src[ 0] * src[ 6] * src[15]
        + src[ 0] * src[ 7] * src[14]
        + src[ 4] * src[ 2] * src[15]
        - src[ 4] * src[ 3] * src[14]
        - src[12] * src[ 2] * src[ 7]
        + src[12] * src[ 3] * src[ 6];

    dst[7] =
        + src[ 0] * src[ 6] * src[11]
        - src[ 0] * src[ 7] * src[10]
        - src[ 4] * src[ 2] * src[11]
        + src[ 4] * src[ 3] * src[10]
        + src[ 8] * src[ 2] * src[ 7]
        - src[ 8] * src[ 3] * src[ 6];

    dst[8] =
        + src[ 4] * src[ 9] * src[15]
        - src[ 4] * src[11] * src[13]
        - src[ 8] * src[ 5] * src[15]
        + src[ 8] * src[ 7] * src[13]
        + src[12] * src[ 5] * src[11]
        - src[12] * src[ 7] * src[ 9];

    dst[9] =
        - src[ 0] * src[ 9] * src[15]
        + src[ 0] * src[11] * src[13]
        + src[ 8] * src[ 1] * src[15]
        - src[ 8] * src[ 3] * src[13]
        - src[12] * src[ 1] * src[11]
        + src[12] * src[ 3] * src[ 9];

    dst[10] =
        + src[ 0] * src[ 5] * src[15]
        - src[ 0] * src[ 7] * src[13]
        - src[ 4] * src[ 1] * src[15]
        + src[ 4] * src[ 3] * src[13]
        + src[12] * src[ 1] * src[ 7]
        - src[12] * src[ 3] * src[ 5];

    dst[11] =
        - src[ 0] * src[ 5] * src[11]
        + src[ 0] * src[ 7] * src[ 9]
        + src[ 4] * src[ 1] * src[11]
        - src[ 4] * src[ 3] * src[ 9]
        - src[ 8] * src[ 1] * src[ 7]
        + src[ 8] * src[ 3] * src[ 5];

    dst[12] =
        - src[ 4] * src[ 9] * src[14]
        + src[ 4] * src[10] * src[13]
        + src[ 8] * src[ 5] * src[14]
        - src[ 8] * src[ 6] * src[13]
        - src[12] * src[ 5] * src[10]
        + src[12] * src[ 6] * src[ 9];

    dst[13] =
        + src[ 0] * src[ 9] * src[14]
        - src[ 0] * src[10] * src[13]
        - src[ 8] * src[ 1] * src[14]
        + src[ 8] * src[ 2] * src[13]
        + src[12] * src[ 1] * src[10]
        - src[12] * src[ 2] * src[ 9];

    dst[14] =
        - src[ 0] * src[ 5] * src[14]
        + src[ 0] * src[ 6] * src[13]
        + src[ 4] * src[ 1] * src[14]
        - src[ 4] * src[ 2] * src[13]
        - src[12] * src[ 1] * src[ 6]
        + src[12] * src[ 2] * src[ 5];

    dst[15] =
        + src[ 0] * src[ 5] * src[10]
        - src[ 0] * src[ 6] * src[ 9]
        - src[ 4] * src[ 1] * src[10]
        + src[ 4] * src[ 2] * src[ 9]
        + src[ 8] * src[ 1] * src[ 6]
        - src[ 8] * src[ 2] * src[ 5];

    /* Compute determinant: */

    det = + src[0] * dst[0] + src[1] * dst[4] + src[2] * dst[8] + src[3] * dst[12];

    /* Multiply adjoint with reciprocal of determinant: */

    det = 1.0f / det;

    dst[ 0] *= det;
    dst[ 1] *= det;
    dst[ 2] *= det;
    dst[ 3] *= det;
    dst[ 4] *= det;
    dst[ 5] *= det;
    dst[ 6] *= det;
    dst[ 7] *= det;
    dst[ 8] *= det;
    dst[ 9] *= det;
    dst[10] *= det;
    dst[11] *= det;
    dst[12] *= det;
    dst[13] *= det;
    dst[14] *= det;
    dst[15] *= det;
}
} // end anonymous namespace

mat4f inverse(mat4f a)
{
    // a,b should be either row-major or column-major
    assert(sizeof(a) == 16*sizeof(data_t));
    mat4f b;
    invert4x4(a[0], b[0]);
    return b;
}


