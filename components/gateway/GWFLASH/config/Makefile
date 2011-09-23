include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@echo "*.sdp --> $(DISTRIB_DIR)/isabelgw/lib/defaults/."
	@mkdir -p $(DISTRIB_DIR)/isabelgw/lib/defaults
	@cp *.sdp $(DISTRIB_DIR)/isabelgw/lib/defaults/.
	@echo "preseth264 --> $(DISTRIB_DIR)/isabelgw/lib/defaults/."
	@cp preseth264 $(DISTRIB_DIR)/isabelgw/lib/defaults/.
	@echo "preseth264HttpLive --> $(DISTRIB_DIR)/isabelgw/lib/defaults/."
	@cp preseth264HttpLive $(DISTRIB_DIR)/isabelgw/lib/defaults/.

cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelgw/lib/defaults/video.sdp
	@rm -f  $(DISTRIB_DIR)/isabelgw/lib/defaults/audio.sdp
	@rm -f  $(DISTRIB_DIR)/isabelgw/lib/defaults/todo.sdp
	@rm -f $(DISTRIB_DIR)/isabelgw/lib/defaults/preseth264
	@rm -f $(DISTRIB_DIR)/isabelgw/lib/defaults/preseth264HttpLive


include $(PROJECT_ROOT_DIR)/Makefile.tail
