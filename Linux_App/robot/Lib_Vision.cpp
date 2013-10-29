
//-- For OpenNI2
//
#include <stdio.h>
#include <OpenNI.h>

#include "OniSampleUtilities.h"

//-- For OpenCV
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>

#include "main.h"
#include "./Main_Lib/Thread_Lib.h"  


#include "cv.h"
#include "highgui.h"


#include "./cvblob_lib/cvblob.h"

using namespace cvb;
using namespace openni;


clock_t	start_point, end_point; 
clock_t start_point_frame;
clock_t end_point_frame;



#define DEPTH_MAX_MM		3000.0

#define _DEBUG_WINDOW_		0
#define _DEBUG_LABELING_	0


#define MAX_LABEL_ID      	254

#define _FALSE 0
#define _TRUE  1

#define BINARY_THRESHOLD		155
#define RED_THRESHOLD	    	80
#define LABEL_SIZE_THRESHOLD	20

typedef struct
{
	int numPixel;
	int id;
} LabelST;


#define SAMPLE_READ_WAIT_TIMEOUT	2000




int Lib_Vision_Debug = 0;


//-- OpenNI 변수 
//
VideoStream depth, color;
Device device;

IplImage *IplImage_depth;
IplImage *IplImage_color;

IplImage *IplImage_depth_resize;
IplImage *IplImage_color_resize;



	
cv::Mat MatImage_depth;
cv::Mat MatImage_color;


VideoFrameRef *frame_ptr;	
VideoFrameRef  frame_depth;
VideoFrameRef  frame_color;	
VideoStream*   streams[] = {&depth, &color};


int CapturedFlag = 0;


double 	process_time;
int 	process_rate;
double  process_time_depth;
double  process_time_color;
double  process_time_frame;




int OpenNI_Init(void);
int OpenNI_Close(void);
int OpenNI_Capture( THREAD_OBJ *pArg );

int BlobLabeling(IplImage *imSRC, IplImage *imLabel, LabelST *stLabel, int *countLabel);
int GetMainBlob(IplImage *imLabel, LabelST *stLabel, int countLabel);
int GetObjDistance(IplImage *imLabel);

int Tracking_Color( THREAD_OBJ *pArg );
int Detect_Red( THREAD_OBJ *pArg );



extern int Lib_Motor_PwmLeft;
extern int Lib_Motor_PwmRight;


void *Lib_Vision(void *Arg)
{
	THREAD_OBJ *pArg = (THREAD_OBJ *)Arg;

	printf("Enter Vision \n");


	pArg->Thread_Ret = Tracking_Color( pArg );
	//pArg->Thread_Ret = Detect_Red( pArg );


	printf("Exit Vision Ret : %d\n", pArg->Thread_Ret);

	return (void *)&(pArg->Thread_Ret);
}





