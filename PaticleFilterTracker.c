/*
A SIMPLE PARTICLETRACKER DEMO
Author��SUNWENTAO  FROM XMU

Because the time is so limited, and the language(Chinese) has some error to display. I just give up reasoning the detail of my code. In my mind, my code is very easy to understand.
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
#include<string.h>
#define M 800*3		//Image's max rows
#define N 1000		//Image's max cols
#define IMAGE_NUMBER 100
#define SCALECHANGE 2
#define MAXMODEL 4 
#define MODELSIZE 200
typedef unsigned char BYTE;

typedef struct tagPICSIZE  {
    int width;//Image's width
    int height;//Image's height
    int length;//Image's total bytes
    int offset;//前三部分字节�?
}PICSIZE;




//定义全局数组用于存储后续图像数据信息
BYTE PictureIn[M*N];//读文件的
BYTE PictureOut[M*N];//写入文件�?
FILE *fInput;   //输入文件
FILE *fOutput;//输出文件
FILE *nameInput; // name file
FILE *nameOutput; //
BITMAPFILEHEADER FileHeader; //图像第一部分存储变量
BITMAPINFOHEADER InfoHeader;//图像第二部分存储变量
RGBQUAD Quad[256];//调色板存储的数组
PICSIZE PiSize;  //存储图像大小信息的结构体变量

// Define some parameters for Particle Filter tracking
//  in this demo. We use two model template to get the tracking object
//	The template max size is [50 50]
typedef struct onetemplate
{
	/* data */
	int data[MODELSIZE][MODELSIZE];
	int height;
	int width;
	double weight;
	double affine; //scale or shrink

}onetemplate;
typedef struct Template
{
	/* data */
	onetemplate All_Template[MAXMODEL];
	int num;
	
}Template;
typedef struct currentPos
{
	/* data */
	int row;
	int col;
}currentPos;
typedef struct ParticleFilterParemters
{
	/* data */
	int numOfParticles;
	int searchRange;
}ParticleFilterParemters;
typedef struct Particles
{
	/* data */
	int num;
	int pos[500][2];
}Particles;



int model[MODELSIZE][MODELSIZE];	

int main()
{
	int i,j,k;
	char FileName[50];
	
	void ColorToGray();
	void tracker_initialize(ParticleFilterParemters *PFParameters,currentPos *Pos, Template *temp);
	void drawRect(int row,int col,int height,int width);
	void judge(Particles *ptcls, Template *temp,currentPos *Pos);
	void gen_particles(ParticleFilterParemters *PFParameters,currentPos *Pos ,Particles *ptcls );
	void pOutput(char OutName[20],int length);
	// *** **** define some variables for the tracker
	ParticleFilterParemters PFParemters;
	currentPos Pos;
	Template Temp;
	Particles Ptcls;
	nameInput=fopen("car1.txt","rb");
	nameOutput=fopen("car1out.txt" ,"rb");
	for(i=0;i<IMAGE_NUMBER;i++)
	{
		// first step read the images from files
		fscanf(nameInput,"%s",FileName);
		fInput=fopen(FileName,"rb");
		fread(&FileHeader,sizeof(BITMAPFILEHEADER),1,fInput);
        fread(&InfoHeader,sizeof(BITMAPINFOHEADER),1,fInput);
		PiSize.width=InfoHeader.biWidth;
		PiSize.height=InfoHeader.biHeight;
		PiSize.offset=FileHeader.bfOffBits;
		if(InfoHeader.biBitCount==24)PiSize.width=PiSize.width*3;            //判断是否是真彩图，并读取第三部分调色�? Palette 存放在Quad数组�?
			else fread(Quad,sizeof(RGBQUAD),pow(2,InfoHeader.biBitCount),fInput);
		PiSize.length=PiSize.width*PiSize.height;

		fseek(fInput,PiSize.offset,0);                                      //读取ImageDate位图数据
		fread(PictureIn,sizeof(BYTE),PiSize.length,fInput);
		
		ColorToGray();
		// second step to set some parameters
		if(i==0)
		{
			tracker_initialize(&PFParemters,&Pos,&Temp);
		}
		else
		{
			gen_particles(&PFParemters,&Pos,&Ptcls);
			judge(&Ptcls,&Temp,&Pos);
			//drawRect(Pos.row,Pos.col,)
		}
		


		


		fscanf(nameOutput,"%s",FileName);
		pOutput(FileName,PiSize.length);
		

	}
	
	

	
		return 0;
}



