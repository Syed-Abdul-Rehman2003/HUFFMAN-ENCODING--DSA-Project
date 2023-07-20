#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
using namespace std;

class PixelFreq{
	public:
	int pix;
	double freq;
	PixelFreq *left,*right;
	string code="";
};
class HuffCode{
	public:
	int pix,arrLoc;
	double freq;
};

class ImageCompression{
	int **image;
	int *freq;
	char file_name[6] ={'2','4','.','b','m','p'};
	string coded;
	int leafNodes;
	
	//retrived data 
	string *ret_code;
	int *ret_pixels;	
	
	
	//for reading and writing image
	DWORD write=0;
	HANDLE file;
	BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfo;
    RGBTRIPLE *rgbImg;
	
	//for huffman coding
	PixelFreq *pixel_freq;
	HuffCode *huffcode;
	
	public:		
		void compressImage(){
			readImage();
			getPixelFreq();
			getLeafNodeCount();
			initialize_pix_freq();
			sortingHuffCodes();
			buildHuffmanTree();
			assignCode();
			codeImage();
			storeCompressed();
		}
		void decompressImage(){
			retriveCompressed();
			decompressToPixels();
			writeBack();
		}
		
		void print(){
			for(int i=0;i<fileInfo.biHeight;i++){
				for(int j=0;j<fileInfo.biWidth*3;j++){
					cout<<image[i][j]<<" ";
				}cout<<endl;
			}
		}
		void readImage(){
			FILE *image_file;			
			image_file=fopen(file_name,"rb");

			if(image_file==NULL){
				cout<<"CANNOT OPEN FILE"<<endl;
			}else{
				
				// getting data
//				fseek(image_file,bmpdata_offset,SEEK_SET);
				fread(&fileHeader.bfType,sizeof(fileHeader.bfType),1,image_file);
				fseek(image_file,2,SEEK_SET);
				fread(&fileHeader.bfSize,sizeof(fileHeader.bfSize),1,image_file);
				fseek(image_file,6,SEEK_SET);
				fread(&fileHeader.bfReserved1,sizeof(fileHeader.bfReserved1),1,image_file);
				fseek(image_file,8,SEEK_SET);
				fread(&fileHeader.bfReserved2,sizeof(fileHeader.bfReserved2),1,image_file);
				fseek(image_file,10,SEEK_SET);
				fread(&fileHeader.bfOffBits,sizeof(fileHeader.bfOffBits),1,image_file);
				
				fseek(image_file,14,SEEK_SET);
				fread(&fileInfo.biSize,sizeof(fileInfo.biSize),1,image_file);
				fseek(image_file,18,SEEK_SET);
				fread(&fileInfo.biWidth,sizeof(fileInfo.biWidth),1,image_file);
				fseek(image_file,22,SEEK_SET);
				fread(&fileInfo.biHeight,sizeof(fileInfo.biHeight),1,image_file);
				fseek(image_file,26,SEEK_SET);
				fread(&fileInfo.biPlanes,sizeof(fileInfo.biPlanes),1,image_file);
				fseek(image_file,28,SEEK_SET);
				fread(&fileInfo.biBitCount,sizeof(fileInfo.biBitCount),1,image_file);
				fseek(image_file,30,SEEK_SET);
				fread(&fileInfo.biCompression,sizeof(fileInfo.biCompression),1,image_file);
				fseek(image_file,34,SEEK_SET);
				fread(&fileInfo.biSizeImage,sizeof(fileInfo.biSizeImage),1,image_file);
				fseek(image_file,38,SEEK_SET);
				fread(&fileInfo.biXPelsPerMeter,sizeof(fileInfo.biXPelsPerMeter),1,image_file);
				fseek(image_file,42,SEEK_SET);
				fread(&fileInfo.biYPelsPerMeter,sizeof(fileInfo.biYPelsPerMeter),1,image_file);
				fseek(image_file,46,SEEK_SET);
				fread(&fileInfo.biClrUsed,sizeof(fileInfo.biClrUsed),1,image_file);
				fseek(image_file,50,SEEK_SET);
				fread(&fileInfo.biClrImportant,sizeof(fileInfo.biClrImportant),1,image_file);
				
				image = new int*[fileInfo.biHeight];
				for(int i=0;i<fileInfo.biHeight;i++){
					image[i]=new int[fileInfo.biWidth*3];
				}
				
				rgbImg =new RGBTRIPLE[fileInfo.biHeight*fileInfo.biWidth];			
				for(int i=0;i<fileInfo.biHeight*fileInfo.biWidth;i++){	
						fread(&rgbImg[i],3,1,image_file);				
				}
						
				fclose(image_file);
				
			
				int k=0;
				for(int i=0;i<fileInfo.biHeight;i++)
					for(int j=0;j<fileInfo.biWidth*3;j+=3){
						image[i][j] = rgbImg[k].rgbtBlue;
						image[i][j+1] = rgbImg[k].rgbtGreen;
						image[i][j+2] = rgbImg[k].rgbtRed;
						k++;
					}			
			}
		}
		void getPixelFreq(){
			freq= new int[256];
			for(int i=0;i<256;i++) freq[i]=0;
			for(int i=0;i<fileInfo.biHeight;i++)
				for(int j=0;j<fileInfo.biWidth;j++)
					freq[image[i][j]]++;
		}
		int getLeafNodeCount(){
			int count=0;
			for(int i=0;i<256;i++) 
				if(freq[i]!=0) 
					count++;
			leafNodes = count;	
			return count;
		}
		
