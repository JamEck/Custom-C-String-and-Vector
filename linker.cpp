template <class C>
struct linker{
private:
    linker* lender = nullptr;
    linker* borrower = nullptr;
    C* host = nullptr;
    bool protection = false;
public:
    
    linker(C* inp):host(inp){}
    
    void wipe(){
        lender = nullptr;
        borrower = nullptr;
        protection = false;
    }
    
    linker* defer(linker* inp){
        if(inp->borrower)
            return defer(inp->borrower);
        return inp;
    }
    
    void relink(){
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
    
    void link(linker& inp){
        lender = defer(&inp);
        lender->borrower = this;
        
        protection = true;
        lender->protection = true;
    }
    
    ////// accessors //////
    
    bool isProtected(){
        return protection;
    }
    
    C& getHost(){
        return *host;
    }
    linker* getBorrower(){
        if(borrower){
            return borrower;
        }else{
            printf("No Borrower\n");
        }
        return this;
    }
    linker* getLender(){
        if(lender){
            return lender;
        }else{
            printf("No Lender\n");
        }
        return this;
    }
    
    void protect(bool inp){
        protection = inp;
    }
    
    void print(){
        linker* curr = defer(this);
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
};