/**
 * @file wm_hal_rsa.c
 *
 * @brief RSA HAL Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <stdio.h>
#include <string.h>
#include "wm_soc.h"
#include "wm_reg_op.h"
#include "wm_ll_rsa.h"
#include "wm_hal_rsa.h"

#undef LOG_TAG
#define LOG_TAG "hal_rsa"
#include "wm_log.h"

#define WM_HAL_RSA_DEBUG 1

#if WM_HAL_RSA_DEBUG

#define WM_HAL_RSA_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#define WM_HAL_LOG_INFO()       //wm_log_info("%s@%d", __func__, __LINE__)
#define WM_HAL_RSA_LOG_I(...)   wm_log_info(__VA_ARGS__)

#else

#define WM_HAL_RSA_TRACE_FAIL(...)
#define WM_HAL_LOG_INFO(...)
#define WM_HAL_RSA_LOG_I(...)

#endif

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_HAL_RSA_ASSERT_PARAM(p)   \
    if (!(p)) {                      \
        WM_HAL_RSA_TRACE_FAIL();     \
        return WM_ERR_INVALID_PARAM; \
    }

#define ciL (sizeof(mbedtls_mpi_uint)) /* chars in limb  */
#define biL (ciL << 3)                 /* bits  in limb  */
#define biH (ciL << 2)                 /* half limb size */

/**
 * @brief Wait for complete flag to be raised.
 *
 * @param dev pointer to a wm_hal_rsa_dev_t structure.
 *
 */
void wm_hal_rsa_wait_complete(wm_hal_rsa_dev_t *dev)
{
    uint32_t time_out = 0xFFFFFF;
    while (!wm_ll_rsa_get_cmplt_flag(dev->reg_base) && time_out) {
        time_out--;
    }
    if (time_out == 0) {
        WM_HAL_RSA_TRACE_FAIL();
    }
}

int wm_hal_rsa_init(wm_hal_rsa_dev_t *dev)
{
    wm_rsa_reg_t *reg = dev->reg_base;

    /* Clear complete Flag */
    wm_ll_rsa_clear_cmplt_flag(reg);
    /* reset RSA modular */
    wm_ll_rsa_set_soft_reset(reg);

    return WM_ERR_SUCCESS;
}

/* reverse an array, used for radix code */
static void mp_reverse(unsigned char *s, int len)
{
    int ix, iy;
    unsigned char t;

    ix = 0;
    iy = len - 1;
    while (ix < iy) {
        t     = s[ix];
        s[ix] = s[iy];
        s[iy] = t;
        ++ix;
        --iy;
    }
}

static int wm_hal_rsa_mul_mod_read(wm_hal_rsa_dev_t *dev, unsigned char w, mbedtls_mpi *a)
{
    uint32_t out[WM_HAL_RSA_MAX_HARD_EXPTMOD_WORDLEN];
    uint32_t n_value = wm_ll_rsa_get_n_value(dev->reg_base);
    int err          = 0;
    memset(out, 0, WM_HAL_RSA_MAX_HARD_EXPTMOD_WORDLEN * sizeof(uint32_t));
    switch (w) {
        case 'A':
            wm_ll_rsa_monmul_read_a(dev->reg_base, out);
            break;
        case 'B':
            wm_ll_rsa_monmul_read_b(dev->reg_base, out);
            break;
        case 'D':
            wm_ll_rsa_monmul_read_d(dev->reg_base, out);
            break;
    }
    mp_reverse((unsigned char *)out, n_value * sizeof(uint32_t)); //fixme #include "libtommath.h" not found
    if ((err = mbedtls_mpi_read_binary(a, (unsigned char *)out, n_value * sizeof(uint32_t))) != 0) {
        mbedtls_mpi_free(a);
        WM_HAL_RSA_TRACE_FAIL();
        return err;
    }
    return 0;
}

static void wm_hal_rsa_mul_mod_write(wm_hal_rsa_dev_t *dev, unsigned char w, mbedtls_mpi *a)
{
    uint32_t in[WM_HAL_RSA_MAX_HARD_EXPTMOD_WORDLEN];
    memset(in, 0, WM_HAL_RSA_MAX_HARD_EXPTMOD_WORDLEN * sizeof(uint32_t));
    mbedtls_mpi_write_binary_le(a, (unsigned char *)in, a->MBEDTLS_PRIVATE(n) * ciL);
    //printf("rsaMulModWrite %c\n", w);
    //dumpUint32("a", a->p, a->n);
    //dumpUint32("in", in, a->n);
    switch (w) {
        case 'A':
            wm_ll_rsa_monmul_write_a(dev->reg_base, in);
            break;
        case 'B':
            wm_ll_rsa_monmul_write_b(dev->reg_base, in);
            break;
        case 'M':
            wm_ll_rsa_monmul_write_m(dev->reg_base, in);
            break;
    }
}

static void wm_hal_rsa_mon_mul_dd(wm_hal_rsa_dev_t *dev)
{
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
    wm_ll_rsa_set_modmul_mode(dev->reg_base, WM_LL_RSA_MODMUL_MODE_DD);
    wm_ll_rsa_set_modmul_start(dev->reg_base);
    wm_hal_rsa_wait_complete(dev);
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
}

static void wm_hal_rsa_mon_mul_ab(wm_hal_rsa_dev_t *dev)
{
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
    wm_ll_rsa_set_modmul_mode(dev->reg_base, WM_LL_RSA_MODMUL_MODE_AB);
    wm_ll_rsa_set_modmul_start(dev->reg_base);
    wm_hal_rsa_wait_complete(dev);
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
}

