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

status SudokuExist=NO;//�ñ���ָʾ�����Ƿ��ʼ��
int Sudoku[10][10];//����������ά�����У���0�пյ�
int dignumber[81][2];//���汣��Ҫ�ڵ������ֵ��к���
int changenumber[9][2];//�������ڸı������е�����
int chosen[9];//��ѡ�˵�����
int hole_num=0;//��ǰ��������
int all_hole_num=0;//�ܹ�Ҫ�ڵĶ�������
int SAT_table[81][10][2];//���ڴ������תSAT����Ϣ
int ziju_num=0;//�����cnf�ļ����Ӿ�����
int v_num=0;//�����cnf�ļ������������
int same_number[9];//������ʱ����ܴ�ͬһ�����ֵĿո���ͬһ���ֶ�Ӧ�����

status InitFinalSudoku(FILE *fp);//��ʼ������һ���վ�
void ShowSudoku(void);//����������
status MakeRandomSudoku(void);//����ʼ���˵��վֽ��иı䣬����������վ�
status ChangeNum(void);//��ԭ����������Ӧ�ı任
status isNumberChosen(int number);//�жϱ任���ֵ�ʱ�������Ƿ��Ѵ���������chosen��
status DigHole(void);//���Ѿ����ɵ����������ڶ�
status sudoku_to_SAT(void);//������ת��ΪSAT�������Ϣ����������
status Can_exist_in_line(int number, int i);//�ж�number�ڵ�i�����Ƿ��Ѵ��ڣ������򷵻�NO
status Can_exist_in_col(int number, int j);//�ж�number�ڵ�j�����Ƿ��Ѵ��ڣ������򷵻�NO
status Can_exist_in_square(int number,int i, int j);//�ж�number�ڿո������ľŹ����Ƿ��Ѿ����ڣ���������򷵻�NO
status SAT_to_cnf_file(FILE *fp, char *cnf_filename);//�����е�SAT��Ϣת����cnf�ļ�
status Solve_Sudoku(FILE *fp, char *res_filename);//��SAT��Ľ���������������


int main(void){
    FILE *fp=NULL;
    //�ֽ�����ȫ����Ϊ0
    int i,j,k;
    for(i=0;i<=9;i++)
    {
        for(j=0;j<=9;j++)
        {
            Sudoku[i][j]=0;
        }
    }
    //��same_number[9]����
    for(i=0;i<9;i++){
        same_number[i]=0;
    }
    //׼�������ֵ�ת��
    for(i=0;i<=8;i++){
        changenumber[i][0]=i+11;
        changenumber[i][1]=0;
    }
    //׼���ñ�ѡ���ֵĳ�ʼ��
    for(i=0;i<=8;i++){
        chosen[i]=0;
    }
    //׼���ñ������ֵ�������Ϣ��ʼ��
    for(i=0;i<=80;i++){
        dignumber[i][0]=0;
        dignumber[i][1]=0;
    }
    //��SAT_table����
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
	    printf("    ��ѡ����Ĳ���[0~12]:");
	    scanf("%d",&op);
        switch(op){
            case 1:
            {
                if(InitFinalSudoku(fp)==OK)
                    printf("��ȡ�����ɹ���\n");
                else
                    printf("��ȡ����ʧ�ܣ�\n");
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
                    printf("�������������ɣ�\n");
                    SudokuExist=TRUE;
                }
                else
                    printf("����ʧ�ܣ�\n");
                getchar();getchar();
                break;  
            }
            case 4:
            {
                if(DigHole()==INFEASTABLE) 
                    printf("������û��ʼ����\n");
                else
                    printf("�ڶ��ɹ���\n");
                getchar();getchar();
                break;
            }
            case 5:
            {
                if(sudoku_to_SAT()==TRUE)
                    printf("����תSAT����Ϣ�Ѿ���������ˣ�\n");
                else
                    printf("ת��ʧ�ܣ�\n");
                getchar();getchar();
                break;
            }
            case 6:
            {
                char cnf_filename[40];
                printf("������Ҫ�����cnf�ļ�������:");
                scanf("%s",cnf_filename);
                if(SAT_to_cnf_file(fp,cnf_filename)==OK)
                    printf("ת���ɹ���\n");
                else
                    printf("ת��ʧ�ܣ�");
                getchar();getchar();
                break;
            }
            case 7:
            {
                char res_filename[40];
                printf("������Ҫ������յ�res�ļ����ƣ�");
                scanf("%s",res_filename);
                if(Solve_Sudoku(fp,res_filename)==OK)
                    printf("�������ɹ���\n");
                else
                    printf("������ʧ�ܣ�\n");
                getchar();getchar();
                break;
            }
        }
    }
    return 0;
    
}

status InitFinalSudoku(FILE *fp){
    int number;//���ڶ�ȡ�ļ��е�����
    int i,j;//���ڱ���������ά������
    char filename[40];
    printf("��������ļ�������(initial_sudoku.txt)��");
    scanf("%s", filename);
    if((fp=fopen(filename,"r"))==NULL)//�Զ��ķ�ʽ���ļ�filename
    {
        return ERROR;//���ʧ���򱨴�
    }
    //��ȡ�ļ��еĳ�ʼ�վ�
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
    printf("�����ɣ�\n");
}

