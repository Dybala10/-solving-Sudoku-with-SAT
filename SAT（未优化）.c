#include <stdio.h>
#include<time.h>
#include<string.h>
#define OK 1
#define FALSE 0
#define TRUE 1
#define ERROR 0
#define INFEASTABLE -1
#define YES 1
#define NO 0

typedef int status;
typedef int ELemType;//变元的命名
//一个变元放在一个节点中
typedef struct Node{
    ELemType data;//节点包含数据域
    struct Node* next;//指针域
    status is_node_hidden;//状态：节点是否被删除
}Node;

typedef struct HeadNode{
    int data;//数据域存放子句的长度
    int all_data;//该子句初始状态的长度
    struct HeadNode* down;//下一个子句
    struct Node* next;//指向该子句的第一个节点
    status is_headnode_hidden;//状态：该子句是否被删除
    ELemType why;//如果子句被删除，则记录第一次被删除是因为哪个文字。若未被删除则为0
}HeadNode;

int ziju_num=0;//当前子句的个数
int all_ziju_num=0;//电脑中的子句总数，由于删除只是隐藏，因此不会把子句真正删掉
int Stack[10000][2];//栈第一列用于存放满足的文字，第二列存放changed的状态
int v_num;//变元数
clock_t start, stop;//用于统计DPLL所用时间
long duration;//duration是DPLL花费时间


status CreateClause(HeadNode** root, HeadNode** tail);//创造子句集
status DestroyClause(HeadNode** root, HeadNode** tail);//删除子句集(这个是真的删除，会free)
status AddClause(HeadNode** root, HeadNode** tail);//添加子句
status RemoveClause(HeadNode** root, HeadNode** tail, int n);//删除第n个子句
void Show_ziju_num(void);//显示子句个数
void ShowAllClause(HeadNode *root);//展示所有的子句
status isUnitClause(HeadNode *root);//判断子句集中是否存在单子句
int firstUnitClause(HeadNode *root);//返回子句集中第一个单子句的序号(包含已隐藏的)
int Special_lit_place(HeadNode *p, int lit);//找到当前子句中第一个为lit的文字的位置(包含隐藏的节点)，没有则返回0
status Dele_special_lit(HeadNode**root,int lit);//在所有子句中删除指定的文字
status isBlankClause(HeadNode *root);//判断子句集中是否有空子句
int firstBlankClause(HeadNode* root);//返回子句集中第一个空子句的序号(包含被隐藏的子句)
status isSetBlank(void);//判断子句集是否为空
status DPLL(HeadNode *root, HeadNode* tail);//递归的DPLL算法
status RemoveSpecialClause(HeadNode **root, HeadNode** tail, int L);//删除包含文字L的子句
status Recover(HeadNode **root,ELemType v);//恢复包含v的子句，以及包含-v的节点
status is_lit_in_clause(HeadNode *p, ELemType lit);//判断lit是否在子句中(被隐藏了也算在里面！！！！！)
status Read_to_Create(FILE *fp, char *filename, HeadNode **root, HeadNode **tail);//读取cnf文件并且创建子句集
status SaveFile(FILE *fp, char *filename, int duration, int ans);//将DPLL解析的信息存入文件中
void Analyze_cnf(HeadNode* root);//对cnf文件进行解析
status Validate_ans(HeadNode *root, HeadNode *tail);//用栈中的变元验证cnf文件

