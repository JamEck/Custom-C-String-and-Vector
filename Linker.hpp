#ifndef Linker_h
#define Linker_h

#include <stdio.h>

template <class C>
struct Linker{
private:
    Linker* lender = nullptr;
    Linker* borrower = nullptr;
    C* host = nullptr;
    bool protection = false;
public:
    
    Linker(C*);
    
    void wipe();
    
    Linker* defer(Linker*);
    
    void relink();
    
    void link(Linker&);
    
    ////// accessors //////
    
    bool isProtected();
    
    C& getHost();
    
    Linker* getBorrower();
    
    Linker* getLender();
    
    void print();
};



///////////////////////////////////////////////////////////////


//                      IMPLEMENTATION                       //


///////////////////////////////////////////////////////////////





template <class C>
Linker<C>::Linker(C* inp):host(inp){}

template <class C>
void Linker<C>::wipe(){
    lender = nullptr;
    borrower = nullptr;
    protection = false;
}

template <class C>
Linker<C>* Linker<C>::defer(Linker* inp){
    if(inp->borrower)
        return defer(inp->borrower);
    return inp;
}

template <class C>
void Linker<C>::relink(){
    if(lender && borrower){
        lender->borrower = borrower;
        borrower->lender = lender;
    }else
        if(lender){
            lender->borrower = nullptr;
        }else
            if(borrower){
                borrower->lender = nullptr;
            }
    
    if(!(lender || borrower)){
        protection = false;
    }
}

template <class C>
void Linker<C>::link(Linker& inp){
    lender = defer(&inp);
    lender->borrower = this;
    
    protection = true;
    lender->protection = true;
}

////// accessors //////

template <class C>
bool Linker<C>::isProtected(){
    return protection;
}

template <class C>
C& Linker<C>::getHost(){
    return *host;
}

template <class C>
Linker<C>* Linker<C>::getBorrower(){
    if(borrower){
        return borrower;
    }else{
        printf("No Borrower\n");
    }
    return this;
}

template <class C>
Linker<C>* Linker<C>::getLender(){
    if(lender){
        return lender;
    }else{
        printf("No Lender\n");
    }
    return this;
}

template <class C>
void Linker<C>::print(){
    Linker* curr = defer(this);
    int count = 1;
    printf("END\n");
    printf("%d. %x\n", count, curr);
    while(curr->lender){
        count++;
        curr = curr->lender;
        printf("%d. %x\n", count, curr);
    }
    printf("START\n");
}

#endif /* Linker_h */
