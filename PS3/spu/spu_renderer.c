#include <spu_mfcio.h>
#include "spu_render.h"

// Define a tag for DMA transfers. Can be any value from 0-31.
#define MFC_TAG 1

int main(uint64_t arg1_ea_control_block) {
    // The PPU will pass the effective address of the control block in arg1.

    // Allocate space for the control block in the SPU's local store, aligned to a 16-byte boundary.
    SPU_Renderer_Control_Block control_block __attribute__ ((aligned (16)));

    // Use DMA to transfer the control block from main memory into our local store.
    mfc_get(&control_block, arg1_ea_control_block, sizeof(SPU_Renderer_Control_Block), MFC_TAG, 0, 0);

    // Wait for the DMA transfer to complete.
    mfc_write_tag_mask(1 << MFC_TAG);
    mfc_read_tag_status_all();

    // Visual confirmation: Draw a 10x10 red square in the top-left corner of the frame buffer.
    // This demonstrates that the SPU can write to the video memory.
    uint16_t red_pixel = 0xF800; // Red in RGB565 format
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            uint64_t pixel_ea = control_block.frame_buffer_ea + ((y * control_block.width) + x) * sizeof(uint16_t);
            mfc_put(&red_pixel, pixel_ea, sizeof(uint16_t), MFC_TAG, 0, 0);
        }
    }

    // Wait for all DMA transfers to complete.
    mfc_write_tag_mask(1 << MFC_TAG);
    mfc_read_tag_status_all();

    return 0;
}
