#define LOG_TAG "test"
#include "wm_log.h"

#include "csky_math.h"
#include "csky_dsp2_nnfunctions.h"

#include "examples_cifar10_parameter.h"
#include "examples_cifar10_weights.h"
#include "examples_cifar10_inputs.h"

// include the input and weights

static q7_t conv1_wt[CONV1_IM_CH * CONV1_KER_DIM * CONV1_KER_DIM * CONV1_OUT_CH] = CONV1_WT;
static q7_t conv1_bias[CONV1_OUT_CH]                                             = CONV1_BIAS;

static q7_t conv2_wt[CONV2_IM_CH * CONV2_KER_DIM * CONV2_KER_DIM * CONV2_OUT_CH] = CONV2_WT;
static q7_t conv2_bias[CONV2_OUT_CH]                                             = CONV2_BIAS;

static q7_t conv3_wt[CONV3_IM_CH * CONV3_KER_DIM * CONV3_KER_DIM * CONV3_OUT_CH] = CONV3_WT;
static q7_t conv3_bias[CONV3_OUT_CH]                                             = CONV3_BIAS;

static q7_t ip1_wt[IP1_DIM * IP1_OUT] = IP1_WT;
static q7_t ip1_bias[IP1_OUT]         = IP1_BIAS;

q7_t input_data[CONV1_IM_CH * CONV1_IM_DIM * CONV1_IM_DIM] = IMG_DATA;
q7_t output_data[IP1_OUT];

//vector buffer: max(im2col buffer,average pool buffer, fully connected buffer)
q7_t col_buffer[2 * 5 * 5 * 32 * 2];

q7_t scratch_buffer[32 * 32 * 10 * 4];

int main(void)
{
    q7_t *img_buffer1 = scratch_buffer;
    q7_t *img_buffer2 = img_buffer1 + 32 * 32 * 32;

    // conv1 input_data -> img_buffer1
    csky_dsp2_convolve_HWC_q7_RGB(input_data, CONV1_IM_DIM, conv1_wt, CONV1_OUT_CH, CONV1_KER_DIM, CONV1_PADDING, CONV1_STRIDE,
                                  conv1_bias, CONV1_BIAS_LSHIFT, CONV1_OUT_RSHIFT, img_buffer1, CONV1_OUT_DIM,
                                  (q15_t *)col_buffer);

    csky_dsp2_relu_q7(img_buffer1, CONV1_OUT_DIM * CONV1_OUT_DIM * CONV1_OUT_CH);

    // pool1 img_buffer1 -> img_buffer2
    csky_dsp2_maxpool_q7_HWC(img_buffer1, CONV1_OUT_DIM, CONV1_OUT_CH, POOL1_KER_DIM, POOL1_PADDING, POOL1_STRIDE,
                             POOL1_OUT_DIM, NULL, img_buffer2);

    // conv2 img_buffer2 -> img_buffer1
    csky_dsp2_convolve_HWC_q7_basic(img_buffer2, CONV2_IM_DIM, CONV2_IM_CH, conv2_wt, CONV2_OUT_CH, CONV2_KER_DIM,
                                    CONV2_PADDING, CONV2_STRIDE, conv2_bias, CONV2_BIAS_LSHIFT, CONV2_OUT_RSHIFT, img_buffer1,
                                    CONV2_OUT_DIM, (q15_t *)col_buffer);

    csky_dsp2_relu_q7(img_buffer1, CONV2_OUT_DIM * CONV2_OUT_DIM * CONV2_OUT_CH);

    // pool2 img_buffer1 -> img_buffer2
    csky_dsp2_avepool_q7_HWC(img_buffer1, CONV2_OUT_DIM, CONV2_OUT_CH, POOL2_KER_DIM, POOL2_PADDING, POOL2_STRIDE,
                             POOL2_OUT_DIM, col_buffer, img_buffer2);

    // conv3 img_buffer2 -> img_buffer1
    csky_dsp2_convolve_HWC_q7_basic(img_buffer2, CONV3_IM_DIM, CONV3_IM_CH, conv3_wt, CONV3_OUT_CH, CONV3_KER_DIM,
                                    CONV3_PADDING, CONV3_STRIDE, conv3_bias, CONV3_BIAS_LSHIFT, CONV3_OUT_RSHIFT, img_buffer1,
                                    CONV3_OUT_DIM, (q15_t *)col_buffer);

    csky_dsp2_relu_q7(img_buffer1, CONV3_OUT_DIM * CONV3_OUT_DIM * CONV3_OUT_CH);

    // pool3 img_buffer-> img_buffer2
    csky_dsp2_avepool_q7_HWC(img_buffer1, CONV3_OUT_DIM, CONV3_OUT_CH, POOL3_KER_DIM, POOL3_PADDING, POOL3_STRIDE,
                             POOL3_OUT_DIM, col_buffer, img_buffer2);

    csky_dsp2_fully_connected_q7(img_buffer2, ip1_wt, IP1_DIM, IP1_OUT, IP1_BIAS_LSHIFT, IP1_OUT_RSHIFT, ip1_bias, output_data);

    csky_dsp2_softmax_q7(output_data, 10, output_data);

    for (int i = 0; i < 10; i++) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "%d: %d\n", i, output_data[i]);
    }

    wm_log_info("Example run successfully!");

    return 0;
}
