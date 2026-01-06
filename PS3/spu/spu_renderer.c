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

    // At this point, the SPU has the control block and could begin rendering.
    // The actual rendering logic will be added in a future step.
    // For now, we'll just return 0 to indicate that the data was received successfully.
    return 0;
}