int Detect_Red( THREAD_OBJ *pArg )
{
	char 	ch;
	double 	process_time;
	int 	process_rate;

	clock_t start_point;
	clock_t end_point;
		
	// 프레임 포인터
	IplImage *frame;
	IplImage *image;
	IplImage *img_bin;	
	IplImage *img_red;
	IplImage *img_green;
	IplImage *img_blue;
	IplImage *img_dst;
	
	int threadhold_red_value;
	int threadhold_bin_value;
	
	
	image = cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 3);

	img_bin 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);	
	img_red 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	img_green 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	img_blue 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	
	img_dst		= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	
	threadhold_red_value = 120;	// 120
	threadhold_bin_value = 165;	// 220
		

	if( OpenNI_Init() > 0 )
	{
		printf("OpenNI Init Failed\n");

		pArg->Thread_Ret = 0;
	
		return 1;
	}
	else
	{
		printf("OpenNI Init OK\n");
	}


	//-- 알고리즘 실행 
	//
	while ( pArg->Thread_Stop == FALSE )
	{

		if( OpenNI_Capture( pArg ) != TRUE )
		{
			continue;
		}	

		
		// 카메라로 부터 1프레임 이미지를 읽어온다.
		frame = IplImage_color;

		// YCrCb 형태로 변경
		cvCvtColor(frame, image, CV_RGB2YCrCb);
		
		// 채널별로 분리
		cvSplit(image, img_blue, img_green, img_red, NULL);
		cvMaxS(img_red, threadhold_red_value, img_red);
		

		// 이진화
		cvThreshold(img_red, img_bin, threadhold_bin_value, 255, CV_THRESH_BINARY);
		
		// 팽창
		cvDilate(img_bin, img_dst, NULL, 4);
		
		// 침식
		cvErode(img_dst, img_dst, NULL, 4);
		
		// 모멘트 구하기
		CvMoments moments;
		
		int binary = 1;
		
		cvMoments(img_dst, &moments, binary);
		
		
		
		double M;
		int x_order;
		int y_order;
		double cX, cY;
		double m00;
		
		for( y_order=0; y_order<=3; y_order++ )
		{
			for( x_order=0; x_order<=3; x_order++ )
			{
				if( x_order+y_order > 3 )
				{
					continue;
				}
				
				M = cvGetSpatialMoment( &moments, x_order, y_order );
				
				if( x_order == 0 && y_order == 0 )
				{
					m00 = M;
				}
				else if( x_order == 1 && y_order == 0 )
				{
					cX = M;
				}
				else if( x_order == 0 && y_order == 1 )
				{
					cY = M;
				}
			}
		}
		
		cX = cX / m00;
		cY = cY / m00;
		
		cvCircle(frame, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(0, 0, 255), 2);
		
		int x_offset;
		
		x_offset = cX - (IMG_WIDTH/2);

		Lib_Motor_PwmLeft  =  x_offset;
		Lib_Motor_PwmRight = -x_offset; 
		
		// Labeling
		// 구형 예정
		
		
		cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth);  
		cvSaveImage("/mnt/ramdisk/color/color.jpg",frame); 

		//cvResize(IplImage_depth, IplImage_depth_resize);
		//cvResize(frame, IplImage_color_resize);
		//cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth_resize);  
		//cvSaveImage("/mnt/ramdisk/color/color.jpg",IplImage_color_resize); 

		//usleep(100*1000);
		
		end_point = clock();
		process_time = ((double)(end_point - start_point)/(CLOCKS_PER_SEC/1000));

		if( Lib_Vision_Debug == 1 )
		{
			printf("Exe time : %04f msec  %03.1f frames\r\n", process_time, 1000./process_time ); 
		}
		
		start_point = clock();
	}
	

	// Release All Images and Windows
	cvReleaseImage(&image);
	cvReleaseImage(&img_bin);
	cvReleaseImage(&img_red);
	cvReleaseImage(&img_green);
	cvReleaseImage(&img_blue);
	cvReleaseImage(&img_dst);

	OpenNI_Close();

	return 0;
}





int BlobLabeling(IplImage *imSRC, IplImage *imLabel, LabelST *stLabel, int *countLabel);
int GetMainBlob(IplImage *imLabel, LabelST *stLabel, int countLabel);