		void initialize_pix_freq(){			
			pixel_freq = new PixelFreq [2*leafNodes-1];
			huffcode = new HuffCode[leafNodes];			
			int j = 0;
			for(int i=0;i<256;i++){
				if(freq[i]!=0){
					
					huffcode[j].pix = i;
					pixel_freq[j].pix = i;
					
					huffcode[j].arrLoc = j;
					
				//	pixel_freq[j].freq = huffcode[j].freq = (double)freq[i]/(double)(height*width);
					pixel_freq[j].freq = huffcode[j].freq = freq[i];
					
					pixel_freq[j].left=NULL;
					pixel_freq[j].right=NULL;
					
					j++;		
				}
			}						
		}
		void sortingHuffCodes(){
			HuffCode temp;
			for(int i=0;i<leafNodes-1;i++){
				for(int j=0;j<leafNodes-1-i;j++){
					if(huffcode[j].freq < huffcode[j+1].freq){
						temp = huffcode[j];						
						huffcode[j] = huffcode[j+1];
						huffcode[j+1]=temp;
					}
				}
			}		
		}
		
		void buildHuffmanTree(){
			int nextNode = leafNodes;
			int sumFreq,sumPixel;
			int dec=0,i=0;
			// summing freq of two nodes with smallest freq
			while(dec<leafNodes-1){
				sumFreq = pixel_freq[huffcode[leafNodes-dec-1].arrLoc].freq + pixel_freq[huffcode[leafNodes-dec-2].arrLoc].freq;
				sumPixel = pixel_freq[huffcode[leafNodes-dec-1].arrLoc].pix + pixel_freq[huffcode[leafNodes-dec-2].arrLoc].pix;
			
				pixel_freq[nextNode].freq = sumFreq;
				pixel_freq[nextNode].pix =	sumPixel;
			
				pixel_freq[nextNode].left = &pixel_freq[huffcode[leafNodes-dec-1].arrLoc];
				pixel_freq[nextNode].right = &pixel_freq[huffcode[leafNodes-dec-2].arrLoc];
			
				i=0;
				while(sumFreq <= huffcode[i].freq) i++;
			
				for(int j=leafNodes-1 ;j>=i;j--){
					if(i==j){
						huffcode[i].freq = sumFreq;
						huffcode[i].pix = sumFreq;
						huffcode[i].arrLoc = nextNode;
					}else{
						huffcode[j] = huffcode[j-1];
					}
				}
				dec++;
				nextNode++;	
			}
			
			
		}
		
		void assignCode(){
			for(int i=2*leafNodes-2;i>=0;i--){
				if(pixel_freq[i].left!=NULL){
					pixel_freq[i].left->code+=pixel_freq[i].code;
					pixel_freq[i].left->code += "0" ;
				}
				if(pixel_freq[i].right!=NULL){
					pixel_freq[i].right->code+=pixel_freq[i].code;
					pixel_freq[i].right->code += "1" ;
				}
			}
		}
		
		void codeImage(){
			int pixel;
			for (int i=0;i<fileInfo.biHeight;i++)
				for(int j=0;j<fileInfo.biWidth*3;j++){
					pixel = image[i][j];
						for(int k=0;k<leafNodes;k++)
							if(pixel_freq[k].pix==pixel)
								coded += pixel_freq[k].code;
				}			
		}
		
