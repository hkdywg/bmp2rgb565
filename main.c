#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short  int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;

/*
    第一部分    位图文件头
该结构的长度是固定的，为14个字节，各个域的依次如下：
    2byte   ：文件类型，必须是0x4d42，即字符串"BM"。
    4byte   ：整个文件大小
    4byte   ：保留字，为0
    4byte   ：从文件头到实际的位图图像数据的偏移字节数。
*/
struct bmp_filehead
{
    uint16_t bmp_type;
    uint32_t bmp_size;
    uint32_t bmp_Reserved;
    uint32_t bmp_offbits;
};

/*
第二部分    位图信息头
该结构的长度也是固定的，为40个字节，各个域的依次说明如下：
    4byte   ：本结构的长度，值为40
    4byte   ：图像的宽度是多少象素。
    4byte   ：图像的高度是多少象素。
    2Byte   ：必须是1。
    2Byte   ：表示颜色时用到的位数，常用的值为1(黑白二色图)、4(16色图)、8(256色图)、24(真彩色图)。
    4byte   ：指定位图是否压缩，有效值为BI_RGB，BI_RLE8，BI_RLE4，BI_BITFIELDS。Windows位图可采用RLE4和RLE8的压缩格式，BI_RGB表示不压缩。
    4byte   ：指定实际的位图图像数据占用的字节数，可用以下的公式计算出来：
     图像数据 = Width' * Height * 表示每个象素颜色占用的byte数(即颜色位数/8,24bit图为3，256色为1）
     要注意的是：上述公式中的biWidth'必须是4的整数倍(不是biWidth，而是大于或等于biWidth的最小4的整数倍)。
     如果biCompression为BI_RGB，则该项可能为0。
    4byte   ：目标设备的水平分辨率。
    4byte   ：目标设备的垂直分辨率。
    4byte   ：本图像实际用到的颜色数，如果该值为0，则用到的颜色数为2的(颜色位数)次幂,如颜色位数为8，2^8=256,即256色的位图
    4byte   ：指定本图像中重要的颜色数，如果该值为0，则认为所有的颜色都是重要的。
*/
struct bmp_head
{
    uint32_t bm_poffbits;
    uint32_t bmp_width;
    uint32_t bmp_hight;
    uint16_t bmp_lans;
    uint16_t bitcount;
    uint32_t bmp_compression;
    uint32_t image_size;

    uint32_t xpels;
    uint32_t ypels;
    uint32_t clrused;
    uint32_t clrimportant;
};


struct BGR_data
{
    uint8_t B_data;
    uint8_t G_data;
    uint8_t R_data;
};

int main(int argc, char *argv[])
{
    FILE *input_fp, *output_fp;
    struct bmp_filehead file_head;
    struct bmp_head head;
    struct BGR_data bmp_data[185*145];
    
    input_fp = fopen(argv[1],"r");
    if(input_fp != NULL)
    {
        fseek(input_fp, 0, 0);
        fread(&file_head, sizeof(file_head), 1, input_fp);

        fseek(input_fp, 14, 0);
        fread(&head, sizeof(head), 1, input_fp);
        printf("head.bmp_hight = %d, head.bmp_width = %d\n", head.bmp_hight, head.bmp_width);
        fseek(input_fp, 54, 0);
        //fread(bmp_data, sizeof(struct BGR_data), (head.bmp_width * head.bmp_hight), input_fp);    
        fread(bmp_data, 3, (head.bmp_width * head.bmp_hight), input_fp);    
    }
    fclose(input_fp);

    output_fp = fopen(argv[2], "w+");
    if(output_fp != NULL)
    {
        for(uint32_t i = 0; i < (head.bmp_hight * head.bmp_width); i++)
        {
            uint16_t data = (uint16_t)((bmp_data[i].R_data>>3)<<11) + (uint16_t)((bmp_data[i].G_data>>2)<<5) + (uint16_t)((bmp_data[i].B_data>>3));
            fprintf(output_fp, "0x%x, 0x%x, ", (data>>8), (data&0xff));
            //fprintf(output_fp, "0x%x, 0x%x,0x%x, ", (bmp_data[i].R_data), (bmp_data[i].G_data), bmp_data[i].B_data);
            if(i%16 == 0)
                fprintf(output_fp, "%s", "\n");
            //fprintf(output_fp, "%x%x", (data&0xff), (data>>8));
        }
    }
    fclose(output_fp);

    return 0;
}

