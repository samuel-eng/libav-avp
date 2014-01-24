/*
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

#ifndef AVCODEC_IDCTDSP_H
#define AVCODEC_IDCTDSP_H

#include <stdint.h>

#include "avcodec.h"

/**
 * Scantable.
 */
typedef struct ScanTable {
    const uint8_t *scantable;
    uint8_t permutated[64];
    uint8_t raster_end[64];
} ScanTable;

void ff_init_scantable(uint8_t *permutation, ScanTable *st,
                       const uint8_t *src_scantable);
void ff_init_scantable_permutation(uint8_t *idct_permutation,
                                   int idct_permutation_type);
int ff_init_scantable_permutation_x86(uint8_t *idct_permutation,
                                      int idct_permutation_type);

typedef struct IDCTDSPContext {
    /* pixel ops : interface with DCT */
    void (*put_pixels_clamped)(const int16_t *block /* align 16 */,
                               uint8_t *pixels /* align 8 */,
                               int line_size);
    void (*put_signed_pixels_clamped)(const int16_t *block /* align 16 */,
                                      uint8_t *pixels /* align 8 */,
                                      int line_size);
    void (*add_pixels_clamped)(const int16_t *block /* align 16 */,
                               uint8_t *pixels /* align 8 */,
                               int line_size);

    void (*idct)(int16_t *block /* align 16 */);

    /**
     * block -> idct -> clip to unsigned 8 bit -> dest.
     * (-1392, 0, 0, ...) -> idct -> (-174, -174, ...) -> put -> (0, 0, ...)
     * @param line_size size in bytes of a horizontal line of dest
     */
    void (*idct_put)(uint8_t *dest /* align 8 */,
                     int line_size, int16_t *block /* align 16 */);

    /**
     * block -> idct -> add dest -> clip to unsigned 8 bit -> dest.
     * @param line_size size in bytes of a horizontal line of dest
     */
    void (*idct_add)(uint8_t *dest /* align 8 */,
                     int line_size, int16_t *block /* align 16 */);

    /**
     * IDCT input permutation.
     * Several optimized IDCTs need a permutated input (relative to the
     * normal order of the reference IDCT).
     * This permutation must be performed before the idct_put/add.
     * Note, normally this can be merged with the zigzag/alternate scan<br>
     * An example to avoid confusion:
     * - (->decode coeffs -> zigzag reorder -> dequant -> reference IDCT -> ...)
     * - (x -> reference DCT -> reference IDCT -> x)
     * - (x -> reference DCT -> simple_mmx_perm = idct_permutation
     *    -> simple_idct_mmx -> x)
     * - (-> decode coeffs -> zigzag reorder -> simple_mmx_perm -> dequant
     *    -> simple_idct_mmx -> ...)
     */
    uint8_t idct_permutation[64];
    int idct_permutation_type;
#define FF_NO_IDCT_PERM 1
#define FF_LIBMPEG2_IDCT_PERM 2
#define FF_SIMPLE_IDCT_PERM 3
#define FF_TRANSPOSE_IDCT_PERM 4
#define FF_PARTTRANS_IDCT_PERM 5
#define FF_SSE2_IDCT_PERM 6
} IDCTDSPContext;

void ff_idctdsp_init(IDCTDSPContext *c, AVCodecContext *avctx);

void ff_idctdsp_init_arm(IDCTDSPContext *c, AVCodecContext *avctx,
                         unsigned high_bit_depth);
void ff_idctdsp_init_ppc(IDCTDSPContext *c, AVCodecContext *avctx,
                         unsigned high_bit_depth);
void ff_idctdsp_init_x86(IDCTDSPContext *c, AVCodecContext *avctx,
                         unsigned high_bit_depth);

#endif /* AVCODEC_IDCTDSP_H */
