#include"conv2d.h"
#define N 28
#define P 1
int main(){
	data_t In[N+2*P][N+2*P];
	data_t Out[N][N];
	for(int i=0;i<N+2*P;i++)
		for(int j=0;j<N+2*P;j++){
			if(i>=P&&j>=P&&i<N+P&&j<N+P)
				In[i][j].data=(i-P)*N+(j-P);
			else
				In[i][j].data=0;
		}
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			data_t sum;
			sum.data=0;
			for(int kx=0;kx<K;kx++)
				for(int ky=0;ky<K;ky++)
					sum.data+=In[i+kx][j+ky].data;
			Out[i][j]=sum;
		}
	hls::stream<data_t> featureIn;
	hls::stream<data_t> featureOut;
	data_t tmp;
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			tmp.data=i*N+j;
			tmp.dest=0;
			tmp.id=0;
			tmp.keep=3;
			tmp.strb=3;
			tmp.user=0;
			tmp.last=((i==(N-1))&&(j==(N-1)))?1:0;
			featureIn<<tmp;
		}
	Conv2d(featureIn,featureOut,N,P);
	cout<<"HLS RESULT\n";
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			data_t tmp;
			featureOut>>tmp;
			cout<<tmp.data<<",";
		}
		cout<<endl;
	}
	cout<<"GOLDEN OUTPUT\n";
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			cout<<Out[i][j].data<<",";
		}
		cout<<endl;
	}
	return 0;
}
