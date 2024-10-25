#include <stdio.h>
#include<math.h>
#include <stdlib.h>
#include <time.h>
#include <winsock.h>

#define MAX_GUESS_FRE 6

char st_equation[8]="00000000";

//������0
void print_rule();

//������1��������
int is_valid_equation(char input[]);//��������Ƿ�Ϸ�
float op(int n[],char s[],int tn,int ts);
float cal(float a,float b,char op);//������ʽ

//������2������ƥ��
int check_diff(char input[]);//�Ƚϳ�ʼ��������ʽ
int en_num(char a);//������תΪ��ַ

//������3�������ʽ����
void random_equation();//���������ʽ
char random_sign(int seed);//������ɼ����
int random_num(int digit,int seed);//�������digitλ����
int quick_check(int loc,char stick[],int tail);//������ѡ  ���Ч��
int digit_num(int n);//�ж�λ��

int main(){
    random_equation();

    print_rule();

    int n=1,key=0;

    while(n<=MAX_GUESS_FRE){
        char guess[20];
        printf("------------------ GUESS #%d ------------------\n                   ",n++);
        scanf("%s",guess);
        if (is_valid_equation(guess)) {
            if (check_diff(guess)) {
                printf("------------------ SUCCESS! ------------------");
                key=1;
                break;
            }
        }else{
            n--;
        }
    }
    if(!key){
        printf("-------------------- LOSS --------------------\n");
        printf("               ANS:");
        for(int i=0;i<8;i++){
            printf("%s",st_equation[i]);
        }
    }
    getch();
    return 0;
}

void print_rule(){
    printf("-----------------NERDLE 2.1.0-----------------\n\n");
    printf("----------------------RULE--------------------\n");
    printf(" ! is not in the target equation at all\n");
    printf(" ^ is in the equation ,but in the wrong space\n");
    printf(" $ is in the equation and in the correct spot\n");
    printf("----------------------------------------------\n");
    printf(" !  There is no more 0 before each word  !\n");
    printf("----------------------------------------------\n\n");
    printf("Developer Model?(Y/n)");
    char input;
    scanf("%c",&input);
    if(input=='Y'){
        printf("ANS:");
        for(int i=0;i<8;i++){
            printf("%c",st_equation[i]);
        }
    }
    printf("\n\n");
}


int is_valid_equation(char input[]){
    if (strlen(input)>8){
        printf("! Too long.\n");
        return 0;
    }
    if (strlen(input)<8){
        printf("! Too short.\n");
        return 0;
    }//ckpt(checkpoint)1������

    int tail_num=-1,tail_sign=-1,stick_num[8];char stick_sign[8];
    int temp=0;
    int i=0,flag=0;//����
    while(i<8){
        if (input[i]=='+'||input[i]=='-'||input[i]=='*'||input[i]=='/'){
            if(flag){
                stick_num[++tail_num]=temp;
                temp=0;
                flag=0;
            }
            stick_sign[++tail_sign]=input[i];
        }else if(input[i]=='='){
            if(temp==0){
                printf("! Invalid equation\n");
                return 0;
            }else{
                stick_num[++tail_num]=temp;
                break;
            }
        }else if('0'<=input[i]&&input[i]<='9'){
            temp=temp*10+((int)input[i]-48);
            flag=1;
        }else{
            printf("! Invalid character.\n");
            return 0;
        }//ckpt2���ַ�
        i++;
    }
    if(i==8){
        printf("! No equal \"=\" sign.\n");
        return 0;
    }//ckpt3���Ⱥ�

    int RHS=0;i++;
    for(;i<8;i++){
        if('0'<=input[i]&&input[i]<='9'){
            RHS=RHS*10+((int)input[i]-48);
        }else{
            printf("! The right side of the equals sign must be a number.\n");
            return 0;
        }//ckpt4����ʽ�Ҳ�
    }

    if(tail_sign==-1){
        printf("! There must be at least one sign + - * /.\n");
        return 0;
    }
    if(tail_num!=1+tail_sign){
        printf("! Invalid equation.\n");
        return 0;
    }//ckpt5�����Ÿ�����ƥ��

    for(int i=0;i<=tail_sign;i++){
        if(stick_sign[i]=='/'&&stick_num[i+1]==0){//ckpt6������Ҫ��
            printf("! Invalid equation.\n");
            return 0;
        }
    }
    float  LHS=op(stick_num,stick_sign,tail_num,tail_sign);
    if(LHS!=(float)RHS){
        printf("! The left side is not equal to the right.\n");
        return 0;
    }//ckpt7���Ⱥų���

    return 1;
}