int Tracking_Color( THREAD_OBJ *pArg )
{
	IplImage *frame = NULL;//NULL « ºˆ.
	IplImage *image;
	IplImage *imgRed = NULL;
	IplImage *imgBlue = NULL;
	IplImage *imgGreen = NULL;
	IplImage *imgBin = NULL;
	IplImage *imgMorph = NULL;
	IplImage *imgLabel   = NULL;
	LabelST stLabel[MAX_LABEL_ID];
	int countLabel;
	char strLine[255];
	int  ObjDistance;	


	if( OpenNI_Init() > 0 )
	{
		printf("OpenNI Init Failed\n");

		pArg->Thread_Ret = 0;
	
		return 1;
	}
	else
	{
		printf("OpenNI Init OK\n");
	}


	image 		= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 3);
	
	imgBlue 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	imgGreen 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	imgRed 		= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	imgBin 		= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	imgMorph 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	imgLabel 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	
	

    CvFont font;
    //cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.8, 0.8, 0, 1, CV_AA);
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
    
	

	while ( pArg->Thread_Stop == FALSE )
	{
		if( OpenNI_Capture( pArg ) == FALSE )
		{
			continue;
		}
		
		// 카메라로 부터 1프레임 이미지를 읽어온다.
		frame = IplImage_color;

		// YCrCb 형태로 변경
		cvCvtColor(frame, image, CV_RGB2YCrCb);

		//√§≥Œ ∫–∏Æ, red color
		cvSplit(image, imgBlue, imgGreen, imgRed, NULL);
		//RED
		cvMaxS(imgRed, RED_THRESHOLD, imgRed);
		//¿Ã¡¯»≠
		cvThreshold(imgRed, imgBin, BINARY_THRESHOLD, 255, CV_THRESH_BINARY);
		
		// ∆ÿ√¢
		cvDilate(imgBin, imgMorph, NULL, 4);
		
		// ƒßΩƒ
		cvErode(imgMorph, imgMorph, NULL, 4);
		
		
		
		double M;
		int x_order;
		int y_order;
		double cX, cY;
		double m00;
		int binary = 1;
		int DetectObj = _FALSE;
		int ObjDistance = 0;

		//Labeling
		if(BlobLabeling(imgMorph, imgLabel, stLabel, &countLabel) == _FALSE);
		else
		{
			//Tracking - ∞°¿Â ≈©±‚∞° ≈´ Blob æÚ±‚
			DetectObj = GetMainBlob(imgLabel, stLabel, countLabel);
			
			// 물체의 거리를 구한다.
			//
			ObjDistance = GetObjDistance(imgLabel);

			//Tracking - ∏∏‡∆Æ ±∏«œ±‚
			CvMoments moments;
			cvMoments(imgLabel, &moments, binary);
			
			for( y_order=0; y_order<=3; y_order++ )
			{
				for( x_order=0; x_order<=3; x_order++ )
				{
					if( x_order+y_order > 3 )
					{
						continue;
					}
					
					M = cvGetSpatialMoment( &moments, x_order, y_order );
					
					if( x_order == 0 && y_order == 0 )
					{
						m00 = M;
					}
					else if( x_order == 1 && y_order == 0 )
					{
						cX = M;
					}
					else if( x_order == 0 && y_order == 1 )
					{
						cY = M;
					}
				}
			}
			
			cX = cX / m00;
			cY = cY / m00;
		}
		

		if( DetectObj == _TRUE && ObjDistance > 0 )
		{
			int x_offset;
			int z_offset;
			int   M_Speed  = 0;
			float M_Handle = 0.0;

			x_offset = cX - (IMG_WIDTH/2);
			z_offset = -(900 - ObjDistance);

			M_Speed = z_offset / 10;

			M_Handle = abs(M_Speed) * (float)x_offset/100.;


			Lib_Motor_PwmLeft  = M_Speed + M_Handle;
			Lib_Motor_PwmRight = M_Speed - M_Handle; 


		}
		else
		{
			Lib_Motor_PwmLeft  = 0;
			Lib_Motor_PwmRight = 0; 
		}
		if( DetectObj == _TRUE )
		{

			cvCircle(frame, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(255, 0, 0), 2);
			cvCircle(imgBin, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(255, 255, 255), 2);
			cvCircle(imgRed, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(255, 255, 255), 2);
			cvCircle(IplImage_depth, cvPoint(cvRound(cX*1.06), cvRound(cY*1.04)), 20, CV_RGB(255, 255, 255), 2);

			sprintf( strLine, "%d mm", ObjDistance );
			cvPutText(frame, strLine, cvPoint(220, 20), &font, cvScalar(0, 0, 255, 0));
		}
		else
		{
			cX = 0;
			cY = 0;

			cvPutText(frame, "No Red", cvPoint(220, 20), &font, cvScalar(0, 0, 255, 0));			
		}

		
		#if 0
		// RGB 값 구하기
		
		cvRectangle(frame, cvPoint(frame->width/2-5,frame->height/2-5), cvPoint(frame->width/2+5,frame->height/2+5), cvScalar(255,0,0), 2,8,0);
		
		int R, G, B;
		int x, y;
		int RedFlag;
		
		for ( int i = 0 ; i < frame->height ; i++)
		{
			
			for( int j = 0 ; j < frame->width ; j++)
			{
				CvScalar v = cvGet2D(frame, i , j );
				B = v.val[0];// 이미지에 j,i 좌표의 Blue 성분 추출
				G = v.val[1]; // 이미지에 j,i 좌표의 Green 성분 추출
				R = v.val[2]; // 이미지에 j,i 좌표의 Red 성분 추출
			
				RedFlag = 0;
				
				//if(R > 200 && R-G > 70 && R-B > 70) // 임의의 빨간색 조건
				
				if( R >= 70 && R <= 255 ) RedFlag++;
				if( G >=  0 && G <=  40 ) RedFlag++;
				if( B >=  0 && B <=  40 ) RedFlag++;
				
				
				if( RedFlag == 3 )
				{
					x = j;
					y = i;
					cvRectangle(frame, cvPoint(x,y), cvPoint(x+4,y+4), cvScalar(255,0,0), 2,8,0);// 만족하는 경우 사각형을 그려주게 됨 .
				}
			}
		}
		#endif

		CvScalar sRef  = cvGet2D(frame, frame->width/2 , frame->height/2 );
		
		sprintf( strLine, "R: %d, G: %d, B: %d", (int)sRef.val[2], (int)sRef.val[1], (int)sRef.val[0] );
		cvPutText(frame, strLine, cvPoint(5, 20), &font, cvScalar(0, 0, 255, 0));
		

		//-- 영상 출력
		//
		cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth);  
		//cvSaveImage("/mnt/ramdisk/color/color.jpg",imgLabel); 
		cvSaveImage("/mnt/ramdisk/color/color.jpg",IplImage_color); 

		//cvResize(IplImage_depth, IplImage_depth_resize);
		//cvResize(frame, IplImage_color_resize);
		//cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth_resize);  
		//cvSaveImage("/mnt/ramdisk/color/color.jpg",IplImage_color_resize); 		
		

		end_point = clock();
		process_time = ((double)(end_point - start_point)/(CLOCKS_PER_SEC/1000));

		if( Lib_Vision_Debug == 1 )
		{
			printf("Exe time : %04f msec  %03.1f frames\r\n", process_time, 1000./process_time ); 
		}
		
		start_point = clock();


	
		
	}
	
	//cvReleaseImage(&frame);
	cvReleaseImage(&image);
	cvReleaseImage(&imgRed);
	cvReleaseImage(&imgBlue);
	cvReleaseImage(&imgGreen);
	cvReleaseImage(&imgBin);
	cvReleaseImage(&imgMorph);
	cvReleaseImage(&imgLabel);
	

	OpenNI_Close();

	return 0;
}



