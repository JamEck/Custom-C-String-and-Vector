template <typename T>
class vector{
protected:
    static bool printing;
    static Uint64 vecMemLeak;
    T* data = nullptr;
    Uint32 len = 0;
    bool allocated = false;

    linker<vector<T>> links;
    
    vector<T>* ans = nullptr;
    
public:
    
    vector():links(this){}
    vector(Uint32 size):links(this){create(size);}
    vector(vector<T>& inp):links(this){create(inp);}
    ~vector(){wipe(); if(ans) delete ans; }
    
    ///////// operators ////////////////////////////////////////////
    
    T& operator[] (int index){
        if(index < 0){
            index += len;
            if(index < 0) index = 0;
        }
        else if(index >= len){index = len-1;}
        return data[index];
    }
    void operator= (vector<T>& inp){share(inp);}
    
    template <typename B>
    vector<T>& operator+ (vector<B>& inp){
        if(len == inp.getLen()){
            if (ans) delete ans;
            ans = new vector<T>(len);
            for(int i = 0; i < len; i++){
                (*ans)[i] = (*this)[i] + inp[i];
            }
            return *ans;
        }
        printf("Vectors Lengths Do Not Conform\n");
        printf("Returning First Addend\n");
        return *this;
    }
    
    template <typename B>
    T operator* (vector<B>& inp){
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
    
    template <typename B>
    vector<T>& operator* (B scalar){
        if(ans) delete ans;
        ans = new vector<T>(len);
        for(int i = 0; i < len; i++){
            (*ans)[i] = (*this)[i] * scalar;
        }
        return *ans;
    }
    
    template <typename B>
    void operator*= (B scalar){
        for(int i = 0; i < len; i++){
            data[i] *= scalar;
        }
    }
    
    template <typename B>
    vector<T>& operator/ (B scalar){
        if(ans) delete ans;
        ans = new vector<T>(len);
        for(int i = 0; i < len; i++){
            (*ans)[i] = (*this)[i] / scalar;
        }
        return *ans;
    }
    
    template <typename B>
    void operator/= (B scalar){
        for(int i = 0; i < len; i++){
            data[i] /= scalar;
        }
    }
    
    bool operator== (vector<T>& inp){
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

    void wipe(){
        if(allocated){
            links.relink();
            if(!links.isProtected()){
                if(printing)printf("Deleting Vector\n");
                vecMemLeak-=sizeof(T)*(len);
                if(printing)printf("-vector:\t%x\t%u\n", data, vecMemLeak);
                delete [] data;
            }
        }
        links.wipe();
        data = nullptr;
        allocated = false;
        len = 0;
    }
    
    void share(vector<T>& inp){
        wipe();

        links.link(inp.links);
        
        data = inp.data;
        len = inp.len;
        allocated = true;
    }
    void share(vector<T>&& inp){
         share(inp);
    }
    
    void copy(vector<T>& inp){
        wipe();
        len = inp.len;
        data = new T[len];
        allocated = true;
        vecMemLeak+=sizeof(T)*(len);
        if(printing)printf("+vector:\t%x\t%u\n", data, vecMemLeak);
        
        for(int i = 0; i < len; i++){
            data[i] = inp.data[i];
        }
    }
    void copy(vector<T>&& inp){
         copy(inp);
    }

    void create(Uint32 size){
        wipe();
        len = size;
        data = new T[len];
        allocated = true;
        vecMemLeak+=sizeof(T)*(len);
        if(printing)printf("+vector:\t%x\t%u\n", data, vecMemLeak);
    }
    void create(vector<T>& inp){
        share(inp);
    }
    void create(T* inpdata, Uint32 size){
        wipe();
        len = size;
        data = inpdata;
        allocated = true;
        vecMemLeak+=sizeof(T)*(len);
        if(printing)printf("+vector:\t%x\t%u\n", data, vecMemLeak);
    }
    
    bool loadin(char* inp){
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
    bool loadin(string& inp){
        if(loadin(inp.getData()))
            return true;
        return false;
    }
    
    ///////////// initialization /////////////////////////
    
    void fill(T val){
        if(allocated){
            for(int i = 0; i < len; i++){
                data[i] = val;
            }
        }
    }
    
    ///////////// organization ///////////////////////////
    
    // selection sort
    vector<T>& sort(Uint32 mask = 0xFFFFFFFF){
        int cursor = 0;
        int min = 0;
        int imin = 0;
        Uint32 transfer = 0;
        if(ans) delete ans;
        ans = new vector<T>(len); // ans holds the new arrangement
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
    
    void rearrange(vector<int>& instr){
        if(instr.len == len){
            T temp = 0;
            Uint32 dir = 0;
            for(int i = 0; i < instr.len; i++){
                dir = instr[i];
                while(dir < i) dir = instr[dir];
                
                temp = data[dir];
                data[dir] = data[i];
                data[i] = temp;
            }
        }else{
            printf("vector lengths dont match\n");
        }
    }
    
    ///////////// Math Funcs /////////////////////////////
    
    T max(){
        T ans = data[0];
        for(int i = 0; i < len; i++){
            if(data[i]>ans)
                ans = data[i];
        }
        return ans;
    }
    T min(){
        T ans = data[0];
        for(int i = 0; i < len; i++){
            if(data[i]<ans)
                ans = data[i];
        }
        return ans;
    }

    T sum(){
        T ans = 0;
        for(int i = 0; i < len; i++){
            ans += data[i];
        }
        return ans;
    }
    
    
    ///////////// Printing ///////////////////////////////

    void print(){
        if(typeid(*data).hash_code() == typeid(float). hash_code()
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
    
    Uint32 getLen(){
        return len;
    }
    
    T* getData(){
        return data;
    }
    
    bool isAlloc(){
        return allocated;
    }
    
    vector<T>& getLender(){
        if(links.getLender())
            return links.getLender()->getHost();
        return *this;
    }
    vector<T>& getBorrower(){
        if(links.getBorrower())
            return links.getBorrower()->getHost();
        return *this;
    }
    vector<T>& getAns(){
        return *ans;
    }
    
    ///////////// Mutators ///////////////////////////////
    
    void protect(bool inp){
        links.protect(inp);
    }
};
template <typename T>
bool vector<T>::printing = false;
template <typename T>
Uint64 vector<T>::vecMemLeak = 0;

template<>
pair<int> vector<pair<int>>::min(){
    pair<int> min;
    min = data[0];
    for(int i = 0; i < len; i++){
        if(data[i].x < min.x) min.x = data[i].x;
        if(data[i].y < min.y) min.y = data[i].y;
    }
    return min;
}
template<>
pair<int> vector<pair<int>>::max(){
    pair<int> max;
    max = data[0];
    for(int i = 0; i < len; i++){
        if(data[i].x > max.x) max.x = data[i].x;
        if(data[i].y > max.y) max.y = data[i].y;
    }
    return max;
}

template<>
void vector<pair<int>>::print(){
    for(int i = 0; i < len; i++){
        data[i].print();
    }
}



template <typename T>
class matrix : public vector<T>{
private:
    bool ansAlloc = false;
    int dim  = 0;
    int rows = 1;
    int cols = 1;
    int lays = 1;
    matrix<T>* ans = nullptr;
public:

    matrix(){}
    matrix(int dim1, int dim2 = 1, int dim3 = 1){
        rows = (dim1>0)?dim1:1;
        cols = (dim2>0)?dim2:1;
        lays = (dim3>0)?dim3:1;
        dim = (rows>1)+(cols>1)+(lays>1);
        this->create(rows*cols*lays);
    }
    ~matrix(){
        if(ansAlloc)
            delete ans;
        ansAlloc = false;
    }

    void share(matrix<T>& inp){
        vector<T>::share(inp);
        dim = inp.dim;
        rows = inp.rows;
        cols = inp.cols;
        lays = inp.lays;
    }
    
    T& operator() (Uint32 row = 0, Uint32 col = 0, Uint32 lay = 0){
        row = (row<rows)?row:rows-1;
        col = (col<cols)?col:cols-1;
        lay = (lay<lays)?lay:lays-1;
        return this->data[lay*rows*cols + row*cols + col];
    }

    bool checkConform(matrix<T>& inp){
        if(rows == inp.rows &&
           cols == inp.cols &&
           lays == inp.lays)
        {
            return true;
        }
        return false;
    }
    
    matrix<T>& t (){
        if(ansAlloc) delete ans;
        ans = new matrix<T>(cols, rows, lays);
        ansAlloc = true;
        
        for(int i = 0, j = 0; i < this->len; i++, j+=cols){
            if(!(i%cols)) j = i/cols;
            ans->getData()[j] = (*this)[i];
        }
        return *ans;
    }
    
    matrix<T>& max(int dir = 1){
        int dim1 = (dir == 1)?1:rows;
        int dim2 = (dir == 2)?1:cols;
        int dim3 = (dir == 3)?1:lays;
        if(dir > dim) dir = dim;
        if(dim < 2) dim1 = dim2 = dim3 = 1;

        if(ansAlloc) delete ans;
        ans = new matrix<T>(dim1, dim2, dim3);
        ansAlloc = true;
        for(int d3 = 0; d3 < dim3; d3++){
            for(int d2 = 0; d2 < dim2; d2++){
                for(int d1 = 0; d1 < dim1; d1++){
                    (*ans)(d1, d2, d3) = (*this)(d1, d2, d3);
                }
            }
        }
        for(int d3 = 0; d3 < lays; d3++){
            for(int d2 = 0; d2 < cols; d2++){
                for(int d1 = 0; d1 < rows; d1++){
                    if((*ans)(d1, d2, d3) < (*this)(d1, d2, d3)){
                        (*ans)(d1, d2, d3) = (*this)(d1, d2, d3);
                    }
                }
            }
        }
        return *ans;
    }
    matrix<T>& min(int dir = 1){
        int dim1 = (dir == 1)?1:rows;
        int dim2 = (dir == 2)?1:cols;
        int dim3 = (dir == 3)?1:lays;
        if(dir > dim) dir = dim;
        if(dim < 2) dim1 = dim2 = dim3 = 1;
        
        if(ansAlloc) delete ans;
        ans = new matrix<T>(dim1, dim2, dim3);
        ansAlloc = true;
        for(int d3 = 0; d3 < dim3; d3++){
            for(int d2 = 0; d2 < dim2; d2++){
                for(int d1 = 0; d1 < dim1; d1++){
                    (*ans)(d1, d2, d3) = (*this)(d1, d2, d3);
                }
            }
        }
        for(int d3 = 0; d3 < lays; d3++){
            for(int d2 = 0; d2 < cols; d2++){
                for(int d1 = 0; d1 < rows; d1++){
                    if((*ans)(d1, d2, d3) > (*this)(d1, d2, d3)){
                        (*ans)(d1, d2, d3) = (*this)(d1, d2, d3);
                    }
                }
            }
        }
        return *ans;
    }
    
    matrix<T>& sum(int dir = 1){
        int dim1 = (dir == 1)?1:rows;
        int dim2 = (dir == 2)?1:cols;
        int dim3 = (dir == 3)?1:lays;
        if(dir > dim) dir = dim;
        if(dim < 2) dim1 = dim2 = dim3 = 1;
        
        if(ansAlloc) delete ans;
        ans = new matrix<T>(dim1, dim2, dim3);
        ans->fill(0);
        ansAlloc = true;

        for(int d3 = 0; d3 < lays; d3++){
            for(int d2 = 0; d2 < cols; d2++){
                for(int d1 = 0; d1 < rows; d1++){
                    (*ans)(d1, d2, d3) += (*this)(d1, d2, d3);
                }
            }
        }
        return *ans;
    }
    
    matrix<T>& getCol(int col){
        col = (col > cols)?cols:col;
        col = (col <    0)?   0:col;
        
        if(ansAlloc) delete ans;
        ans = new matrix<T>(rows, 1, lays);
        ansAlloc = true;
        for(int d3 = 0; d3 < lays; d3++){
            for(int d1 = 0; d1 < rows; d1++){
                (*ans)(d1, col, d3) = (*this)(d1, col, d3);
            }
        }
        return *ans;
    }
    matrix<T>& getRow(int row){
        row = (row > rows)?rows:row;
        row = (row <    0)?   0:row;
        
        if(ansAlloc) delete ans;
        ans = new matrix<T>(1, cols, lays);
        ansAlloc = true;
        for(int d3 = 0; d3 < lays; d3++){
            for(int d2 = 0; d2 < cols; d2++){
                (*ans)(row, d2, d3) = (*this)(row, d2, d3);
            }
        }
        return *ans;
    }
    matrix<T>& getLay(int lay){
        lay = (lay > lays)?lays:lay;
        lay = (lay <    0)?   0:lay;
        
        if(ansAlloc) delete ans;
        ans = new matrix<T>(rows, cols, 1);
        ansAlloc = true;
        
        for(int d2 = 0; d2 < cols; d2++){
            for(int d1 = 0; d1 < rows; d1++){
                (*ans)(d1, d2, lay) = (*this)(d1, d2, lay);
            }
        }
        return *ans;
    }
    
    matrix<T>& operator* (matrix<T>& inp){
        if(dim < 3 && inp.dim < 3){
            if(cols == inp.rows){
                
                matrix<T>* prod = new matrix<T>(rows, inp.cols, 1);
                
                for(int m1row = 0; m1row < rows; m1row++){
                    for(int m2col = 0; m2col < inp.cols; m2col++){
                        (*prod)(m1row, m2col) = vector<T>::dot((*this).getRow(m1row), inp.getCol(m2col));
                    }
                }
                
                
                if(ansAlloc) delete ans;
                ans = prod;
                ansAlloc = true;
                
                return *ans;
            }
            printf("Matrix Dimensions Do Not Conform\n");
            return *this;
        }
        printf("Cannot Multiply 3+ Dimensional Matrices\n");
        return *this;
    }
    matrix<T>& operator+ (matrix<T>& inp){
        if(checkConform(inp)){
            
            if(ansAlloc) delete ans;
            ans = new matrix<T>(rows, cols, lays);
            ansAlloc = true;
            
            for(int i = 0; i < this->len; i++){
                ans->getData()[i] = (*this)[i] + inp[i];
            }
            return *ans;
        }
        printf("Matrix Dimensions Do Not Conform\n");
        return (*this);
    }
    matrix<T>& operator- (matrix<T>& inp){
        if(checkConform(inp)){
            
            if(ansAlloc) delete ans;
            ans = new matrix<T>(rows, cols, lays);
            ansAlloc = true;
            
            for(int i = 0; i < this->len; i++){
                ans->getData()[i] = (*this)[i] - inp[i];
            }
            return *ans;
        }
        printf("Matrix Dimensions Do Not Conform\n");
        return (*this);
    }
    
    void print(){
        if(typeid(*this->data).hash_code() == typeid(float). hash_code()
           || typeid(*this->data).hash_code() == typeid(double).hash_code()){
            for(int i = 0; i < this->len; i++){
                printf("%.4lf ", this->data[i]);
                if(!((i+1)%cols)) printf("\n");
                if(!((i+1)%(rows*cols))) printf("\n");
            }
            printf("\n");
        }else{
            for(int i = 0; i < this->len; i++){
                printf("%3d ", this->data[i]);
                if(!((i+1)%cols)) printf("\n");
                if(!((i+1)%(rows*cols))) printf("\n");
            }
            printf("\n");
        }
    }
    
    matrix<T>& getAns(){
        if(ans)
            return *ans;
        else
            return *this;
    }
};