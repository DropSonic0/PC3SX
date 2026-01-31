
# CELL_GPU_TYPE (currently RSX is only one option)  
CELL_GPU_TYPE = RSX    
#CELL_PSGL_VERSION is debug, dpm or opt  
CELL_PSGL_VERSION = opt  

CELL_SDK ?= /usr/local/cell
CELL_MK_DIR ?= $(CELL_SDK)/samples/mk
include $(CELL_MK_DIR)/sdk.makedef.mk

CONTENT_ID = IV0002-PCSX00001_00-SAMPLE0000000001
MKFSELF_NPDRM = $(CELL_SDK)/host-win32/bin/make_fself_npdrm
MKPKG_NPDRM = $(CELL_SDK)/host-win32/bin/make_package_npdrm
MAKE_EBOOT = scetool --sce-type=SELF --compress-data=TRUE --skip-sections=FALSE --key-revision=04 --self-ctrl-flags=4000000000000000000000000000000000000000000000000000000000000002 --self-auth-id=1010000001000003 --self-app-version=0001000000000000 --self-add-shdrs=TRUE --self-vendor-id=01000002 --self-type=NPDRM --self-fw-version=0003004000000000 --np-license-type=FREE --np-content-id=$(CONTENT_ID) --np-app-type=EXEC --np-real-fname=EBOOT.BIN --encrypt 

PPU_OPTIMIZE_LV = -O2

PPU_SRCS		:= Ps3sxMain.cpp ./PS3/ps3audio.cpp ./PS3/ps3video.cpp ./PS3/rom_list.cpp \
	./PS3/input/cellInput.cpp ./PS3/graphics/PSGLGraphics.cpp ./PS3/fileio/FileBrowser.cpp\
	./PS3/buffer.c ./PS3/resampler.c ./PS3/Ini/ini.c ./PS3/Ini/IniFile.cpp \
	./pcsxcore/PsxBios.c	\
	./pcsxcore/psxcommon.c	\
	./pcsxcore/cheat.c	\
	./pcsxcore/ppf.c	\
	./pcsxcore/psxgpu.c	\
	./pcsxcore/CdRom.c	\
	./pcsxcore/PsxCounters.c	\
	./pcsxcore/PsxDma.c	\
	./pcsxcore/DisR3000A.c	\
	./pcsxcore/sio.c	\
	./pcsxcore/PsxHw.c	\
	./pcsxcore/Mdec.c	\
	./pcsxcore/PsxMem.c	\
	./pcsxcore/misc.c	\
	./pcsxcore/GPU_SOFT/cfg.c \
	./pcsxcore/GPU_SOFT/Gudraw.c \
	./pcsxcore/GPU_SOFT/fps.c \
	./pcsxcore/GPU_SOFT/gpu.c \
	./pcsxcore/GPU_SOFT/menu.c \
	./pcsxcore/GPU_SOFT/prim.c \
	./pcsxcore/GPU_SOFT/soft.c \
	./pcsxcore/GPU_SOFT/zn.c \
	./pcsxcore/plugins.c	\
	./pcsxcore/Plugin.c	\
	./pcsxcore/Decode_XA.c	\
	./pcsxcore/ppc/pasm.s	\
	./pcsxcore/R3000A.c	\
	./pcsxcore/PsxInterpreter.c	\
	./pcsxcore/gte.c	\
	./pcsxcore/PsxHLE.c	\
	./pcsxcore/ppc/pR3000A.c	\
	./pcsxcore/ppc/ppc.c	\
	./pcsxcore/ppc/reguse.c \
	./pcsxcore/PlugPAD.c \
	./pcsxcore/PlugCD.c \
	./pcsxcore/spu_mednafen/Channel.cpp \
	./pcsxcore/spu_mednafen/DiscStreamer.cpp \
	./pcsxcore/spu_mednafen/Envelope.cpp \
	./pcsxcore/spu_mednafen/Interpolate.cpp \
	./pcsxcore/spu_mednafen/NoiseGenerator.cpp \
	./pcsxcore/spu_mednafen/Plugin.cpp \
	./pcsxcore/spu_mednafen/SPU.cpp \
	./pcsxcore/spu_mednafen/ps3snd.cpp \
	./pcsxcore/zlib/adler32.c \
	./pcsxcore/zlib/compress.c \
	./pcsxcore/zlib/crc32.c \
	./pcsxcore/zlib/deflate.c \
	./pcsxcore/zlib/gzio.c \
	./pcsxcore/zlib/infblock.c \
	./pcsxcore/zlib/infcodes.c \
	./pcsxcore/zlib/inffast.c \
	./pcsxcore/zlib/inflate.c \
	./pcsxcore/zlib/inftrees.c \
	./pcsxcore/zlib/infutil.c \
	./pcsxcore/zlib/trees.c \
	./pcsxcore/zlib/uncompr.c \
	./pcsxcore/zlib/zutil.c 

PPU_TARGET		=	ps3sx.elf

PPU_CPPFLAGS	+= -D__BIGENDIAN__ -D__ppc__ -D_BIG_ENDIAN -DBIG_ENDIAN -D'PACKAGE_VERSION="0.3"' -DPS3_SDK_3_41 -DPSGL -DMDFNPS3 \
		   -I. -I./PS3 -I./pcsxcore -I./pcsxcore/zlib -I./pcsxcore/ppc -I./PS3/threads -I./PS3/Audio -I./PS3/input -I./PS3/Ini -I./pcsxcore/spu_mednafen

PPU_ASFLAGS		+=

PPU_LDFLAGS		= -Wl -finline-limit=5000

PPU_CXXFLAGS		+=
PPU_LIBS		    +=	$(CELL_TARGET_PATH)/ppu/lib/libgcm_cmd.a \
					$(CELL_TARGET_PATH)/ppu/lib/libgcm_sys_stub.a $(CELL_TARGET_PATH)/ppu/lib/libfs_stub.a
PPU_LDLIBS		+= 	-L$(CELL_SDK)/target/ppu/lib/PSGL/RSX/opt -lPSGL -lPSGLcgc -lcgc -lsysmodule_stub 
PPU_LDLIBS		+=  -lresc_stub -lm -ldbgfont -lsysutil_stub -lio_stub -laudio_stub -lpthread -lsysutil_game_stub
include $(CELL_MK_DIR)/sdk.target.mk

$(VPSHADER_PPU_OBJS): $(OBJS_DIR)/%.ppu.o : %.vpo
	@mkdir -p $(dir $(@))
	$(PPU_OBJCOPY)  -I binary -O elf64-powerpc-celloslv2 -B powerpc $< $@

$(FPSHADER_PPU_OBJS): $(OBJS_DIR)/%.ppu.o : %.fpo
	@mkdir -p $(dir $(@))
	$(PPU_OBJCOPY)  -I binary -O elf64-powerpc-celloslv2 -B powerpc $< $@

pkg: $(PPU_TARGET)
	$(PPU_STRIP) -s $< -o $(OBJS_DIR)/$(PPU_TARGET)
	#$(MAKE_EBOOT) $(OBJS_DIR)/$(PPU_TARGET) release/NPDRM_RELEASE/USRDIR/EBOOT.BIN 
	$(MKFSELF_NPDRM) $(PPU_TARGET) release/NPDRM_RELEASE/USRDIR/EBOOT.BIN 
	$(CELL_HOST_PATH)/bin/psn_package_npdrm$(EXE_SUFFIX) release/package.conf release/NPDRM_RELEASE
	

