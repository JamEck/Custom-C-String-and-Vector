class string{
private:
    bool allocated = false;
    char* data = nullptr;
    unsigned int length = 0;
    
    linker<string> links;
    
    string* ans = nullptr;
    //////////////////////////////// set-up and destroy ///////////
    
    int getLength(char* inp){
        int count = 0;
        if(inp != nullptr){
            for (count = 0; inp[count]; count++);
        }
        return count;
    }
    int getNumLength(int num){
        int ans;
        for(ans = 0; num; ans++){
            num /= 10;
        }
        return ans + !ans;
    }
    
    void wipe(){
        links.relink();
        if(allocated && !links.isProtected()){
            delete [] data;
        }
        allocated = false;
        links.wipe();
        data = nullptr;
        length = 0;
    }
    
public:
    
    //////////////////////////////// constructors  ////////////////
    
    string():links(this){}
    string(char* inp):links(this){create(inp);}
    string(string& inp):links(this){create(inp);} // share constructor
    string(string&& inp):links(this){create(inp);}// share from temp
    
    string(string& inp1, string& inp2):links(this){concat(inp1, inp2);}
    string(string& inp1, string& inp2, string& inp3):links(this){concat(inp1, inp2, inp3);}
    
    string(int num):links(this){ itoc(num);}
    string(double num, char dig):links(this){dtoc(num, dig);}
    ~string(){wipe();} // destructor
    
    //////////////////////////////// operators ////////////////

    void    operator=  (string& inp){create(inp);} // share
    string& operator+  (string& inp){
        if(ans) delete ans;
        ans = new string;
        ans->concat(*this, inp);
        return *ans;
    } // concat and return
    bool    operator== (string& inp){return compare(inp);} // compare
    char    operator[] (int ind){
        if(ind < 0){
            ind = length + ind;
            if(ind < 0) ind = 0;
        }
        else ind = (ind >= length)?length-1:ind;
        return data[ind];
    } // get element

    //////////////////////////////// creation  ////////////////
    
    void create(char* inp){
        if(inp != nullptr){
            wipe();
            int l = getLength(inp);
            data = new char[l+1];
            allocated = true;
            for(int i = 0; i <= l; i++)
                data[i] = inp[i];
            length = l;
        }
    } // creation from raw char array
    void create(string& inp){
        share(inp);
    } // shares
    void create(string&& inp){
        share(inp);
    } // shares with temp
    void share(string& inp){
        wipe();
        links.link(inp.links);
        data = inp.data;
        length = inp.length;

        allocated = true;
    } // establishes sharing links
    
    void copy(string& inp){
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
    void concat(T1&& inp1, T2&& inp2){
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
    void concat(T1&& inp1, T2&& inp2, T3&& inp3){
        wipe();
        
        length = inp1.length + inp2.length + inp3.length;
        data = new char[length+1];
        allocated = true;
        
//        stringMemLeak+=sizeof(char)*(length+1);
//        if(printing)printf("+newConcat:\t%x\t%u\n", data, stringMemLeak);
        
        
        for(int i = 0; i < inp1.length; i++)
            data[i] = inp1.data[i];
        for(int i = 0; i < inp2.length; i++)
            data[inp1.length+i] = inp2.data[i];
        for(int i = 0; i < inp3.length; i++)
            data[inp1.length+inp2.length+i] = inp3.data[i];
        
        data[length] = '\0';
    }
    
    void append(string&& inp){
        concat(string(*this), inp);
    }
    void append(string& inp){
        concat(string(*this), inp);
    }
    void append(char* inp){
        append(string(inp));
    }
    
    void fill(unsigned int size, char item){
        wipe();
        length = size;
        data = new char[size+1];
        allocated = true;
        for(int i = 0; i < length; i++) data[i] = item;
        data[length] = '\0';
    }
    
    void itoc(int num){
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
//        stringMemLeak-=sizeof(char)*(numlen+1);
//        if(printing)printf("-itoc.buf:\t%x\t%u\n", buf, stringMemLeak);
        delete [] buf;
    }
    void dtoc(double num, char dig){ // dig is number of sigfigs after dec
        if(!isnan(num)){
            if(dig < 1){
//                if(printing)printf("Enter decimal length of 1 or more. Defaulting to 4.\n");
                dig = 4;
            }
            string whole((int)num);  // make string from whole part of num
            num = (num<0)?-num:num;  // abs(num)
            num -= (int)num;         // get rid of whole part
            for(int i = 0; i < dig; i++)// put 'dig' digits left of decimal
                num *= 10;
            string dec((int)num); // make string from decimal part
            string dot("."); // make string for decimal itself
            string zeros;       // make up for lost zeros
            zeros.fill(dig - getNumLength(num), '0');
            dot.append(zeros);
            
            this->concat(whole, dot, dec);
        }else{
            printf("Not a Number!\n");
            create("NaN");
        }
    }
    
    void truncate(unsigned int amount){
        if(amount > length){
            printf("Cannot truncate more than entire length of string.\n");
            return;
        }
        
        string temp(*this);
        wipe();
        
        length  = temp.length - amount;
        data = new char[length+1];
        allocated = true;
//        stringMemLeak+=sizeof(char)*(length+1);
//        if(printing)printf("+newConcat:\t%x\t%u\n", data, stringMemLeak);
        
        for(int i = 0; i < length; i++)
            data[i] = temp.data[i];
        data[length] = '\0';
    }
    
    void popPath(int numOfPaths = 1){
        int amount = -1;
        for(int i = length; numOfPaths && i; i--){
            if(data[i] == '/') numOfPaths--;
            amount++;
        }
        truncate(amount);
    }
    
    void addPath(string&& path){
        if(length) append("/");
        append(path);
    }
    void addPath(string& path){
        if(length) append("/");
        append(path);
    }
    void addPath(char* path){
        string temp(path);
        addPath(temp);
    }
    
    string& ripPath(){
        int amount = 0;
        for(int i = length; data[i] != '/' && i; i--) amount++;
        if(ans) delete ans;
        ans = new string;
        ans->create(&data[length-amount+1]);
        return *ans;
    }
    
    //////////////////////////////// misc      ////////////////
    void clear(){
        for(int i = 0; i < length; i++) data[i] = '\0';
    }
    
    int stoi(){
        int result = 0;
        for(int i = 0; (data[i] != 0) && (data[i] != '.'); i++){
            result *= 10;
            result += data[i] - 48;
        }
        return result;
    }
    
    bool compare(char* inp){
        for(int i = 0; data[i]; i++){
            if(data[i] != inp[i])
                return false;
        }
        return true;
    }
    bool compare(string& inp){
        for(int i = 0; data[i]; i++){
            if(data[i] != inp.data[i])
                return false;
        }
        return true;
    }
    
    void print(){
        if(data){
            for(int i = 0; data[i]; i++){
                printf("%c", data[i]);
            }
        }
    }
    void println(){
        print(); printf("\n");
    }
    void printHex(){
        if(data){
            for(int i = 0; data[i]; i++){
                printf("%.2x ", data[i]);
            }
        }
        printf("\n");
    }
    
    //////////////////////////////// accessors ////////////////
    char* getData(){
        return data;
    }
    
    unsigned int getLen(){
        return length;
    }
    
    bool isAlloc(){
        return allocated;
    }
    
    string& getLender(){
        if(links.getLender())
            return links.getLender()->getHost();
        return *this;
    }
    string& getBorrower(){
        if(links.getBorrower())
            return links.getBorrower()->getHost();
        return *this;
    }
    //////////////////////////////// mutators  ////////////////
    
};