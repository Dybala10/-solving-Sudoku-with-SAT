#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define ERROR 0
#define OK 1
#define YES 1
#define NO 0
#define TRUE 1
#define FALSE 0
#define INFEASTABLE -1
#define random(x) (rand()%x)

typedef int status;

status SudokuExist=NO;//该变量指示数独是否初始化
int Sudoku[10][10];//数独存至二维数组中，第0行空掉
int dignumber[81][2];//里面保存要挖掉的数字的行和列
int changenumber[9][2];//数组用于改变数独中的数字
int chosen[9];//被选了的数字
int hole_num=0;//当前洞的数量
int all_hole_num=0;//总共要挖的洞的数量
int SAT_table[81][10][2];//用于存放数独转SAT的信息
int ziju_num=0;//输出成cnf文件的子句数量
int v_num=0;//输出成cnf文件后的文字数量
int same_number[9];//用于暂时存放能存同一个数字的空格中同一数字对应的序号

status InitFinalSudoku(FILE *fp);//初始化生成一个终局
void ShowSudoku(void);//输出数独格局
status MakeRandomSudoku(void);//将初始化了的终局进行改变，生成随机的终局
status ChangeNum(void);//将原有数独做相应的变换
status isNumberChosen(int number);//判断变换数字的时候数字是否已存在于数组chosen中
status DigHole(void);//对已经生成的数独进行挖洞
status sudoku_to_SAT(void);//将数独转换为SAT问题的信息存至数组中
status Can_exist_in_line(int number, int i);//判断number在第i行中是否已存在，存在则返回NO
status Can_exist_in_col(int number, int j);//判断number在第j列中是否已存在，存在则返回NO
status Can_exist_in_square(int number,int i, int j);//判断number在空格所处的九宫格是否已经存在，如果存在则返回NO
status SAT_to_cnf_file(FILE *fp, char *cnf_filename);//将表中的SAT信息转换成cnf文件
status Solve_Sudoku(FILE *fp, char *res_filename);//将SAT解的结果读至数独中填空


int main(void){
    FILE *fp=NULL;
    //现将数独全部置为0
    int i,j,k;
    for(i=0;i<=9;i++)
    {
        for(j=0;j<=9;j++)
        {
            Sudoku[i][j]=0;
        }
    }
    //将same_number[9]置零
    for(i=0;i<9;i++){
        same_number[i]=0;
    }
    //准备好数字的转化
    for(i=0;i<=8;i++){
        changenumber[i][0]=i+11;
        changenumber[i][1]=0;
    }
    //准备好被选数字的初始化
    for(i=0;i<=8;i++){
        chosen[i]=0;
    }
    //准备好被挖数字的行列信息初始化
    for(i=0;i<=80;i++){
        dignumber[i][0]=0;
        dignumber[i][1]=0;
    }
    //将SAT_table置零
    for(i=0;i<81;i++){
        for(j=0;j<10;j++){
            SAT_table[i][j][0]=0;
            SAT_table[i][j][1]=0;
        }
    }

    int op=1;
    while(op)
    {
        system("cls");	printf("\n\n");
	    printf("      Menu for Sudoku Structure \n");
	    printf("-------------------------------------------------\n");
	    printf("    	  1. InitFinalSudoku            \n");
        printf("          2.ShowSudoku     \n");
        printf("          3.MakeRandomSudoku   \n");
        printf("          4.DigHole      \n");
        printf("          5.Sudoku_to_SAT   \n");
        printf("          6.SAT_to_cnf    \n");
        printf("          7.SolveSudoku    \n");
	    printf("    	  0. Exit          \n");
    	printf("-------------------------------------------------\n");
	    printf("    请选择你的操作[0~12]:");
	    scanf("%d",&op);
        switch(op){
            case 1:
            {
                if(InitFinalSudoku(fp)==OK)
                    printf("读取数独成功！\n");
                else
                    printf("读取数独失败！\n");
                getchar();getchar();
                break;
            }
            case 2:
            {
                ShowSudoku();
                getchar();getchar();
                break;
            }
            case 3:
            {
                if(MakeRandomSudoku()==OK)
                {
                    printf("随机数独生成完成！\n");
                    SudokuExist=TRUE;
                }
                else
                    printf("生成失败！\n");
                getchar();getchar();
                break;  
            }
            case 4:
            {
                if(DigHole()==INFEASTABLE) 
                    printf("数独还没初始化！\n");
                else
                    printf("挖洞成功！\n");
                getchar();getchar();
                break;
            }
            case 5:
            {
                if(sudoku_to_SAT()==TRUE)
                    printf("数独转SAT的信息已经存入表中了！\n");
                else
                    printf("转化失败！\n");
                getchar();getchar();
                break;
            }
            case 6:
            {
                char cnf_filename[40];
                printf("请输入要保存的cnf文件的名字:");
                scanf("%s",cnf_filename);
                if(SAT_to_cnf_file(fp,cnf_filename)==OK)
                    printf("转化成功！\n");
                else
                    printf("转化失败！");
                getchar();getchar();
                break;
            }
            case 7:
            {
                char res_filename[40];
                printf("请输入要用于填空的res文件名称：");
                scanf("%s",res_filename);
                if(Solve_Sudoku(fp,res_filename)==OK)
                    printf("解数独成功！\n");
                else
                    printf("解数独失败！\n");
                getchar();getchar();
                break;
            }
        }
    }
    return 0;
    
}

