/***************************** Include Files *********************************/
#include "xaxidma.h"
#include "xparameters.h"
#include "xdebug.h"
#include "xconv2d.h"
#include "stdio.h"
#include "xtime_l.h"

/******************** Constant Definitions **********************************/
/*
 * Device hardware build related constants.
 */
#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID
#define MEM_BASE_ADDR		0x01000000
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define N 28
#define P 1

int XAxiDma_SimplePollExample(u16 DeviceId);
static int CheckData(void);

XAxiDma AxiDma;
XConv2d gExample;
short *TxBufferPtr;
short *RxBufferPtr;

XTime tEnd, tCur;
u32 tUsed;

short IN[N+2*P][N+2*P];
short OUT[N][N];

void Init_IN(short IN[N+2*P][N+2*P]){
	for(int i=0;i<N+2*P;i++)
		for(int j=0;j<N+2*P;j++)
			if(i>=P&&i<N+P&&j>=P&&j<N+P)
				IN[i][j]=(i-P)*N+j-P;
			else
				IN[i][j]=0;
}

void golden_conv(short IN[N+2*P][N+2*P],short OUT[N][N]){
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			short tmp=0;
			for(int kx=0;kx<3;kx++)
				for(int ky=0;ky<3;ky++)
					tmp+=IN[i+kx][j+ky];
			OUT[i][j]=tmp;
		}
}

int main()
{

	int Status;
	printf("\r\n--- Entering main() --- \r\n");
	//init HLS IP
	int _status = XConv2d_Initialize(&gExample,XPAR_CONV2D_0_DEVICE_ID);
	if(_status != XST_SUCCESS)
	{
		printf("XExample_initialize failed\n");
		return XST_FAILURE;
	}
	//start HLS IP
	XConv2d_Set_N(&gExample,N);
	XConv2d_Set_P(&gExample,P);

	/* Run the poll example for simple transfer */
	Status = XAxiDma_SimplePollExample(DMA_DEV_ID);
	if (Status != XST_SUCCESS) {
		printf("XAxiDma_SimplePoll Example Failed\r\n");
		return XST_FAILURE;
	}
	printf("Successfully ran XAxiDma_SimplePoll Example\r\n");
	printf("--- Exiting main() --- \r\n");
	return XST_SUCCESS;
}

int XAxiDma_SimplePollExample(u16 DeviceId)
{
	XAxiDma_Config *CfgPtr;
	int Status;
	TxBufferPtr = (short *)TX_BUFFER_BASE ;
	RxBufferPtr = (short *)RX_BUFFER_BASE;
	/* Initialize the XAxiDma device.
	 */
	CfgPtr = XAxiDma_LookupConfig(DeviceId);
	if (!CfgPtr) {
		printf("No config found for %d\r\n", DeviceId);
		return XST_FAILURE;
	}
	Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
	if (Status != XST_SUCCESS) {
		printf("Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}
	if(XAxiDma_HasSg(&AxiDma)){
		printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}
	/* Disable interrupts, we use polling mode
	 */
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
						XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
						XAXIDMA_DMA_TO_DEVICE);
	//disable the cache
    Xil_DCacheDisable();
	//write to the TxBuffer and RxBuffer default value
	for(int i = 0; i < N*N; i++)
	{
			TxBufferPtr[i] = i;
			RxBufferPtr[i] = 0;
	}

	Status = XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) RxBufferPtr,
				N*N*sizeof(short), XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) TxBufferPtr,
				N*N*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XTime_GetTime(&tCur);
	XConv2d_Start(&gExample);
	while (!XConv2d_IsDone(&gExample));
	XTime_GetTime(&tEnd);
	tUsed = ((tEnd-tCur)*1000000)/(COUNTS_PER_SECOND);
	xil_printf("time elapsed is %d us\r\n",tUsed);
	while ((XAxiDma_Busy(&AxiDma,XAXIDMA_DEVICE_TO_DMA)) ||
		(XAxiDma_Busy(&AxiDma,XAXIDMA_DMA_TO_DEVICE))) {
			/* Wait */
	}
	Status = CheckData();
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/* Test finishes successfully
	 */
	return XST_SUCCESS;
}

static int CheckData(void)
{
	printf("The RxData are\r\n");
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++)
		{
			printf("%6d,",RxBufferPtr[i*N+j]);
		}
		printf("\n");
	}
	int error=0;
	Init_IN(IN);
	golden_conv(IN,OUT);
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			if(OUT[i][j]!=RxBufferPtr[i*N+j])
				error++;
	printf("error count=%d\r\n",error);
	print("end\r\n");
	return XST_SUCCESS;
}