int main(void)
{
    int s=0;
    FILE *fp =NULL;
    char filename[40];
    //将栈初始化
    for(; s<100;s++)
    {
        Stack[s][0]=0;
        Stack[s][1]=NO;
    }
    HeadNode* root = NULL;
    HeadNode* tail =NULL;
    HeadNode** root_p=&root;
    HeadNode** tail_p=&tail;
    int op=1;
    int choice;//用于选择是否打印出文件
    while(op)
    {
        system("cls");	printf("\n\n");
	    printf("      Menu for CNF-SAT Table On Sequence Structure \n");
	    printf("-------------------------------------------------\n");
	    printf("    	  1. CreateClause            9.Dele_special_lit\n");
	    printf("    	  2. DestroyList             10.isBlankClause \n");
        printf("          3. AddClause               11.isSetBlank\n ");
        printf("         4. RemoveClause            12.AddUnitClause \n");
        printf("          5. ShowClauseNum           13.DPLL \n");
        printf("          6. ShowAllClause           14.RemoveSpecialClause\n");
        printf("          7.isUnitClause             15.Recover\n");
        printf("    	  17.Analyze_file            18.Validate_ans\n");
        printf("          8.firstUnitClause          16.Read_to_Create\n");
	    printf("    	  0. Exit          \n");
    	printf("-------------------------------------------------\n");
	    printf("    请选择你的操作[0~16]:");
	    scanf("%d",&op);
        switch(op){
            case 1:
            {
                printf("请输入子句的个数");
                scanf("%d", &ziju_num);
                all_ziju_num=ziju_num;//给all_ziju_num赋初值
                if(CreateClause(root_p,tail_p)==OK)
                    printf("子句集创建成功！\n");
                else if(root!=NULL)
                    printf("创建失败！子句集已经存在了！\n");
                 getchar();getchar();
                 break;
            }
            case 2:
            {
                if(DestroyClause(root_p, tail_p)==OK)
                    printf("子句集销毁成功！\n");
                else if(!root)
                    printf("销毁失败！子句集尚未创建！\n");
                getchar();getchar();
                break;
            }
            case 3:
            {
                if(AddClause(root_p,tail_p)==OK)
                    printf("添加子句成功!\n");
                else
                    printf("添加子句失败!\n");
                getchar();getchar();
                break;
            }
            case 4:
            {
                int n;
                printf("请输入要删除的子句的序号:");
                scanf("%d",&n);
                if(!root)
                    printf("删除失败!子句集尚未初始化！\n");
                else if(RemoveClause(root_p,tail_p,n)==OK)
                    printf("删除子句成功！\n");
                else
                    printf("删除失败！\n");
                getchar();getchar();
                break;
            }
            case 5:
            {
                Show_ziju_num();
                getchar();getchar();
                break;
            }
            case 6:
            {
                if(!root)
                {
                    printf("子句集尚未初始化！\n");
                    getchar();getchar();
                }
                else
                {
                    ShowAllClause(*root_p);
                    getchar();getchar();
                }
                break;
            }
            case 7:
            {
                if(!root)  printf("子句集尚未初始化！\n");
                else if(isUnitClause(*root_p)==TRUE)
                    printf("存在单子句！\n");
                else 
                    printf("不存在单子句！\n");
                getchar();getchar();
                break;
            }
            case 8:
            {
                if(isUnitClause(*root_p)==TRUE)
                    printf("第一个单子句的序号是%d！\n", firstUnitClause(*root_p));
                else 
                    printf("不存在单子句！\n");
                getchar();getchar();
                break;
            }
            case 9:
            {
                int lit;
                printf("请输入要删除的文字:");
                scanf("%d", &lit);
                if(Dele_special_lit(root_p, lit)==OK)
                    printf("删除成功！\n");
                else
                    printf("删除失败！\n");
                getchar();getchar();
                break;
            }
            case 10:
            {
                if(isBlankClause(root)==TRUE)
                    printf("存在空子句！\n");
                else
                    printf("不存在空子句！\n");
                getchar();getchar();
                break;
            }
            case 11:
            {
                if(isSetBlank()==TRUE)
                    printf("子句集为空！\n");
                else
                    printf("子句集非空！\n");
                getchar();getchar();
                break;
            }
            case 13:
            {
                int ans;
                start = clock();//开始计时
                ans=DPLL(root, tail);
                if(ans==TRUE)
                {
                    printf("子句集可满足！\n");
                    printf("文字的赋值为:\n");
                    int i=0;
                    while(Stack[i][0]!=0)
                    {
                        printf("%d\n",Stack[i][0]);
                        i++;
                    }
                    printf("\n其余的随意！\n");
                }
                else
                    printf("子句集不可满足！\n");
                stop=clock();
                duration = ((long)(stop - start));
                printf("花费时间:%ldms!\n",duration);
                getchar();getchar();
                printf("是否将本次运行的信息存至文件中？(按1保存，按0不保存)");
                scanf("%d", &choice);
                if(choice==1)
                {
                    printf("请输入一个文件名，将本次运行的信息保存进去: ");
                    scanf("%s",filename);
                    if(SaveFile(fp,filename,duration,ans)==TRUE)
                        printf("保存成功！\n");
                    else
                        printf("保存失败！\n");
                }
                else
                    printf("你说不存就不存吧\n");
                getchar();getchar();
                break;
            }
            case 14:{
                int L;
                printf("请输入文字L，将删除含有L的子句：");
                scanf("%d",&L);
                RemoveSpecialClause(root_p,tail_p,L);
                printf("删除成功！\n");
                getchar();getchar();
                break;
            }
            case 15:{
                ELemType v;
                printf("请输入文字v，用于恢复之前处理v带来的影响\n");
                scanf("%d",&v);
                if(Recover(root_p,v)==OK)
                    printf("恢复成功！\n");
                getchar();getchar();
                break;
            }
            case 16:{
                printf("请输入文件名以读取！\n");
                scanf("%s",filename);//将一个指定的链表存到filename中
                if(Read_to_Create(fp,filename,root_p, tail_p)==OK)
                    printf("文件读取成功！\n");
                else
                    printf("文件读取失败！\n");
                getchar();getchar();
                break;
            }
            case 17:
            {
                printf("文件解析如下：\n");
                Analyze_cnf(root);
                printf("解析完成!\n");
                getchar();getchar();
                break;
            }
            case 18:
            {
                if(Validate_ans(root, tail)==OK)
                    printf("\n验证结果为满足！\n");
                else
                    printf("\n验证结果为不满足！\n");
                getchar();getchar();
                break;
            }
        }//end of switch
    }//end of while
    
    return 0;
}