status InitFinalSudoku(FILE *fp){
    int number;//用于读取文件中的数据
    int i,j;//用于遍历读至二维数组中
    char filename[40];
    printf("请输入根文件的名字(initial_sudoku.txt)：");
    scanf("%s", filename);
    if((fp=fopen(filename,"r"))==NULL)//以读的方式打开文件filename
    {
        return ERROR;//如果失败则报错
    }
    //读取文件中的初始终局
    for(i=1;i<=9;i++)
    {
        for(j=1;j<=9;j++)
        {
            fscanf(fp, "%d", &number);
            Sudoku[i][j]=number;
        }
    }
    fclose(fp);
    return OK;
}

void ShowSudoku(void){
    int i,j;
    for(i=1;i<=9;i++){
        for(j=1;j<=9;j++){
            printf("%d ",Sudoku[i][j]);
        }
        printf("\n");
    }
    printf("输出完成！\n");
}

status ChangeNum(void){
    srand(time(NULL));//初始化随机数种子
    int i,number;
    for(i=0;i<=8;i++){
        number=rand()%9+1;
        if(isNumberChosen(number)==NO)
            chosen[i]=number;
        else
        {
            do
            {
                number=random(9)+1;
            } while (isNumberChosen(number)==YES);
            chosen[i]=number;
        }
        changenumber[i][1]=chosen[i];
    }
    return OK;
}

status isNumberChosen(int number){
    int i=0;
    while((chosen[i]!=0)&&(i<=8)){
        if(chosen[i]==number)
            return YES;
        i++;
    }
    return NO;
}

status MakeRandomSudoku(void){
    int i,j,k,number;
    ChangeNum();
    //先将每个元素加10，然后进行变换
    for(i=1;i<=9;i++){
        for(j=1;j<=9;j++){
            Sudoku[i][j]+=10;
        }
    }
    //开始进行变换
    for(k=0;k<=8;k++){
        number=changenumber[k][0];
        for(i=1;i<=9;i++){
            for(j=1;j<=9;j++){
                if(Sudoku[i][j]==number)
                    Sudoku[i][j]=changenumber[k][1];
            }
        }
    }
    return OK;
}

status DigHole(void){
    if(SudokuExist==NO)
        return INFEASTABLE;
    else
    {
        srand(time(NULL));//初始化随机数种子
        int i,j,k;
        printf("请输入你要挖的洞的数量：");
        scanf("%d",&all_hole_num);
        for(k=0;k<all_hole_num;k++){
            i=rand()%9+1;//i和j是行号和列号
            j=rand()%9+1;
            //如果该处已经被挖过了就换一个没挖过的
            while(Sudoku[i][j]==0){
                i=rand()%9+1;
                j=rand()%9+1;
            }
            dignumber[k][0]=i;
            dignumber[k][1]=j;
            Sudoku[i][j]=0;
            hole_num++;
        }
        return OK;
    }
}


status Can_exist_in_line(int number, int i){
    int j;
    for(j=1;j<=9;j++){
        if(Sudoku[i][j]==number)
            return NO;
    }
    return YES;
}

status Can_exist_in_col(int number, int j){
    int i;
    for(i=1;i<=9;i++){
        if(Sudoku[i][j]==number)
            return NO;
    }
    return YES;
}

