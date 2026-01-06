#ifndef __SPU_RENDER_H__
#define __SPU_RENDER_H__

#include <stdint.h>

// This control block will be passed from the PPU to the SPU.
// It contains the information the SPU needs to render.
typedef struct {
    uint64_t frame_buffer_ea; // Effective Address of the frame buffer in main memory
    uint32_t width;
    uint32_t height;
    // More rendering data can be added here later.
} SPU_Renderer_Control_Block;

#ifdef __cplusplus
extern "C" {
#endif

void spu_render(void *frame_buffer, int width, int height);

#ifdef __cplusplus
}
#endif

#endif // __SPU_RENDER_H__