status CreateClause(HeadNode** root, HeadNode** tail){
    if(*root != NULL)
        return INFEASTABLE;
    else
    {
        *root = (HeadNode*)malloc(sizeof(HeadNode));
        (*root)->next=NULL;
        (*root)->is_headnode_hidden=NO;
        (*root)->why=0;
        HeadNode* p=*root;
        Node *q=NULL;
        int i;//用于初始化多个链表
        int number;//用于读取输入
        //创建头结点
        for( i=1 ;i<ziju_num;i++)
        {
            p->data=0;
            p->all_data=0;
            p->down=(HeadNode*)malloc(sizeof(HeadNode));
            p=p->down;
            p->next=NULL;
            p->is_headnode_hidden=NO;
            p->why=0;
        }
        p->data=0;//最后一个子句初始长度为0
        p->all_data=0;
        *tail=p;
        p->down = NULL;
        //输入每个链表的信息
        p=*root;
        for(i=1;i<=ziju_num;i++)
        {
            if(scanf("%d", &number)&&number==0)
            {
                p->next=NULL;
                p=p->down;
            }
            else
            {
                q=(Node*)malloc(sizeof(Node));
                q->data=number;
                q->is_node_hidden=NO;
                p->data++;
                p->all_data++;
                p->next=q;
                while(scanf("%d",&number)&&(number!=0))
                {
                    q->next=(Node*)malloc(sizeof(Node));
                    q=q->next;
                    q->is_node_hidden=NO;
                    q->data=number;
                    p->data++;
                    p->all_data++;
                }
                q->next=NULL;
                p=p->down;
            }
        }
        return OK;
    }
}

