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
typedef int ELemType;//��Ԫ������
//һ����Ԫ����һ���ڵ���
typedef struct Node{
    ELemType data;//�ڵ����������
    struct Node* next;//ָ����
    status is_node_hidden;//״̬���ڵ��Ƿ�ɾ��
}Node;

typedef struct HeadNode{
    int data;//���������Ӿ�ĳ���
    int all_data;//���Ӿ��ʼ״̬�ĳ���
    struct HeadNode* down;//��һ���Ӿ�
    struct Node* next;//ָ����Ӿ�ĵ�һ���ڵ�
    status is_headnode_hidden;//״̬�����Ӿ��Ƿ�ɾ��
    ELemType why;//����Ӿ䱻ɾ�������¼��һ�α�ɾ������Ϊ�ĸ����֡���δ��ɾ����Ϊ0
}HeadNode;

int ziju_num=0;//��ǰ�Ӿ�ĸ���
int all_ziju_num=0;//�����е��Ӿ�����������ɾ��ֻ�����أ���˲�����Ӿ�����ɾ��
int Stack[10000][2];//ջ��һ�����ڴ����������֣��ڶ��д��changed��״̬
int v_num;//��Ԫ��
clock_t start, stop;//����ͳ��DPLL����ʱ��
long duration;//duration��DPLL����ʱ��


status CreateClause(HeadNode** root, HeadNode** tail);//�����Ӿ伯
status DestroyClause(HeadNode** root, HeadNode** tail);//ɾ���Ӿ伯(��������ɾ������free)
status AddClause(HeadNode** root, HeadNode** tail);//����Ӿ�
status RemoveClause(HeadNode** root, HeadNode** tail, int n);//ɾ����n���Ӿ�
void Show_ziju_num(void);//��ʾ�Ӿ����
void ShowAllClause(HeadNode *root);//չʾ���е��Ӿ�
status isUnitClause(HeadNode *root);//�ж��Ӿ伯���Ƿ���ڵ��Ӿ�
int firstUnitClause(HeadNode *root);//�����Ӿ伯�е�һ�����Ӿ�����(���������ص�)
int Special_lit_place(HeadNode *p, int lit);//�ҵ���ǰ�Ӿ��е�һ��Ϊlit�����ֵ�λ��(�������صĽڵ�)��û���򷵻�0
status Dele_special_lit(HeadNode**root,int lit);//�������Ӿ���ɾ��ָ��������
status isBlankClause(HeadNode *root);//�ж��Ӿ伯���Ƿ��п��Ӿ�
int firstBlankClause(HeadNode* root);//�����Ӿ伯�е�һ�����Ӿ�����(���������ص��Ӿ�)
status isSetBlank(void);//�ж��Ӿ伯�Ƿ�Ϊ��
status DPLL(HeadNode *root, HeadNode* tail);//�ݹ��DPLL�㷨
status RemoveSpecialClause(HeadNode **root, HeadNode** tail, int L);//ɾ����������L���Ӿ�
status Recover(HeadNode **root,ELemType v);//�ָ�����v���Ӿ䣬�Լ�����-v�Ľڵ�
status is_lit_in_clause(HeadNode *p, ELemType lit);//�ж�lit�Ƿ����Ӿ���(��������Ҳ�������棡��������)
status Read_to_Create(FILE *fp, char *filename, HeadNode **root, HeadNode **tail);//��ȡcnf�ļ����Ҵ����Ӿ伯
status SaveFile(FILE *fp, char *filename, int duration, int ans);//��DPLL��������Ϣ�����ļ���
void Analyze_cnf(HeadNode* root);//��cnf�ļ����н���
status Validate_ans(HeadNode *root, HeadNode *tail);//��ջ�еı�Ԫ��֤cnf�ļ�

