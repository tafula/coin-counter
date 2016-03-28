all: CoinCounter Calibrator

FLAGS = -g
LIBS = -isystem/usr/include `pkg-config --libs opencv`
X11FLAG = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CPPVERSION = --std=c++11


clean:
	rm -rf CoinCounter
	rm -rf Calibrator
	rm -rf oldCalib
	rm -f *.o
	rm -f processamento-de-imgs/*.o
	rm -rf *.dSYM

BlobDetector.o: processamento-de-imgs/BlobDetector.cpp
	g++ -c processamento-de-imgs/BlobDetector.cpp

Util.o: processamento-de-imgs/Util.cpp
	g++ $< -o $@ -c $(X11FLAG)

CoinCounter: CoinCounter.cpp processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o $(CPPVERSION) $(FLAGS)

Calibrator: Calibrator.cpp processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o $(CPPVERSION) $(FLAGS)

oldCalib: expCalib.cpp processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o $(CPPVERSION) $(FLAGS)
