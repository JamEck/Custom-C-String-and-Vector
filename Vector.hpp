#ifndef Vector_hpp
#define Vector_hpp

#include <stdio.h>
#include <typeinfo>  //for 'typeid' to work

#include "Linker.hpp"

template <typename T>
class Vector{
protected:
    static bool printing;
    static unsigned long vecMemLeak;
    T* data = nullptr;
    unsigned int len = 0;
    bool allocated = false;

    Linker<Vector<T>> links;
    
    Vector<T>* ans = nullptr;
    
public:
    
    Vector();
    Vector(unsigned int);
    Vector(Vector<T>&);
    ~Vector();
    
    ///////// operators ////////////////////////////////////////////
    
    T& operator[] (int);
    void operator= (Vector<T>&);
    
    template <typename B>
    Vector<T>& operator+ (Vector<B>&);
    
    template <typename B>
    T operator* (Vector<B>&);
    
    template <typename B>
    Vector<T>& operator* (B);
    
    template <typename B>
    void operator*= (B);
    
    template <typename B>
    Vector<T>& operator/ (B);
    
    template <typename B>
    void operator/= (B);
    
    bool operator== (Vector<T>& );
    
    ///////// creation & destruction ///////////////////////////////

    void wipe();
    
    void share(Vector<T>&);
    void share(Vector<T>&&);
    
    void copy(Vector<T>&);
    void copy(Vector<T>&&);
    
    void create(unsigned int);
    void create(Vector<T>&);
    void create(T*, unsigned int);
    
    bool loadin(char*);
    
    ///////////// initialization /////////////////////////
    
    void fill(T);
    
    ///////// Math Funcs /////////////////////////////////
    
    T max();
    T min();
    
    T sum();
    
    double avg();
    
    double var();

    double std();
    
    ///////////// organization ///////////////////////////
    
    // selection sort
    Vector<T>& sort(unsigned int mask = 0xFFFFFFFF);
    
    void rearrange(Vector<int>&);
    
    ///////////// Printing ///////////////////////////////

    void print();
    
    ///////////// Accessors ///////////////////////////////
    
    unsigned int getLen();
    
    T* getData();
    
    bool isAlloc();
    
    Vector<T>& getLender();
    Vector<T>& getBorrower();
    Vector<T>& getAns();
    
    ///////////// Mutators ///////////////////////////////
    
    void protect(bool);
};
template <typename T>
bool Vector<T>::printing = false;
template <typename T>
unsigned long Vector<T>::vecMemLeak = 0;



///////////////////////////////////////////////////////////////


//                      IMPLEMENTATION                       //


///////////////////////////////////////////////////////////////

template <typename T>
Vector<T>::Vector():links(this){}
template <typename T>
Vector<T>::Vector(unsigned int size):links(this){create(size);}
template <typename T>
Vector<T>::Vector(Vector<T>& inp):links(this){create(inp);}
template <typename T>
Vector<T>::~Vector(){wipe(); if(ans) delete ans; }

///////// operators ////////////////////////////////////////////

template <typename T>
T& Vector<T>::operator[] (int index){
    if(index < 0){
        index += len;
        if(index < 0) index = 0;
    }
    else if(index >= len){index = len-1;}
    return data[index];
}
template <typename T>
void Vector<T>::operator= (Vector<T>& inp){share(inp);}

template <typename T>
template <typename B>
Vector<T>& Vector<T>::operator+ (Vector<B>& inp){
    if(len == inp.getLen()){
        if (ans) delete ans;
        ans = new Vector<T>(len);
        for(int i = 0; i < len; i++){
            (*ans)[i] = (*this)[i] + inp[i];
        }
        return *ans;
    }
    printf("Vectors Lengths Do Not Conform\n");
    printf("Returning First Addend\n");
    return *this;
}

template <typename T>
template <typename B>
T Vector<T>::operator* (Vector<B>& inp){
    if(len == inp.getLen()){
        T ans = 0;
        for(int i = 0; i < len; i++){
            ans += ((*this)[i] * (T)inp.getData()[i]);
        }
        return ans;
    }
    printf("Vectors don't conform.\n");
    return 0;
}

template <typename T>
template <typename B>
Vector<T>& Vector<T>::operator* (B scalar){
    if(ans) delete ans;
    ans = new Vector<T>(len);
    for(int i = 0; i < len; i++){
        (*ans)[i] = (*this)[i] * scalar;
    }
    return *ans;
}

template <typename T>
template <typename B>
void Vector<T>::operator*= (B scalar){
    for(int i = 0; i < len; i++){
        data[i] *= scalar;
    }
}

template <typename T>
template <typename B>
Vector<T>& Vector<T>::operator/ (B scalar){
    if(ans) delete ans;
    ans = new Vector<T>(len);
    for(int i = 0; i < len; i++){
        (*ans)[i] = (*this)[i] / scalar;
    }
    return *ans;
}

template <typename T>
template <typename B>
void Vector<T>::operator/= (B scalar){
    for(int i = 0; i < len; i++){
        data[i] /= scalar;
    }
}

template <typename T>
bool Vector<T>::operator== (Vector<T>& inp){
    if(len == inp.getLen()){
        for(int i = 0; i < len; i++){
            if(data[i] != inp.data[i]){
                return false;
            }
        }
        return true;
    }
    return false;
}

///////// creation & destruction ///////////////////////////////

template <typename T>
void Vector<T>::wipe(){
    if(allocated){
        links.relink();
        if(!links.isProtected()){
            if(printing)printf("Deleting Vector\n");
            vecMemLeak-=sizeof(T)*(len);
            if(printing)printf("-Vector:\t%x\t%u\n", data, vecMemLeak);
            delete [] data;
        }
    }
    links.wipe();
    data = nullptr;
    allocated = false;
    len = 0;
}