void tracker_initialize(ParticleFilterParemters *PFParameters,currentPos *Pos, Template *temp)
{
	void drawRect(int row,int col,int height,int width);
	int i,j;
	PFParameters->numOfParticles=150;
	PFParameters->searchRange = 100;
	
	temp->num = 1;
	Pos->col = 23;
	Pos->row=240-88-55;
	temp->All_Template[0].width= 66;
	temp->All_Template[0].height=55;
	
	
	temp->All_Template[0].affine=1.0;
	temp->All_Template[0].weight=1.0;
	for(j=0;j<temp->All_Template[0].height;j++)
	{
	  for(i=0;i<temp->All_Template[0].width;i++)
	  {
		  temp->All_Template[0].data[j][i] = PictureOut[(j+Pos->row)*PiSize.width+(i+Pos->col)*3];
	  }
	}
	drawRect(Pos->row,Pos->col,temp->All_Template[0].height,temp->All_Template[0].width);

}

/* In Zt-1 last frame position of the tracking object, I generate some particls*/
void gen_particles(ParticleFilterParemters *PFParameters,currentPos *Pos ,Particles *ptcls )
{
	int num,range;
	int i,j,c,step,p1;
	num = PFParameters->numOfParticles;
	range =PFParameters->searchRange;
	c=sqrt(num);
	step =range/c;
	p1=0;
	if(step<1)
	{
		step =1;
	}

	for(i=0;i<c;i++)
	{
		for(j=0;j<c;j++)
		{
			ptcls->pos[p1][0]=(i-c/2)*step+Pos->row;
			ptcls->pos[p1][1]=(j-c/2)*step+Pos->col;
			p1++;
		}
	}
	for(i=0;i<p1;i++)
	{
		if(ptcls->pos[i][0]<0)ptcls->pos[i][0]=0;
		if(ptcls->pos[i][0]>PiSize.height)ptcls->pos[i][0]=PiSize.height-1;

		if(ptcls->pos[i][1]<0)ptcls->pos[i][1]= 0;
		if(ptcls->pos[i][1]>PiSize.width/3)ptcls->pos[i][1]=PiSize.width/3-1;
	}
	ptcls->num =p1;

}

/*
This function is to judge each particle and get the weight or the most likely particle as the tracking result
*/
void judge(Particles *ptcls, Template *temp,currentPos *Pos)
{
	void drawRect(int row,int col,int height,int width);
	void compare(onetemplate *temp, int pos_r,int pos_c,double *los);
	void update(Template *temp,currentPos *Pos);
	int i,j,k;
	int pos_r,pos_c;
	int predict_r,predict_c;
	double min_loss=-1,loss,loss2;
	
	for(k=0;k<ptcls->num;k++)
	{
		loss =100;
		loss2=0;
		pos_r = ptcls->pos[k][0];
		pos_c = ptcls->pos[k][1];
		
		for(i=0;i<temp->num;i++)
		{
			compare(&(temp->All_Template[i]),pos_r,pos_c,&loss);
			loss2+=loss*temp->All_Template[i].weight;
		}
		
		if((min_loss==-1)||(min_loss>loss2))
		{
			min_loss=loss2;
			predict_r=pos_r;
			predict_c=pos_c;
		}	
		
	}
	Pos->col=predict_c;
	Pos->row=predict_r;
	
	
	update(temp,Pos);
	//drawRect(predict_r,predict_c,temp->All_Template[0].height,temp->All_Template[0].width);
	
	
}

