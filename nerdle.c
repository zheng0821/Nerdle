#include <stdio.h>
#include<math.h>
#include <stdlib.h>
#include <time.h>
#include <winsock.h>

#define MAX_GUESS_FRE 6

char st_equation[8]="00000000";

//函数组0
void print_rule();

//函数组1：输入检测
int is_valid_equation(char input[]);//检查输入是否合法
int cal(int a,int b,char op);//计算算式

//函数组2：输入匹配
int check_diff(char input[]);//比较初始与输入算式
int en_num(char a);//将符号转为地址

//函数组3：随机等式生成
void random_equation();//随机生成算式
char random_sign(int seed);//随机生成计算符
int random_num(int digit,int seed);//随机生成digit位数字
int quick_check();//快速遴选  提高效率
int digit_num(int n);//判断位数

int main(){
    print_rule();
    random_equation();
    int n=1,key=0;
    while(n<=MAX_GUESS_FRE){
        char guess[20];
        printf("---- GUESS #%d ----\n     ",n++);
        scanf("%s",guess);printf("  ");
        if (is_valid_equation(guess)) {
            if (check_diff(guess)) {
                printf("---- SUCCESS! ----");
                key=1;
                break;
            }
        }else{
            n--;
        }
    }
    if(!key){
        printf("----- LOSE! -----\n");
        for(int i=0;i<8;i++){
            printf("%s",st_equation[i]);
        }
    }
    getch();
    return 0;
}

void print_rule(){
    printf("------------------NERDLE_RULE------------------\n");
    printf(" ! is not in the target equation at all\n");
    printf(" ^ is in the equation ,but in the wrong space\n");
    printf(" $ is in the equation and in the correct spot\n");
    printf("-----------------------------------------------\n");
    printf(" !  There is no more 0 before each word  !\n");
    printf("-----------------------------------------------\n\n");
}


int is_valid_equation(char input[]){
    if (strlen(input)>8){
        printf("! Too long.\n");
        return 0;
    }
    if (strlen(input)<8){
        printf("! Too short.\n");
        return 0;
    }//ckpt(checkpoint)1：长度

    int top_num=-1,top_sign=-1,stick_num[8];char stick_sign[8];
    int temp=0;
    int i=0,flag=0;//遍历
    while(i<8){
        if (input[i]=='+'||input[i]=='-'||input[i]=='*'||input[i]=='/'){
            if(flag){
                stick_num[++top_num]=temp;
                temp=0;
                flag=0;
            }
            stick_sign[++top_sign]=input[i];//后缀栈
        }else if(input[i]=='='){
            if(temp==0){
                printf("! Invalid equation\n");
                return 0;
            }else{
                stick_num[++top_num]=temp;
                break;
            }
        }else if('0'<=input[i]&&input[i]<='9'){
            temp=temp*10+((int)input[i]-48);
            flag=1;
        }else{
            printf("! Invalid character.\n");
            return 0;
        }//ckpt2：字符
        i++;
    }
    if(i==8){
        printf("! No equal \"=\" sign.\n");
        return 0;
    }//ckpt3：等号

    int RHS=0;i++;
    for(;i<8;i++){
        if('0'<=input[i]&&input[i]<='9'){
            RHS=RHS*10+((int)input[i]-48);
        }else{
            printf("! The right side of the equals sign must be a number.\n");
            return 0;
        }//ckpt4：等式右侧
    }

    if(top_sign==-1){
        printf("! There must be at least one sign + - * /.\n");
        return 0;
    }
    if(top_num!=1+top_sign){
        printf("! Invalid equation.\n");
        return 0;
    }//ckpt5：符号个数及匹配

    while(top_num!=0){
        if(stick_sign[top_sign]=='/'){//ckpt6：运算要求
            if(stick_num[top_num]==0){
                printf("! Invalid equation.\n");
                return 0;
            }
            float temp_div=stick_num[top_num-1]*1.0/stick_num[top_num];
            if(floor(temp_div)!=ceil(temp_div)){
                printf("! Invalid equation.\n");
                return 0;
            }//出现小数一定不可能相等
        }
        stick_num[top_num-1]=cal(stick_num[top_num],stick_num[top_num-1],stick_sign[top_sign]);
        top_num--;top_sign--;//
    }
    if(stick_num[top_num]!=RHS){
        printf("! The left side is not equal to the right.\n");
        return 0;
    }//ckpt7：等号成立

    return 1;
}

int cal(int a,int b,char op){
    switch(op){
        case '+':return b+a;
        case '-':return b-a;
        case '*':return b*a;
        case '/':return b/a;
    }
}