template <typename T>
void Vector<T>::share(Vector<T>& inp){
    wipe();
    
    links.link(inp.links);
    
    data = inp.data;
    len = inp.len;
    allocated = true;
}

template <typename T>
void Vector<T>::share(Vector<T>&& inp){
    share(inp);
}

template <typename T>
void Vector<T>::copy(Vector<T>& inp){
    wipe();
    len = inp.len;
    data = new T[len];
    allocated = true;
    vecMemLeak+=sizeof(T)*(len);
    if(printing)printf("+Vector:\t%x\t%u\n", data, vecMemLeak);
    
    for(int i = 0; i < len; i++){
        data[i] = inp.data[i];
    }
}

template <typename T>
void Vector<T>::copy(Vector<T>&& inp){
    copy(inp);
}

template <typename T>
void Vector<T>::create(unsigned int size){
    wipe();
    len = size;
    data = new T[len];
    allocated = true;
    vecMemLeak+=sizeof(T)*(len);
    if(printing)printf("+Vector:\t%x\t%u\n", data, vecMemLeak);
}
template <typename T>
void Vector<T>::create(Vector<T>& inp){
    share(inp);
}
template <typename T>
void Vector<T>::create(T* inpdata, unsigned int size){
    wipe();
    len = size;
    data = inpdata;
    allocated = true;
    vecMemLeak+=sizeof(T)*(len);
    if(printing)printf("+Vector:\t%x\t%u\n", data, vecMemLeak);
}

template <typename T>
bool Vector<T>::loadin(char* inp){
    if(FILE* f = fopen(inp, "r")){
        fseek(f, 0, SEEK_END);
        wipe();
        len = ftell(f)/sizeof(T);
        data = new T[len];
        allocated = true;
        
        fseek(f, 0, SEEK_SET);
        fread(data, sizeof(T), len, f);
        
        return true;
    }else{
        printf("Failed to Open File: %s\n", inp);
        return false;
    }
}


///////////// initialization /////////////////////////

template <typename T>
void Vector<T>::fill(T val){
    if(allocated){
        for(int i = 0; i < len; i++){
            data[i] = val;
        }
    }
}

///////////// organization ///////////////////////////

// selection sort
template <typename T>
Vector<T>& Vector<T>::sort(unsigned int mask){
    int cursor = 0;
    int min = 0;
    int imin = 0;
    unsigned int transfer = 0;
    if(ans) delete ans;
    ans = new Vector<T>(len); // ans holds the new arrangement
    for(int i = 0; i < len; i++){
        ans->data[i] = i;
    }
    while(cursor < len){
        min = data[cursor] & mask;
        imin = cursor;
        for(int i = cursor+1; i < len; i++){
            if((data[i] & mask) < min){
                min = data[i] & mask;
                imin = i;
            }
        }
        transfer = data[imin];
        data[imin] = data[cursor];
        data[cursor] = transfer;
        
        transfer = ans->data[imin];
        ans->data[imin] = ans->data[cursor];
        ans->data[cursor] = transfer;
        
        cursor++;
    }
    return *ans;
}

template <typename T>
void Vector<T>::rearrange(Vector<int>& instr){
    if(instr.len == len){
        T temp = 0;
        unsigned int dir = 0;
        for(int i = 0; i < instr.len; i++){
            dir = instr[i];
            while(dir < i) dir = instr[dir];
            
            temp = data[dir];
            data[dir] = data[i];
            data[i] = temp;
        }
    }else{
        printf("Vector lengths dont match\n");
    }
}

///////////// Math Funcs /////////////////////////////

template <typename T>
T Vector<T>::max(){
    T ans = data[0];
    for(int i = 0; i < len; i++){
        if(data[i]>ans)
            ans = data[i];
    }
    return ans;
}
template <typename T>
T Vector<T>::min(){
    T ans = data[0];
    for(int i = 0; i < len; i++){
        if(data[i]<ans)
            ans = data[i];
    }
    return ans;
}

template <typename T>
T Vector<T>::sum(){
    T ans = 0;
    for(int i = 0; i < len; i++){
        ans += data[i];
    }
    return ans;
}


///////////// Printing ///////////////////////////////

template <typename T>
void Vector<T>::print(){
    if(typeid(*data).hash_code() == typeid(float).hash_code()
       || typeid(*data).hash_code() == typeid(double).hash_code()){
        for(int i = 0; i < len; i++){
            printf("%.4lf ", data[i]);
        }
        printf("\n");
    }else{
        for(int i = 0; i < len; i++){
            printf("%d ", data[i]);
        }
        printf("\n");
    }
}

///////////// Accessors ///////////////////////////////

template <typename T>
unsigned int Vector<T>::getLen(){
    return len;
}

template <typename T>
T* Vector<T>::getData(){
    return data;
}

template <typename T>
bool Vector<T>::isAlloc(){
    return allocated;
}

template <typename T>
Vector<T>& Vector<T>::getLender(){
    if(links.getLender())
        return links.getLender()->getHost();
    return *this;
}
template <typename T>
Vector<T>& Vector<T>::getBorrower(){
    if(links.getBorrower())
        return links.getBorrower()->getHost();
    return *this;
}
template <typename T>
Vector<T>& Vector<T>::getAns(){
    return *ans;
}

///////////// Mutators ///////////////////////////////

template <typename T>
void Vector<T>::protect(bool inp){
    links.protect(inp);
}

#endif /* String_h */