status DestroyClause(HeadNode** root, HeadNode** tail){
    if(!(*root))  return INFEASTABLE;
    else
    {
        *root = NULL;
        free(*root);
        *tail=NULL;
        free(*tail);
        ziju_num=0;
        all_ziju_num=0;
        return OK;
    }
}
status AddClause(HeadNode** root, HeadNode**tail){
    Node* q=NULL;
    HeadNode* p=NULL;
    int number;
    ziju_num++;
    all_ziju_num++;
    printf("请输入要添加的子句:\n");
    if(!(*root))//还没有初始化
    {
        *root=(HeadNode*)malloc(sizeof(HeadNode));
        *tail=*root;
        (*tail)->data=0;
        (*tail)->all_data=0;
        (*tail)->is_headnode_hidden=NO;
        int number;//用于读取输入
        if(scanf("%d", &number)&&number==0)
        {
            (*tail)->next=NULL;
            (*tail)->down=NULL;
        }
        else
        {
            q=(Node*)malloc(sizeof(Node));
            q->data=number;
            q->is_node_hidden=NO;
            (*tail)->data++;
            (*tail)->all_data++;
            (*tail)->next=q;
            while(scanf("%d",&number)&&(number!=0))
            {
                q->next=(Node*)malloc(sizeof(Node));
                q=q->next;
                q->data=number;
                q->is_node_hidden=NO;
                (*tail)->data++;
                (*tail)->all_data++;
            }
            q->next=NULL;
            (*tail)->down=NULL;
        }
        return OK;
    }
    else//已经初始化
    {
        p=(HeadNode*)malloc(sizeof(HeadNode));
        (*tail)->down=p;
        *tail=p;
        (*tail)->data=0;
        (*tail)->all_data=0;
        (*tail)->is_headnode_hidden=NO;
        if(scanf("%d", &number)&&number==0)
        {
            (*tail)->next=NULL;
            (*tail)->down=NULL;
        }
        else
        {
            q=(Node*)malloc(sizeof(Node));
            q->data=number;
            q->is_node_hidden=NO;
            (*tail)->data++;
            (*tail)->next=q;
            (*tail)->all_data++;
            while(scanf("%d",&number)&&(number!=0))
            {
                q->next=(Node*)malloc(sizeof(Node));
                q=q->next;
                q->data=number;
                q->is_node_hidden=NO;
                (*tail)->data++;
                (*tail)->all_data++;
            }
            q->next=NULL;
            (*tail)->down=NULL;
        }
        return OK;
    }
}

status RemoveClause(HeadNode** root, HeadNode** tail, int n){
    int i;//n是要删除的子句的序号，i用于找到那个子句
    HeadNode* p=*root;//用于找到那个子句的头指针
    HeadNode* q=*root;
    if(n>all_ziju_num)
        printf("该子句不存在！\n");
    else if((n>1)&&(n!=all_ziju_num))//要删除的不是最后一个子句！！！！这里不能直接写n>1，而且都要写成all
    {
        for(i=1;i<n;i++)
        {
            p=p->down;//p是要删除的子句
        }
        p->is_headnode_hidden=YES;
        ziju_num--;
        return OK;
    }
    else if((n==1)&&(n!=all_ziju_num))
    {
        p->is_headnode_hidden=YES;
        ziju_num--;
        return OK;
    }
    else if((n==all_ziju_num)&&(n!=1))//删除的是最后一个子句
    {
        for(i=1;i<n;i++)
        {
            p=p->down;//p是要删除的子句
        }//p是最后一个子句
        ziju_num--;
        p->is_headnode_hidden=YES;
        return OK;
    }
    else if((n==all_ziju_num)&&(n==1))//只剩一个子句的时候
    {
        (*root)->is_headnode_hidden=YES;
        ziju_num--;
        return OK;
    }
    else
        return ERROR;
}

void Show_ziju_num(void){
    printf("子句的数目为%d",ziju_num);
}