int check_diff(char input[]){
    int flag[8]={0,0,0,0,0,0,0,0},sgn=0;
    int count[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};//0-9;+-/*=
    for(int i=0;i<8;i++){
        if(input[i]==st_equation[i]){
            flag[i]=1;
            sgn+=1;
        }else{
            count[en_num(st_equation[i])]+=1;
        }
    }
    printf("   ");
    if(sgn==8){
        printf("$$$$$$$$\n");
        return 1;
    }
    for(int i=0;i<8;i++){
        if(flag[i]==1){
            printf("$");
            continue;}
        if(count[en_num(input[i])]!=0){
            count[en_num(input[i])]-=1;
            printf("^");
        }else{printf("!");}
    }
    printf("\n\n");
    return 0;
}

int en_num(char a){
    switch(a){
        case '+':return 10;
        case '-':return 11;
        case '*':return 12;
        case '/':return 13;
        case '=':return 14;
        default:return (int)a-48;
    }
}


void random_equation(){
    int flag=0,seed=1;
    while(!flag){
        seed=(seed)%9+1;
        for(int i=0;i<8;i++){st_equation[i]='0';}
        srand((unsigned int)time(NULL)*seed);
        int loc_eq=rand()%3+4;
        st_equation[loc_eq]='=';//先确定‘=’位置

        int i=-1;
        int end=0;//记录最后一个字符位置
        char stick_sign[2]="oo";int top_sign=-1;
        int stick_num[3]={0,0,0};int top_num=-1;

        while(i<loc_eq-2){//再确定符号位置
            srand((unsigned int)time(NULL)*seed);
            i+=rand()%3+2;

            if(i<loc_eq-1){
                st_equation[i]=random_sign(seed);

                stick_sign[++top_sign]=st_equation[i];
                end=i;
            }
        }

        if(quick_check(loc_eq,stick_sign,top_sign)){
            continue;
        }//情况特判，提高效率


        //最后填数字,保证除号整除
        int j=0,beg=0;
        while(1){//前面几位随机生成，最后一位遍历
            if(j==end){
                stick_num[++top_num]=random_num(j-beg,seed);
                break;
            }else if(st_equation[j]=='+'||st_equation[j]=='-'||
                      st_equation[j]=='*'||st_equation[j]=='/'){
                stick_num[++top_num]=random_num(j-beg,seed);
                beg=i+1;
            }
            j++;
        }

        int num_min=(int)pow(10.0,(float)(loc_eq-end-2));
        int stick_ans[100],top_ans=-1;
        for(int i=num_min;i<num_min*10;i++){
            int temp=i,temp_tn=top_num,temp_ts=top_sign;
            while(temp_tn!=-1){
                if(stick_sign[temp_ts]=='/'){
                    if(temp==0){temp=-404;break;}
                    float temp_div=stick_num[temp_tn]*1.0/temp;
                    if(floor(temp_div)!=ceil(temp_div)){temp=-404;break;}
                }//除法特别考虑
                temp=cal(temp,stick_num[temp_tn],stick_sign[temp_ts]);
                temp_tn--;temp_ts--;
            }
            if(temp>0&&digit_num(temp)==7-loc_eq){//匹配位数
                flag=1;
                stick_ans[++top_ans]=i;//记录全部可行值再随机抽取
                if(top_ans==99){break;}
            }
        }

        if(flag){
            srand((unsigned int)time(NULL)*seed);
            int random_ans;
            if(top_ans!=0){random_ans=rand()%(top_ans);}
            else{random_ans=0;}
            stick_num[++top_num]=stick_ans[random_ans];//随机选取最后的运算数

            int RHS=stick_ans[random_ans];
            int temp_top_num=top_num-1;
            while(temp_top_num!=-1){
                RHS=cal(RHS,stick_num[temp_top_num],stick_sign[top_sign]);
                temp_top_num--;top_sign--;
            }//计算右值

            for(int i=7;i>loc_eq;i--){
                st_equation[i]=RHS%10+'0';
                RHS/=10;
            }
            for(int i=loc_eq-1;i>=0;i--){
                if(st_equation[i]=='0'){
                    st_equation[i]=stick_num[top_num]%10+'0';
                    stick_num[top_num]/=10;
                    if(stick_num[top_num]==0){top_num--;}
                }
            }//更新字符数组

        }
    }
}

char random_sign(int seed){
    srand((unsigned int)time(NULL)*seed);
    int temp=rand()%4;
    switch(temp){
        case 0:return '+';
        case 1:return '-';
        case 2:return '*';
        case 3:return '/';
    }
}

int random_num(int digit,int seed){
    srand((unsigned int)time(NULL)*seed);
    int num=rand();
    switch(digit){
        case 1:return num%9+1;
        case 2:return num%90+10;
        case 3:return num%900+100;//避免前导0
    }
}

int quick_check(int loc,char stick[],int top){
    if(top==-1){return 1;}
    if(top==0){
        if(loc==6&&(stick[top]=='+'||stick[top]=='*')){return 1;}
        if(loc==4&&(stick[top]=='-'||stick[top]=='/')){return 1;}
    }
    return 0;//有可能生成
}

int digit_num(int n){
    if(n==-404){return -1;}
    int digit=0;
    while(n!=0){
        n/=10;
        digit++;
    }
    return digit;
}