status Can_exist_in_square(int number,int i, int j){
    //以下分为九种情况讨论
    if((i>=1)&&(i<=3)){//第一行的九宫格
        if((j>=1)&&(j<=3)){//第一个九宫格
            int m,n;
            for(m=1;m<=3;m++){
                for(n=1;n<=3;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=4)&&(j<=6)){//第二个九宫格
            int m,n;
            for(m=1;m<=3;m++){
                for(n=4;n<=6;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=7)&&(j<=9)){//第三个九宫格
            int m,n;
            for(m=1;m<=3;m++){
                for(n=7;n<=9;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
    }
    else if((i>=4)&&(i<=6)){//第二行的九宫格
        if((j>=1)&&(j<=3)){//第四个九宫格
            int m,n;
            for(m=4;m<=6;m++){
                for(n=1;n<=3;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=4)&&(j<=6)){//第五个九宫格
            int m,n;
            for(m=4;m<=6;m++){
                for(n=4;n<=6;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=7)&&(j<=9)){//第六个九宫格
            int m,n;
            for(m=4;m<=6;m++){
                for(n=7;n<=9;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
    }
    else if((i>=7)&&(i<=9)){//第三行的九宫格
        if((j>=1)&&(j<=3)){//第七个九宫格
            int m,n;
            for(m=7;m<=9;m++){
                for(n=1;n<=3;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=4)&&(j<=6)){//第八个九宫格
            int m,n;
            for(m=7;m<=9;m++){
                for(n=4;n<=6;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=7)&&(j<=9)){//第六个九宫格
            int m,n;
            for(m=7;m<=9;m++){
                for(n=7;n<=9;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
    }
}

status sudoku_to_SAT(void){
    int i,j,m,n,number,xuhao;//i和j是数独的行和列，m和n是SAT_table的行和列,number用于尝试空格能填哪些数
    m=0,n=0,xuhao=1;
    for(i=1;i<=9;i++){
        for(j=1;j<=9;j++){
            //如果数独的这一格是空的就存下来能填哪些数
            if(Sudoku[i][j]==0){
                SAT_table[m][n][0]=i;//空格在数独中的行号和列号填进去
                SAT_table[m][n][1]=j;
                n++;
                for(number=1;number<=9;number++){
                    if(Can_exist_in_line(number,i)&&Can_exist_in_col(number,j)&&Can_exist_in_square(number,i,j)){
                        SAT_table[m][n][0]=number;
                        SAT_table[m][n][1]=xuhao;
                        xuhao++;
                        n++;
                    }
                }
                m++;
                n=0;
            }
        }
    }
    xuhao--;
    v_num=xuhao;//文字的数量
    return OK;
}

status SAT_to_cnf_file(FILE *fp, char *cnf_filename){
    if((fp=fopen(cnf_filename,"w"))==NULL)//以读的方式打开文件filename
    {
        return ERROR;//如果失败则报错
    }
    int i,j,m,n,number;
    for(i=1;i<=300;i++){
        fprintf(fp, " ");
    }
    fprintf(fp, "\n");
    //先输入对于一个空格而言所有能填的数
    for(m=0;m<all_hole_num;m++){
        n=1;
        while(SAT_table[m][n][0]!=0)
        {
            fprintf(fp, "%d ", SAT_table[m][n][1]);
            n++;
            if(SAT_table[m][n][0]==0){
                fprintf(fp, "0\n");
                ziju_num++;
            }
        }
    }
    m=0,n=0;
    //再输入对于一行而言能填相同数字的空格中相同数字的序号
    for(i=1;i<=9;i++){
        for(number=1;number<=9;number++){//对于一个数字而言，找到能填这个数字的空格
            int k,k_k;
            k=0,k_k=0;
            for(m=0;m<all_hole_num;m++){
                if(SAT_table[m][n][0]==i){
                    n++;
                    while(SAT_table[m][n][0]!=0){
                        if(SAT_table[m][n][0]==number){
                            same_number[k]=SAT_table[m][n][1];
                            k++;
                            break;
                        }
                        n++;
                    }
                }
                n=0;
            }
            if(k==0)
                continue;
            for(k_k=0;k_k<k;k_k++){
                fprintf(fp,"%d ",same_number[k_k]);
            }
            fprintf(fp, "0\n");
            ziju_num++;
        }
    }
    //再输入对于一列而言能填的数字相同的空格中相同数字的序号
     for(j=1;j<=9;j++){
        for(number=1;number<=9;number++){//对于一个数字而言，找到能填这个数字的空格
            int k,k_k;
            k=0,k_k=0;
            for(m=0;m<all_hole_num;m++){
                if(SAT_table[m][n][1]==j){
                    n++;
                    while(SAT_table[m][n][0]!=0){
                        if(SAT_table[m][n][0]==number){
                            same_number[k]=SAT_table[m][n][1];
                            k++;
                            break;
                        }
                        n++;
                    }
                }
                n=0;
            }
            if(k==0)
                continue;
            for(k_k=0;k_k<k;k_k++){
                fprintf(fp,"%d ",same_number[k_k]);
            }
            fprintf(fp, "0\n");
            ziju_num++;
        }
    }
    //再输出对于一个九宫格而言能填相同数字的空格中相同数字的序号
    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //第一个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //第2个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //第3个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //第4个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //第5个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //第6个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //第7个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //第8个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //第9个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        if(k==0)
            continue;
        for(k_k=0;k_k<k;k_k++){
            fprintf(fp,"%d ",same_number[k_k]);
        }
        fprintf(fp, "0\n");
        ziju_num++;
    }

    //下面输出同行中能填相同数字的空格，每个数字只能在一个空格中出现
    for(i=1;i<=9;i++){
        for(number=1;number<=9;number++){//对于一个数字而言，找到能填这个数字的空格
            int k,k_k,start,end,middle;
            k=0,k_k=0;
            for(m=0;m<all_hole_num;m++){
                if(SAT_table[m][n][0]==i){
                    n++;
                    while(SAT_table[m][n][0]!=0){
                        if(SAT_table[m][n][0]==number){
                            same_number[k]=SAT_table[m][n][1];
                            k++;
                            break;
                        }
                        n++;
                    }
                }
                n=0;
            }
            end=k;
            if((k==0)&&(k==1))
                continue;
            for(start=0;start<end;start++){
                middle=start+1;
                for(;middle<end;middle++){
                    fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                    ziju_num++;
                }
            }
        }
    }
    //下面输出同列中能填相同数字的空格，每个数字只能在一个空格中出现
    for(j=1;j<=9;j++){
        for(number=1;number<=9;number++){//对于一个数字而言，找到能填这个数字的空格
            int k,k_k,start,end,middle;
            k=0,k_k=0;
            for(m=0;m<all_hole_num;m++){
                if(SAT_table[m][n][1]==j){
                    n++;
                    while(SAT_table[m][n][0]!=0){
                        if(SAT_table[m][n][0]==number){
                            same_number[k]=SAT_table[m][n][1];
                            k++;
                            break;
                        }
                        n++;
                    }
                }
                n=0;
            }
            end=k;
            if((k==0)&&(k==1))
                continue;
            for(start=0;start<end;start++){
                middle=start+1;
                for(;middle<end;middle++){
                    fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                    ziju_num++;
                }
            }
        }
    }
    //再输出同一个九宫格中能填同一数字的空格中同一数字只能在一个地方出现
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //第1个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第2个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //第2个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第3个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //第3个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第4个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //第4个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第5个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //第5个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第6个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //第6个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第7个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //第7个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第8个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //第8个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //第9个九宫格
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //第9个九宫格
                n++;
                while(SAT_table[m][n][0]!=0){
                    if(SAT_table[m][n][0]==number){
                        same_number[k]=SAT_table[m][n][1];
                        k++;
                        break;
                    }
                    n++;
                }
            }
            n=0;
        }
        end=k;
        if((k==0)&&(k==1))
            continue;
        for(start=0;start<end;start++){
            middle=start+1;
            for(;middle<end;middle++){
                fprintf(fp, "%d %d 0\n",-same_number[start], -same_number[middle]);
                ziju_num++;
            }
        }
    }
    //再输出对于每个空格，只能有一个数填进去
    for(m=0;m<all_hole_num;m++){
        n=0;
        int start=1,end=start,middle;
        while(SAT_table[m][end][0]!=0){
            end++;
        }
        if(end==2){
            fprintf(fp,"%d 0\n",SAT_table[m][1][1]);
            ziju_num++;
        }
        for(start=1;start<end;start++){
            for(middle=start+1;middle<end;middle++){
                fprintf(fp, "%d %d 0\n", -SAT_table[m][start][1], -SAT_table[m][middle][1]);
                ziju_num++;
            }
        }
    }
    //下面将前几行的注释打印出来
    rewind(fp);
    fprintf(fp, "c sudoku, order: 9, holes: %d\n", all_hole_num);
    fprintf(fp, "c \n");
    for(i=1;i<=9;i++){
        fprintf(fp, "c ");
        for(j=1;j<=9;j++){
            if(Sudoku[i][j]!=0){
                fprintf(fp,"%d",Sudoku[i][j]);
            }
            else{
                fprintf(fp,".");
            }
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"c \n");
    fprintf(fp,"p cnf %d %d\n", v_num, ziju_num);
    fclose(fp);
    return OK;
}

status Solve_Sudoku(FILE *fp, char *res_filename){
    if((fp=fopen(res_filename,"r"))==NULL)//以读的方式打开文件filename
    {
        return ERROR;//如果失败则报错
    }
    char s[100];
    fgets(s,100,fp);
    fgets(s,2,fp);
    int m,n,i,j,xuhao;//m,n是SAT_table的行列号、i,j是数独的行列号
    while(fscanf(fp,"%d", &xuhao)&&(xuhao!=0)){
        if(xuhao>0){
            for(m=0;m<=all_hole_num;m++){
                for(n=1;n<=9;n++){
                    if(SAT_table[m][n][1]==xuhao){
                        i=SAT_table[m][0][0];
                        j=SAT_table[m][0][1];
                        Sudoku[i][j]=SAT_table[m][n][0];
                        break;
                    }
                    else if(SAT_table[m][n][1]==0){
                        break;
                    }
                }
            }
        }
    }
    fclose(fp);
    return OK;
}