float cal(float a,float b,char op){
    switch(op){
        case '+':return b+a;
        case '-':return b-a;
        case '*':return b*a;
        case '/':return b*1.0/a;
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
    printf("                   ");
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
        st_equation[loc_eq]='=';//��ȷ����=��λ��

        int i=-1;
        int end=0;//��¼���һ���ַ�λ��
        char stick_sign[2]="oo";int tail_sign=-1;
        int stick_num[3]={0,0,0};int tail_num=-1;

        while(i<loc_eq-2){//��ȷ������λ��
            seed=(seed)%9+1;
            srand((unsigned int)time(NULL)*seed);
            i+=rand()%3+2;

            if(i<loc_eq-1){
                st_equation[i]=random_sign(seed);
                stick_sign[++tail_sign]=st_equation[i];
                end=i;
            }
        }

        if(quick_check(loc_eq,stick_sign,tail_sign)){
            continue;
        }//������У����Ч��


        //���������,��֤��������
        int j=0,beg=0;
        while(1){//ǰ�漸λ������ɣ����һλ����
            if(j==end){
                stick_num[++tail_num]=random_num(j-beg,(seed)%9+1);
                break;
            }else if(st_equation[j]=='+'||st_equation[j]=='-'||
                      st_equation[j]=='*'||st_equation[j]=='/'){
                stick_num[++tail_num]=random_num(j-beg,(seed)%9+1);
                beg=i+1;
            }
            j++;
        }

        int num_min=(int)pow(10.0,(float)(loc_eq-end-2));
        if(num_min==0){num_min++;}
        int stick_ans[100],tail_ans=-1;
        for(int i=num_min;i<num_min*10;i++){
            stick_num[tail_num+1]=i;
            float temp=op(stick_num,stick_sign,tail_num,tail_sign);
            if(floor(temp)!=ceil(temp)){continue;}
            if(temp>=0&&digit_num((int)temp)==7-loc_eq){//ƥ��λ��
                flag=1;
                stick_ans[++tail_ans]=i;//��¼ȫ������ֵ�������ȡ
                if(tail_ans==99){break;}
            }
        }

        if(flag){
            srand((unsigned int)time(NULL)*seed);
            int random_ans;
            if(tail_ans!=0){random_ans=rand()%(tail_ans);}
            else{random_ans=0;}
            stick_num[++tail_num]=stick_ans[random_ans];//���ѡȡ����������

            int RHS=op(stick_num,stick_sign,tail_num,tail_sign);
            for(int i=7;i>loc_eq;i--){
                st_equation[i]=RHS%10+'0';
                RHS/=10;
            }
            for(int i=loc_eq-1;i>=0;i--){
                if(st_equation[i]=='0'){
                    st_equation[i]=stick_num[tail_num]%10+'0';
                    stick_num[tail_num]/=10;
                    if(stick_num[tail_num]==0){tail_num--;}
                }
            }//�����ַ�����
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
        case 3:return num%900+100;//����ǰ��0
    }
}

int quick_check(int loc,char stick[],int tail){
    if(tail==-1){return 1;}
    if(tail==0){
        if(loc==6&&(stick[tail]=='+'||stick[tail]=='*')){return 1;}
        if(loc==4&&(stick[tail]=='-'||stick[tail]=='/')){return 1;}
    }
    return 0;//�п�������
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

float op(int n[],char s[],int tn,int ts){
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
