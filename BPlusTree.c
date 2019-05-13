/******This is a B_tree implementaion in C language :
Author : Saurav Rai
****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define N 50 
#define KEYLEN 25 
#define NODE_SIZE 4096
#define BUF_SIZE 1024



//1.DEFINITION OF MY BPLUSTREE NODE:
typedef struct BPlusTreeNode {
    char* K[N-1]; // N-1 strings(keys) of length 25
    void* P[N];   // pointers to N child nodes
    char Id; // leaf node 'l' or internal node 'i'
    int keys_count; // count of number of keys present
    struct BPlusTreeNode* parent;     // pointer to the parent node (for traversal purpose)
}Node;

/* IN MY B PLUS TREE IMPLEMENTAION N IS THE ORDER OF THE TREE
   KEYS OF LENGTH 25 EACH 8 BYTES SO
   THE SIZE OF ARRAY IS 99 * (25 + 8)  , WITH 99 KEYS
   THE POINTER ARRAY SIZE 100 * 8 
   SO THE TOTAL NODE SIZE = 3267 + 8000 + 1 (FOR Id) + 4 (FOR KEYS COUNT ) + 8 (FOR POINTER) = 11280
    
*/  


//2.DEFINTION OF Global variable for the root of the tree
Node* TREE_ROOT;


//3.DEFINITION structure that encapsulates a key and its corresponding index in file
typedef struct insertElement {
    char key[KEYLEN];
    long offset;  //index of the key in the file
} InsertEl;



//4.PROTOTYPE OF VARIOUS DEFINTIONS
Node* createNewNode(char);
void printKey(char*);
void printTree (Node *,int); 
int binarysearchInd(char*, char**, int);  
Node* findLeafNode (Node*, char* );
void insertIntoParent (Node*,InsertEl* ); 
void insert (InsertEl*);
InsertEl* getNextElement (FILE*);
long query (char*);
void deleteTree(Node*);


//5.PROCEDURE TO INSERT A KEY ONTO A NODE
//used by user invoked insert() function
void insertIntoArrays(char** keyarr, void** ptrarr, int nk, InsertEl* el) {
    //special case for the very first insert into the root
    if (nk == 0) {
        strncpy(keyarr[0],el->key,KEYLEN);
        ptrarr[0]= (void*)el->offset;
        return;
    }

    int i= 0;
    int ind= -1; //last element by default
    // linear search on the keys for position to insert the new key
    for (i= 0; i < nk; i++) {
        if (strncmp(keyarr[i],el->key,KEYLEN) > 0) {
            ind= i;
            break;
        }
    }
    //if the position to insert is blank space in the end
    if (ind == -1) {
        strncpy(keyarr[nk],el->key,KEYLEN);
        ptrarr[nk]= (void* )el->offset;
        return;
    }
    
    char bufkey1[KEYLEN], bufkey2[KEYLEN];
    void* bufptr1, *bufptr2;
    //do the shifting of keys to create space
    strncpy(bufkey1,keyarr[ind],KEYLEN);//bufkey1= keyarr[i+1];
    bufptr1= ptrarr[ind];
    for (i= ind+1; i <= nk; i++) {
        strncpy(bufkey2,keyarr[i],KEYLEN);//bufkey2= leaf->K[i];
        strncpy(keyarr[i],bufkey1,KEYLEN);//bufkey[i]= bufkey1;
        strncpy(bufkey1,bufkey2,KEYLEN);  //bufkey1= bufkey2
        bufptr2= ptrarr[i];
        ptrarr[i]= bufptr1; 
        bufptr1= bufptr2;
    }

    //insert the given element in its place
    strncpy(keyarr[ind],el->key,KEYLEN);//leaf->K[i]= bufptr;
    ptrarr[ind]= (void *)el->offset;
}

