
ifeq ($(VCODECS_DIR),)
ifeq ($(PROJECT_ROOT_DIR),)
$(error ERROR: Set your VCODECS_DIR environment variable)
else
VCODECS_DIR=$(PROJECT_ROOT_DIR)/lib/vCodecs
endif
endif

ifeq ($(findstring ___$(VCODECS_DIR),___$(shell pwd)),)
$(error ERROR: I'm not below VCODECS_DIR. Check this variable.)
endif

Pll:
	@echo "VCODECS_DIR=$(VCODECS_DIR)"
	cd $(VCODECS_DIR);\
	mkdir -p build/vCodecs;\
	mkdir -p build/CellB;\
	mkdir -p build/jpeg;\
	mkdir -p build/h263;\
        mkdir -p build/h264;\
        mkdir -p build/libavcodec;\
	mkdir -p build/xvid;

	@echo "BUILDING xvid"
	cd $(VCODECS_DIR)/lib/codecs/xvid/build/generic;\
	chmod +x configure;\
	./configure;\
	make;\
        cp ./=build/libxvidcore.a $(VCODECS_DIR)/build/xvid;
	@echo "DONE WITH xvid"

	@echo "BUILDING h264"
	cd $(VCODECS_DIR)/lib/codecs/h264; \
	chmod +x configure;\
        ./configure;\
	make; \
        cp libx264.a $(VCODECS_DIR)/build/h264;
	@echo "DONE WITH h264"

	@echo "BUILDING ffmpeg"
	cd $(VCODECS_DIR)/lib/codecs/ffmpeg;\
        ./configure;\
        make -C libavutil all;\
        make -C libavcodec all;\
        cp ./libavcodec/libavcodec.a ./libavutil/libavutil.a $(VCODECS_DIR)/build/libavcodec;
	@echo "DONE WITH ffmpeg"

	@echo "BUILDING vCodecs"
	cd $(VCODECS_DIR)/lib;\
	make;
	@echo "DONE WITH vCodecs"
	
	@echo "BUILDING CellB"
	cd $(VCODECS_DIR)/lib/codecs/CellB;\
	make;
	@echo "DONE WITH CellB"

	@echo "BUILDING h263"
	cd $(VCODECS_DIR)/lib/codecs/H263;\
	make;
	@echo "DONE WITH h263"

	@echo "BUILDING jpeg"
	cd $(VCODECS_DIR)/lib/codecs/JPEG;\
	make;
	@echo "DONE WITH jpeg"

pkg::
	@rm -rf /tmp/vCodecs
	@echo "----------------------------------"
	@echo "Creating PACKAGE file for $(LINUX_DISTRIB)"
	@echo "----------------------------------"
	@./scripts/mkPkg-Linux

clean::
	cd $(VCODECS_DIR)/lib/codecs/xvid/build/generic;\
	make clean;

	cd $(VCODECS_DIR)/lib/codecs/ffmpeg;\
	make clean;

	cd $(VCODECS_DIR)/lib/codecs/h264;\
	make clean;


	cd $(VCODECS_DIR)/lib;\
        make clean;

	cd $(VCODECS_DIR)/lib/codecs/H263;\
        make clean;
  
	cd $(VCODECS_DIR)/lib/codecs/JPEG;\
        make clean;

