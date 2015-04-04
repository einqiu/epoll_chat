#ifndef _QUERY_H_
#define _QUERY_H_

#include "local.h"
using namespace std;
class Query{
   private:
    string que;
    string query_word;
    string key_word;
    string info;
    vector<string> vec;
    map<string,string> weather;
   public:
    Query(string);
    void resolve();
    pair<string,string> getkey();
    string query_all();
    string& query_weather();
    string& query_train();
    string& query_plane();
    string& query_stock();
};  
Query::Query(string temp){
     que=temp;
     query_word="";
     key_word="";
     info="WE HAVN'T PROVIDED THIS SERVICE,PLEASE WAIT,THANKS!";
     vec.push_back("weather");
     vec.push_back("train");
     vec.push_back("plane");
     vec.push_back("stock");
     weather.insert(pair<string,string>("beijing","sunny"));
     weather.insert(pair<string,string>("nanjing","rain"));
     weather.insert(pair<string,string>("shanghai","cloudy"));
     weather.insert(pair<string,string>("newyork","snow"));
}
void Query::resolve(){
     int i=1;
     for(;i<que.length();i++){
        if(que[i]==' ') break;  
        query_word.push_back(que[i]);
     }
     i++;
     for(;i<que.length()-2;i++){
         key_word.push_back(que[i]);
     }
}
pair<string,string> Query::getkey(){
    return pair<string,string>(query_word,key_word);
}
string Query::query_all(){
    int i=0;
    for(;i<vec.size();i++){
        if(vec[i]==query_word) break;
    }
    if(i==vec.size()) return info+'\n';
    switch(i){
      case 0: return query_weather()+'\n';
      case 1: return query_train()+'\n';
      case 2: return query_plane()+'\n'; 
      case 3: return query_stock()+'\n';
   }
}
string& Query::query_weather(){
      
     return weather[key_word];
     //return key_word;
}
string& Query::query_train(){
    return info;
}
string& Query::query_plane(){
    return info;
}
string& Query::query_stock(){
    return info;
}

#endif
