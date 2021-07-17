#include<ap_int.h>
#include<hls_stream.h>
#include "ap_axi_sdata.h"
#include<iostream>

#define MAX_LEN 50
#define K 3

using namespace std;

typedef ap_axis<16,1,1,1> data_t;

void Conv2d(hls::stream<data_t> &FeatureIn,hls::stream<data_t> &FeatureOut,int N,int P);

