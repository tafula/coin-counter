all: main

FLAGS = -g
LIBS = -isystem/usr/include `pkg-config --libs opencv`
X11FLAG = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CPPVERSION = --std=c++11


clean:
	rm -rf main
	rm -f *.o
	rm -f processamento-de-imgs/*.o
	rm -rf *.dSYM

Util.o: processamento-de-imgs/Util.cpp
	g++ $< -o processamento-de-imgs/$@ -c $(X11FLAG)

BlobDetector.o: processamento-de-imgs/BlobDetector.cpp
	g++ $< -o processamento-de-imgs/$@ -c

ToposCorrector.o: processamento-de-imgs/ToposCorrector.cpp
	g++ $< -o processamento-de-imgs/$@ -c $(CPPVERSION)

CoinCounter.o: CoinCounter.cpp
	g++ $< -o $@ -c $(X11FLAG) $(CPPVERSION)

main: main.cpp Util.o BlobDetector.o ToposCorrector.o CoinCounter.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o processamento-de-imgs/ToposCorrector.o CoinCounter.o $(CPPVERSION) $(FLAGS)