int BlobLabeling(IplImage *imSRC, IplImage *imLabel, LabelST *stLabel, int *pCountLabel)
{
	int ix, iy, ig;
	int arNumBuf[MAX_LABEL_ID] = {0, };
	int groupBuf[9] = { 0, };
	int minLabel;
	int maxLabel;
	int bufLabel;
	int curLabel;
	int outflag;
	int width, height;
	unsigned char *pImg   = (unsigned char *)imSRC->imageData;
	unsigned char *pLabel = (unsigned char *)imLabel->imageData;
	int yOffM1, yOff, yOffP1;
	int countLabel;
	
	
	width  = IMG_WIDTH;
	height = IMG_HEIGHT;
	
	
	// ∑π¿Ã∫Ì øµªÛ √ ±‚»≠
	for(iy=0; iy<height; iy++)
	{
		yOff   = iy * width;
		for(ix=0; ix<width; ix++)
		{
			pLabel[yOff + ix] = 255;
		}
	}
	for(ig=0; ig<MAX_LABEL_ID; ig++)
	{
		stLabel[ig].id         = 0;
		stLabel[ig].numPixel = 0;
		arNumBuf[ig] = 0;
	}
	
	
	// 1¬˜  ∑π¿Ã∫Ì ºˆ«‡
	maxLabel = -1;
	for(iy=1; iy<height-1; iy++)
	{
		yOffM1 = (iy-1) * width;
		yOff   = iy * width;
		
		for(ix=1; ix<width-1; ix++)
		{
			if(pImg[yOff+ix] == 255)
			{
				groupBuf[0] = pLabel[yOffM1+ix-1];
				groupBuf[1] = pLabel[yOffM1+ix  ];
				groupBuf[2] = pLabel[yOffM1+ix+1];
				groupBuf[3] = pLabel[yOff  +ix-1];
				
				minLabel = 255;
				for(ig=0; ig<4; ig++)
				{
					bufLabel = groupBuf[ig];
					if(bufLabel < minLabel)
						minLabel = bufLabel;
				}
				if(minLabel == 255)
				{
					maxLabel++;
					pLabel[yOff + ix] = maxLabel;
				}
				else
					pLabel[yOff + ix] = minLabel;
			}
		}
	}
	
	if(maxLabel == -1)
		return _FALSE;
	
	
	while(1)
	{
		outflag = 1;
		for(iy=1; iy<height-1; iy++)
		{
			yOffM1 = (iy-1) * width;
			yOff   = iy * width;
			yOffP1 = (iy+1) * width;
			for(ix=1; ix<width-1; ix++)
			{
				if(pImg[yOff + ix] == 255)
				{
					groupBuf[0] = pLabel[yOffM1 + ix-1];
					groupBuf[1] = pLabel[yOffM1 + ix  ];
					groupBuf[2] = pLabel[yOffM1 + ix+1];
					groupBuf[3] = pLabel[yOff   + ix-1];
					groupBuf[4] = pLabel[yOff   + ix  ];
					groupBuf[5] = pLabel[yOff   + ix+1];
					groupBuf[6] = pLabel[yOffP1 + ix-1];
					groupBuf[7] = pLabel[yOffP1 + ix  ];
					groupBuf[8] = pLabel[yOffP1 + ix+1];
					
					curLabel = groupBuf[4];
					minLabel = 255;
					for(ig=0; ig<9; ig++)
					{
						bufLabel = groupBuf[ig];
						if(bufLabel < minLabel)
							minLabel = bufLabel;
					}
					
					if(minLabel == 255 || curLabel == minLabel);
					else
					{
						pLabel[yOff + ix] = minLabel;
						outflag = 0;
					}
				}
			}
		}
		if(outflag == 1)
			break;
	}
	
	// ∑π¿Ã∫Ì º≥¡§.
	for(iy=1; iy<height-1; iy++)
	{
		yOff   = iy * width;
		for(ix=1; ix<width-1; ix++)
		{
			arNumBuf[pLabel[yOff + ix]]++;
		}
	}
	
	countLabel = 0;
	for(ig=0;ig<MAX_LABEL_ID; ig++)
	{
		if(arNumBuf[ig] != 0)
		{
			stLabel[countLabel].id       = ig;
			stLabel[countLabel].numPixel = arNumBuf[ig];
			countLabel++;
		}
	}
	
	if(countLabel == 0)
		return _FALSE;
	
	*pCountLabel = countLabel;
	
#if _DEBUG_WINDOW_ == 1
#if _DEBUG_LABELING_ == 1
	int arID[MAX_LABEL_ID] = {0, };
	int count;
	
	for(ig=0; ig<MAX_LABEL_ID; ig++)
	{
		arID[ig] = 0;
	}
	
	count = 0;
	
	for(ig=0;ig<MAX_LABEL_ID; ig++)
	{
		if(arNumBuf[ig] != 0)
		{
			arID[ig] = count;
			count++;
		}
	}
	
	for(iy=1; iy<height-1; iy++)
	{
		yOff   = iy * width;
		for(ix=1; ix<width-1; ix++)
		{
			if(pLabel[yOff + ix] == 255);
			else
				pLabel[yOff + ix] = 255/(count+1)*arID[pLabel[yOff+ix]];
		}
	}
#endif
#endif
	
	return _TRUE;
}


