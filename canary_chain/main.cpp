#pragma GCC optimize("O3","unroll-loops","omit-frame-pointer","inline","no-stack-protector")
#pragma GCC target "tune=native"
#include<bits/stdc++.h>
#define f first
#define s second
using namespace std;
typedef pair<int,int>par;
typedef pair<int,par>pr;
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
int hex_to_int(const string &s){
    int result=-1;
    if(~sscanf(s.c_str(),"%x",&result))return result;
    return -1;
    }
vector<string> split(const string &s,char c){
    std::stringstream test(s);
    std::string segment;
    std::vector<std::string> seglist;
    while(std::getline(test, segment, c)){
        seglist.push_back(segment);
        }
    return seglist;
    }
map<string,vector<int>>mp;
map<int,vector<string>>disas;
void init(string input){
    fstream fin;
    fin.open(input,ios::in);
    string now;
    while(getline(fin,now)){
        auto p=now.find(":");
        //cout<<now<<endl;
        if(p==string::npos)continue;
        int x=hex_to_int(now.substr(0,p));
        if(~x){
            auto v=split(now,'\t');
            if(v.size()<4)continue;
            disas[x]=v;
            if(v[2]=="")continue;
            //cout<<v[2]<<endl;
            mp[v[2]].push_back(x);
            }
        }
    }
void show_disas(const vector<string>&v){
    for(auto &s:v){
        cout<<s<<" | ";
        }
    cout<<endl;
    }
void show_disas(int x){
    if(disas.find(x)!=disas.end())
        show_disas(disas[x]);
    }
int canery_pos=0;
int find_mp(string s){
    int x=upper_bound(mp[s].begin(),mp[s].end(),canery_pos)-mp[s].begin();
    if(x==(int)mp[s].size())return 0x7fffffff;
    return mp[s][x];
    }
int check_pop(int now){
    //show_disas(now);
    if(disas[now][2]=="ldmia.w"){
        if(disas[now][3].find("sp!")==string::npos)
            return false;
        if(disas[now][3].find("pc")!=string::npos)
            return true;
        else
            return false;
        }
    else if(disas[now][2]=="pop"){
        if(disas[now][3].find("pc")!=string::npos)
            return true;
        else
            return false;
        }
    return false;
    }
string first_obj(const string &s){
    auto p=s.find(",");
    if(p==string::npos)return "";
    return s.substr(0,p);
    }
pr find_canery(int pos){
    canery_pos=max(pos,canery_pos);
    while(true){
        int a=find_mp("pop");
        int b=find_mp("ldmia.w");
        int now=min(a,b);
        canery_pos=now;
        //printf("%x\n",now);
        if(check_pop(now)){
            pr result=pr(-1,par(-1,-1));;
            auto it=disas.find(now);
            auto nxt=it;
            nxt++;
            result.s.s=nxt->f;
            while(true){
                it--;
                if(it->s[2]=="bne"||it->s[2]=="bne.n"||it->s[2]=="bne.w"){
                    if(~result.s.f)
                        goto bad_pos;
                    nxt=it;
                    nxt++;
                    result.s.f=nxt->f;
                    }
                else if(it->s[2]=="vpop"){
                    }
                else if(it->s[2]=="cmp"){
                    if(it->s[3]!="r2, r3"){
                        if(~result.s.f)
                            goto bad_pos;
                        //puts("cmp no r2 r3");
                        goto bad_style;
                        }
                    }
                else if(it->s[2]=="add"){
                    if(first_obj(it->s[3])!="sp"){
                        //puts("add no sp");
                        goto bad_pos;
                        }
                    }
                else if(it->s[2]=="ldr"||it->s[2]=="ldr.w"){
                    if(first_obj(it->s[3])!="r2"&&first_obj(it->s[3])!="r3"){
                        //puts("ldr no r2 r3");
                        goto bad_pos;
                        }
                    }
                else
                    goto bad_pos;
                }
            bad_pos:;
            it++;
            if(~result.s.f){
                result.f=it->f;
                if(result.f>=result.s.f)goto bad_style;
                return result;
                }
            bad_style:;
            }
        }
    return pr(-1,par(-1,-1));
    }
