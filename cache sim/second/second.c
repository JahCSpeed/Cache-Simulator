#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include<math.h>
struct cache{
    long *valid;
    long **tag;
    long *used;
    unsigned long *num;
    int counter;
};
void printData(long* b, int size){
    for(int i = 0; i <size; i ++ ){
        printf("%ld",b[i]);
    }
    printf("\n");
}
long* getBinaryRep(unsigned long binary,long bits){
    long* unsignedBit = calloc(bits,sizeof(long*));
    unsigned long quotent = binary;
    for(long i = bits - 1; i >= 0; i--){
        unsignedBit[i] = quotent % 2;
        quotent = quotent / 2;      
    }
    return unsignedBit;
}
long* getBinaryRep2(long binary,long bits){
    long* unsignedBit = calloc(bits,sizeof(long*));
    long quotent = binary;
    for(int i = bits - 1; i >= 0; i--){
        unsignedBit[i] = quotent % 2;
        quotent = quotent / 2;      
    }
    return unsignedBit;
}
long getValue(long* binary, long size){
    long value = 0;
    for(long i = 0; i < size; i ++){
        value += pow(2,(size-1) - i) * binary[i];
    }
    return value;
}
int getLow(long * array, int size){
    int temp = array[0];
    int ret = 0;
    for(int i = 1; i < size; i++){
        if(array[i] < temp){
            ret = i;
            temp = array[i];
        }
    }
    return ret;
}
long compData(long* a, long*b, long size){
    for(long i = 0; i < size; i ++){
        if(a[i] != b[i]){
            return -1;
        }
    }
    return 0;
}
void transData(long* a, long*b, long size){
    for(long i = 0; i < size; i ++){
        printf("Transfered: %ld to Index %ld\n",b[i],i);
        a[i] = b[i];
    }
}
void freeList(struct cache* head, long size, int tagBits, int asso){
        for(long i = 0; i < size; i ++){
            for(long j = 0; j < asso; j++){
                free(head[i].tag[j]);
            }
            free(head[i].tag);
            
            free(head[i].valid);
            free(head[i].used);
            free(head[i].num);
        }
        free(head);
}
int findSpot(struct cache* head, int size, int bitSize, int p){
    int empty = getLow(head[p].used,bitSize);
        for(int j = 0; j < bitSize; j++){
            if(head[p].valid[j] == 0){
                empty = j;
                return empty;
            }
        }

    return empty;
}
int isZero(struct cache* head, int validBitSize, int setTotal){
        for(int j = 0; j < validBitSize; j++){
            if(head[setTotal].valid[j] == 0){
                return -1;
            }
        }

    return 0;
}
void printData3(long* b, int size){
    for(int i = 0; i <size; i ++ ){
        printf("%ld",b[i]);
    }
    //printf("\n");
}
void printCache(struct cache* head, int asso, int tagSize, int setSize){
    for(int i = 0; i < setSize; i++){
        for(int k = 0; k < asso; k++){
            printf("Set Index: %d",i);
            printf("\tValid Bit: %ld",head[i].valid[k]);
            printf("\tTag: ");
            printData3(head[i].tag[k],tagSize);
            printf("(%ld)",getValue(head[i].tag[k],tagSize));
            printf("\n");
        }
        printf("---------------------------------------------------------\n");
    }
    printf("\n\n");
}
int findTag(struct cache*mainCache,long* tag, int size,int tagBits, int setIndex, int setSize){
    int save = 0;
    for(int i = 0; i < size; i++){
       if(compData(mainCache[setIndex].tag[i],tag,tagBits) == 0){
           save = i;
           return save;
       }
       
    }   
    return findSpot(mainCache, setSize,size,setIndex);
}
int isTagHere(struct cache*mainCache,long* tag, int size,int tagBits, int setIndex, int setSize){
    for(int i = 0; i < size; i++){
       if(compData(mainCache[setIndex].tag[i],tag,tagBits) == 0 && mainCache[setIndex].valid[i] == 1){
           return 0;
       }
       
    }
    return -1;
}
int main(int argc, char** argv){
    long cacheMiss = 0;
    long cacheHit = 0;
    long l2cacheHit = 0;
    long l2cacheMiss = 0;
    long read = 0;
    long write = 0;
    //L1 Cache setup
    long cacheSize = atoi(argv[1]);
    char* associativity = argv[2];
    long blockSize = atoi(argv[4]);
    int asso = 0;
    char* policy = argv[3];
    long setSize = 0;
    long setBits = 0;
    long blockBits = 0;
    long tagBit = 0;
    struct cache *mainCache;
    if(strlen(associativity) > 6){
        int n = associativity[6] -'0';
        setSize = cacheSize/(blockSize * n);
        asso = n;
        setBits = (log(setSize)/log(2));
        blockBits = (log(blockSize)/log(2));
        tagBit = (48 - blockBits - setBits);
        mainCache = malloc(sizeof(struct cache) * setSize);
        for(long i = 0; i < setSize; i ++){
            mainCache[i].tag = calloc(asso,sizeof(long*));
            for(long j = 0; j < asso; j++){
                mainCache[i].tag[j] = calloc(tagBit,sizeof(long*));
            }
            mainCache[i].valid = calloc(asso,sizeof(long*));
            mainCache[i].used = calloc(asso,sizeof(long*));
            mainCache[i].num = calloc(asso,sizeof(unsigned long*));
            mainCache[i].counter = 0;
        }
    }else if(strcmp(associativity,"assoc") == 0){
        asso = cacheSize/blockSize;
        setSize = 1;
        setBits = (log(setSize)/log(2));
        blockBits = (log(blockSize)/log(2));
        tagBit = (48 - blockBits - setBits);
        mainCache = malloc(sizeof(struct cache) * setSize);
        for(long i = 0; i < setSize; i ++){
            mainCache[i].tag = calloc(asso,sizeof(long*));
            for(long j = 0; j < asso; j++){
                mainCache[i].tag[j] = calloc(tagBit,sizeof(long*));
            }
            mainCache[i].valid = calloc(asso,sizeof(long*));
            mainCache[i].used = calloc(asso,sizeof(long*));
            mainCache[i].num = calloc(asso,sizeof(unsigned long*));
            mainCache[i].counter = 0;
        }
    }else if(strcmp(associativity,"direct") == 0){
        asso = 1;
        setSize = cacheSize/blockSize;
        setBits = (log(setSize)/log(2));
        blockBits = (log(blockSize)/log(2));
        tagBit = (48 - blockBits - setBits);
        mainCache = malloc(sizeof(struct cache) * setSize);
        for(long i = 0; i < setSize; i ++){
            mainCache[i].tag = calloc(asso,sizeof(long*));
            for(long j = 0; j < asso; j++){
                mainCache[i].tag[j] = calloc(tagBit,sizeof(long*));
            }
            mainCache[i].valid = calloc(asso,sizeof(long*));
            mainCache[i].used = calloc(asso,sizeof(long*));
            mainCache[i].num = calloc(asso,sizeof(unsigned long*));
            mainCache[i].counter = 0;

        }
    }else{
        printf("error\n");
        return EXIT_SUCCESS;
    }
    //L2 Cache setup
    long cacheSize2 = atoi(argv[5]);
    char* associativity2 = argv[6];
    int asso2 = 0;
    //char* policy2 = argv[7];
    long setSize2 = 0;
    long setBits2 = 0;
    long blockBits2 = 0;
    long tagBit2 = 0;
    struct cache *sideCache;
    if(strlen(associativity2) > 6){
        int n = associativity2[6] -'0';
        setSize2 = cacheSize2/(blockSize * n);
        asso2 = n;
        setBits2 = (log(setSize2)/log(2));
        blockBits2 = (log(blockSize)/log(2));
        tagBit2 = (48 - blockBits2 - setBits2);
        sideCache = malloc(sizeof(struct cache) * setSize2);
        for(long i = 0; i < setSize2; i ++){
            sideCache[i].tag = calloc(asso2,sizeof(long*));
            for(long j = 0; j < asso2; j++){
                sideCache[i].tag[j] = calloc(tagBit2,sizeof(long*));
            }
            sideCache[i].valid = calloc(asso2,sizeof(long*));
            sideCache[i].used = calloc(asso2,sizeof(long*));
            sideCache[i].num = calloc(asso2,sizeof(unsigned long*));
            sideCache[i].counter = 0;
        }
    }else if(strcmp(associativity2,"assoc") == 0){
        asso2 = cacheSize2/blockSize;
        setSize2 = 1;
        setBits2 = (log(setSize2)/log(2));
        blockBits2 = (log(blockSize)/log(2));
        tagBit2 = (48 - blockBits2 - setBits2);
        sideCache = malloc(sizeof(struct cache) * setSize2);
        for(long i = 0; i < setSize2; i ++){
            sideCache[i].tag = calloc(asso2,sizeof(long*));
            for(long j = 0; j < asso2; j++){
                sideCache[i].tag[j] = calloc(tagBit2,sizeof(long*));
            }
            sideCache[i].valid = calloc(asso2,sizeof(long*));
            sideCache[i].used = calloc(asso2,sizeof(long*));
            sideCache[i].num = calloc(asso2,sizeof(unsigned long*));
            sideCache[i].counter = 0;
        }
    }else if(strcmp(associativity2,"direct") == 0){
        asso2 = 1;
        setSize2 = cacheSize2/blockSize;
        setBits2 = (log(setSize2)/log(2));
        blockBits2 = (log(blockSize)/log(2));
        tagBit2 = (48 - blockBits2 - setBits2);
        sideCache = malloc(sizeof(struct cache) * setSize2);
        for(long i = 0; i < setSize2; i ++){
            sideCache[i].tag = calloc(asso2,sizeof(long*));
            for(long j = 0; j < asso2; j++){
                sideCache[i].tag[j] = calloc(tagBit2,sizeof(long*));
            }
            sideCache[i].valid = calloc(asso2,sizeof(long*));
            sideCache[i].used = calloc(asso2,sizeof(long*));
            sideCache[i].num = calloc(asso2,sizeof(unsigned long*));
            sideCache[i].counter = 0;
        }
    }else{
        printf("error\n");
        return EXIT_SUCCESS;
    }
    FILE* traceFile = fopen(argv[8],"r");
    if(argc != 9 || cacheSize % 2 != 0 || blockSize % 2 != 0 || traceFile == 0|| cacheSize2 % 2 != 0){
        printf("error\n");
        return EXIT_SUCCESS;
    }

    

    char action = '0';
    unsigned long num = 0;
    while(fscanf(traceFile, "%c %lx\n",&action,&num) != EOF){
     
    if(strcmp(policy,"fifo") == 0){
        // L1 CACHE
	    int beta2 = (pow(2,setBits)-1);
        long* binaryRep = getBinaryRep(num,48);
        long* tag = calloc(tagBit,sizeof(long*));
        for(int i = 0; i < tagBit; i++){
             tag[i] = binaryRep[i] ; 
        }
        long * setIndex = getBinaryRep2( (getValue(binaryRep,48) >>blockBits) & beta2,setBits);
        long setTotal = getValue(setIndex,setBits);
       
        //L2 Cache
	    int l2beta2 = (pow(2,setBits2)-1);
        long* l2tag = calloc(tagBit2,sizeof(long*));
        for(int i = 0; i < tagBit2; i++){
            l2tag[i] = binaryRep[i] ; 
        }
        long * l2setIndex = getBinaryRep2( (getValue(binaryRep,48) >>blockBits) & l2beta2,setBits2);
        long l2setTotal = getValue(l2setIndex,setBits2);
        

        long outter = findTag(mainCache,tag,asso,tagBit,setTotal,setSize);
        long inner = findTag(sideCache,l2tag,asso2,tagBit2,l2setTotal,setSize2);
        if((mainCache[setTotal].valid[outter] == 1) && compData(mainCache[setTotal].tag[outter],tag,tagBit) == 0){
                if(action == 'R'){
                    cacheHit++;   
                }
                if(action == 'W'){
                    write++;
                    cacheHit++;
                } 
                
        }else if((sideCache[l2setTotal].valid[inner] == 1) && compData(sideCache[l2setTotal].tag[inner],l2tag,tagBit2) == 0){
                if(action == 'R'){
                    l2cacheHit++;
                    cacheMiss++;
                    
                }
                if(action == 'W'){
                    write++;
                    l2cacheHit++;
                    cacheMiss++;
                }
                    sideCache[l2setTotal].valid[inner] = 0;
                    //Create L2 setup from suppsoed evicted block
                    
                        long testbeta2 = (pow(2,setBits2)-1);
                        long* testbinaryRep = getBinaryRep(mainCache[setTotal].num[outter],48);
                        long* testtag = calloc(tagBit2,sizeof(long*));
                        for(int hi = 0; hi < tagBit2; hi++){
                            testtag[hi] = testbinaryRep[hi] ; 
                        }
                        long * testsetIndex = getBinaryRep2( (getValue(testbinaryRep,48) >>blockBits) & testbeta2,setBits2);
                        long testsetTotal = getValue(testsetIndex,setBits2);
                        long replace = findTag(sideCache,testtag,asso2,tagBit2,testsetTotal,setSize2);
                        sideCache[testsetTotal].valid[replace] = 1;
                        for(int k = 0; k < tagBit2; k++){
                            sideCache[testsetTotal].tag[replace][k] = testtag[k] ; 
                        }
                        sideCache[testsetTotal].used[replace] =  sideCache[testsetTotal].counter++;
                        sideCache[testsetTotal].num[replace] = mainCache[setTotal].num[outter];
                        free(testbinaryRep);
                        free(testtag);
                        free(testsetIndex);
                    
                    
                
                //Input num into L1 as normal
                mainCache[setTotal].valid[outter] = 1;
                for(int k = 0; k < tagBit; k++){
                    mainCache[setTotal].tag[outter][k] = tag[k]; 
                }
                mainCache[setTotal].used[outter] = mainCache[setTotal].counter++;
                mainCache[setTotal].num[outter] = num;
                            
            
        }else{
                if(action == 'R'){
                    cacheMiss++;
                    l2cacheMiss++;
                    read++;
                }
                if(action == 'W'){
                    write++;
                    read++;
                    cacheMiss++;
                    l2cacheMiss++;
                }
                //If the block at L1 valid bit is 1, then it needs to get put into L2 cache(The L1 cache is full).
                if(mainCache[setTotal].valid[outter] == 1){
                    long testbeta2 = (pow(2,setBits2)-1);
                    long* testbinaryRep = getBinaryRep(mainCache[setTotal].num[outter],48);
                    long* testtag = calloc(tagBit2,sizeof(long*));
                    for(int hi = 0; hi < tagBit2; hi++){
                        testtag[hi] = testbinaryRep[hi] ; 
                    }
                    long * testsetIndex = getBinaryRep2( (getValue(testbinaryRep,48) >>blockBits) & testbeta2,setBits2);
                    long testsetTotal = getValue(testsetIndex,setBits2);
                    long replace = findTag(sideCache,testtag,asso2,tagBit2,testsetTotal,setSize2);
                    sideCache[testsetTotal].valid[replace] = 1;
                    for(int k = 0; k < tagBit2; k++){
                        sideCache[testsetTotal].tag[replace][k] = testtag[k] ; 
                    }
                    sideCache[testsetTotal].used[replace] =  sideCache[testsetTotal].counter++;
                    sideCache[testsetTotal].num[replace] = mainCache[setTotal].num[outter];
                    free(testbinaryRep);
                    free(testtag);
                    free(testsetIndex);
                    
                }
                
                //Input num into L1 as normal
                mainCache[setTotal].valid[outter] = 1;
                for(int k = 0; k < tagBit; k++){
                    mainCache[setTotal].tag[outter][k] = tag[k]; 
                }
                mainCache[setTotal].used[outter] = mainCache[setTotal].counter++;
                mainCache[setTotal].num[outter] = num;
        }
        
        free(binaryRep);
        free(tag);
        free(setIndex);
        free(l2tag);
        free(l2setIndex);
    }else if(strcmp(policy,"lru") == 0){
        // L1 CACHE
	    // L1 CACHE
	    int beta2 = (pow(2,setBits)-1);
        long* binaryRep = getBinaryRep(num,48);
        long* tag = calloc(tagBit,sizeof(long*));
        for(int i = 0; i < tagBit; i++){
             tag[i] = binaryRep[i] ; 
        }
        long * setIndex = getBinaryRep2( (getValue(binaryRep,48) >>blockBits) & beta2,setBits);
        long setTotal = getValue(setIndex,setBits);
       
        //L2 Cache
	    int l2beta2 = (pow(2,setBits2)-1);
        long* l2tag = calloc(tagBit2,sizeof(long*));
        for(int i = 0; i < tagBit2; i++){
            l2tag[i] = binaryRep[i] ; 
        }
        long * l2setIndex = getBinaryRep2( (getValue(binaryRep,48) >>blockBits) & l2beta2,setBits2);
        long l2setTotal = getValue(l2setIndex,setBits2);
        

        long outter = findTag(mainCache,tag,asso,tagBit,setTotal,setSize);
        long inner = findTag(sideCache,l2tag,asso2,tagBit2,l2setTotal,setSize2);
        if((mainCache[setTotal].valid[outter] == 1) && compData(mainCache[setTotal].tag[outter],tag,tagBit) == 0){
                if(action == 'R'){
                    cacheHit++;   
                }
                if(action == 'W'){
                    write++;
                    cacheHit++;
                } 
                mainCache[setTotal].used[outter] = mainCache[setTotal].counter++; 
        }else if((sideCache[l2setTotal].valid[inner] == 1) && compData(sideCache[l2setTotal].tag[inner],l2tag,tagBit2) == 0){
                if(action == 'R'){
                    l2cacheHit++;
                    cacheMiss++;
                    
                }
                if(action == 'W'){
                    write++;
                    l2cacheHit++;
                    cacheMiss++;
                }
                mainCache[setTotal].used[outter] = mainCache[setTotal].counter++;
                    sideCache[l2setTotal].valid[inner] = 0;
                    //Create L2 setup from suppsoed evicted block
                    
                        long testbeta2 = (pow(2,setBits2)-1);
                        long* testbinaryRep = getBinaryRep(mainCache[setTotal].num[outter],48);
                        long* testtag = calloc(tagBit2,sizeof(long*));
                        for(int hi = 0; hi < tagBit2; hi++){
                            testtag[hi] = testbinaryRep[hi] ; 
                        }
                        long * testsetIndex = getBinaryRep2( (getValue(testbinaryRep,48) >>blockBits) & testbeta2,setBits2);
                        long testsetTotal = getValue(testsetIndex,setBits2);
                        long replace = findTag(sideCache,testtag,asso2,tagBit2,testsetTotal,setSize2);
                        sideCache[testsetTotal].valid[replace] = 1;
                        for(int k = 0; k < tagBit2; k++){
                            sideCache[testsetTotal].tag[replace][k] = testtag[k] ; 
                        }
                        sideCache[testsetTotal].used[replace] =  sideCache[testsetTotal].counter++;
                        sideCache[testsetTotal].num[replace] = mainCache[setTotal].num[outter];
                        free(testbinaryRep);
                        free(testtag);
                        free(testsetIndex);
                    
                    
                
                //Input num into L1 as normal
                mainCache[setTotal].valid[outter] = 1;
                for(int k = 0; k < tagBit; k++){
                    mainCache[setTotal].tag[outter][k] = tag[k]; 
                }
                mainCache[setTotal].used[outter] = mainCache[setTotal].counter++;
                mainCache[setTotal].num[outter] = num;
                            
            
        }else{
                if(action == 'R'){
                    cacheMiss++;
                    l2cacheMiss++;
                    read++;
                }
                if(action == 'W'){
                    write++;
                    read++;
                    cacheMiss++;
                    l2cacheMiss++;
                }
                mainCache[setTotal].used[outter] = mainCache[setTotal].counter++;
                
                //If the block at L1 valid bit is 1, then it needs to get put into L2 cache(The L1 cache is full).
                if(mainCache[setTotal].valid[outter] == 1){
                    long testbeta2 = (pow(2,setBits2)-1);
                    long* testbinaryRep = getBinaryRep(mainCache[setTotal].num[outter],48);
                    long* testtag = calloc(tagBit2,sizeof(long*));
                    for(int hi = 0; hi < tagBit2; hi++){
                        testtag[hi] = testbinaryRep[hi] ; 
                    }
                    long * testsetIndex = getBinaryRep2( (getValue(testbinaryRep,48) >>blockBits) & testbeta2,setBits2);
                    long testsetTotal = getValue(testsetIndex,setBits2);
                    long replace = findTag(sideCache,testtag,asso2,tagBit2,testsetTotal,setSize2);
                    sideCache[testsetTotal].valid[replace] = 1;
                    for(int k = 0; k < tagBit2; k++){
                        sideCache[testsetTotal].tag[replace][k] = testtag[k] ; 
                    }
                    sideCache[testsetTotal].used[replace] =  sideCache[testsetTotal].counter++;
                    sideCache[testsetTotal].num[replace] = mainCache[setTotal].num[outter];
                    free(testbinaryRep);
                    free(testtag);
                    free(testsetIndex);
                    
                }
                
                //Input num into L1 as normal
                mainCache[setTotal].valid[outter] = 1;
                for(int k = 0; k < tagBit; k++){
                    mainCache[setTotal].tag[outter][k] = tag[k]; 
                }
                mainCache[setTotal].used[outter] = mainCache[setTotal].counter++;
                mainCache[setTotal].num[outter] = num;
        }
        
        free(binaryRep);
        free(tag);
        free(setIndex);
        free(l2tag);
        free(l2setIndex);
       
    }else{
        printf("error\n");
        return EXIT_SUCCESS;
    }
    //printf("NUMBER: %ld\n",num);
    /*
    printf("L1 CACHE:\n");
    printCache(mainCache,asso,tagBit,setSize);
    printf("L2 CACHE:\n");
    printCache(sideCache,asso2,tagBit2,setSize2);
    */
    }

        printf("memread:%ld\n",read);
        printf("memwrite:%ld\n",write);
        printf("l1cachehit:%ld\n",cacheHit);
        printf("l1cachemiss:%ld\n",cacheMiss);
        printf("l2cachehit:%ld\n",l2cacheHit);
        printf("l2cachemiss:%ld\n",l2cacheMiss);
    
        freeList(mainCache,setSize,tagBit,asso);
        freeList(sideCache,setSize2,tagBit2,asso2);

    fclose(traceFile);
    return EXIT_SUCCESS;
}