//6.MAIN METHOD OF MY BPLUS TREE:
int main (int argc, char** argv) 
 {
    printf("B+ TREE IMPLEMENTATION\n");
    //Open the input file
    FILE* input= fopen(argv[1],"rw");
     if(argc < 2)
     {
       printf("USAGE: <./a.out> <filename>\n");
       return 1;
     }
    
    if(input == NULL)
    {
      printf("FILE %s does not exists\n",argv[1]);
      return 1;
    } 
    
    
    
    
    // THE MAIN ROOT NODE HAS TO BE CREATED EXPLICITLY 
    // AS THE INSERT FUNCTION EXPECTS IT TO BE NOT NULL
    TREE_ROOT= createNewNode('l');
    
    
    
    //1.THE INSERT PROCEDURE
    while(1)
        {
           InsertEl* newEl= getNextElement(input);

           if (newEl == NULL)
             break;

           printf("The key to be inserted is: ");
           printKey(newEl->key);
           printf(" and file index is: %ld\n", newEl->offset);

           insert(newEl);
        }
   
      printf("Tree structure after insert is:\n");
      printTree(TREE_ROOT, 0);
      printf("--------------------------------------------\n");

    printf("The TREE_ROOT NODE is\n");
    
    int a= 0, b= 0;
    for (a= 0; a < TREE_ROOT->keys_count; a++)
         {
            printKey(TREE_ROOT->K[a]);
         } 



     int i;
      
    //2.THE SEARCH PROCEDURE INTO THE B + TREE
     char findkey[KEYLEN+1];
    while (1)
      {
          //user input for the key to search
        
        printf("\nPlease insert the key you want to search for (type \"quit\" to delete an element): ");
        scanf("%s",findkey);
        if (strcmp(findkey,"quit") == 0)
            break;
        //format the given key by padding spaces wherever required
        int startpadding= 0;
        for (i= 0; i < KEYLEN; i++)
         {
            if (findkey[i] == '\0')
                startpadding= 1;
            if (startpadding)
                findkey[i]= ' ';
         }
         findkey[i]= '\0';

        //Search and print the result
        int offset= query((char*)findkey);
        if (offset == -1)
            printf("Sorry, the key you are looking for is not present in the tree.\n");
        else
            printf("The record for the given key is in the file at offset %d.\n", offset);
     }

    //3.DELETE IN THE LEAF NODE
    int startpadding= 0;
        for (i= 0; i < KEYLEN; i++)
         {
            if (findkey[i] == '\0')
                startpadding= 1;
            if (startpadding)
                findkey[i]= ' ';
         }
         findkey[i]= '\0';
      
      char choice ='y';
      char option;
      
      printf("Enter 'y' to delete an element\n");
      scanf("%c",&option);
     while(option)
       { 
        printf("Enter the key which you want to delete\n");
        scanf("%s",findkey);
   
    
        //Search and print the result
        int offset= query((char*)findkey);
        printf("The key to be deleted is at offset %d :",offset);
        Node* target = findLeafNode(TREE_ROOT,findkey);

        //free(findkey->offset);

       //printf("The key is at the node\n",target);
       
       for (i= 0; i < target->keys_count; i++) 
        {
          if(strcmp(target->K[i],findkey)==0)
            {
              free(target->K[i]);
              //free(findkey->offset);
             
           } 
          else
	     	continue;
		 } 
	   }
	   
	 
      
     	  	        
        printf("Tree structure after deletion is:\n");
        printTree(TREE_ROOT, 0);
        printf("--------------------------------------------\n");

     
        //findLeafNode(TREE_ROOT,root,findkey) 

    //4.DELETE THE WHOLE TREE
    printf("The full tree is now deleted\n");
    deleteTree(TREE_ROOT);
}




//7.PROCEDURE TO CREATE AND INITIALIZE A NEW B TREE NODE
//Argument is the identifier of node (internal 'i' or leaf 'l')
Node* createNewNode(char id) {
    // allocate memory for exactly the node size
    Node* newNode= (Node* ) malloc(sizeof(Node));
    int i, j;
    for (i=0; i < N; i++) {
        //allocate space for the keys
        newNode->K[i]= (char*) malloc(KEYLEN * sizeof(char));
        //Initialize all keys as emprty strings
        newNode->K[i][0]= '\0';
        //Initialize all pointers as NULL
        newNode->P[i]= NULL;
    }
    //set the id of the node to given value
    newNode->Id= id;
    //set the number of keys present in the node to zero
    newNode->keys_count= 0;
    //set the parent to null
    newNode->parent= NULL;
    
    return newNode;
}


//8.FUNCTION TO PRINT A KEY
void printKey(char* key) {
    int i= 0;
    for (;i < KEYLEN; i++)
        printf("%c",key[i]);
}


//9.PROCEDURE TO PRINT THE CURRENT TREE STRUCTURE
void printTree (Node *root, int level) {
    int i= 0, j= 0;
    for (i= 0; i < root->keys_count; i++) {
        if (root->Id == 'l') {
            for (j= 0; j < level; j++)
                printf("\t");
            printKey(root->K[i]);
            printf(":%ld\n",(long)root->P[i]);
        } else if (root->Id == 'i') {
            printTree(root->P[i],level+1);
            for (j= 0; j < level; j++)
                printf("\t");
            printKey(root->K[i]);
            printf("\n");
        }
    }
    if (root->Id == 'i') {
        printTree(root->P[i],level+1);
    }
}





