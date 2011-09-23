

SRCS=d3des.c vncauth.c vncpasswd.c


all:
	make isabel_vncpasswdgen


isabel_vncpasswdgen: $(SRCS)
	gcc -o isabel_vncpasswdgen -I. $(SRCS)


clean:
	rm isabel_vncpasswdgen

cleanall: clean