int GetMainBlob(IplImage *imLabel, LabelST *stLabel, int countLabel)
{
	int ix, iy, ig;
	int countBuf;
	int width, height;
	unsigned char *pLabel = (unsigned char *)imLabel->imageData;
	int yOff;
	int max = 0;
	int mainID;
	
	width = IMG_WIDTH;
	height = IMG_HEIGHT;
	
	if(countLabel == 1)
		mainID = 0;
	else
	{
		ig = 0;
		ix = 0;
		max = 0;
		for(ix=0; ix<countLabel; ix++)
		{
			countBuf = stLabel[ix].numPixel;
			if(countBuf > max)
			{
				max = countBuf;
				mainID = stLabel[ix].id;
			}
		}
	}
	
	for(iy=0; iy<height; iy++)
	{
		yOff = iy*width;
		for(ix=0; ix<width; ix++)
		{
			if(pLabel[yOff + ix] == mainID)
				pLabel[yOff + ix] = 255;
			else
				pLabel[yOff + ix] = 0;
		}
	}
	

	if( max > LABEL_SIZE_THRESHOLD )
	{
		return _TRUE;
	}
	else
	{
		return _FALSE;
	}
}





int GetObjDistance(IplImage *imLabel)
{
	int ix, iy, ig;
	int width, height;
	unsigned char *pLabel = (unsigned char *)imLabel->imageData;
	unsigned char *pDepth = (unsigned char *)IplImage_depth->imageData;
	int yOff;
	int Sum;
	int SumCnt;
	int Avg;
	int Distance;

	int Hist[256];
	int i;
	int j;
	int y;
	int Pos;

	width  = imLabel->width;
	height = imLabel->height;

	for( i=0; i<256; i++ )
	{
		Hist[i] = 0;
	}		

	Sum    = 0;
	SumCnt = 0;
	for(iy=0; iy<height; iy++)
	{
		yOff = iy*width;

		for(ix=0; ix<width; ix++)
		{
			if(pLabel[yOff + ix] == 255)
			{

				//pDepth[yOff + ix] = 255;
				Pos = yOff + ix;

				if( pDepth[Pos] > 10 && pDepth[Pos] < 230 )
				{
					//Sum += pDepth[yOff + ix];
					//SumCnt++;

					Hist[ pDepth[Pos] ]++;

					pDepth[yOff + ix] = 255;

				}
			}
		}
	}
	
	int Cnt;
	int CntSum;

	// 히스토그램을 그린다.
	/*
	Cnt    = 0;
	CntSum = 0;
	for( i=0; i<256; i++ )
	{
		for( y=0; y<Hist[i]/3; y++ )
		{
			pDepth[y*width + i] = 0;
		}
	}
	*/

	Cnt    = 0;
	CntSum = 0;
	Sum    = 0;
	SumCnt = 0;
	for( i=10; i<230; i++ )
	{

		CntSum += Hist[i];

		if( CntSum > 5 )
		{
			for( j=i; j<i+10; j++ )
			{
				Sum    += Hist[j] * i;
				SumCnt += Hist[j];  
			}
			break;
		}
	}


	if( SumCnt > 0 )
	{
		Avg = Sum/SumCnt;
		Distance = Avg * DEPTH_MAX_MM / 255;
	}
	else
	{
		Distance = 0;
	}




	return Distance;
}


