void ShowAllClause(HeadNode *root){
    int i=1,j=1;
    HeadNode* p=root;
    Node* q=p->next;//不能写NULL->next!!!!!!!!!!!!
    printf("总共有%d个子句\n",ziju_num);
    if(ziju_num>0)
        for(;i<=all_ziju_num;i++)
        {
            if(p->is_headnode_hidden==NO)
            {
                printf("第%d个子句: ",j);
                while(q!=NULL)
                {   
                    if(q->is_node_hidden==NO)
                        printf("%d ",q->data);
                    q=q->next;
                }
                printf("共%d个变元,",p->data);
                printf("位置序号:%d\n",i);
                j++;
            }
                p=p->down;
                if(p!=NULL)
                    q=p->next;//这里不能直接写q=p->next，因为没有说明NULL->next=NULL
        }
}

status isUnitClause(HeadNode *root){
    HeadNode *p=root;
    int i=1;
    for(; i<=all_ziju_num; i++)
    {
        if((p->data==1)&&(p->is_headnode_hidden==NO))
            return TRUE;
        p=p->down;
    }
    return FALSE;
}

int firstUnitClause(HeadNode *root){
    HeadNode *p=root;
    int i=1;
    for(; i<=all_ziju_num; i++)
    {
        if((p->data==1)&&(p->is_headnode_hidden==NO))
            return i;
        p=p->down;
    }
}
int Special_lit_place(HeadNode *p, int lit){
    Node *q=p->next;
    int i=1;
    while(q!=NULL)
    {
        if((q->data==lit)&&(q->is_node_hidden==NO))
            return i;
        i++;
        q=q->next;
    }
    return 0;
}

status Dele_special_lit(HeadNode **root, int lit){
    HeadNode *p=*root;
    Node *q=p->next, *r=p->next;//这里也是不能写成NULL->next=NULL
    int i=1, j=1 ,place;
    for(; i<=all_ziju_num; i++)//这里要写成all!!!!!!!!!!!
    {
        if(p->is_headnode_hidden==NO)
        {
            while(Special_lit_place(p,lit)!=0)
            {
                place=Special_lit_place(p,lit);
                q=p->next;
                for(j=1;j<place;j++)
                {
                    q=q->next;
                }
                q->is_node_hidden=YES;
                p->data--;
            }//end of while
        }
        p=p->down;
    }
    return OK;
}

status isBlankClause(HeadNode *root){
    HeadNode *p=root;
    int i=1;
    for(; i<=all_ziju_num; i++)
    {
        if((p->data==0)&&(p->is_headnode_hidden==NO))
            return TRUE;
        p=p->down;
    }
    return FALSE;
}

status isSetBlank(void){
    if(ziju_num==0)
        return TRUE;
    else 
        return FALSE;
}

int firstBlankClause(HeadNode* root){
    HeadNode *p=root;
    int i=1;
    for(; i<=all_ziju_num; i++)
    {
        if(p->data==0)
            return i;
        p=p->down;
    }
}

status RemoveSpecialClause(HeadNode **root, HeadNode** tail, int L){
    HeadNode *p=*root;
    Node *q=p->next;
    int i=1,n=ziju_num;
    while(p!=NULL)
    {   
        if(p->is_headnode_hidden==NO)
        {
            while(q!=NULL)
            {
                if((q->data==L)&&(q->is_node_hidden==NO))
                {
                    RemoveClause(root,tail,i);
                    p->why=L;
                    break;
                }
                q=q->next;
            }
            p=p->down;
            if(p!=NULL)
                q=p->next;
        }
        else
        {
            p=p->down;
            if(p!=NULL)
                q=p->next;
        }
        i++;
    }
    return OK;
}

status is_lit_in_clause(HeadNode *p, ELemType lit)
{
    Node *q=p->next;
    while(q!=NULL)
    {
        if(q->data==lit)
            return TRUE;
        q=q->next;
    }
    return FALSE;
}


