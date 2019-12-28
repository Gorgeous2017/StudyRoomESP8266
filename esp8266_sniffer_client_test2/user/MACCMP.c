#include "MACCMP.h"

// 状态数组  -->   (=0 ->死亡)   (>0  ->正常)
int Status[MACarraylength] = {0};
// MAC数组
char MAC[MACarraylength][MAClength] = {0};  

//int main(void){
//	int a,b,c;
//    while (1){
//        char test[10];
//        printf("Enter MAC:");
//        scanf("%s",&test);
//        printf("member : %s\t",test);
//        a++;
//        b++;
//        c++;
//        add(MAC , test);
//        printf("member alive:%d\n",get_memberNum(Status , MACarraylength));
//    }
//}

void add(char insertArray[MACarraylength][MAClength] , char* targetmember){
	int i;
	int NOT_FOUNT = 0;
    //如果存在重复则恢复它的状态
    for(i=0;i<MACarraylength;i++){
    	//!strcmp(insertArray[i],targetmember)
        if( (os_strncmp(*(insertArray+i),targetmember, MAClength) == 0) ){
            recoverStatus(Status , i);
            goto RED;
        }
    }
    //没有相同的，插入某个死亡状态的成员,并恢复它的状态
    	for(i=0;i<MACarraylength;i++){
     	   if(!*(Status + i)){
				os_memcpy(*(insertArray + i),targetmember,MAClength);
    	        recoverStatus(Status, i);
    	       break;
			}
		}
    //所有非零成员状态减一
    RED:reduceProgressively(Status , MACarraylength);
}

void reduceProgressively(int* Statusarray , int arrayLength){
	int i;
    for(i=0;i<arrayLength;i++){
    	if(*(Statusarray + i) > 0) {
    		(*(Statusarray + i))--;
    	}
    }
}

void recoverStatus(int* Statusarray , int mamberNum){
    *(Statusarray + mamberNum) = MAXStatus;
}

uint8 get_memberNum(int* Statusarray , int arrayLength){
    uint8 count = 0;
    uint8 i;
    for(i=0;i<arrayLength;i++){
        if(*(Statusarray + i) != 0) {
        	count++;
		}
    }
    return count;
}
