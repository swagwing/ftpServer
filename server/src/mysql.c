#include "../include/factory.h"

char* query_login(char* value,char* word)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="123456";
    char* database="baidu";
    char query[300]={0};
    static char result[100]={0};
    sprintf(query,"%s%s%s%s%s","select ",value," from login where name='",word,"'");
    int t;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        res=mysql_use_result(conn);
        if(res){
            row=mysql_fetch_row(res);
            if(row!=NULL){
                strcpy(result,(char*)row[0]);
            }else{
                strcpy(result,"null");
                printf("Don't find data\n");
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    
    mysql_close(conn);
    return result;
}

void insert_log(char* name,char* operate,char* time)
{
    MYSQL *conn;
    char* server="localhost";
    char* user="root";
    char* password="123456";
    char* database="baidu";
    char query[200];
    sprintf(query,"%s%s%s%s%s%s%s","insert into log(name,operate,time) values('",name,"','",operate,"','",time,"')");
    int t;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("insert success\n");
    }
    mysql_close(conn);
}
//根据precode、filename、filetype、belong查找code
int query_files(int precode,char *filename,char *filetype,char* belong ){
    int code=0;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char *user="root";
    char *password="123456";
    char *database="baidu";
    char query[200]={0};
    static char result[20]={0};
    sprintf(query,"%s%d%s%s%s%s%s%s%s","select code from files where precode=",precode, 
            " AND filename='",filename,"' AND filetype='",filetype,"'AND belong='",belong,"'");
    int t;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        res=mysql_use_result(conn);
        if(res){
            row=mysql_fetch_row(res);
            if(row!=NULL){
                strcpy(result,(char*)row[0]);
                code=atoi(result);
            }else{
                code=0;
                printf("Don't find data\n");
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    mysql_close(conn);
    return code;
}

//在虚拟目录里根据code获取precode,没有查找到返回2（因为在我建的数据库里2是文件）
int query_files_precode(int code){
    int precode=2;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char *user="root";
    char *password="123456";
    char *database="baidu";
    char query[200]={0};
    static char result[20]={0};
    sprintf(query,"%s%d","select precode from files where code=",code);
    int t;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        res=mysql_use_result(conn);
        if(res){
            row=mysql_fetch_row(res);
            if(row!=NULL){
                strcpy(result,(char*)row[0]);
                precode=atoi(result);
            }else{
                printf("Don't find data\n");
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    mysql_close(conn);
    return precode;
}
//根据code值查询filename
char *query_files_filename(int code){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char *user="root";
    char *password="123456";
    char *database="baidu";
    char query[200]={0};
    static char result[20]={0};
    sprintf(query,"%s%d%s","select filename from files where code=",code," AND filetype='d'");
    int t;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        res=mysql_use_result(conn);
        if(res){
            row=mysql_fetch_row(res);
            if(row!=NULL){
                strcpy(result,(char*)row[0]);
            }else{
                printf("Don't find data\n");
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    mysql_close(conn);
    return result;
}

//根据precode查找文件，拼接成字符串返回，实现ls功能
char *query_files_ls(int precode,char* belong){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char *user="root";
    char *password="123456";
    char *database="baidu";
    char query[200]={0};
    static char result[1000]={0};
    printf("precode:%d\n",precode);
    sprintf(query,"%s%d%s%s%s","select filename from files where precode=",precode," AND belong='",belong,"'");
    int t;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
        return NULL;
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        res=mysql_use_result(conn);
        if(res){
            bzero(result,sizeof(result));
            while((row=mysql_fetch_row(res))!=NULL){
                sprintf(result,"%s%-10s",result,row[0]);
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    mysql_close(conn);
    printf("result:%s\n",result);
    return result;
}
