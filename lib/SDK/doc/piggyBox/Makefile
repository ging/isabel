.SUFFIXES: .tex .dpi .ps .ftc .iftc .itex .pdf

.ps.pdf:
	ps2pdf $< $@
.tex.dvi:
	@aux=`basename $< .tex`.aux	;\
	touch $$aux			;\
	cp $$aux .$$aux			;\
	latex $<			;\
	if diff $$aux .$$aux>/dev/null	;\
	then				\
	echo done $@ from $<		;\
	else				\
	echo "**"			;\
	echo "**"			;\
	echo "** second pass"		;\
	echo "**"			;\
	echo "**"			;\
	sleep 1				;\
	touch $<			;\
	$(MAKE) $@			;\
	fi
.dvi.ps:
	dvips $< -o
.ftc.tex:
	./ftc.sed < $< > $@
.iftc.itex:
	./ftc.sed < $< > $@



TARGET= piggy.pdf

target: $(TARGET)

piggy.dvi:	\
	piggy.tex		\
	abstract.itex		\
	dlm.itex		\
	format.itex image.itex	\
	factory.itex		\
	codec.itex		\
	grabber.itex		\
	player.itex

clean:
	rm -f *.aux *.itex 
	rm -f *.toc *.dvi *.ps *.pdf
	rm -f *.log