//10.PROCEDURE TO FIND THE INDEX OF THE POINTER TO NEXT NODE
//Input: the key to be searched for
int binarysearchInd(char* key, char** k_array, int k_count) 
  {
     //special conditions
     if (k_count == 0) return -1;
      if (k_count == 1) 
      {
        if (strncmp(key,k_array[0],KEYLEN) < 0)
            return 0;
        else return 1;
      }

    //recursive call
    int mid= k_count/2;
    if (strncmp(key,k_array[mid],KEYLEN) < 0) {
        return binarysearchInd(key,k_array,mid);
    } else {
        return mid + binarysearchInd(key,&k_array[mid],k_count-mid);
    }
    
}



//11.PROCEDURE TO FIND A LEAF NODE TO INSERT THE KEY VALUE. 
Node* findLeafNode (Node* root, char* key)
  {
     if(root->Id == 'l')
      { //if leaf node
        return root;
      } 
     else 
     {
        int ind= binarysearchInd(key, (char **)root->K, root->keys_count);
        return findLeafNode(root->P[ind],key);
    }
}

//12.PROCEDURE TO INSERT A VALUE IN THE TREE
//Args: root - root of the Tree
//      newVal -  new key value
//      ptr -   pointer related to the key
void insert (InsertEl* el) {
    //Do nothing if the root is NULL.
    //Root is expected to be pre-allocated but can have all values empty.
    if (TREE_ROOT == NULL) 
          return;
    //find the leaf node in which new value need to be inserted
    Node* target= findLeafNode(TREE_ROOT,el->key);
    
    //if the leaf node has overflow
    if (target->keys_count < N-1) { //only N-1 keys are legal
        //insert the new value appropriately in the leaf node
        insertIntoArrays((char**)target->K, target->P, target->keys_count, el);
        target->keys_count++;
    } else {
        
        //SPLIT LOGIC FOR LEAF NODE
        Node* newNode= createNewNode('l');
   
   		newNode->P[N-1]= target->P[N-1];
        target->P[N-1]= newNode;

        //declare bigger buffers
        char* keyArr[N];
        void* ptrArr[N+1];
        int i= 0;
        for (i= 0; i < N; i++)
            keyArr[i]= (char* ) malloc(KEYLEN * sizeof (char));
        
        //copy into bigger buffer
        for (i= 0; i < N-1; i++) {
            strncpy(keyArr[i],target->K[i],KEYLEN);
            ptrArr[i]= target->P[i];
        }
        ptrArr[i]= target->P[i];

        //perform the insert
        insertIntoArrays((char**)keyArr, ptrArr, N-1, el);

        //erase contents of target array
        for (i= 0; i < N-1; i++) {
            strncpy(target->K[i],"",KEYLEN);
            target->P[i]= NULL;
        }
        target->P[i]= NULL;

        //copy first half into target node
        int midInd= ((N-1)/2) + 1; //key at midInd becomes 0th key at new node
        for (i= 0; i < midInd; i++) {
            strncpy (target->K[i],keyArr[i],KEYLEN);
            target->P[i]= ptrArr[i]; 
        }
        target->keys_count= midInd;
        // copy other half into new node
        for (; i < N; i++) {
            strncpy (newNode->K[i-midInd],keyArr[i],KEYLEN);
            newNode->P[i-midInd]= ptrArr[i];
        }
        newNode->keys_count= N-midInd;

        //complete the tree structure by
        //inserting the pointer to new node recursively into parent nodes
        strncpy(el->key,newNode->K[0],KEYLEN);
        el->offset= (long)newNode;
        insertIntoParent(target,el);
    }
}

