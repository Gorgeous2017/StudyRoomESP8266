@mainpage
# ESP8266 project

## 说明文档
[app_study_room.c](esp8266_centre_NB/doxygen/html/app__study__room_8c.html)   
[app_study_room.h](esp8266_centre_NB/doxygen/html/app__study__room_8h.html)  
[nb_bc35.c](esp8266_centre_NB/doxygen/html/nb__bc35_8c.html)   
[nb_bc35.h](esp8266_centre_NB/doxygen/html/nb__bc35_8h.html)  
[main.c](esp8266_centre_NB/doxygen/html/main_8c.html)  
[main.h](esp8266_centre_NB/doxygen/html/main_8h.html)  


## Develop Log
### NB模组通讯时波特率的问题

NB模组接收发送的波特率都是9600。115200的波特率会输出两段英文
“HI2115-ssb-codeloader” 之类的，后面显示不出来，因为后面的数据是9600波特率的

而由于在8266工程中，我是用UART0的TX作为平常DEBUG和printf输出的，UART0的RX用于接收NB发送的消息，UART1的TX用于发送命令给NB，这就导致了必须两个串口的波特率一起改成9600才能正常通讯。

### 内存操作
不能只声明指针不分配内存控制。8266没有直接分配内存空间的接口，所以一般是直接声明一个数组的。
将一个字符串赋给另一个字符串要用**os_memcpy()**而不是**os_memset()**,后者是用来批量赋值的，如置零置一等。

### 串口通讯问题
软串口（流控口）是D8、D7,分别为TX、RX，使用**system_uart_swap()**后UART0由TX、RX口转为D8、D7。其中一定要注意，
而UART1口只能用作TX, 作为DEBUG串口。而在本仓库中，它被作为跟其他模块的通讯口。

