____all:
	@ISA_OSFULL=`uname`-`uname -r`			;\
	case $$ISA_OSFULL in				 \
		SunOS-4*) ISA_OS=SunOS   ;;		 \
		SunOS-5*) ISA_OS=Solaris ;;		 \
		Linux-*)  ISA_OS=Linux   ;;		 \
		IRIX-*)   ISA_OS=IRIX    ;;		 \
		CYGWIN*)  ISA_OS=WIN32   ;;		 \
		*)        ISA_OS=Unknown ;;		 \
	esac						;\
	$(MAKE) -f Makefile-$$ISA_OS $(MKFLAGS)

.DEFAULT:
	@ISA_OSFULL=`uname`-`uname -r`			;\
	case $$ISA_OSFULL in				 \
		SunOS-4*) ISA_OS=SunOS   ;;		 \
		SunOS-5*) ISA_OS=Solaris ;;		 \
		Linux-*)  ISA_OS=Linux   ;;		 \
		IRIX-*)   ISA_OS=IRIX    ;;		 \
		CYGWIN*)  ISA_OS=WIN32   ;;		 \
		*)        ISA_OS=Unknown ;;		 \
	esac						;\
	$(MAKE) -f Makefile-$$ISA_OS $(MKFLAGS) $@