//13.HELPER FUNCTION INVOKED BY THE INSERT FUNCTION
// TO MAINTAIN THE STRUCTURE OF THE TREE
// by inserting the pointer to the new node created by split recursively in the internal nodes
void insertIntoParent (Node* node, InsertEl* el) 
 {
    
    if (node == TREE_ROOT) { //if this parent node is the root of the tree
        //create root node
        Node* treeRoot= createNewNode('i');
        //update values in the node
        strncpy(treeRoot->K[0],el->key,KEYLEN);
        treeRoot->P[0]= node;
        treeRoot->P[1]= (void*)el->offset;
        treeRoot->keys_count= 1;
        //set parent relationships
        ((Node*)treeRoot->P[0])->parent= treeRoot;
        ((Node*)treeRoot->P[1])->parent= treeRoot;
        //update the tree node
        TREE_ROOT= treeRoot;
        return;
    }
    
    
    Node* parent= node->parent;
    if (parent->keys_count < N-1)
      { //If space exists for the new key,pointer pair
        //insert the key,pointer in the parent node
        insertIntoArrays(parent->K,&parent->P[1],parent->keys_count,el);
        ((Node*)el->offset)->parent= parent;
        parent->keys_count++;
       } 
    else 
       {
        //SPLIT LOGIC FOR INTERNAL NODE
        //create a new node
        Node* newNode= createNewNode('i');

        //declare bigger buffers
        char* keyArr[N];
        void* ptrArr[N+1];
        int i= 0;
        for (i= 0; i < N; i++)
            keyArr[i]= (char* ) malloc(KEYLEN * sizeof (char));

        //copy into bigger buffer
        for (i= 0; i < N-1; i++) {
            strncpy(keyArr[i],parent->K[i],KEYLEN);
            ptrArr[i]= parent->P[i];
        }
        ptrArr[i]= parent->P[i];

        //perform the insert
        insertIntoArrays((char**)keyArr, &ptrArr[1], N-1, el);

        //erase contents of parent node
        for (i= 0; i < N-1; i++) {
            strncpy(parent->K[i],"",KEYLEN);
            parent->P[i]= NULL;
        }
        parent->P[i]= NULL;

        //copy first half into target node
        int midInd= ((N-1)/2) + 1; //key at midInd becomes 0th key at new node
        for (i= 0; i < midInd; i++) {
            strncpy (parent->K[i],keyArr[i],KEYLEN);
            parent->P[i]= ptrArr[i];
            ((Node*)parent->P[i])->parent= parent;
        }
        //copy one pointer more
        parent->P[i]= ptrArr[i];
        ((Node*)parent->P[i])->parent= parent;
        parent->keys_count= midInd;
        
        //The key in mid index should be inserted in parent, copy in the insertEl
        strncpy(el->key,keyArr[i],KEYLEN);
        el->offset= (long)newNode;
        //increment index to start copying to new node
        i++;
        // copy other half into new node
        for (; i < N; i++) {
            strncpy (newNode->K[i-midInd],keyArr[i],KEYLEN);
            newNode->P[i-midInd]= ptrArr[i];
            ((Node*)newNode->P[i-midInd])->parent= newNode;
        }
        //copy the last pointer
        newNode->P[i-midInd]= ptrArr[i];
        ((Node*)newNode->P[i-midInd])->parent= newNode;
        newNode->keys_count= N-midInd;

        //complete the tree structure by
        //inserting the pointer to new node recursively into parent nodes
        insertIntoParent(parent,el);
    }
}


//14.PROCEDURE TO GET NEXT LINE OF STRINGS FROM THE INPUT FILE IN 25 CHARS
InsertEl* getNextElement (FILE* input) {
    char buffer[BUF_SIZE];

    int offset= ftell(input);
    int i= 0;
    if(fgets(buffer, BUF_SIZE, input)) {
        InsertEl* el= (InsertEl*) malloc(sizeof(InsertEl));
        for (i= 0; i < KEYLEN; i++) {
            if (buffer[i] != '\0' && buffer[i] != '\n')
                el->key[i]= buffer[i];
            else break;
        }
        for (; i < KEYLEN; i++) {
            el->key[i]= ' ';
        }
        el->offset= offset;
        return el;
    } else {
        return NULL;
    }
}



//15.PROCEDURE TO DELETE THE FULL BPLUS TREE
void deleteTree(Node* root) {
    int i= 0;
    //if not internal node, delete all the chilren recursively
    if (root->Id == 'i') {
        for (i= 0; i <= root->keys_count; i++) {
            deleteTree(root->P[i]);
        }
    }
    //delete all the keys
    for (i= 0; i < root->keys_count; i++) {
        free(root->K[i]);
    }
    //delete the root node
    free(root);
}



//16.PROCEDURE THAT SEARCHES TO FIND THJE INDEX IN TH TREE
//corresponding to the record with given key
//return: long integer representing the offset of the beginning of the record in file
//        -1 if the key is not found in the tree 
long query (char* key) {
    Node* leafNode= findLeafNode(TREE_ROOT,key);
    int i= 0;
    for (i= 0; i < leafNode->keys_count; i++) {
        if (strncmp(leafNode->K[i],key,KEYLEN) == 0)
            return (long)leafNode->P[i];
    }
    return -1;
}