status Recover(HeadNode **root,ELemType v){
    HeadNode *p=*root;
    Node *q;
    while(p!=NULL)
    {   
        //先恢复删除的子句
        if((is_lit_in_clause(p,v)==TRUE)&&(p->is_headnode_hidden==YES)&&(p->why==v))
        {
            ziju_num++;
            p->is_headnode_hidden=NO;
        }
        p=p->down;
    }
    //再恢复删除的文字
    p=*root;
    while(p!=NULL)
    {
        if((p->is_headnode_hidden==NO)&&(is_lit_in_clause(p,-v)==TRUE))
        {
            q=p->next;
            while(q!=NULL)
            {
                if((q->data==-v)&&(q->is_node_hidden==YES))
                {
                    q->is_node_hidden=NO;
                    p->data++;
                }
                q=q->next;
            }
        }
        p=p->down;
    }
    return OK;
}

status DPLL(HeadNode  *root, HeadNode *tail){
    ELemType v;
    HeadNode **root_p=&root, **tail_p=&root;
    HeadNode *p=root;
    Node *q;
    int top=-1;
    int n,i;
    if(isBlankClause(root)==TRUE)
        return FALSE;
    while(isSetBlank()==NO)
    {
        p=root;
        while(p->is_headnode_hidden==YES)//p是第一个非隐藏子句
        {
            p=p->down;
        }
        q=p->next;
        while((q!=NULL)&&(q->is_node_hidden==YES))//q是第一个非隐藏子句中第一个非隐藏文字
        {
            q=q->next;
        }
        v=q->data;
        top++;//v入栈
        Stack[top][0]=v;
        RemoveSpecialClause(root_p, tail_p,v);
        Dele_special_lit(root_p,-v);
        //再消除一些单子句
        while(isUnitClause(root)==TRUE)//有单子句
        {
            n=firstUnitClause(root);
            p=root;//p指向第一个单子句
            for(i=1; i<n; i++)
            {
                p=p->down;
            }
            q=p->next;//q是单子句的节点
            while((q!=NULL)&&(q->is_node_hidden==YES))
            {
                q=q->next;
            }
            v=q->data;
            top++;//v入栈
            Stack[top][0]=v;
            RemoveSpecialClause(root_p, tail_p,v);
            Dele_special_lit(root_p,-v);
        }
        while(isBlankClause(root)==TRUE)
        {
            if(Stack[top][1]==NO)//栈顶is not changed
            {
                Recover(root_p,Stack[top][0]);//消除v的影响
                Stack[top][0]*=-1;//v改-v
                v=Stack[top][0];
                Stack[top][1]=YES;
                RemoveSpecialClause(root_p, tail_p, v);//用-v化简
                Dele_special_lit(root_p, -v);
            }
            else if(Stack[top][1]==YES)
            {
                Recover(root_p,Stack[top][0]);
                //将栈顶元素出栈
                Stack[top][0]=0;
                Stack[top][1]=NO;
                top--;
                if(top==-1)
                    return FALSE;
                //找到上一个not changed的栈内元素
                while(Stack[top][1]==YES)
                {
                    Recover(root_p,Stack[top][0]);
                    Stack[top][0]=0;
                    Stack[top][1]=NO;
                    top--;
                    if(top==-1)
                        return FALSE;
                }
                Recover(root_p,Stack[top][0]);//消除栈顶元素的影响
                Stack[top][0]*=-1;//v改-v
                v=Stack[top][0];
                Stack[top][1]=YES;
                RemoveSpecialClause(root_p, tail_p, v);//用-v化简
                Dele_special_lit(root_p, -v);
                
            }
        }
    }
    return TRUE;
}

