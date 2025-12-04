// Encoding of this file: GB 2312

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100          // 地图最大宽、高
#define MAX_HISTORY 100  // 最大历史记录

struct Action {       // 移动一步的数据
    int px,py;        // px,py表示人的坐标
    int box_x,box_y;  // 被移动的箱子坐标(如果有)
    char box_prev;    // 箱子原来位置的字符
    int moved_box;    // 是否移动了箱子
    int aim_x,aim_y;  // 移动目标位置
};

struct Action history[MAX_HISTORY];  // 操作历史的栈
struct Action current,temp;
int history_top=-1;  // 操作历史的栈顶
int undone_top=-1;   // 撤销操作记录的栈顶
struct Action undone[MAX_HISTORY];  // 撤销操作记录的栈

// px,py表示人的坐标
int px,py,box_number=0;  

// time: 已消耗的体力  total_time: 总体力  
unsigned long long int time=0,total_time;

int find_person(char (*data)[MAX]){
    int flag1=0,flag2=0,flag3=0;
    for (int i = 0; i < MAX; i++){
        if (!data[i][0]) 
            break;
        for (int j = 0; j < MAX; j++){
            if (!data[i][j]) 
                break;
            if (data[i][j]=='4'){
                px=i;py=j;
                data[i][j]='0';
                flag1=1;
            }
            if (data[i][j]=='2') flag2=1;
            if (data[i][j]=='3') flag3=1;
        }
    }
    return (flag1 && flag2 && flag3);    // 如果箱子、玩家、目的地其中之一没找到，返回0要求重新输入
}

char (*read_text_file(const char *filename))[MAX] {
    static char data[MAX][MAX]; 
    FILE *fp=fopen(filename,"r");
    
    if (!fp) return NULL; // 文件打开失败

    fscanf(fp,"%llu %llu\n",&time,&total_time);
    int row=0;
    while (fgets(data[row],MAX,fp)) {
        data[row][strcspn(data[row],"\n")] = '\0';
        row++;
    }
    fclose(fp);

    find_person(data);
    return data; 
}

char (*Custimize_map())[MAX]{
    int x,y;
    unsigned long long TIME; 
    
    printf("输入地图的宽和高(5~100):\n");
    scanf("%d %d",&y,&x);
    while(getchar()!='\n');

    while(x<5||x>100||y<5||y>100){
        printf("请输入正确的宽和高(5~100):\n");
        scanf("%d %d",&y,&x);
        while(getchar()!='\n');
    }

    static char data[MAX][MAX];
    Q:
    printf("请依次输入每一行:(0:空地 1:墙 2:目标 3:箱子 4:人)\n注意:每个数字间不要加空格!!!\n");
    int row=0,answer;
    while (fgets(data[row],MAX,stdin)) {
        data[row][strcspn(data[row],"\n")] = '\0';
        row++;
        if (row==x) 
            break;
    }

    for (int i=0;i<x;i++){
        if (strlen(data[i])!=y){
            printf("地图宽度输入错误,请重新输入！\n");
            goto Q;
            break;
        }
    }
    answer=find_person(data);
    if (!answer){
        printf("未输入人、箱子或目标,请重新输入!\n");
        goto Q;
    }
    return data;
}

void print_data(char (*data)[MAX]){
    for (int i = 0; i < MAX; i++) {
        if (!data[i][0]) 
            break;
        for (int j=0;j<MAX;j++){
            if (!data[i][j]) break;
            if (data[i][j]=='1') printf("#");
            else if (i==px && j==py && data[i][j]!='2') printf("@");
            else if (i==px && j==py && data[i][j]=='2') printf("!");
            else if (data[i][j]=='3') printf("$");
            else if (data[i][j]=='2') printf(".");
            else printf(" ");
        }
        printf("\n");
    }
}

int is_finish(char (*data)[MAX],int x,int y){
    if (data[x][y]=='2') 
        return 1;
    return 0;
}