char hex_char(string s){
    int res=0;
    if(s[0]>='0'&&s[0]<='9')
        res+=int(s[0]-'0')<<4;
    else if(s[0]>='a'&&s[0]<='f')
        res+=int(s[0]-'a'+10)<<4;
    else if(s[0]>='A'&&s[0]<='F')
        res+=int(s[0]-'A'+10)<<4;
    if(s[1]>='0'&&s[1]<='9')
        res+=int(s[1]-'0');
    else if(s[1]>='a'&&s[1]<='f')
        res+=int(s[1]-'a'+10);
    else if(s[1]>='A'&&s[1]<='F')
        res+=int(s[1]-'A'+10);
    return (char)res;
    }
string get_hex_res(string s){
    stringstream str(s);
    string ss;
    string result;
    while(str>>ss){
        if(ss.length()>=4){
            result+=hex_char(ss.substr(2,2));
            result+=hex_char(ss.substr(0,2));
            }
        else{
            result+=hex_char(ss.substr(0,2));
            }
        }
    return result;
    }
string sorg;
int try_patch(string data,int ps){
    while(true){
        auto pos=find_canery(ps);
        printf("%x %x %x\n",pos.f,pos.s.f,pos.s.s);
        for(int i=pos.f;i<pos.s.f;i++)
            show_disas(i);
        puts("");
            show_disas(pos.s.f);
        puts("");
        for(int i=pos.s.f+1;i<pos.s.s;i++)
            show_disas(i);
        }
    }
