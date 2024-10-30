#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock.h>

#define MAX(a,b) a>b ? a:b
#define MIN(a,b) a>b ? b:a

int count=1;//交互次数

char input[9]="12+36=48";//首次输出
char diff_input[9];//用于第二次输出，负责产生差异化更大的数组
int max_diff=0;//衡量差异化的参数

char input_confirmed[9]="????????";
int confirmed[3]={0,0,0};//分别记录三种(数字，运算符，等号)已确定的数量


//通过数组来记录每次交互得到的限制条件
//              位置:0,1,2,3,4,5,6,7
int dis[15][9]={    {1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},
                    {1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},
                    {1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},
                    {1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},
                    {1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},//数字0-9
                    {0,1,1,1,1,0,0,0},{0,1,1,1,1,0,0,0},
                    {0,1,1,1,1,0,0,0},{0,1,1,1,1,0,0,0},//四个运算符
                    {0,0,0,0,1,1,1,0}                  };//等号

int possible_dis[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//额外的可能情况(面向一个数同时反馈'^!'的情况）

//通过等号和一元算术符快速判断是否可行 避免DFS浪费 减轻搜索压力
//                       位置:1,2,3,4
int quick_check[3][4][4]={ { {1,1,0,0},  // +   //loc_eq(等号位置)=4
                             {0,0,0,0},  // -
                             {1,1,0,0},  // *
                             {0,0,0,0} },// /
                           { {0,1,0,0},         //loc_eq(等号位置)=5
                             {0,1,1,0},
                             {0,0,0,0},
                             {0,0,1,0} },
                           { {0,0,0,0},         //loc_eq(等号位置)=6
                             {0,0,1,0},
                             {0,0,0,0},
                             {0,0,1,0} } };

//通用辅助模组
int output_check(char a[]);//简单的输入检测，但假定所有的输入均逻辑自洽
int en_num(char a);
char de_num(int a);
int ceil(float a);//补充math.h

//限制模组：根据反馈修改限制条件
void modify_limitation();
void nearby_process(char trait,int loc);//处理临近情况
void quick_process(int loc);


//预测模组：根据限制条件预测
void modify_equation();
int fulfill_sign(int num,int loc_eq);//填运算符
int fulfill_num(int loc);//填数字
int match(int i);//判断LHS是否能填入右侧
int diff();//比较差异

//计算模组
float cal(float a,float b,char op);
float op(char temp[]);


int main(){
    int DM=print_rule();

    while(1){
        if(count==7){printf("\n!-!-!-!-LOSE-!-!-!-!\n\n");}
        printf("\n----------------------TRY%d--------------------\n",count++);
        printf("              predict : %s \n",input);
        printf("             feedback : ");
        char output[9];
        scanf("%s",output);

        if(!output_check(output)){
            printf("Invalid\n");
            count--;
            continue;
        }

        modify_limitation(output);
        if (confirmed[0]+confirmed[1]+confirmed[2]==8){
            printf("nerdle_LocalSolve did it!\n");
            break;
        }

        if(DM){
            printf("\nPossible distribution:\n");
            for(int i=0;i<15;i++){
                printf("%c  ： ",de_num(i));
                for(int j=0;j<8;j++){
                    printf("%d ",dis[i][j]);
                }
                printf("\n");
            }
            printf("others:");
            for(int i=0;i<15;i++){
                printf("\'%c\':%d ;",de_num(i),possible_dis[i]);
                if(i==6){
                    printf("\n");
                }
            }
            printf("\n");
            printf("Confirmed equation: %s\n",input_confirmed);
            printf("Confirmed number(number,operator,equal-sign): ");
            for(int i=0;i<3;i++){
                printf("%d ",confirmed[i]);
            }
            printf("\n");
        }

        modify_equation();

    }
    return 0;
}

int print_rule(){
    char temp;
    printf("------------NerdleLocalSolve 3.1.0------------\n\n");
    printf("----------------------RULE--------------------\n");
    printf(" ! is not in the target equation at all\n");
    printf(" ^ is in the equation ,but in the wrong space\n");
    printf(" $ is in the equation and in the correct spot\n");
    printf("----------------------------------------------\n");
    printf("!Ensure the logical rationality of the input!\n");
    printf("----------------------------------------------\n\n");

    printf("Developer Mode?(Y/n)");
    scanf("%c",&temp);
    if(temp=='Y'){
        return 1;
    }else{
        return 0;
    }
}

int output_check(char a[]){
    if(strlen(a)!=8){return 0;}
    for(int i=0;i<8;i++){
        if(a[i]!='!'&&a[i]!='^'&&a[i]!='$'){
            return 0;
        }
    }
    return 1;
}

int ceil(float a){
    if(a>(int)a){
        return (int)a+1;
    }else{
        return (int)a;
    }
}


void modify_limitation(char feedback[]){
    int temp_poss_dis[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(int j=0;j<3;j++){
        confirmed[j]=0;
    }
    for(int i=0;i<8;i++){
        if(feedback[i]=='$'){
            if (dis[en_num(input[i])][i]==0){
                possible_dis[en_num(input[i])]-=1;//还愿
            }

            if(en_num(input[i])<=9){
                confirmed[0]++;
            }else if(en_num(input[i])<=13){
                confirmed[1]++;
            }else{
                confirmed[2]++;
            }
            input_confirmed[i]=input[i];

            nearby_process(input[i],i);
            dis[en_num(input[i])][i]=2;
        }else if(feedback[i]=='^'){
            dis[en_num(input[i])][i]=0;
            temp_poss_dis[en_num(input[i])]-=1;//^!
        }else{// !
            for(int j=0;j<8;j++){
                if(dis[en_num(input[i])][j]!=2){
                    dis[en_num(input[i])][j]=0;
                }
            }
            temp_poss_dis[en_num(input[i])]=-temp_poss_dis[en_num(input[i])];//翻正
        }
    }
    for(int i=0;i<15;i++){
        possible_dis[i]=MAX(0,temp_poss_dis[i]);
    }
}

void nearby_process(char trait,int loc){//处理附近情况
    for(int i=0;i<15;i++){
        dis[i][loc]=0;
    }
    if(trait=='+'||trait=='-'||trait=='*'||trait=='/'||trait=='='){
        dis[0][MIN(loc+1,7)]=0;//剥离前导0
        for(int i=10;i<14;i++){
            dis[i][MAX(loc-1,0)]=0;
            dis[i][MIN(loc+1,7)]=0;
        }
    }
    if(trait=='='){
        for(int i=0;i<8;i++){
            dis[en_num(trait)][i]=0;
        }
        quick_process(loc);
    }
}

void quick_process(int loc){
    if(loc==4){
        for(int i=1;i<=4;i++){
            dis[en_num('/')][i]=0;
            dis[en_num('-')][i]=0;
        }
    }
}


//更强调全面性而不是随机性
void modify_equation(){
    for(int i=0;i<8;i++){input[i]=input_confirmed[i];}//拷贝
    for(int i=4;i<=6;i++){
        if(dis[14][i]==0){continue;}
        input[i]='=';
        for(int j=MAX(confirmed[1],1);j<=ceil(i*0.25);j++){
            if(fulfill_sign(j-confirmed[1],i)){
                break;
            }
        }
        if(count==2){
            for(int i=0;i<8;i++){
                input[i]=diff_input[i];//归档
            }
        }
    }//确定等号>>运算符>>数字三步确定
}

int fulfill_sign(int num,int loc_eq){
    if(num==-1){
        return 0;
    }else if(num==0){
        if(fulfill_num(0)){return 1;}
    }else if(num==1){
        for(int i=1;i<=4;i++){
            if(input[i]!='?'){continue;}
            for(int sign=10;sign<=13;sign++){
                if(quick_check[loc_eq-4][sign-10][i-1]&&dis[sign][i]){
                    input[i]=de_num(sign);
                    if(fulfill_num(0)){return 1;}
                    input[i]='?';
                }
            }
        }
    }else if(num==2){
        int flag[2]={0,0};
        for(int i=1;i<=2;i++){
            if(input[i]!='?'){continue;}
            for(int sign1=10;sign1<=13;sign1++){
                if(possible_dis[sign1]>0){
                    flag[0]=1;
                }
                if(flag[0]||dis[sign1][i]==1){
                    if(flag[0]){
                        possible_dis[sign1]--;
                    }
                    for(int j=i+2;j<=4;j++){
                        if(input[j]!='?'){continue;}
                        for(int sign2=10;sign2<=13;sign2++){
                            if(possible_dis[sign2]>0){
                                flag[1]=1;
                            }
                            if(flag[1]||dis[sign2][j]==1){
                                if(flag[1]){
                                    possible_dis[sign2]--;
                                }
                                input[i]=de_num(sign1);
                                input[j]=de_num(sign2);

                                if(fulfill_num(0)){return 1;}

                                input[i]='?';
                                input[j]='?';
                                if(flag[1]){
                                    possible_dis[sign2]++;
                                }
                            }
                        }
                    }
                    if(flag[0]){
                        possible_dis[sign1]++;
                    }
                }
            }
        }
    }
    return 0;
}

//DFS递归生成
int fulfill_num(int loc){
    while(input[loc]!='?'&&input[loc]!='='){
        loc++;
    }//定位到第一步可设值的位置
    if(input[loc]=='='){//检查
        for(int i=0;i<15;i++){
            if(possible_dis[i]){
                return 0;//保证都被使用
            }
        }
        int LHS=match(loc);
        if(LHS>0){
            for(int i=7;i>loc;i--){
                input[i]=(char)(LHS%10+48);
                LHS/=10;
            }
            if(count==2){
                if(!diff()){
                    return 0;//继续深搜,除非差异化已经最大了
                }
            }
            return 1;
        }else{
            return 0;
        }
    }
    for(int i=0;i<10;i++){
        int flag=0;
        if(possible_dis[i]>0){
            flag=1;
        }
        if(flag||dis[i][loc]==1){
            if(i==0&&(loc==0||input[loc-1]<'0'||input[loc-1]>'9')){continue;}

            if(flag){possible_dis[i]--;}

            input[loc]=(char)(i+48);
            if(fulfill_num(loc+1)){
                return 1;
            }
            input[loc]='?';
            if(flag){possible_dis[i]++;}
        }
    }
    return 0;
}

int match(int i){//匹配输出右值
    float temp=op(input);
    if(ceil(temp)!=floor(temp)||temp<0){
        return -1;
    }
    int LHS=(int)temp;
    if(LHS==0&&i==6){
        return 1;
    }
    for(int j=7;j>i;j--){
        if(input_confirmed[j]=='?'){
            if (LHS==0&&j==i+1){
                return -1;//避免前导0
            }
            if (dis[LHS%10][j]==0){
                return -1;
            }
        }else if(input_confirmed[j]!=(char)(LHS%10+48)){
            return -1;
        }
        LHS/=10;
    }
    if(LHS==0){
        return (int)temp;
    }else{
        return -1;
    }
}

int diff(){
    int unconfirmed=8-confirmed[0]-confirmed[1]-confirmed[2];
    int diff[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int temp_diff;
    for(int i=0;i<8;i++){
        if(input_confirmed[i]=='?'){
            if(!diff[en_num(input[i])]){
                diff[en_num(input[i])]++;
            }
        }
    }
    for(int i=0;i<15;i++){
        if(diff[i]){
            temp_diff++;
        }
    }
    if(temp_diff>max_diff){
        max_diff=temp_diff;
        for(int i=0;i<8;i++){
            diff_input[i]=input[i];
        }
        if(temp_diff==unconfirmed){//两两不一
            return 1;
        }
    }
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

char de_num(int a){
    switch(a){
        case 10:return '+';
        case 11:return '-';
        case 12:return '*';
        case 13:return '/';
        case 14:return '=';
        default:return (char)(a+48);
    }
}


float op(char temp[]){
    int temp_num=0,i=-1;
    int n[3]={0,0,0};
    char s[2]="oo";
    int tn=-1,ts=-1;
    while(temp[i]!='='){
        i++;
        if (temp[i]=='+'||temp[i]=='-'||temp[i]=='*'||temp[i]=='/'){
            n[++tn]=temp_num;
            temp_num=0;
            s[++ts]=temp[i];
        }else if(temp[i]=='='){
            n[++tn]=temp_num;
        }else{
            temp_num=temp_num*10+((int)temp[i]-48);
        }
    }
    if(ts==0){
        return cal(n[1],n[0],s[0]);
    }else{
        if(s[1]=='*'||s[1]=='/'){
            return cal(cal(n[2],n[1],s[1]),n[0],s[0]);
        }else{
            return cal(n[2],cal(n[1],n[0],s[0]),s[1]);
        }
    }
}

float cal(float a,float b,char op){
    switch(op){
        case '+':return b+a;
        case '-':return b-a;
        case '*':return b*a;
        case '/':return b*1.0/a;
    }
}
