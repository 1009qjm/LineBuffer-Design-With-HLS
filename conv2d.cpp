#include"conv2d.h"

void Conv2d(hls::stream<data_t> &FeatureIn,hls::stream<data_t> &FeatureOut,int N,int P){
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=P bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=N bundle=CTRL
#pragma HLS INTERFACE axis register both port=FeatureOut
#pragma HLS INTERFACE axis register both port=FeatureIn
#pragma HLS DATAFLOW
	hls::stream<data_t> Line1;
#pragma HLS STREAM variable=Line1 depth=50 dim=1
	hls::stream<data_t> Line2;
#pragma HLS STREAM variable=Line2 depth=50 dim=1
	data_t Window[K][K];
#pragma HLS ARRAY_PARTITION variable=Window complete dim=0
	data_t pixIn,Line1Rd,Line2Rd;
	int LEN1,LEN2;
	LEN1=0;
	LEN2=0;
	int start_index=2*N+4*P+K-1;
	for(int i=0;i<(N+2*P)*(N+2*P);i++){
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=900 max=900 avg=900
		//处理PADDING问题
		int row=i/(N+2*P);
		int col=i%(N+2*P);
		if(row>=P&&row<N+P&&col>=P&&col<N+P)
			FeatureIn>>pixIn;
		else
			pixIn.data=0;
		//LineBuffer
		if(LEN1==(N+2*P)){
			Line1>>Line1Rd;
		    LEN1-=1;
		}
		if(LEN2==(N+2*P)){
			Line2>>Line2Rd;
		    LEN2-=1;
		}
		Line1<<pixIn;
		Line2<<Line1Rd;
		LEN1+=1;
		LEN2+=1;
		//Window
		Window[0][0]=Window[0][1];
		Window[0][1]=Window[0][2];
		Window[0][2]=Line2Rd;
		Window[1][0]=Window[1][1];
		Window[1][1]=Window[1][2];
		Window[1][2]=Line1Rd;
		Window[2][0]=Window[2][1];
		Window[2][1]=Window[2][2];
		Window[2][2]=pixIn;
		data_t sum;
		sum.data=0;
		if(i>=start_index&&(i-start_index)%(N+2*P)<N){
			for(int i=0;i<K;i++)
#pragma HLS UNROLL
				for(int j=0;j<K;j++){
#pragma HLS UNROLL
					sum.data+=Window[i][j].data;
				}
			sum.dest=0;
			sum.id=0;
			sum.keep=3;
			sum.strb=3;
			sum.user=0;
			sum.last=(i==((N+2*P)*(N+2*P)-1))?1:0;
			FeatureOut<<sum;
		}
		//print
		if(i>=start_index&&(i-start_index)%(N+2*P)<N){
			cout<<"i="<<i<<endl;
			for(int kx=0;kx<K;kx++){
				for(int ky=0;ky<K;ky++)
					cout<<Window[kx][ky].data<<",";
				cout<<endl;
			}
		}
	}
}
