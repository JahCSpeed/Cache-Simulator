#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include<math.h>
struct cache{
    long *valid;
    long **tag;
    long *used;
};
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
void freeList(struct cache* head, long size, int tagBits, int asso){
        for(long i = 0; i < size; i ++){
            for(long j = 0; j < asso; j++){
                free(head[i].tag[j]);
            }
            free(head[i].tag);
            
            free(head[i].valid);
            free(head[i].used);
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
int main(int argc, char** argv){
    long cacheMiss = 0;
    long cacheHit = 0;
    long read = 0;
    long write = 0;
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
    FILE* traceFile = fopen(argv[5],"r");
    if(argc != 6 || cacheSize % 2 != 0 || blockSize % 2 != 0 || traceFile == 0){
        printf("error\n");
        return EXIT_SUCCESS;
    }

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
        }
    }else{
        printf("error");
        return EXIT_FAILURE;
    }

    char action = '0';
    unsigned long num = 0;
    int counter = 1;
    int counter2 = 1;
    while(fscanf(traceFile, "%c %lx\n",&action,&num) != EOF){
    if(strcmp(policy,"fifo") == 0){
	    int beta = (pow(2,blockBits)-1);
	    int beta2 = (pow(2,setBits)-1);
        long* binaryRep = getBinaryRep(num,48);
        long* tag = calloc(tagBit,sizeof(long*));
        for(int i = 0; i < tagBit; i++){
             tag[i] = binaryRep[i] ; 
        }
        long * setIndex = getBinaryRep2( (getValue(binaryRep,48) >>blockBits) & beta2,setBits);
        long* blockOffset = getBinaryRep2((getValue(binaryRep,48) & beta) ,blockBits);
        long setTotal = getValue(setIndex,setBits);
        int i = findTag(mainCache,tag,asso,tagBit,setTotal,setSize);
        if((mainCache[setTotal].valid[i] == 1) && compData(mainCache[setTotal].tag[i],tag,tagBit) == 0){
            if(action == 'R'){
                cacheHit++;
                
            }
            if(action == 'W'){
                write++;
                cacheHit++;
            }
        }else{
            if(action == 'R'){
                cacheMiss++;
                read++;
            }
            if(action == 'W'){
                write++;
                read++;
                cacheMiss++;
            }
            mainCache[setTotal].valid[i] = 1;
            for(int k = 0; k < tagBit; k++){
                mainCache[setTotal].tag[i][k] = tag[k] ; 
            }
            mainCache[setTotal].used[i] = counter;
        }
        
        free(binaryRep);
        free(tag);
        free(setIndex);
        free(blockOffset);
        counter++;
    }else if(strcmp(policy,"lru") == 0){
	    int beta = (pow(2,blockBits)-1);
	    int beta2 = (pow(2,setBits)-1);
        long* binaryRep = getBinaryRep(num,48);
        long* tag = calloc(tagBit,sizeof(long*));
        for(int i = 0; i < tagBit; i++){
             tag[i] = binaryRep[i] ; 
        }
        long * setIndex = getBinaryRep2( (getValue(binaryRep,48) >>blockBits) & beta2,setBits);
        long* blockOffset = getBinaryRep2((getValue(binaryRep,48) & beta) ,blockBits);
        long setTotal = getValue(setIndex,setBits);
        int i = findTag(mainCache,tag,asso,tagBit,setTotal,setSize);
        if((mainCache[setTotal].valid[i] == 1) && compData(mainCache[setTotal].tag[i],tag,tagBit) == 0){
            if(action == 'R'){
                cacheHit++;
            }
            if(action == 'W'){
                write++;
                cacheHit++;
                
            }
            mainCache[setTotal].used[i] = counter2;
            
        }else{
            if(action == 'R'){
                cacheMiss++;
                read++;
            }
            if(action == 'W'){
                write++;
                read++;
                cacheMiss++;
            }
            mainCache[setTotal].valid[i] = 1;
            for(int k = 0; k < tagBit; k++){
                mainCache[setTotal].tag[i][k] = tag[k] ; 
            }
            mainCache[setTotal].used[i] = counter2;
        }
       
        free(binaryRep);
        free(tag);
        free(setIndex);
        free(blockOffset);
        counter2++;
    }else{
        printf("error\n");
        return EXIT_SUCCESS;
    }
    
    }
    
        printf("memread:%ld\n",read);
        printf("memwrite:%ld\n",write);
        printf("cachehit:%ld\n",cacheHit);
        printf("cachemiss:%ld\n",cacheMiss);
    
        freeList(mainCache,setSize,tagBit,asso);

    fclose(traceFile);
    return EXIT_SUCCESS;
}