int move(char input,char (*data)[MAX]){
    int flag=0;

    // 在执行移动前保存当前状态
    current.px = px;
    current.py = py;
    switch (input){
        case 'w':
            current.moved_box = 0;
            if (data[px-1][py]!='1'){
                if (data[px-1][py]=='3' && data[px-2][py]!='1'&& data[px-2][py]!='3'){
                    current.moved_box=1;
                    current.box_x=px-1;
                    current.box_y=py;
                    current.box_prev=data[px-2][py];    
                    current.aim_x=px-2;current.aim_y=py;
                    
                    data[px-1][py]='0';px--;
                    if (is_finish(data,px-1,py))  flag=1;
                    else   data[px-1][py]='3';
                    time++;
                }
                else if (data[px-1][py]!='3')   px--;
                }        
            break;

        case 's':
            current.moved_box = 0;
            if (data[px+1][py]!='1'){
                if (data[px+1][py]=='3' && data[px+2][py]!='1'&& data[px+2][py]!='3'){
                    current.moved_box=1;
                    current.box_x=px+1;
                    current.box_y=py;
                    current.box_prev=data[px+2][py];  
                    current.aim_x=px+2;current.aim_y=py;  

                    data[px+1][py]='0';px++;
                    if (is_finish(data,px+1,py)) 
                        flag=1;
                    else 
                        data[px+1][py]='3';
                    time++;
                }
                else if (data[px+1][py]!='3')   px++;
            }        
            break;        

        case 'a':
            current.moved_box = 0;
            if (data[px][py-1]!='1'){
                if (data[px][py-1]=='3' && data[px][py-2]!='1'&& data[px][py-2]!='3'){
                    current.moved_box=1;
                    current.box_x=px;
                    current.box_y=py-1;
                    current.box_prev=data[px][py-2];    
                    current.aim_x=px;current.aim_y=py-2;

                    data[px][py-1]='0';py--;
                    if (is_finish(data,px,py-1)) 
                        flag=1;
                    else 
                        data[px][py-1]='3';
                    time++;
                }
                else if (data[px][py-1]!='3')   py--;
            }
            break;

        case 'd':
            current.moved_box = 0;
            if (data[px][py+1]!='1'){
                if (data[px][py+1]=='3' && data[px][py+2]!='1'&& data[px][py+2]!='3'){
                    current.moved_box=1;
                    current.box_x=px;
                    current.box_y=py+1;
                    current.box_prev=data[px][py+2]; 
                    current.aim_x=px;current.aim_y=py+2;   

                    data[px][py+1]='0';py++;
                    if (is_finish(data,px,py+1)) 
                        flag=1;
                    else 
                        data[px][py+1]='3';
                    time++;
                }
                else if (data[px][py+1]!='3')   py++;
            }
            break;

        case 'q':
            return 2;

        case 'z':
            if (history_top>=0) {
                // 保存当前状态到 undone 栈
                temp.moved_box=history[history_top].moved_box;
                temp.px=px;temp.py=py;
                temp.box_x=history[history_top].aim_x;
                temp.box_y=history[history_top].aim_y;
                undone_top++;undone[undone_top]=temp;
                
                // 恢复历史状态
                struct Action last=history[history_top];
                px=last.px;
                py=last.py;
                current=history[history_top];
                if (last.moved_box) {
                    // 恢复箱子位置
                    data[last.box_x][last.box_y]='3';
                    data[last.aim_x][last.aim_y]=last.box_prev;  // 恢复箱子原来位置的字符
                    
                    // 如果是推到了目标点，需要恢复箱子计数
                    if (last.box_prev=='2') {
                        box_number++;
                    }
                    time--;
                }
                history_top--;
                time--;
                return 3;
            } else {
                printf("没有可撤销的操作！\n");
                return 3;
            }
            break;

        case 'y':
            if (undone_top >= 0) {
                // 保存当前状态到 history 栈
                history_top++;
                history[history_top]=current;
                // 恢复 undone 状态
                struct Action next=undone[undone_top];
                px=next.px;
                py=next.py;
                if (next.moved_box) {
                    // 重新移动箱子
                    if (data[next.box_x][next.box_y]=='2') {
                        box_number--;
                    }
                    else{
                        data[next.box_x][next.box_y]='3';
                    }
                    data[px][py]='0';                        
                    time++;
                }
                
                undone_top--;
                time++;
                return 3;
            } else {
                printf("没有可恢复的操作！\n");
                return 3;
            }
            break;

        default:
            printf("错误输入!\n");
            return 3;
        }

        // 如果是有效的移动操作，保存到历史记录
        if (input=='w' || input =='a'|| input =='s'|| input =='d') {
            history_top++;
            history[history_top] = current;
            // 清空重做栈，因为新的操作会使之前的重做记录无效
            undone_top=-1;
        }
        if (flag) return 1;
        return 0;
}

