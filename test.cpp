#include"conv2d.h"
#define N 28
#define P 1
int main(){
	data_t In[N+2*P][N+2*P];
	data_t Out[N][N];
	for(int i=0;i<N+2*P;i++)
		for(int j=0;j<N+2*P;j++){
			if(i>=P&&j>=P&&i<N+P&&j<N+P)
				In[i][j]=(i-P)*N+(j-P);
			else
				In[i][j]=0;
		}
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			data_t sum=0;
			for(int kx=0;kx<K;kx++)
				for(int ky=0;ky<K;ky++)
					sum+=In[i+kx][j+ky];
			Out[i][j]=sum;
		}
	hls::stream<data_t> featureIn;
	hls::stream<data_t> featureOut;
	for(int i=0;i<(N+2*P);i++)
		for(int j=0;j<(N+2*P);j++){
			data_t tmp=i*(N+2*P)+j;
			featureIn<<tmp;
		}
	Conv2d(featureIn,featureOut,N,P);
	cout<<"HLS RESULT\n";
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			data_t tmp;
			featureOut>>tmp;
			cout<<tmp<<",";
		}
		cout<<endl;
	}
	cout<<"GOLDEN OUTPUT\n";
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			cout<<Out[i][j]<<",";
		}
		cout<<endl;
	}
	return 0;
}