status ChangeNum(void){
    srand(time(NULL));//��ʼ�����������
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
    //�Ƚ�ÿ��Ԫ�ؼ�10��Ȼ����б任
    for(i=1;i<=9;i++){
        for(j=1;j<=9;j++){
            Sudoku[i][j]+=10;
        }
    }
    //��ʼ���б任
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
        srand(time(NULL));//��ʼ�����������
        int i,j,k;
        printf("��������Ҫ�ڵĶ���������");
        scanf("%d",&all_hole_num);
        for(k=0;k<all_hole_num;k++){
            i=rand()%9+1;//i��j���кź��к�
            j=rand()%9+1;
            //����ô��Ѿ����ڹ��˾ͻ�һ��û�ڹ���
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
    //���·�Ϊ�����������
    if((i>=1)&&(i<=3)){//��һ�еľŹ���
        if((j>=1)&&(j<=3)){//��һ���Ź���
            int m,n;
            for(m=1;m<=3;m++){
                for(n=1;n<=3;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=4)&&(j<=6)){//�ڶ����Ź���
            int m,n;
            for(m=1;m<=3;m++){
                for(n=4;n<=6;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=7)&&(j<=9)){//�������Ź���
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
    else if((i>=4)&&(i<=6)){//�ڶ��еľŹ���
        if((j>=1)&&(j<=3)){//���ĸ��Ź���
            int m,n;
            for(m=4;m<=6;m++){
                for(n=1;n<=3;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=4)&&(j<=6)){//������Ź���
            int m,n;
            for(m=4;m<=6;m++){
                for(n=4;n<=6;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=7)&&(j<=9)){//�������Ź���
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
    else if((i>=7)&&(i<=9)){//�����еľŹ���
        if((j>=1)&&(j<=3)){//���߸��Ź���
            int m,n;
            for(m=7;m<=9;m++){
                for(n=1;n<=3;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=4)&&(j<=6)){//�ڰ˸��Ź���
            int m,n;
            for(m=7;m<=9;m++){
                for(n=4;n<=6;n++){
                    if(Sudoku[m][n]==number)
                        return NO;
                }
            }
            return YES;
        }
        else if((j>=7)&&(j<=9)){//�������Ź���
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
    int i,j,m,n,number,xuhao;//i��j���������к��У�m��n��SAT_table���к���,number���ڳ��Կո�������Щ��
    m=0,n=0,xuhao=1;
    for(i=1;i<=9;i++){
        for(j=1;j<=9;j++){
            //�����������һ���ǿյľʹ�����������Щ��
            if(Sudoku[i][j]==0){
                SAT_table[m][n][0]=i;//�ո��������е��кź��к����ȥ
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
    v_num=xuhao;//���ֵ�����
    return OK;
}

status SAT_to_cnf_file(FILE *fp, char *cnf_filename){
    if((fp=fopen(cnf_filename,"w"))==NULL)//�Զ��ķ�ʽ���ļ�filename
    {
        return ERROR;//���ʧ���򱨴�
    }
    int i,j,m,n,number;
    for(i=1;i<=300;i++){
        fprintf(fp, " ");
    }
    fprintf(fp, "\n");
    //���������һ���ո���������������
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
    //���������һ�ж���������ͬ���ֵĿո�����ͬ���ֵ����
    for(i=1;i<=9;i++){
        for(number=1;number<=9;number++){//����һ�����ֶ��ԣ��ҵ�����������ֵĿո�
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
    //���������һ�ж��������������ͬ�Ŀո�����ͬ���ֵ����
     for(j=1;j<=9;j++){
        for(number=1;number<=9;number++){//����һ�����ֶ��ԣ��ҵ�����������ֵĿո�
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
    //���������һ���Ź������������ͬ���ֵĿո�����ͬ���ֵ����
    for(number=1;number<=9;number++){
        int k,k_k;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //��һ���Ź���
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
                //��2���Ź���
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
                //��3���Ź���
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
                //��4���Ź���
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
                //��5���Ź���
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
                //��6���Ź���
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
                //��7���Ź���
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
                //��8���Ź���
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
                //��9���Ź���
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

    //�������ͬ����������ͬ���ֵĿո�ÿ������ֻ����һ���ո��г���
    for(i=1;i<=9;i++){
        for(number=1;number<=9;number++){//����һ�����ֶ��ԣ��ҵ�����������ֵĿո�
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
    //�������ͬ����������ͬ���ֵĿո�ÿ������ֻ����һ���ո��г���
    for(j=1;j<=9;j++){
        for(number=1;number<=9;number++){//����һ�����ֶ��ԣ��ҵ�����������ֵĿո�
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
    //�����ͬһ���Ź���������ͬһ���ֵĿո���ͬһ����ֻ����һ���ط�����
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //��1���Ź���
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
    //��2���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //��2���Ź���
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
    //��3���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=1)&&(SAT_table[m][n][0]<=3)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //��3���Ź���
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
    //��4���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //��4���Ź���
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
    //��5���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //��5���Ź���
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
    //��6���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=4)&&(SAT_table[m][n][0]<=6)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //��6���Ź���
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
    //��7���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=1)&&(SAT_table[m][n][1]<=3)){
                //��7���Ź���
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
    //��8���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=4)&&(SAT_table[m][n][1]<=6)){
                //��8���Ź���
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
    //��9���Ź���
    for(number=1;number<=9;number++){
        int k,k_k,start,end,middle;
        k=0,k_k=0;
        for(m=0;m<all_hole_num;m++){
            if((SAT_table[m][n][0]>=7)&&(SAT_table[m][n][0]<=9)&&(SAT_table[m][n][1]>=7)&&(SAT_table[m][n][1]<=9)){
                //��9���Ź���
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
    //���������ÿ���ո�ֻ����һ�������ȥ
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
    //���潫ǰ���е�ע�ʹ�ӡ����
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
    if((fp=fopen(res_filename,"r"))==NULL)//�Զ��ķ�ʽ���ļ�filename
    {
        return ERROR;//���ʧ���򱨴�
    }
    char s[100];
    fgets(s,100,fp);
    fgets(s,2,fp);
    int m,n,i,j,xuhao;//m,n��SAT_table�����кš�i,j�����������к�
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

