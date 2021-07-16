#include<ap_int.h>
#include<hls_stream.h>
#include<iostream>

#define MAX_LEN 50
#define K 3

using namespace std;

typedef ap_int<16> data_t;

void Conv2d(hls::stream<data_t> &FeatureIn,hls::stream<data_t> &FeatureOut,int N,int P);

