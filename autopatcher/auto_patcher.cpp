#include<bits/stdc++.h>
using namespace std;
string FileToStr(string fil){
    fstream fin;
    fin.open(fil,ios::in|ios::binary);
    char c;
    string ss;
    while(fin.get(c))
        ss+=c;
    fin.close();
    return ss;
    }
void StrToFile(string s,string fil){
    fstream fout;
    fout.open(fil,ios::out|ios::binary);
    fout<<s;
    fout.close();
    }
const int match_len=64;
int main(int argc,char** argv){
    string org="org_libcocos2dcpp.so",patch="patch_libcocos2dcpp.so",target="target_libcocos2dcpp.so",result="result_libcocos2dcpp.so";
    if(argc>4)
        org=argv[1],
        patch=argv[2],
        target=argv[3],
        result=argv[4];
    string sorg,spat,star,sres;
    sorg=FileToStr(org);
    spat=FileToStr(patch);
    star=FileToStr(target);
    if(sorg.length()!=spat.length()){
        puts("length not match");
        return 0;
        }
    sres=star;
    int len=sorg.length();
    vector<int>pos;
    vector<int>tps;
    for(int i=0;i<len;i++){
        if(sorg[i]!=spat[i])
            printf("diff: %09x %02x %02x\n",i,(unsigned char)sorg[i],(unsigned char)spat[i]),
            pos.push_back(i);
        }
    int sz=pos.size();
    int len2=star.length();
    for(int i=0;i<sz;i++){
        int x=pos[i];
        int px=-1;
        if(i)
            px=pos[i]-pos[i-1]+tps[i-1];
        string s="";
        for(int i=0;i<match_len;i++)
            s+=sorg[x+i];
        int beg=0;
        //if(i)
            //beg=tps[i-1]+1;
        int p=-1;//=star.substr(beg).find(s);
        int mat=0;
        for(int j=beg;j<len-match_len;j++){
            if(s[0]!=star[j])continue;
            int cnt=1;
            for(int k=1;k<match_len;k++)
                cnt+=s[k]==star[j+k];
            if(cnt>mat){
                mat=cnt;
                p=j;
                }
            }
        printf("    %d\n",mat);
        if(mat<=match_len/2)p=-1;
        if(~p){
            if(i&&px!=p)
                printf("strange: %09x %09x\n",px,p);
            printf("  patch: %09x %09x %02x %02x %02x\n",p,x,(unsigned char)sorg[x],(unsigned char)star[p],(unsigned char)spat[x]);
            }
        else{
            if(!~px){
                puts("begin fail!");
                return 0;
                }
            p=px;
            printf("  fail: %09x %09x %02x %02x %02x\n",p,x,(unsigned char)sorg[x],(unsigned char)star[p],(unsigned char)spat[x]);
            }
        tps.push_back(p);
        sres[p]=spat[x];
        }
    StrToFile(sres,result);
    return 0;
    }
