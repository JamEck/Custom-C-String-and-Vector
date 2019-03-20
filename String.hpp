#ifndef String_h
#define String_h

#include <stdio.h>
#include "Linker.hpp"

class String{
private:
    bool allocated = false;
    char* data = nullptr;
    unsigned int length = 0;
    
    Linker<String> links;
    
    String* ans = nullptr;
    //////////////////////////////// set-up and destroy ///////////
    
    int getLength(char* );
    
    int getNumLength(int);
    
    void wipe();
public:
    
    //////////////////////////////// constructors  ////////////////
    
    String();
    String(char* inp);
    String(String& inp); // share constructor
    String(String&& inp);// share from temp
    
    String(String&, String&); // concat
    String(String&, String&, String&); // 2 concats
    
    String(int);
    String(double, char);
    
    ~String(); // destructor
    
    //////////////////////////////// operators ////////////////
    
    void    operator=  (String&); // share
    String& operator+  (String&); // concat and return
    bool    operator== (String&); // get element
    char    operator[] (int);     // get element

    //////////////////////////////// creation  ////////////////
    
    void create(   char*); // creation from raw char array
    void create(String& ); // shares
    void create(String&&); // shares with temp
    
    void share(String&); // establishes sharing links
    
    void copy(String&);
    
    //////////////////////////////// features  ////////////////
    template <typename T1, typename T2 >
    void concat(T1&&, T2&&);
    template <typename T1, typename T2, typename T3>
    void concat(T1&&, T2&&, T3&&);
    
    void append(String&&);
    void append(String&);
    void append(char*);
    
    void fill(unsigned int, char);
    
    void itoc(int); // convert integer to string
    
    void dtoc(double, char); // convert double to string
    
    void truncate(unsigned int); // truncate by amount
    
    void popPath(int numOfPaths = 1); // removes x path segments off end
    void addPath(String&&); // adds path segment to end
    void addPath(String&);
    void addPath(char*);
    
    String& ripPath(); // copies and returns final path segment
    
    //////////////////////////////// misc      ////////////////
    void clear(); // clear contents
    
    int stoi(); // string to integer
    
    bool compare(char*);   // compare to character array
    bool compare(String&); // .. to another string
    
    void print();
    void println();
    
    void printHex(); // print each char in hex
    
    //////////////////////////////// accessors ////////////////
    char* getData();
    
    unsigned int getLen();
    
    bool isAlloc();
    
    String& getLender();
    String& getBorrower();
    
    //////////////////////////////// mutators  ////////////////
    
};




///////////////////////////////////////////////////////////////


//                      IMPLEMENTATION                       //


///////////////////////////////////////////////////////////////



//////////////////////////////// set-up and destroy ///////////

int String::getLength(char* inp){
    int count = 0;
    if(inp != nullptr){
        for (count = 0; inp[count]; count++);
    }
    return count;
}
int String::getNumLength(int num){
    int ans;
    for(ans = 0; num; ans++){
        num /= 10;
    }
    return ans + !ans;
}

void String::wipe(){
    links.relink();
    if(allocated && !links.isProtected()){
        delete [] data;
    }
    allocated = false;
    links.wipe();
    data = nullptr;
    length = 0;
}


//////////////////////////////// constructors  ////////////////

String::String():links(this){}
String::String(char* inp):links(this){create(inp);}
String::String(String& inp):links(this){create(inp);} // share constructor
String::String(String&& inp):links(this){create(inp);}// share from temp

String::String(String& inp1, String& inp2):links(this){concat(inp1, inp2);}
String::String(String& inp1, String& inp2, String& inp3):links(this){concat(inp1, inp2, inp3);}

String::String(int num):links(this){ itoc(num);}
String::String(double num, char dig):links(this){dtoc(num, dig);}
String::~String(){wipe();} // destructor

//////////////////////////////// operators ////////////////

void String::operator=  (String& inp){create(inp);} // share

String& String::operator+ (String& inp){ // concat and return
    if(ans) delete ans;
    ans = new String;
    ans->concat(*this, inp);
    return *ans;
}

bool String::operator== (String& inp){return compare(inp);} // compare

char String::operator[] (int ind){ // get element
    if(ind < 0){
        ind += length;
        if(ind < 0) ind = 0;
    }
    else ind = (ind >= length)?length-1:ind;
    return data[ind];
}

//////////////////////////////// creation  ////////////////

void String::create(char* inp){ // creation from raw char array
    if(inp != nullptr){
        wipe();
        int l = getLength(inp);
        data = new char[l+1];
        allocated = true;
        for(int i = 0; i <= l; i++)
            data[i] = inp[i];
        length = l;
    }
}
void String::create(String& inp){ // shares
    share(inp);
}
void String::create(String&& inp){ // shares with temp
    share(inp);
}
void String::share(String& inp){ // establishes sharing links
    wipe();
    links.link(inp.links);
    data = inp.data;
    length = inp.length;
    
    allocated = true;
}