deque<pr>reserve;
int main(int argc,char** argv){
    //string tmp="4241 4443";
    //cout<<get_hex_res(tmp)<<endl;
    //return 0;
    string org="libcocos2dcpp.so",dis="disas.txt",result="res_libcocos2dcpp.so";
    if(argc>1)
        org=argv[1];
    if(argc>2)
        dis=argv[2];
    if(argc>3)
        result=argv[3];
    sorg=FileToStr(org);
    init(dis);
    printf("instructions: %d\n",(int)disas.size());
    printf("inited!\n");
    while(true){
        string cmd;
        printf("Cmd: ");
        cin>>cmd;
        if(cmd=="q")break;
        if(cmd=="s"){
            printf("pos: ");
            int pos;
            scanf("%x",&pos);
            if(disas.find(pos)!=disas.end()){
                show_disas(disas[pos]);
                }
            }
        else if(cmd=="f"){
            printf("pos: ");
            int ps;
            scanf("%x",&ps);
            while(true){
                auto pos=find_canery(ps);
                if(pos.s.f-pos.f-4<=0)continue;
                reserve.push_back(pos);
                printf("%x %x %x\n",pos.f,pos.s.f,pos.s.s);
                /*
                for(int i=pos.f;i<pos.s.f;i++)
                    show_disas(i);
                puts("");
                    show_disas(pos.s.f);
                puts("");
                for(int i=pos.s.f+1;i<pos.s.s;i++)
                    show_disas(i);
                */
                break;
                }
            }
        else if(cmd=="p"){
            printf("pos: ");
            int ps;
            scanf("%x",&ps);
            string input;
            getline(cin,input);
            getline(cin,input);
            string s=get_hex_res(input);
            for(int i=0;i<(int)s.length();i++)
                sorg[ps++]=s[i];
            }
        else if(cmd=="c"){
            printf("pos: ");
            int ps;
            scanf("%x",&ps);
            string input;
            getline(cin,input);
            show_disas(ps);
            /*string s=get_hex_res(disas[ps][1]);
            try_patch(s,ps);
            string input;
            while(getline(cin,input)){
                if(input=="done")break;
                string s=get_hex_res(input);
                auto pos=find_canery(ps);
                }*/
            vector<par>jp_list;
            //jp_list.push_back(par(ps+get_hex_res(disas[ps][1]).length(),ps));
            jp_list.push_back(par(ps+2,ps));
            pr pos=pr(-1,par(-1,-1));
            bool new_input=true;
            while(true){
                int old_cannery=canery_pos;
                bool push=false;
                //printf("~~%x %x %x\n",pos.f,pos.s.f,pos.s.s);
                if(pos.f==-1){
                    if(jp_list.size()==1&&reserve.size())
                        pos=reserve.front(),reserve.pop_front();
                    else
                        pos=find_canery(ps);
                    push=true;
                    }
                if(pos.s.f-pos.f-4<=0){
                    pos=pr(-1,par(-1,-1));
                    continue;
                    }
                printf("block size: %d (%d)\n",pos.s.f-pos.f-4,push);
                if(new_input)
                    getline(cin,input);
                if(input=="done"){
                    canery_pos=old_cannery;
                    break;
                    }
                if(input=="")continue;
                string s=get_hex_res(input);
                printf("now block: %d\n",(int)s.length());
                if((int)s.length()>pos.s.f-pos.f-4){
                    //puts("too long!");
                    pos=pr(-1,par(-1,-1));
                    new_input=false;
                    continue;
                    }
                new_input=true;
                int p=pos.f;
                for(int i=pos.s.f;i<pos.s.s;i++,p++)
                    sorg[p]=sorg[i];
                int p2=p;
                for(int i=0;i<(int)s.length();i++)
                    sorg[p++]=s[i];
                pos.f=p;
                if(push){
                    jp_list.push_back(par(p2,p));
                    for(;p<pos.s.s;p+=2)
                        sorg[p]=0x00,sorg[p+1]=0xbf;
                    }
                else
                    jp_list.back().s=p;
                pos.s.f=pos.s.s;
                }
            for(int i=0;i<(int)jp_list.size();i++){
                int nw=jp_list[i].s;
                int nx=jp_list[(i+1)%jp_list.size()].f;
                printf("%x %x\n",nw,nx);
                if(nx>nw){
                    int d=(nx-nw)/2-2;
                    if(d<=0x3ff){
                        sorg[nw]=d&0xff;
                        sorg[nw+1]=((d>>8)&0x7)|0xE0;
                        }
                    else{
                        d=(nx-nw-4);
                        d=d/2;
                        d=d&0xFFFFF;
                        int d1=d&0xFF;
                        d>>=8;
                        d*=2;
                        sorg[nw]=(d>>4);
                        sorg[nw+1]=0xf0;
                        sorg[nw+2]=d1&0xff;
                        sorg[nw+3]=((d/2)&0xF)|0xB8;
                        /*
                        int dd=d&0xFFF;
                        dd/=2;
                        sorg[nw]=dd>>8;
                        sorg[nw+1]=0xF0;
                        sorg[nw+2]=dd&0xFF;
                        sorg[nw+3]=(dd>>8)|0xB8;*/
                        //puts("Error!");
                        }
                    }
                else{
                    int d=(nw-nx)/2;
                    if(d<=0x3ff){
                        d=0x7fff-d-1;
                        sorg[nw]=d&0xff;
                        sorg[nw+1]=((d>>8)&0x7)|0xE0;
                        }
                    else{
                        d=(nw-nx+2);
                        //d=0x7fffffff-d;//7fff f119

                        //int dd=d&0xFFF;
                        d=d/2-2;
                        d=0xFFFFFF-d;
                        d=d&0xFFFFF;
                        d-=2;
                        int d1=d&0xFF;
                        d>>=8;
                        d*=2;
                        sorg[nw]=(d>>4);
                        sorg[nw+1]=0xf7;
                        sorg[nw+2]=d1&0xff;
                        sorg[nw+3]=((d/2)&0xF)|0xB8;

                        }
                    }
                }
            }
        }
    StrToFile(sorg,result);
    return 0;
    }
