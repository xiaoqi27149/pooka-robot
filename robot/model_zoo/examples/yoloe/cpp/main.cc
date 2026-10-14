// Copyright (c) 2024 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clip_text.h"
#include "yoloe.h"
#include "file_utils.h"
#include "image_utils.h"
#include "image_drawing.h"


/*-------------------------------------------
                  Main Function
-------------------------------------------*/
int main(int argc, char **argv)
{
    if (argc != 5)
    {
        printf("%s <text_model_path> <text_path> <yoloe_model_path> <image_path>\n", argv[0]);
        return -1;
    }

    const char *text_model_path = argv[1];
    const char *text_path = argv[2];
    const char *yoloe_path = argv[3];
    const char *img_path = argv[4];

    unsigned char class_colors[][3] = {
        {255, 56, 56},   // 'FF3838'
        {255, 157, 151}, // 'FF9D97'
        {255, 112, 31},  // 'FF701F'
        {255, 178, 29},  // 'FFB21D'
        {207, 210, 49},  // 'CFD231'
        {72, 249, 10},   // '48F90A'
        {146, 204, 23},  // '92CC17'
        {61, 219, 134},  // '3DDB86'
        {26, 147, 52},   // '1A9334'
        {0, 212, 187},   // '00D4BB'
        {44, 153, 168},  // '2C99A8'
        {0, 194, 255},   // '00C2FF'
        {52, 69, 147},   // '344593'
        {100, 115, 255}, // '6473FF'
        {0, 24, 236},    // '0018EC'
        {132, 56, 255},  // '8438FF'
        {82, 0, 133},    // '520085'
        {203, 56, 255},  // 'CB38FF'
        {255, 149, 200}, // 'FF95C8'
        {255, 55, 199}   // 'FF37C7'
    };

    int ret;
    rknn_clip_context rknn_clip_ctx;
    rknn_app_context_t rknn_yoloe_ctx;
    memset(&rknn_clip_ctx, 0, sizeof(rknn_clip_context));
    memset(&rknn_yoloe_ctx, 0, sizeof(rknn_app_context_t));

    printf("--> init clip text model\n");
    ret = init_clip_text_model(&rknn_clip_ctx, text_model_path);
    if (ret != 0)
    {
        printf("init clip text model fail! ret=%d\n", ret);
        return -1;
    }

    init_post_process();

    printf("--> init yoloe model\n");
    ret = init_yoloe_model(&rknn_yoloe_ctx, yoloe_path);
    if (ret != 0)
    {
        printf("init yoloe model fail! ret=%d\n", ret);
        return -1;
    }

    int text_lines;
    char** input_texts = read_lines_from_file(text_path, &text_lines);
    if (input_texts == NULL)
    {
        printf("read input texts fail! ret=%d text_path=%s\n", ret, text_path);
        return -1;
    }

    image_buffer_t src_image;
    memset(&src_image, 0, sizeof(image_buffer_t));
    ret = read_image(img_path, &src_image);
    if (ret != 0)
    {
        printf("read image fail! ret=%d image_path=%s\n", ret, img_path);
        return -1;
    }

    int text_size = text_lines * rknn_clip_ctx.output_attrs[0].dims[2];
    printf("----->text_output text_size %d, text_lines: %d, dim: %d \n", text_size, text_lines, rknn_clip_ctx.output_attrs[0].dims[2]);
    float text_output[text_size];

    printf("--> inference clip text model\n");
    ret = inference_clip_text_model(&rknn_clip_ctx, input_texts, text_lines, text_output);
    if (ret != 0)
    {
        printf("inference_clip_model fail! ret=%d\n", ret);
        goto out;
    }

    object_detect_result_list od_results;

    printf("--> inference yoloe model\n");
    ret = inference_yoloe_model(&rknn_yoloe_ctx, &src_image, text_output, text_size, &od_results);
    if (ret != 0)
    {
        printf("init_yoloe_model fail! ret=%d\n", ret);
        goto out;
    }

    // 画框和概率
    char text[256];
    for (int i = 0; i < od_results.count; i++)
    {
        object_detect_result *det_result = &(od_results.results[i]);
        printf("%s @ (%d %d %d %d) %.3f\n", coco_cls_to_name(det_result->cls_id),
               det_result->box.left, det_result->box.top,
               det_result->box.right, det_result->box.bottom,
               det_result->prop);
        int x1 = det_result->box.left;
        int y1 = det_result->box.top;
        int x2 = det_result->box.right;
        int y2 = det_result->box.bottom;

        draw_rectangle(&src_image, x1, y1, x2 - x1, y2 - y1, COLOR_BLUE, 3);

        sprintf(text, "%s %.1f%%", coco_cls_to_name(det_result->cls_id), det_result->prop * 100);
        draw_text(&src_image, text, x1, y1 - 20, COLOR_RED, 10);
    }
    // draw mask
    if (od_results.count >= 1)
    {
        int width = src_image.width;
        int height = src_image.height;
        char *ori_img = (char *)src_image.virt_addr;
        int cls_id = od_results.results[0].cls_id;
        uint8_t *seg_mask = od_results.results_seg[0].seg_mask;
        float alpha = 0.5f; // opacity
        for (int j = 0; j < height; j++)
        {
            for (int k = 0; k < width; k++)
            {
                int pixel_offset = 3 * (j * width + k);
                if (seg_mask[j * width + k] != 0)
                {
                    ori_img[pixel_offset + 0] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][0] * (1 - alpha) + ori_img[pixel_offset + 0] * alpha, 0, 255); // r
                    ori_img[pixel_offset + 1] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][1] * (1 - alpha) + ori_img[pixel_offset + 1] * alpha, 0, 255); // g
                    ori_img[pixel_offset + 2] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][2] * (1 - alpha) + ori_img[pixel_offset + 2] * alpha, 0, 255); // b
                }
            }
        }
        free(seg_mask);
    }
    
    write_image("out.png", &src_image);


out:
    ret = release_clip_text_model(&rknn_clip_ctx);
    if (ret != 0)
    {
        printf("release_clip_model fail! ret=%d\n", ret);
    }

    deinit_post_process();

    ret = release_yoloe_model(&rknn_yoloe_ctx);
    if (ret != 0)
    {
        printf("release_yoloe_model fail! ret=%d\n", ret);
    }

    if (src_image.virt_addr != NULL)
    {

        free(src_image.virt_addr);
    }

    if (input_texts != NULL)
    {
        free_lines(input_texts, text_lines);
    }


    return 0;
}