static void wm_hal_rsa_mon_mul_bd(wm_hal_rsa_dev_t *dev)
{
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
    wm_ll_rsa_set_modmul_mode(dev->reg_base, WM_LL_RSA_MODMUL_MODE_DB);
    wm_ll_rsa_set_modmul_start(dev->reg_base);
    wm_hal_rsa_wait_complete(dev);
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
}

static void wm_hal_rsa_mon_mul_aa(wm_hal_rsa_dev_t *dev)
{
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
    wm_ll_rsa_set_modmul_mode(dev->reg_base, WM_LL_RSA_MODMUL_MODE_AA);
    wm_ll_rsa_set_modmul_start(dev->reg_base);
    wm_hal_rsa_wait_complete(dev);
    wm_ll_rsa_clear_cmplt_flag(dev->reg_base);
}

/**
 * @brief compute mc, s.t. mc * in = 0xffffffff
 */
static void wm_hal_rsa_calc_mc(uint32_t *mc, const uint32_t in)
{
    uint32_t y     = 1;
    uint32_t i     = 31;
    uint32_t left  = 1;
    uint32_t right = 0;
    for (i = 31; i != 0; i--) {
        left <<= 1;              /* 2^(i-1) */
        right = (in * y) & left; /* (n*y) mod 2^i */
        if (right) {
            y += left;
        }
    }
    *mc = ~y + 1;
}

int wm_hal_rsa_expt_mod(wm_hal_rsa_dev_t *dev, mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N)
{
    int i      = 0;
    uint32_t k = 0, mc = 0, dp0;
    volatile uint8_t mon_mul_Flag = 0;
    mbedtls_mpi R, X1, Y;
    //  mbedtls_mpi T;
    int ret = 0;
    size_t max_len;

    WM_HAL_RSA_ASSERT_PARAM(dev && dev->reg_base && X && A && E && N);

    if (mbedtls_mpi_cmp_int(E, 0) < 0) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }

    wm_hal_rsa_init(dev);
    WM_HAL_LOG_INFO();

    max_len = (mbedtls_mpi_bitlen(N) + biL - 1) / biL;

    mbedtls_mpi_init(&X1);
    mbedtls_mpi_init(&Y);
    mbedtls_mpi_init(&R);
    MBEDTLS_MPI_CHK(mbedtls_mpi_shrink((mbedtls_mpi *)N, max_len));

    WM_HAL_LOG_INFO();

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&R, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_l(&R, N->MBEDTLS_PRIVATE(n) * biL));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&R, &R, N));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shrink(&R, N->MBEDTLS_PRIVATE(n)));
    //dumpUint32("R", R.p, R.n);

    WM_HAL_LOG_INFO();

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&X1, A, &R)); //X = A * R
    //dumpUint32("X = A * R", X1.p, X1.n);
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&X1, &X1, N)); //X = A * R mod N
    MBEDTLS_MPI_CHK(mbedtls_mpi_shrink(&X1, N->MBEDTLS_PRIVATE(n)));
    //dumpUint32("X = A * R mod N", X1.p, X1.n);
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&Y, &R));

    dp0 = (uint32_t)N->MBEDTLS_PRIVATE(p)[0];
    wm_hal_rsa_calc_mc(&mc, dp0);
    WM_HAL_LOG_INFO();
    wm_ll_rsa_set_n_value(dev->reg_base, (const uint32_t)N->MBEDTLS_PRIVATE(n));
    wm_ll_rsa_set_mc_value(dev->reg_base, mc);
    wm_hal_rsa_mul_mod_write(dev, 'M', (mbedtls_mpi *)N);
    wm_hal_rsa_mul_mod_write(dev, 'B', &X1);
    wm_hal_rsa_mul_mod_write(dev, 'A', &Y);

    k = mbedtls_mpi_bitlen(E);
    WM_HAL_LOG_INFO();
    //printf("mbedtls e bit len %d\n", k);
    for (i = k - 1; i >= 0; i--) {
        //montMulMod(&Y, &Y, n, &Y);
        //if(pstm_get_bit(e, i))
        //  montMulMod(&Y, &X, n, &Y);
        if (mon_mul_Flag == 0) {
            wm_hal_rsa_mon_mul_aa(dev);
            mon_mul_Flag = 1;
            //rsaMulModDump('D');
        } else {
            wm_hal_rsa_mon_mul_dd(dev);
            mon_mul_Flag = 0;
            //rsaMulModDump('A');
        }

        if (mbedtls_mpi_get_bit(E, i)) {
            if (mon_mul_Flag == 0) {
                wm_hal_rsa_mon_mul_ab(dev);
                mon_mul_Flag = 1;
                //rsaMulModDump('D');
            } else {
                wm_hal_rsa_mon_mul_bd(dev);
                mon_mul_Flag = 0;
                //rsaMulModDump('A');
            }
        }
    }

    WM_HAL_LOG_INFO();
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&R, 1));
    wm_hal_rsa_mul_mod_write(dev, 'B', &R);
    //montMulMod(&Y, &R, n, res);
    if (mon_mul_Flag == 0) {
        wm_hal_rsa_mon_mul_ab(dev);
        wm_hal_rsa_mul_mod_read(dev, 'D', X);
    } else {
        wm_hal_rsa_mon_mul_bd(dev);
        wm_hal_rsa_mul_mod_read(dev, 'A', X);
    }
    MBEDTLS_MPI_CHK(mbedtls_mpi_shrink(X, N->MBEDTLS_PRIVATE(n)));
    /* reset RSA modular */
    wm_hal_rsa_init(dev);
cleanup:
    mbedtls_mpi_free(&X1);
    mbedtls_mpi_free(&Y);
    mbedtls_mpi_free(&R);
    WM_HAL_LOG_INFO();

    return ret;
}