		void storeCompressed(){
			fstream image_file;
			image_file.open("info.txt",ios::out);
			image_file.write((char*)&leafNodes,sizeof(int));
			image_file.write((char*)&fileHeader,sizeof(BITMAPFILEHEADER));
			image_file.write((char*)&fileInfo,sizeof(BITMAPINFOHEADER));
			image_file.close();
			
			fstream f1("hfcodes.txt",ios::out|ios::binary);
			for(int i=0;i<leafNodes;i++)
			f1<<pixel_freq[i].code<<"\n";
			
			for(int i=0;i<leafNodes;i++)
			f1<<pixel_freq[i].pix<<"\n";
			f1.close();
			
			
			fstream f("coded.txt",ios::out|ios::binary);
			f<<coded;
			f.close();
		}

		void retriveCompressed(){
			fstream image_file;
			image_file.open("info.txt",ios::in);
			image_file.read((char*)&leafNodes,sizeof(int));
			pixel_freq=new PixelFreq[2*leafNodes-1];
			huffcode=new HuffCode[leafNodes];
			image_file.read((char*)&fileHeader,sizeof(BITMAPFILEHEADER));
			image_file.read((char*)&fileInfo,sizeof(BITMAPINFOHEADER));
			image_file.close();
			
			ret_code = new string[leafNodes];
			ret_pixels = new int[leafNodes];
			fstream f1("hfcodes.txt",ios::in|ios::binary);
			for(int i=0;i<leafNodes;i++){
				getline(f1,ret_code[i]);
			}
			for(int i=0;i<leafNodes;i++){
				f1>>ret_pixels[i];
			}			
			f1.close();
						
			fstream f("coded.txt",ios::in|ios::binary);
			f>>coded;
			f.close();

			// image initlizing for decompression
			image = new int*[fileInfo.biHeight];
			for (int i=0;i<fileInfo.biHeight;i++)
				image[i] = new int[fileInfo.biWidth*3];
			
			
		}
		
		void decompressToPixels(){
			string temp;
			int k=0,l=0;
			for(int i = 0;i<coded.length();i++){
				temp+=coded[i];
				for(int j=0;j<leafNodes;j++){
					if(ret_code[j]==temp){						
						if(l==fileInfo.biWidth*3){
							k++;
							l=0	;
						} 
						image[k][l] = ret_pixels[j];						
						l++;
						temp = "";					
					}
				}
			}
			rgbImg = new RGBTRIPLE[fileInfo.biHeight*fileInfo.biWidth];
			k=0;
			for(int i=0;i<fileInfo.biHeight;i++)
				for(int j=0;j<fileInfo.biWidth*3;j+=3){
					rgbImg[k].rgbtBlue = image[i][j];
					rgbImg[k].rgbtGreen = image[i][j+1];
					rgbImg[k].rgbtRed = image[i][j+2];
					k++;
				}
		}
		
		void writeBack(){
			file = CreateFile("decompressed.bmp",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			WriteFile(file,&fileHeader,sizeof(fileHeader),&write,NULL);
    		WriteFile(file,&fileInfo,sizeof(fileInfo),&write,NULL);
			WriteFile(file, rgbImg, fileInfo.biSizeImage, &write, NULL);
			CloseHandle(file);
		}
		
		void anaylsis(){
			cout<<"BEFORE COMPRESSION SIZE\n HEIGHT * WIDTH * 24(BITS PER PIXEL) = "<<fileInfo.biHeight*fileInfo.biWidth*24<<endl;
			int len=0;
			// huffcode size;
			for(int i=0;i<leafNodes;i++)
				len+= pixel_freq[i].code.length();
				
			len += leafNodes*32;
			cout<<"AFTER COMPRESSION SIZE\n LENGTH OF CODED IMAGE = "<<coded.length()<<" BITS || SIZE OF HUFFCODES = "<<len<<endl;
			cout<<"TOTAL SIZE AFTER COMPRESSION = "<<coded.length()+len<<" BITS"<<endl;
			double perc = 100-((((double)(coded.length()+len))/(double)(fileInfo.biHeight*fileInfo.biWidth*24))*100);
			cout<<"PERCENTACE REDUCTION = "<< perc<<"%"<<endl;
		}	
};

int main(){
	int choice=0;
	ImageCompression img;
	
	while (choice != 4){
		system("cls");
		cout<<"1-COMPRESS\n2-ANAYLSIS\n3-DECOMPRESS\n4-EXIT\nCHOICE	:	";
		cin >>choice;
	
		switch(choice){
			case 1:
				system("cls");
				img.compressImage();
			break;
			case 2:
				system("cls");
				img.anaylsis();
			break;
			case 3:
				system("cls");
				img.decompressImage();
			break;

	}
	system("pause");
	}	
	return 0;
}
