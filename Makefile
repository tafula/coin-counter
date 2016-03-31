all: calibrador contador

FLAGS = -g
LIBS = -isystem/usr/include `pkg-config --libs opencv`
X11FLAG = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CPPVERSION = --std=c++11


clean:
	rm -rf calibrador
	rm -rf contador
	rm -f *.o
	rm -f processamento-de-imgs/*.o
	rm -rf *.dSYM

Util.o: processamento-de-imgs/Util.cpp
	g++ $< -o $@ -c $(X11FLAG)

BlobDetector.o: processamento-de-imgs/BlobDetector.cpp
	g++ -c processamento-de-imgs/BlobDetector.cpp

CoinCounter.o: CoinCounter.cpp
	g++ $< -o $@ -c $(X11FLAG) $(CPPVERSION)

calibrador: calibrador.cpp processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o CoinCounter.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o CoinCounter.o $(CPPVERSION) $(FLAGS)

contador: contador.cpp processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o CoinCounter.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) processamento-de-imgs/Util.o processamento-de-imgs/BlobDetector.o CoinCounter.o $(CPPVERSION) $(FLAGS)
