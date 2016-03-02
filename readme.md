##基于RSS指纹与惯性测量的混合定位系统

###定位场景
![](https://raw.githubusercontent.com/jiangqideng/resources/master/RSS_Inertial.jpg)


###文件说明

+ 包含十个无线传感器网络结点的代码，硬件使用的是FLY2000模块，主控为51单片机STC10L08XE，惯性传感器使用GY-85模块。
+ 文件夹AP2-AP9是部署在定位区域的8个结点。
+ AP10直接连到PC机上，通过串口与PC机通信。
+ 这些文件不包括上位机matlab处理程序，指纹库和定位算法在matlab中进行。

###细节

这个系统属于本科毕业设计的一部分，详细原理见“本科毕业论文.docx”。