/*
This function is to  draw the square bounding box for the object
*/
void drawRect(int row,int col,int height,int width)
{
	int i,j;
	for(j=row;j<row+height;j++)
	{
		PictureOut[j*PiSize.width+col*3]=0;//将RGB三通道 全部赋值为 同一灰度�?实现照片变黑�?
		PictureOut[j*PiSize.width+col*3+1]=0;
		PictureOut[j*PiSize.width+col*3+2]=0;
		PictureOut[j*PiSize.width+(col+width-1)*3]=0;//将RGB三通道 全部赋值为 同一灰度�?实现照片变黑�?
		PictureOut[j*PiSize.width+(col+width-1)*3+1]=0;
		PictureOut[j*PiSize.width+(col+width-1)*3+2]=0;
	}
	for(i=col;i<col+width;i++)  //PiSize.width/3
	{
		PictureOut[row*PiSize.width+i*3]=0;//将RGB三通道 全部赋值为 同一灰度�?实现照片变黑�?
		PictureOut[row*PiSize.width+i*3+1]=0;
		PictureOut[row*PiSize.width+i*3+2]=0;

		PictureOut[(row+height)*PiSize.width+i*3]=0;//将RGB三通道 全部赋值为 同一灰度�?实现照片变黑�?
		PictureOut[(row+height)*PiSize.width+i*3+1]=0;
		PictureOut[(row+height)*PiSize.width+i*3+2]=0;
	}
	
	
}

/*
change the template size,and compare different size template with the particle
*/
void compare(onetemplate *temp, int pos_r,int pos_c, double *los)
{
	void scaleChange_width(onetemplate *temp,int assi);
	void scaleChange_height(onetemplate *temp,int assi);
	int i,j,kk;
	
	double loss1,loss2,loss3;
	double assi;
	int num=0;
	loss3 =1000;
	
	for(kk=1;kk<SCALECHANGE;kk++)
	{
		num=0;
		loss1=0;
		loss2=0;
		scaleChange_width(temp,kk);
		for(i=0;i<temp->height;i++)
		for(j=0;j<temp->width*kk;j++)
		{
			int co;
			num++;
			co=abs(PictureOut[(i+pos_r)*PiSize.width+(j+pos_c)*3]-model[i][j]);
			loss1+=co; 
		}
		loss1 = loss1/num;
		
		num=0;
		scaleChange_height(temp,kk);
		for(i=0;i<temp->height*kk;i++)
		for(j=0;j<temp->width;j++)
		{
			num++;
			loss2+=abs(PictureOut[(i+pos_r)*PiSize.width+(j+pos_c)*3]-model[i][j]);
		}
		loss2=loss2/num;
		if(loss3>loss1)loss3=loss1;
		if(loss3>loss2)loss3=loss2;
	}
	
	*los =loss3;
}

void scaleChange_width(onetemplate *temp,int assi)
{
	double width,height;
	int i,j;
	width = temp->width*assi;
	height = temp->height;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			if(j%assi == 0)
			{
				model[i][j]= temp->data[i][j/assi];
			}else
			{
				model[i][j]=temp->data[i][(j-1)/assi] + temp->data[i][(j+1)/assi];
				model[i][j]=model[i][j]/2;
			}
			
		}
	}
}
void scaleChange_height(onetemplate *temp,int assi)
{
	double width,height;
	int i,j;
	width = temp->width;
	height = temp->height*assi;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			if(i%assi == 0)
			{
				model[i][j]=temp->data[i/assi][j];
			}else
			{
				model[i][j]=temp->data[(i-1)/assi][j] + temp->data[(i+1)/assi][j];
				model[i][j]=model[i][j]/2;
			}
			
		}
	}
}