void String::copy(String& inp){
    if(inp.isAlloc()){
        wipe();
        length = inp.length;
        data = new char[length+1];
        allocated = true;
        for(int i = 0; i < length+1; i++){
            data[i] = inp.data[i];
        }
    }
}
//////////////////////////////// features  ////////////////
template <typename T1, typename T2 >
void String::concat(T1&& inp1, T2&& inp2){
    if(inp1.getData() && inp2.getData()){
        wipe();
        length = inp1.length + inp2.length;
        data = new char[length+1];
        allocated = true;
        
        for(int i = 0; i < inp1.length; i++)
            data[i] = inp1.data[i];
        for(int i = 0; i < inp2.length; i++)
            data[inp1.length+i] = inp2.data[i];
        
        data[length] = '\0';
    }
}
template <typename T1, typename T2, typename T3>
void String::concat(T1&& inp1, T2&& inp2, T3&& inp3){
    wipe();
    
    length = inp1.length + inp2.length + inp3.length;
    data = new char[length+1];
    allocated = true;
    
    //        StringMemLeak+=sizeof(char)*(length+1);
    //        if(printing)printf("+newConcat:\t%x\t%u\n", data, StringMemLeak);
    
    
    for(int i = 0; i < inp1.length; i++)
        data[i] = inp1.data[i];
    for(int i = 0; i < inp2.length; i++)
        data[inp1.length+i] = inp2.data[i];
    for(int i = 0; i < inp3.length; i++)
        data[inp1.length+inp2.length+i] = inp3.data[i];
    
    data[length] = '\0';
}

void String::append(String&& inp){
    concat(String(*this), inp);
}
void String::append(String& inp){
    concat(String(*this), inp);
}
void String::append(char* inp){
    append(String(inp));
}

void String::fill(unsigned int size, char item){
    wipe();
    length = size;
    data = new char[size+1];
    allocated = true;
    for(int i = 0; i < length; i++) data[i] = item;
    data[length] = '\0';
}

void String::itoc(int num){
    int numlen = getNumLength(num);
    bool neg = (num<0);
    num = (neg)?-num:num;
    numlen += neg;
    char* buf = new char[numlen+1];
    if(num){
        for(int i = numlen-1; num; i--){
            buf[i] = (num % 10) + '0';
            num /= 10;
        }
        if (neg)
            buf[0] = '-';
    }else
        buf[0] = '0';
    buf[numlen] = '\0';
    
    create(buf);
    //        StringMemLeak-=sizeof(char)*(numlen+1);
    //        if(printing)printf("-itoc.buf:\t%x\t%u\n", buf, StringMemLeak);
    delete [] buf;
}
void String::dtoc(double num, char dig){ // dig is number of sigfigs after dec
    if(dig < 1){
        //                if(printing)printf("Enter decimal length of 1 or more. Defaulting to 4.\n");
        dig = 4;
    }
    String whole((int)num);  // make String from whole part of num
    num = (num<0)?-num:num;  // abs(num)
    num -= (int)num;         // get rid of whole part
    for(int i = 0; i < dig; i++)// put 'dig' digits left of decimal
        num *= 10;
    String dec((int)num); // make String from decimal part
    String dot("."); // make String for decimal itself
    String zeros;       // make up for lost zeros
    zeros.fill(dig - getNumLength(num), '0');
    dot.append(zeros);
    
    this->concat(whole, dot, dec);
}

void String::truncate(unsigned int amount){
    if(amount > length){
        printf("Cannot truncate more than entire length of String.\n");
        return;
    }
    
    String temp(*this);
    wipe();
    
    length  = temp.length - amount;
    data = new char[length+1];
    allocated = true;
    //        StringMemLeak+=sizeof(char)*(length+1);
    //        if(printing)printf("+newConcat:\t%x\t%u\n", data, StringMemLeak);
    
    for(int i = 0; i < length; i++)
        data[i] = temp.data[i];
    data[length] = '\0';
}

void String::popPath(int numOfPaths){
    int amount = -1;
    for(int i = length; numOfPaths && i; i--){
        if(data[i] == '/') numOfPaths--;
        amount++;
    }
    
    truncate(amount);
}

void String::addPath(String&& path){
    if(length) append("/");
    append(path);
}
void String::addPath(String& path){
    if(length) append("/");
    append(path);
}
void String::addPath(char* path){
    String temp(path);
    addPath(temp);
}

String& String::ripPath(){
    int amount = 0;
    for(int i = length; data[i] != '/' && i; i--) amount++;
    if(ans) delete ans;
    ans = new String;
    ans->create(data[length-amount+1]);
    return *ans;
}

//////////////////////////////// misc      ////////////////
void String::clear(){
    for(int i = 0; i < length; i++) data[i] = '\0';
}

int String::stoi(){
    int result = 0;
    for(int i = 0; (data[i] != 0) && (data[i] != '.'); i++){
        result *= 10;
        result += data[i] - 48;
    }
    return result;
}

bool String::compare(char* inp){
    for(int i = 0; data[i]; i++){
        if(data[i] != inp[i])
            return false;
    }
    return true;
}
bool String::compare(String& inp){
    for(int i = 0; data[i]; i++){
        if(data[i] != inp.data[i])
            return false;
    }
    return true;
}

void String::print(){
    if(data){
        for(int i = 0; data[i]; i++){
            printf("%c", data[i]);
        }
    }
}
void String::println(){
    print(); printf("\n");
}
void String::printHex(){
    if(data){
        for(int i = 0; data[i]; i++){
            printf("%.2x ", data[i]);
        }
    }
    printf("\n");
}

//////////////////////////////// accessors ////////////////
char* String::getData(){
    return data;
}

unsigned int String::getLen(){
    return length;
}

bool String::isAlloc(){
    return allocated;
}

String& String::getLender(){
    if(links.getLender())
        return links.getLender()->getHost();
    return *this;
}
String& String::getBorrower(){
    if(links.getBorrower())
        return links.getBorrower()->getHost();
    return *this;
}
//////////////////////////////// mutators  ////////////////

#endif /* String_h */