int OpenNI_Capture( THREAD_OBJ *pArg )
{
	Status rc;
	int readyStream;

	start_point = clock();

	
	while ( pArg->Thread_Stop == FALSE )
	{
		readyStream = -1;

		rc = OpenNI::waitForAnyStream(streams, 2, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			return FALSE;
		}

		
		switch (readyStream)
		{
		case 0:
			// Depth
			rc = depth.readFrame(&frame_depth);
			CapturedFlag |= (1<<0);

			if (rc != STATUS_OK)
			{
				printf("Read depth failed!\n%s\n", OpenNI::getExtendedError());
				continue;
			}			

			frame_ptr = &frame_depth;
			break;
		case 1:
			// Color
			rc = color.readFrame(&frame_color);
			CapturedFlag |= (1<<1);
			if (rc != STATUS_OK)
			{
				printf("Read color failed!\n%s\n", OpenNI::getExtendedError());
				continue;
			}			

			frame_ptr = &frame_color;
			break;
		default:
			printf("Unxpected stream\n");
			continue;
			break;
		}
		

		switch (frame_ptr->getVideoMode().getPixelFormat())
		{
		case PIXEL_FORMAT_DEPTH_1_MM:
		case PIXEL_FORMAT_DEPTH_100_UM:
			//rc = depth.readFrame(&frame);
			//printf("Depth Captured %d\n", Cnt++);
			//continue;
			break;

		case PIXEL_FORMAT_RGB888:
			//printf("Color Captured %d\n", Cnt++ );
			//continue;
			break;
		default:
			printf("Unknown format\n");
			continue;
			break;
		}



		if( readyStream == 0 )
		{
			MatImage_depth = cv::Mat(depth.getVideoMode().getResolutionY(),
	                           depth.getVideoMode().getResolutionX(),
	                           CV_16U,
	                           (char *) frame_depth.getData());
	        MatImage_depth.convertTo(MatImage_depth, CV_8U, 255.0/DEPTH_MAX_MM); 

			IplImage_depth->imageData = (char *) MatImage_depth.data;   
			//cv::resize( IplImage_depth, IplImage_depth_resize, IplImage_depth_resize.size(), 0, 0, cv::INTER_LINEAR );
			//cvResize(IplImage_depth, IplImage_depth_resize);
		}
		else
		{
			const openni::RGB888Pixel* imageBuffer = (const openni::RGB888Pixel*)frame_color.getData();

   			MatImage_color.create(frame_color.getHeight(), frame_color.getWidth(), CV_8UC3);
			memcpy( MatImage_color.data, imageBuffer, 3*frame_color.getHeight()*frame_color.getWidth()*sizeof(uint8_t) );

			cv::cvtColor(MatImage_color,MatImage_color,CV_BGR2RGB); //this will put colors right	

			IplImage_color->imageData = (char *) MatImage_color.data;  
			//cv::resize( IplImage_color, IplImage_color_resize, IplImage_clor_resize.size(), 0, 0, cv::INTER_LINEAR );			
			//cvResize(IplImage_color, IplImage_color_resize);
		}


		if( CapturedFlag == 0x03 )
		{	
			//Object_Tracking(IplImage_color, labelImg, segmentated, morphKernel);

			//cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth);  
			//cvSaveImage("/mnt/ramdisk/color/color.jpg",IplImage_color);  

			//imwrite("/mnt/ramdisk/depth/depth.jpg",MatImage_depth);      
			//imwrite("/mnt/ramdisk/color/color.jpg",MatImage_color); 

			CapturedFlag = 0;
			//Sleep(200);	

			end_point_frame = clock();
			process_time_frame = ((double)(end_point_frame - start_point_frame)/(CLOCKS_PER_SEC/1000));

			if( Lib_Vision_Debug == 1 )
			{
				/*
				printf(" %3.1f frames, D : %03.1f, C : %03.1f \n", 1000./process_time_frame,
																   1000./process_time_depth,
																   1000./process_time_color );
				*/
			}

			start_point_frame = clock();

			break;
		}


	}

	return TRUE;
}




