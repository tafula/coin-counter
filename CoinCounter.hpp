#ifndef COINCOUNTER_HPP
#define COINCOUNTER_HPP

#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <sys/time.h>
#include "processamento-de-imgs/Util.hpp"
#include "processamento-de-imgs/BlobDetector.hpp"

#define EROSION_DILATION_SIZE 3
#define QTT_SIGMA 1.1

using namespace cv;
using namespace std;

vector<int> coinValues = {5, 10, 25, 50, 100};
vector<double> ratDiaCoin = {22.0, 20.0, 25.0, 23.0, 27.0};
vector<double> coinFactor = {pow(ratDiaCoin[0],2)/pow(ratDiaCoin[4],2), pow(ratDiaCoin[1],2)/pow(ratDiaCoin[4],2), pow(ratDiaCoin[2],2)/pow(ratDiaCoin[4],2),pow(ratDiaCoin[3],2)/pow(ratDiaCoin[4],2), 1}; //Razao entre areas

#endif
