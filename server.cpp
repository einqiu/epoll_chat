#include "local.h"
#include "utils.h"
#include "query.h"
using namespace std;

// 存放客户端socket描述符的list
list<int> clients_list;
//用户的昵称和描述符对应起来，并放在全局的Map中
map<int,string> nickname;
//用户在服务器上注册的所有昵称，用于判断注册昵称是否存在
vector<string> nicknames;
//此Map对象用作天气数据的缓存，每两个小时clear一次
map<string,string> loc_wea;
int main(int argc, char *argv[]){
    int listener;   //监听socket
    struct sockaddr_in addr, their_addr;  
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t socklen;
    socklen = sizeof(struct sockaddr_in);

    static struct epoll_event ev, events[EPOLL_SIZE];
    ev.events = EPOLLIN | EPOLLET;     //对读感兴趣，边沿触发

    char message[BUF_SIZE];//暂存数据

    int epfd;  //epoll描述符
    int client, res, epoll_events_count;

    CHK2(listener, socket(PF_INET, SOCK_STREAM, 0));             //初始化监听socket
    setnonblocking(listener);                                    //设置监听socket为不阻塞
    CHK(bind(listener, (struct sockaddr *)&addr, sizeof(addr))); //绑定监听socket
    CHK(listen(listener, 1));                                    //设置监听

    CHK2(epfd,epoll_create(EPOLL_SIZE));                         //创建一个epoll描述符，并将监听socket加入epoll
    ev.data.fd = listener;
    CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev));

    while(1){
        CHK2(epoll_events_count,epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT));
        for(int i = 0; i < epoll_events_count ; i++){
            if(events[i].data.fd == listener){                    //新的连接到来，将连接添加到epoll中，并发送欢迎消息
                CHK2(client,accept(listener, (struct sockaddr *) &their_addr, &socklen));
                setnonblocking(client);
                ev.data.fd = client;
                CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev));
                clients_list.push_back(client);                  // 添加新的客户端到list
                nickname.insert(pair<int,string>(client,""));
                bzero(message, BUF_SIZE);
                res = sprintf(message, STR_WELCOME);
                CHK2(res, send(client, message, BUF_SIZE, 0));

            }else{
                CHK2(res,handle_message(events[i].data.fd)); //如果不是新连接，调用handle_message函数进行处理
            }
        }
    }
    close(listener);
    close(epfd);
    return 0;
}
//关闭客户端连接
void close_client( char message[],int client){
        CHK(close(client));
        clients_list.remove(client);
        sprintf(message, STR_OUT, nickname[client].c_str());
        list<int>::iterator it;
        for(it = clients_list.begin(); it != clients_list.end(); it++){
           if(*it != client){
                CHK(send(*it, message, BUF_SIZE, 0));
           }
        }
}
int handle_message(int client){  
    char buf[BUF_SIZE], message[BUF_SIZE];    
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);
    int len;

    CHK2(len,recv(client, buf, BUF_SIZE, 0));  //接收客户端信息
    if(len == 0){   //客户端关闭或出错，关闭socket，并从list移除socket
         close_client(message,client);
    }
    string name=nickname.find(client)->second;//此客户端的昵称
    if(name==""){//昵称是否输入
        char NoSpacebuf[BUF_SIZE];    
        bzero(NoSpacebuf, BUF_SIZE);
        int i=0;
        //处理最后的“\r\n”
        for(;i<BUF_SIZE;i++){ 
            if((int)buf[i]==13) break;
            NoSpacebuf[i]=buf[i];      
        }
        NoSpacebuf[i]='\0';
        vector<string>::iterator it;
        it=find(nicknames.begin(),nicknames.end(),NoSpacebuf);
		//如果该昵称没有人注册过
        if(it==nicknames.end()){
            nickname[client]=NoSpacebuf;
            nicknames.push_back(NoSpacebuf);
            sprintf(message,"MR or MISS %s WELCOME COMING to CHAT,Have Fun!\nOnline numbers are %d\n",NoSpacebuf,int(clients_list.size()));
            CHK(send(client, message, BUF_SIZE, 0));
            bzero(message, BUF_SIZE);
            sprintf(message, STR_IN, nickname[client].c_str());
            list<int>::iterator it;
           //发送给所有的在线用户，某用户已经上线
            for(it = clients_list.begin(); it != clients_list.end(); it++){
                if(*it != client){
                CHK(send(*it, message, BUF_SIZE, 0));
                }
            }
        return len;
        }
		//已经有人注册过，提示重新注册
        else{
         sprintf(message,"NICKNAME HAS EXITED!,PLEASE INPUT AGAIN:\n");
         CHK(send(client, message, BUF_SIZE, 0));
         return len;
        }
     return len;
    }
	if(len>1){        
        sprintf(message, STR_MESSAGE, nickname[client].c_str(), buf);
        //下面检测用户所输入是否是命令
        char fc=*buf;
		//是命令
        if(fc=='/'){
            if(buf[1]=='q'&&buf[2]=='u'&&buf[3]=='i'&&buf[4]=='t'){
                close_client(message,client);
            }
            else{
                Query myquery(buf);
                myquery.resolve();
                pair<string,string>temp_pair=myquery.getkey();
              //判断缓存中是否有需要的数据，如果没有则调用接口查询 
                if(loc_wea.find(temp_pair.second)==loc_wea.end()){
                    string temp=myquery.query_all();
                    loc_wea.insert(pair<string,string>(temp_pair.second,temp));
                    sprintf(message,STR_QUERY,temp.c_str());
                    CHK(send(client, message, BUF_SIZE, 0));
                }
                else{
                    string temp=loc_wea[temp_pair.second];
                    sprintf(message,STR_QUERY,temp.c_str());
                    CHK(send(client, message, BUF_SIZE, 0));
                }
            }
        } 
		//不是命令，对在线的每个客户端发送
        else{
	        list<int>::iterator it;
		    for(it = clients_list.begin(); it != clients_list.end(); it++){
		       if(*it != client){
			       CHK(send(*it, message, BUF_SIZE, 0));
		        }
		    }
        }
	    return len;
    }  
}