int main(void)
{
    int s=0;
    FILE *fp =NULL;
    char filename[40];
    //��ջ��ʼ��
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
    int choice;//����ѡ���Ƿ��ӡ���ļ�
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
	    printf("    ��ѡ����Ĳ���[0~16]:");
	    scanf("%d",&op);
        switch(op){
            case 1:
            {
                printf("�������Ӿ�ĸ���");
                scanf("%d", &ziju_num);
                all_ziju_num=ziju_num;//��all_ziju_num����ֵ
                if(CreateClause(root_p,tail_p)==OK)
                    printf("�Ӿ伯�����ɹ���\n");
                else if(root!=NULL)
                    printf("����ʧ�ܣ��Ӿ伯�Ѿ������ˣ�\n");
                 getchar();getchar();
                 break;
            }
            case 2:
            {
                if(DestroyClause(root_p, tail_p)==OK)
                    printf("�Ӿ伯���ٳɹ���\n");
                else if(!root)
                    printf("����ʧ�ܣ��Ӿ伯��δ������\n");
                getchar();getchar();
                break;
            }
            case 3:
            {
                if(AddClause(root_p,tail_p)==OK)
                    printf("����Ӿ�ɹ�!\n");
                else
                    printf("����Ӿ�ʧ��!\n");
                getchar();getchar();
                break;
            }
            case 4:
            {
                int n;
                printf("������Ҫɾ�����Ӿ�����:");
                scanf("%d",&n);
                if(!root)
                    printf("ɾ��ʧ��!�Ӿ伯��δ��ʼ����\n");
                else if(RemoveClause(root_p,tail_p,n)==OK)
                    printf("ɾ���Ӿ�ɹ���\n");
                else
                    printf("ɾ��ʧ�ܣ�\n");
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
                    printf("�Ӿ伯��δ��ʼ����\n");
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
                if(!root)  printf("�Ӿ伯��δ��ʼ����\n");
                else if(isUnitClause(*root_p)==TRUE)
                    printf("���ڵ��Ӿ䣡\n");
                else 
                    printf("�����ڵ��Ӿ䣡\n");
                getchar();getchar();
                break;
            }
            case 8:
            {
                if(isUnitClause(*root_p)==TRUE)
                    printf("��һ�����Ӿ�������%d��\n", firstUnitClause(*root_p));
                else 
                    printf("�����ڵ��Ӿ䣡\n");
                getchar();getchar();
                break;
            }
            case 9:
            {
                int lit;
                printf("������Ҫɾ��������:");
                scanf("%d", &lit);
                if(Dele_special_lit(root_p, lit)==OK)
                    printf("ɾ���ɹ���\n");
                else
                    printf("ɾ��ʧ�ܣ�\n");
                getchar();getchar();
                break;
            }
            case 10:
            {
                if(isBlankClause(root)==TRUE)
                    printf("���ڿ��Ӿ䣡\n");
                else
                    printf("�����ڿ��Ӿ䣡\n");
                getchar();getchar();
                break;
            }
            case 11:
            {
                if(isSetBlank()==TRUE)
                    printf("�Ӿ伯Ϊ�գ�\n");
                else
                    printf("�Ӿ伯�ǿգ�\n");
                getchar();getchar();
                break;
            }
            case 13:
            {
                int ans;
                start = clock();//��ʼ��ʱ
                ans=DPLL(root, tail);
                if(ans==TRUE)
                {
                    printf("�Ӿ伯�����㣡\n");
                    printf("���ֵĸ�ֵΪ:\n");
                    int i=0;
                    while(Stack[i][0]!=0)
                    {
                        printf("%d\n",Stack[i][0]);
                        i++;
                    }
                    printf("\n��������⣡\n");
                }
                else
                    printf("�Ӿ伯�������㣡\n");
                stop=clock();
                duration = ((long)(stop - start));
                printf("����ʱ��:%ldms!\n",duration);
                getchar();getchar();
                printf("�Ƿ񽫱������е���Ϣ�����ļ��У�(��1���棬��0������)");
                scanf("%d", &choice);
                if(choice==1)
                {
                    printf("������һ���ļ��������������е���Ϣ�����ȥ: ");
                    scanf("%s",filename);
                    if(SaveFile(fp,filename,duration,ans)==TRUE)
                        printf("����ɹ���\n");
                    else
                        printf("����ʧ�ܣ�\n");
                }
                else
                    printf("��˵����Ͳ����\n");
                getchar();getchar();
                break;
            }
            case 14:{
                int L;
                printf("����������L����ɾ������L���Ӿ䣺");
                scanf("%d",&L);
                RemoveSpecialClause(root_p,tail_p,L);
                printf("ɾ���ɹ���\n");
                getchar();getchar();
                break;
            }
            case 15:{
                ELemType v;
                printf("����������v�����ڻָ�֮ǰ����v������Ӱ��\n");
                scanf("%d",&v);
                if(Recover(root_p,v)==OK)
                    printf("�ָ��ɹ���\n");
                getchar();getchar();
                break;
            }
            case 16:{
                printf("�������ļ����Զ�ȡ��\n");
                scanf("%s",filename);//��һ��ָ��������浽filename��
                if(Read_to_Create(fp,filename,root_p, tail_p)==OK)
                    printf("�ļ���ȡ�ɹ���\n");
                else
                    printf("�ļ���ȡʧ�ܣ�\n");
                getchar();getchar();
                break;
            }
            case 17:
            {
                printf("�ļ��������£�\n");
                Analyze_cnf(root);
                printf("�������!\n");
                getchar();getchar();
                break;
            }
            case 18:
            {
                if(Validate_ans(root, tail)==OK)
                    printf("\n��֤���Ϊ���㣡\n");
                else
                    printf("\n��֤���Ϊ�����㣡\n");
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
        int i;//���ڳ�ʼ���������
        int number;//���ڶ�ȡ����
        //����ͷ���
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
        p->data=0;//���һ���Ӿ��ʼ����Ϊ0
        p->all_data=0;
        *tail=p;
        p->down = NULL;
        //����ÿ���������Ϣ
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
    printf("������Ҫ��ӵ��Ӿ�:\n");
    if(!(*root))//��û�г�ʼ��
    {
        *root=(HeadNode*)malloc(sizeof(HeadNode));
        *tail=*root;
        (*tail)->data=0;
        (*tail)->all_data=0;
        (*tail)->is_headnode_hidden=NO;
        int number;//���ڶ�ȡ����
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
    else//�Ѿ���ʼ��
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
    int i;//n��Ҫɾ�����Ӿ����ţ�i�����ҵ��Ǹ��Ӿ�
    HeadNode* p=*root;//�����ҵ��Ǹ��Ӿ��ͷָ��
    HeadNode* q=*root;
    if(n>all_ziju_num)
        printf("���Ӿ䲻���ڣ�\n");
    else if((n>1)&&(n!=all_ziju_num))//Ҫɾ���Ĳ������һ���Ӿ䣡���������ﲻ��ֱ��дn>1�����Ҷ�Ҫд��all
    {
        for(i=1;i<n;i++)
        {
            p=p->down;//p��Ҫɾ�����Ӿ�
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
    else if((n==all_ziju_num)&&(n!=1))//ɾ���������һ���Ӿ�
    {
        for(i=1;i<n;i++)
        {
            p=p->down;//p��Ҫɾ�����Ӿ�
        }//p�����һ���Ӿ�
        ziju_num--;
        p->is_headnode_hidden=YES;
        return OK;
    }
    else if((n==all_ziju_num)&&(n==1))//ֻʣһ���Ӿ��ʱ��
    {
        (*root)->is_headnode_hidden=YES;
        ziju_num--;
        return OK;
    }
    else
        return ERROR;
}

void Show_ziju_num(void){
    printf("�Ӿ����ĿΪ%d",ziju_num);
}

void ShowAllClause(HeadNode *root){
    int i=1,j=1;
    HeadNode* p=root;
    Node* q=p->next;//����дNULL->next!!!!!!!!!!!!
    printf("�ܹ���%d���Ӿ�\n",ziju_num);
    if(ziju_num>0)
        for(;i<=all_ziju_num;i++)
        {
            if(p->is_headnode_hidden==NO)
            {
                printf("��%d���Ӿ�: ",j);
                while(q!=NULL)
                {   
                    if(q->is_node_hidden==NO)
                        printf("%d ",q->data);
                    q=q->next;
                }
                printf("��%d����Ԫ,",p->data);
                printf("λ�����:%d\n",i);
                j++;
            }
                p=p->down;
                if(p!=NULL)
                    q=p->next;//���ﲻ��ֱ��дq=p->next����Ϊû��˵��NULL->next=NULL
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
    Node *q=p->next, *r=p->next;//����Ҳ�ǲ���д��NULL->next=NULL
    int i=1, j=1 ,place;
    for(; i<=all_ziju_num; i++)//����Ҫд��all!!!!!!!!!!!
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
        //�Ȼָ�ɾ�����Ӿ�
        if((is_lit_in_clause(p,v)==TRUE)&&(p->is_headnode_hidden==YES)&&(p->why==v))
        {
            ziju_num++;
            p->is_headnode_hidden=NO;
        }
        p=p->down;
    }
    //�ٻָ�ɾ��������
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
        while(p->is_headnode_hidden==YES)//p�ǵ�һ���������Ӿ�
        {
            p=p->down;
        }
        q=p->next;
        while((q!=NULL)&&(q->is_node_hidden==YES))//q�ǵ�һ���������Ӿ��е�һ������������
        {
            q=q->next;
        }
        v=q->data;
        top++;//v��ջ
        Stack[top][0]=v;
        RemoveSpecialClause(root_p, tail_p,v);
        Dele_special_lit(root_p,-v);
        //������һЩ���Ӿ�
        while(isUnitClause(root)==TRUE)//�е��Ӿ�
        {
            n=firstUnitClause(root);
            p=root;//pָ���һ�����Ӿ�
            for(i=1; i<n; i++)
            {
                p=p->down;
            }
            q=p->next;//q�ǵ��Ӿ�Ľڵ�
            while((q!=NULL)&&(q->is_node_hidden==YES))
            {
                q=q->next;
            }
            v=q->data;
            top++;//v��ջ
            Stack[top][0]=v;
            RemoveSpecialClause(root_p, tail_p,v);
            Dele_special_lit(root_p,-v);
        }
        while(isBlankClause(root)==TRUE)
        {
            if(Stack[top][1]==NO)//ջ��is not changed
            {
                Recover(root_p,Stack[top][0]);//����v��Ӱ��
                Stack[top][0]*=-1;//v��-v
                v=Stack[top][0];
                Stack[top][1]=YES;
                RemoveSpecialClause(root_p, tail_p, v);//��-v����
                Dele_special_lit(root_p, -v);
            }
            else if(Stack[top][1]==YES)
            {
                Recover(root_p,Stack[top][0]);
                //��ջ��Ԫ�س�ջ
                Stack[top][0]=0;
                Stack[top][1]=NO;
                top--;
                if(top==-1)
                    return FALSE;
                //�ҵ���һ��not changed��ջ��Ԫ��
                while(Stack[top][1]==YES)
                {
                    Recover(root_p,Stack[top][0]);
                    Stack[top][0]=0;
                    Stack[top][1]=NO;
                    top--;
                    if(top==-1)
                        return FALSE;
                }
                Recover(root_p,Stack[top][0]);//����ջ��Ԫ�ص�Ӱ��
                Stack[top][0]*=-1;//v��-v
                v=Stack[top][0];
                Stack[top][1]=YES;
                RemoveSpecialClause(root_p, tail_p, v);//��-v����
                Dele_special_lit(root_p, -v);
                
            }
        }
    }
    return TRUE;
}

status Read_to_Create(FILE *fp, char *filename, HeadNode **root, HeadNode **tail){
    int number;
    if((fp=fopen(filename,"r"))==NULL)//�Զ��ķ�ʽ���ļ�filename
    {
        return ERROR;//���ʧ���򱨴�
    }
    char s[100];//������ȡ�ļ�����
    while(fgets(s,7, fp)!=NULL)
    {
        if((s[0]=='p')&&(s[1]==' ')&&(s[2]=='c')&&(s[3]=='n')&&(s[4]=='f')&&(s[5]==' '))//�����˵�һ��
        {
            fscanf(fp, "%d", &v_num);//��ȡ������
            fscanf(fp, "%d", &ziju_num);//��ȡ�Ӿ���
            all_ziju_num=ziju_num;
            //�Ƚ���ͷ���
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
                int i;//���ڳ�ʼ���������
                //����ͷ���
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
                p->data=0;//���һ���Ӿ��ʼ����Ϊ0
                p->all_data=0;
                *tail=p;
                p->down = NULL;
                //����ÿ���������Ϣ
                p=*root;
                for(i=1; i<=ziju_num; i++)
                {
                    if(fscanf(fp,"%d", &number)&&(number==0))//�������Ӿ�
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
    if((fp=fopen(filename,"w"))==NULL)//��д�ķ�ʽ���ļ�filename
    {
        return ERROR;//���ʧ���򱨴�
    }
    fprintf(fp ,"s %d\n",ans);//������
    if(ans==1)//�н�
    {
        fprintf(fp, "v ");
        while(Stack[i][0]!=0)
        {
            fprintf(fp ,"%d ",Stack[i][0]);
            i++;
        }
        fprintf(fp, "\nt %d\n", duration);
    }
    else//�޽�
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
    Node* q=p->next;//����дNULL->next!!!!!!!!!!!!
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
                    printf("��");
                    if(q->is_node_hidden==NO)
                        printf("%d",q->data);
                    q=q->next;
                }
                printf("\n");
            }
                p=p->down;
                if(p!=NULL)
                    q=p->next;//���ﲻ��ֱ��дq=p->next����Ϊû��˵��NULL->next=NULL
        }
}

status Validate_ans(HeadNode *root, HeadNode *tail){
    HeadNode **root_p=&root, **tail_p=&tail;
    //����������Ľ��
    Analyze_cnf(root);
    //�������ֵ�Ľ��
    int top=0;
    printf("s: ");
    while(Stack[top][0]!=0){
        printf("%d ",Stack[top][0]);
        top++;
    }
    //������н���
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


