/*
 * Copyright (c) 2006 Luca Barbato <lu_zero@gentoo.org>
 *
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/ppc/util_altivec.h"
#include "libavcodec/dsputil.h"

#include "dsputil_altivec.h"

static void vector_fmul_reverse_altivec(float *dst, const float *src0,
                                        const float *src1, int len)
{
    int i;
    vector float d, s0, s1, h0, l0,
                 s2, s3, zero = (vector float)vec_splat_u32(0);
    src1 += len-4;
    for(i=0; i<len-7; i+=8) {
        s1 = vec_ld(0, src1-i);              // [a,b,c,d]
        s0 = vec_ld(0, src0+i);
        l0 = vec_mergel(s1, s1);             // [c,c,d,d]
        s3 = vec_ld(-16, src1-i);
        h0 = vec_mergeh(s1, s1);             // [a,a,b,b]
        s2 = vec_ld(16, src0+i);
        s1 = vec_mergeh(vec_mergel(l0,h0),   // [d,b,d,b]
                        vec_mergeh(l0,h0));  // [c,a,c,a]
                                             // [d,c,b,a]
        l0 = vec_mergel(s3, s3);
        d = vec_madd(s0, s1, zero);
        h0 = vec_mergeh(s3, s3);
        vec_st(d, 0, dst+i);
        s3 = vec_mergeh(vec_mergel(l0,h0),
                        vec_mergeh(l0,h0));
        d = vec_madd(s2, s3, zero);
        vec_st(d, 16, dst+i);
    }
}

void ff_float_init_altivec(DSPContext* c, AVCodecContext *avctx)
{
    c->vector_fmul_reverse = vector_fmul_reverse_altivec;
}
