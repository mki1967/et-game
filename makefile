# INC= -I/usr/X11/include/ -I/usr/share/doc/packages/NVIDIA_GLX/include/
# INC= -I/usr/openwin/include/
# INC= -I/usr/include/X11/ -I/usr/share/doc/NVIDIA_GLX-1.0/include/
 LIB= -L/usr/X11/lib 
# LIB= -L/usr/openwin/lib/
# LIB= -L/usr/X11R6/lib 

all: et-game

et-game: et-game.c
	gcc $(INC) $(LIB)  -o et-game et-game.c -lX11 -lm -lGL

src: 
	mv et-game et-game.bin; cd ..; tar zcvf et-game.tgz et-game/{*.c,makefile,README,COPYING,et-game.bin,*.et,*.cfg,runme*}\