int OpenNI_Init(void)
{
	Status rc;


	//-- OpenNI 초기화 
	//
	rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}

	
	rc = device.open(ANY_DEVICE);
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}


	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
			return 3;
		}
	}

	if (device.getSensorInfo(SENSOR_COLOR) != NULL)
	{
		rc = color.create(device, SENSOR_COLOR);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
		}
	}

	device.setDepthColorSyncEnabled(true);


	//-- Start 
	//
	VideoMode mode_depth = depth.getVideoMode();

    mode_depth.setResolution(DEPTH_IMG_WIDTH, DEPTH_IMG_HEIGHT);
	//mode_depth.setFps(DEPTH_IMG_FPS);
    depth.setVideoMode(mode_depth);
    depth.setMirroringEnabled(false);
      
	rc = depth.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}

	VideoMode mode_color = color.getVideoMode();

    mode_color.setResolution(COLOR_IMG_WIDTH, COLOR_IMG_HEIGHT);
	//mode_color.setFps(COLOR_IMG_FPS);
    color.setVideoMode(mode_color);
    color.setMirroringEnabled(false);

 	
	rc = color.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}
	


	IplImage_depth = cvCreateImage(cvSize(DEPTH_IMG_WIDTH,DEPTH_IMG_HEIGHT), IPL_DEPTH_8U, 1);
	IplImage_color = cvCreateImage(cvSize(COLOR_IMG_WIDTH,COLOR_IMG_HEIGHT), IPL_DEPTH_8U, 3);
	IplImage_depth_resize = cvCreateImage(cvSize(DEPTH_IMG_WIDTH/2,DEPTH_IMG_HEIGHT/2), IPL_DEPTH_8U, 1);
	IplImage_color_resize = cvCreateImage(cvSize(COLOR_IMG_WIDTH/2,COLOR_IMG_HEIGHT/2), IPL_DEPTH_8U, 3);


	CapturedFlag = 0;

	return 0;
}