/*
update the object model
*/
void update(Template *temp,currentPos *Pos)
{
	void updateTemplate(onetemplate *temp,int pos_r,int pos_c,int height,int width);
	void compare2(onetemplate *temp,int *scale,int pos_r,int pos_c,double *los);
	int pos_row,pos_col,ass;
	int height,width;
	int i,j;
	int scale,min_scale,min_model,max_model,max_scale;
	double loss,min_loss,max_loss;
	
	pos_row=Pos->row;
	pos_col=Pos->col;
	min_loss=-1;
	max_loss=0;
	min_scale=0;
	max_scale=0;
	if(temp->num<MAXMODEL)
	{
		for(i=0;i<temp->num;i++)
		{
			compare2(&(temp->All_Template[i]),&scale,pos_row,pos_col,&loss);
			if((min_loss>loss)||(min_loss<0))
			{
				min_loss =loss;
				min_scale = scale;
				min_model = i;
			}
		}
		
		if(min_scale%2 ==0)
		{ 
			ass =scale/10;
			height = temp->All_Template[min_model].height;
			width = temp->All_Template[min_model].width;
			width = width*ass;
			printf("Update a new model,in the %d place\n",temp->num+1);
			updateTemplate(&(temp->All_Template[temp->num]),pos_row,pos_col,height,width);
			temp->num++;
			drawRect(pos_row,pos_col,height,width);
		}
		else
		{
			ass = scale/10;
			height = temp->All_Template[min_model].height;
			width = temp->All_Template[min_model].width;
			height = height*ass;
			updateTemplate(&(temp->All_Template[temp->num]),pos_row,pos_col,height,width);
			temp->num++;
			drawRect(pos_row,pos_col,height,width);
		}
		
	}
	else
	{
		
		for(i=1;i<temp->num;i++) // �̶�  ��һ��ģ�� 
		{
			compare2(&(temp->All_Template[i]),&scale,pos_row,pos_col,&loss);
			if((min_loss>loss)||(min_loss<0))
			{
				min_loss =loss;
				min_scale = scale;
				min_model = i;
			}
			if(max_loss<loss)
			{
				max_loss = loss;
				max_scale=scale;
				max_model=i;
			}
		}
		if(max_loss-min_loss>10)
		{
			if(min_scale%2 ==0)
			{
			ass =scale/10;
			printf("Replace %d model in WIDTH_SCALE=%d\n",max_model+1,ass);
			updateTemplate(&(temp->All_Template[max_model]),pos_row,pos_col,temp->All_Template[min_model].height,temp->All_Template[min_model].width*ass);
			}
			else
			{
			ass = scale/10;
			printf("Replace %d model in HEIGHT_SCALE=%d\n",max_model+1,ass);
			updateTemplate(&(temp->All_Template[max_model]),pos_row,pos_col,temp->All_Template[min_model].height*ass,temp->All_Template[min_model].width);
			}	
		}
		
		if(min_scale%2 ==0)
		{
			ass=min_scale/10;
			drawRect(pos_row,pos_col,temp->All_Template[min_model].height,temp->All_Template[min_model].width*ass);
		}
		else
		{
			ass=min_scale/10;
			drawRect(pos_row,pos_col,temp->All_Template[min_model].height*ass,temp->All_Template[min_model].width);
		}
		
		
	
	}
	
}

void compare2(onetemplate *temp,int *scale,int pos_r,int pos_c,double *los)
{
	void scaleChange_width(onetemplate *temp,int assi);
	void scaleChange_height(onetemplate *temp,int assi);
	int i,j,kk;
	double loss1,loss2,loss3;
	int num=0;
	loss3 =1000;
	for(kk=1;kk<SCALECHANGE;kk++)
	{
		num=0;
		loss1=0;
		loss2=0;
		scaleChange_width(temp,kk);
		for(i=0;i<temp->height;i++)
		for(j=0;j<temp->width*kk;j++)
		{
			num++;
			loss1+=abs(PictureOut[(i+pos_r)*PiSize.width+(j+pos_c)*3]-model[i][j]);
		}
		loss1 = loss1/num;
		
		num=0;
		scaleChange_height(temp,kk);
		for(i=0;i<temp->height*kk;i++)
		for(j=0;j<temp->width;j++)
		{
			num++;
			loss2+=abs(PictureOut[(i+pos_r)*PiSize.width+(j+pos_c)*3]-model[i][j]);
		}
		loss2=loss2/num;
		
		if(loss3>loss1)
		{
			loss3=loss1;
			*scale=kk*10+0;
		}
		if(loss3>loss2)
		{
			loss3=loss2;
			*scale=kk*10+1;
		}
	}
	
	*los =loss3;
}

