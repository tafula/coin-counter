all: CoinCounter Calibrator

FLAGS = -g
LIBS = -isystem/usr/include `pkg-config --libs opencv`
X11FLAG = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CPPVERSION = --std=c++11


clean:
	rm -f HandCounter
	rm -f HandCounter_bgRemoval
	rm -f VoteMode
	rm -f DialMode
	rm -rf CoinCounter
	rm -f *.o
	rm -rf *.dSYM

InteractiveObject.o: InteractiveObject.cpp
	g++ $< -o $@ -c $(CPPVERSION)

BlobDetector.o: BlobDetector.cpp
	g++ -c BlobDetector.cpp

Util.o: Util.cpp
	g++ $< -o $@ -c $(X11FLAG)

CoinCounter: CoinCounter.cpp Util.o BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) Util.o BlobDetector.o $(CPPVERSION) $(FLAGS)

Calibrator: Calibrator.cpp Util.o BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) Util.o BlobDetector.o $(CPPVERSION) $(FLAGS)