void data_memorize(char (*data)[MAX],unsigned long long t,unsigned long long tt){
    FILE *fp=fopen("data/上次的游戏.txt","w");  
    
    // 若打开data/上次的游戏.txt文件失败,输出“无法保存游戏数据！”
    if (!fp){
        printf("无法保存游戏数据！\n");
        return ;
    }
    // 写入已消耗体力和总体力
    fprintf(fp,"%llu %llu\n",t,tt);

    // 写入数据
    for (int i=0;i<MAX;i++) {
        // 遇到空行停止
        if (!data[i][0])  
            break;
        
        // 恢复人的位置标记(因为find_person()把'4'替换成了'0')
        if (i==px) {
            for (int j=0;j<MAX;j++) {
                if (!data[i][j]) 
                    break;
                if (j==py){
                    fputc('4',fp);  // 写入人的位置
                    continue;
                }
                fputc(data[i][j],fp);
            }
        }
        else  fputs(data[i],fp);  // 直接写入整行

        fputc('\n',fp); 
    }

    fclose(fp);
}

void data_delete(){
    FILE *fp=fopen("data/上次的游戏.txt","w");  
    fclose(fp);
}

int main() {
    // 文件名存储在questions数组中
    char questions[4][100]={"data/难度：简单.txt","data/难度：一般.txt","data/难度：困难.txt","data/难度：地狱.txt"};

    printf("选择问题难度:1、简单  2、一般  3、困难  4、地狱  5、自定义  6、继续上次的游戏:\n");
    int ques;scanf("%d",&ques);
    while (getchar() != '\n');

    while (!(ques>=1 && ques<=6) ){
        W:       // W 照应下方“读取上次游戏存档失败”的goto语句
        printf("请输入正确的问题难度！\n1、简单  2、一般  3、困难  4、地狱  5、自定义  6、继续上次的游戏:\n");
        scanf("%d",&ques);while (getchar()!='\n');
    }
    
    char input,answer;int m;

    // 读取数据
    char (*data)[MAX]=NULL;
    if (ques>=1 && ques<=4){   
        data=read_text_file(questions[ques-1]);
    }
    else if(ques==5){    
        data=Custimize_map();
        printf("输入体力值(体力无穷输入0):\n");
        scanf("%llu",&total_time);
        while(getchar()!='\n');
        if (total_time==0) 
            total_time=4294967295;
    }
    else{    // 读取上次游戏存档
        data=read_text_file("data/上次的游戏.txt");  
        if(total_time==0) {
            printf("无上次存档的游戏！"); 
            goto W;   // 读取上次游戏的存档失败，跳转到“输入正确的问题难度”这一步
        }
    }
    // 如果无法读取数据
    if (!data) {
        printf("文件打开失败\n");
        return 1;
    }

    // 计算箱子的数量
    for (int i = 0; i < MAX; i++) {
        if (!data[i][0]) 
            break;
        for (int j=0;j<MAX;j++){
            if (!data[i][j]) 
                break;
            if (data[i][j]=='3') 
                box_number++;
        }
    }

    // 初始化历史记录
    history_top=-1;
    undone_top=-1;
    
    // 项目主体
    while (1){
        printf("\n实时地图:\n");
        print_data(data);
        printf("剩余的体力: %llu  剩余的箱子: %d \n(WASD: 移动, Q: 退出, Z: 撤销最近的一步操作, Y: 恢复撤销)\n",total_time-time,box_number);
        input=tolower(getchar());
        while (getchar()!='\n'); 

        m=move(input,data);  // 用户走了一步
        time++;              // 体力消耗 +1
        if (m==1) {
            box_number--;
        }
        if (box_number==0){
            printf("\n恭喜完成任务! 消耗的体力: %llu ",time);

            // “上次的游戏”模式如果成功，清空'data/上次的游戏.txt'中的数据
            if(ques==6)
                data_delete();
            break;
        }
        
        if (m==2){
            time--;    // 退出游戏的一步不消耗体力
            printf("退出游戏......\n");
            printf("是否保存游戏数据?(退出并保存游戏数据后,不能撤回上一步移动) Y-yes N-no:\n");
            answer=tolower(getchar());
            while (getchar() != '\n'); 
            while(answer!='y' && answer!='n'){
                printf("请正确输入是否保存游戏数据?(退出并保存游戏数据后,不能撤回上一步移动) Y-yes N-no:\n");
                answer=tolower(getchar());
                while (getchar()!='\n'); 
            }
            if (answer=='n'){
                if(ques==6)
                    data_delete();
                break;
            }
            else{
                data_memorize(data,time,total_time);
                break;
            }
        }

        // 错误输入不消耗体力
        if (m==3)   time--;

        if (time>=total_time) {
            print_data(data);
            printf("体力不足,游戏失败！");

            // 自定义模式如果失败，清空'data/上次的游戏.txt'中的数据
            if(ques==6)
                data_delete();
    
            break;
        }
    }
    return 0;
}