void updateTemplate(onetemplate *temp,int pos_r,int pos_c,int height,int width)
{
	int i,j;
	
	for(i=0;i<height;i++)
	{
	  for(j=0;j<width;j++)
	  {
	  	temp->data[i][j]= PictureOut[(i+pos_r)*PiSize.width+(j+pos_c)*3];
	  }
	}
	temp->height=height;
	temp->width= width;
	temp->weight=1.0/MAXMODEL;
	
}
/*
pInput() 过程功能是读取指定位置的BMP图像文件
S1先读�?BITMAPFILEHEADER �? BITMAPINFOHEADER 两部�?
分别存放在FileHeader �?InfoHeader 两个结构体变量里�?
S2 算出图像色彩位数是否�?4位真彩图�?并确定第四部分ImageDate位图数据的字节数
*/
void pInput()  //BMP图像的读入部�?
{
    	char FileName[20];
	    //printf("please enter the file name\n");
	    //scanf("%s",FileName);
        //while((fInput=fopen(FileName,"rb"))==NULL)
       //{
        //   printf("sorry,we can not open this file,please Re Input\n");
        //   scanf("%s",FileName);
        //}
		

       fread(&FileHeader,sizeof(BITMAPFILEHEADER),1,fInput);
       fread(&InfoHeader,sizeof(BITMAPINFOHEADER),1,fInput);
	PiSize.width=InfoHeader.biWidth;
	PiSize.height=InfoHeader.biHeight;
	PiSize.offset=FileHeader.bfOffBits;
    if(InfoHeader.biBitCount==24)PiSize.width=PiSize.width*3;            //判断是否是真彩图，并读取第三部分调色�? Palette 存放在Quad数组�?
        else fread(Quad,sizeof(RGBQUAD),pow(2,InfoHeader.biBitCount),fInput);
    PiSize.length=PiSize.width*PiSize.height;

    fseek(fInput,PiSize.offset,0);                                      //读取ImageDate位图数据
    fread(PictureIn,sizeof(BYTE),PiSize.length,fInput);
}




/*
生成变化完成的BMP格式图像文件
*/
void pOutput(char OutName[20],int length)
{
    while((fOutput=fopen(OutName,"wb"))==NULL)
    {
        printf("sorry we can not build a file,please check your computer\n");
        return;
    }
	fwrite(&FileHeader,sizeof(BITMAPFILEHEADER),1,fOutput);
	fwrite(&InfoHeader,sizeof(BITMAPINFOHEADER),1,fOutput);
	if(InfoHeader.biBitCount!=24)fwrite(Quad,sizeof(RGBQUAD),pow(2,InfoHeader.biBitCount),fOutput);
	fwrite(PictureOut,sizeof(BYTE),length,fOutput);
	fclose(fOutput);
	fclose(fInput);
	printf("result is in %s. It has been processed succesfully\n",OutName);
}




/*
灰度图变�?
对真彩色：算每个像素点的灰度值求出，然后使RGB三色全部赋值为此灰�?
对非真彩色图：直接修改对调色板RGB三色的数值为三者和的平均数
*/
void ColorToGray()
{
    void pOutput();
    int i,j;
    //pInput();
    if(InfoHeader.biBitCount==24)
	for(j=0;j<PiSize.height;j++)
	{
	  for(i=0;i<PiSize.width/3;i++)
	  {
		int sum;//求每个像素点的灰度�?
		sum=PictureIn[j*PiSize.width+i*3]*0.299+PictureIn[j*PiSize.width+i*3+1]*0.578+PictureIn[j*PiSize.width+i*3+2]*0.114;
		//sum =PictureIn[j*PiSize.width+i*3+2];
	    PictureOut[j*PiSize.width+i*3]=sum;//将RGB三通道 全部赋值为 同一灰度�?实现照片变黑�?
		PictureOut[j*PiSize.width+i*3+1]=sum;
		PictureOut[j*PiSize.width+i*3+2]=sum;
	  }
	}
	else
    {
        for(i=0;i<PiSize.length;i++)PictureOut[i]=PictureIn[i];
        BYTE sum;
        for(i=0;i<pow(2,InfoHeader.biBitCount);i++)//将调色板不同颜色转化为灰�?
        {

           sum=(Quad[i].rgbBlue+Quad[i].rgbGreen+Quad[i].rgbRed)/3;
            Quad[i].rgbBlue=sum;
            Quad[i].rgbGreen=sum;
            Quad[i].rgbRed=sum;
        }
    }
    //pOutput("color to grey.bmp",PiSize.length);

}






