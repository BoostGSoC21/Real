 #include<bits/stdc++.h>

std::string randomIntegerOfLen(int n){
    std::string str;
    for(int i=0;i<n;++i){
        int x=rand()%10;
        str.push_back(x+'0');
    }
    return str;
 }

std::string randomRealOfLen(int n){
    int x=rand()%n;
    return randomIntegerOfLen(x)+"."+randomIntegerOfLen(n-x);
 }

 void toCharArray(std::string str,char* ptr){
    int n=str.size();

    for(int i=0;i<n;++i)
        ptr[i]=str[i];

    ptr[n]='\0';

    return;
 }