int OpenNI_Close(void)
{
	depth.stop();
	depth.destroy();
	color.stop();
	color.destroy();


	device.close();
	OpenNI::shutdown();

	// OpenCV 종료
	//
	cvReleaseImage(&IplImage_depth);
	cvReleaseImage(&IplImage_color);	
	cvReleaseImage(&IplImage_depth_resize);
	cvReleaseImage(&IplImage_color_resize);
	return 0;
}





#if 0
void calculateHistogram(float* pHistogram, int histogramSize, const openni::VideoFrameRef& frame)
{
	const openni::DepthPixel* pDepth = (const openni::DepthPixel*)frame.getData();
	// Calculate the accumulative histogram (the yellow display...)
	memset(pHistogram, 0, histogramSize*sizeof(float));
	int restOfRow = frame.getStrideInBytes() / sizeof(openni::DepthPixel) - frame.getWidth();
	int height = frame.getHeight();
	int width = frame.getWidth();

	unsigned int nNumberOfPoints = 0;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				pHistogram[*pDepth]++;
				nNumberOfPoints++;
			}
		}
		pDepth += restOfRow;
	}
	for (int nIndex=1; nIndex<histogramSize; nIndex++)
	{
		pHistogram[nIndex] += pHistogram[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<histogramSize; nIndex++)
		{
			pHistogram[nIndex] = (256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
		}
	}
}
#endif