status Read_to_Create(FILE *fp, char *filename, HeadNode **root, HeadNode **tail){
    int number;
    if((fp=fopen(filename,"r"))==NULL)//以读的方式打开文件filename
    {
        return ERROR;//如果失败则报错
    }
    char s[100];//用来读取文件的流
    while(fgets(s,7, fp)!=NULL)
    {
        if((s[0]=='p')&&(s[1]==' ')&&(s[2]=='c')&&(s[3]=='n')&&(s[4]=='f')&&(s[5]==' '))//读到了第一行
        {
            fscanf(fp, "%d", &v_num);//读取文字数
            fscanf(fp, "%d", &ziju_num);//读取子句数
            all_ziju_num=ziju_num;
            //先建立头结点
            if(*root != NULL)
                return INFEASTABLE;
            else
            {
                *root = (HeadNode*)malloc(sizeof(HeadNode));
                (*root)->next=NULL;
                (*root)->is_headnode_hidden=NO;
                (*root)->why=0;
                HeadNode* p=*root;
                Node *q=NULL;
                int i;//用于初始化多个链表
                //创建头结点
                for( i=1 ;i<ziju_num;i++)
                {
                    p->data=0;
                    p->all_data=0;
                    p->down=(HeadNode*)malloc(sizeof(HeadNode));
                    p=p->down;
                    p->next=NULL;
                    p->is_headnode_hidden=NO;
                    p->why=0;
                }
                p->data=0;//最后一个子句初始长度为0
                p->all_data=0;
                *tail=p;
                p->down = NULL;
                //输入每个链表的信息
                p=*root;
                for(i=1; i<=ziju_num; i++)
                {
                    if(fscanf(fp,"%d", &number)&&(number==0))//读到空子句
                    {
                        p->next=NULL;
                        p=p->down;
                    }
                    else
                    {
                        q=(Node*)malloc(sizeof(Node));
                        q->data=number;
                        q->is_node_hidden=NO;
                        p->data++;
                        p->all_data++;
                        p->next=q;
                        while(fscanf(fp ,"%d", &number)&&(number!=0))
                        {
                            q->next=(Node*)malloc(sizeof(Node));
                            q=q->next;
                            q->is_node_hidden=NO;
                            q->data=number;
                            p->data++;
                            p->all_data++;
                        }
                        q->next=NULL;
                        p=p->down;
                    }
                }
                fclose(fp);
                return OK;
            }
        }
        else if(strlen(s)>3)
            fgets(s,100,fp);
    }
}

status SaveFile(FILE *fp, char *filename, int duration, int ans){
    int i=0;
    if((fp=fopen(filename,"w"))==NULL)//以写的方式打开文件filename
    {
        return ERROR;//如果失败则报错
    }
    fprintf(fp ,"s %d\n",ans);//输出结果
    if(ans==1)//有解
    {
        fprintf(fp, "v ");
        while(Stack[i][0]!=0)
        {
            fprintf(fp ,"%d ",Stack[i][0]);
            i++;
        }
        fprintf(fp, "\nt %d\n", duration);
    }
    else//无解
    {
        fprintf(fp, "0\n");
        fprintf(fp, "t %d\n", duration);
    }
    fclose(fp);
    return OK;
}

void Analyze_cnf(HeadNode* root){
    int i=1;
    HeadNode* p=root;
    Node* q=p->next;//不能写NULL->next!!!!!!!!!!!!
    if(ziju_num>0)
        for(;i<=all_ziju_num;i++)
        {
            if(p->is_headnode_hidden==NO)
            {
                if(q!=NULL)
                    printf("%d",q->data);
                q=q->next;
                while(q!=NULL)
                {   
                    printf("∨");
                    if(q->is_node_hidden==NO)
                        printf("%d",q->data);
                    q=q->next;
                }
                printf("\n");
            }
                p=p->down;
                if(p!=NULL)
                    q=p->next;//这里不能直接写q=p->next，因为没有说明NULL->next=NULL
        }
}

status Validate_ans(HeadNode *root, HeadNode *tail){
    HeadNode **root_p=&root, **tail_p=&tail;
    //先输出解析的结果
    Analyze_cnf(root);
    //再输出赋值的结果
    int top=0;
    printf("s: ");
    while(Stack[top][0]!=0){
        printf("%d ",Stack[top][0]);
        top++;
    }
    //下面进行解析
    top=0;
    while(Stack[top][0]!=0){
        RemoveSpecialClause(root_p,tail_p,Stack[top][0]);
        top++;
    }
    if(isSetBlank()==TRUE)
        return OK;
    else
        return FALSE;
}


