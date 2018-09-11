/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include <math.h>
#include <exception>
#include <string>

using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
	basic_block_size = _basic_block_size;
	total_memory_length = _total_memory_length ;	//round to power of 2 if - (_total_memory_length%basic_block_size)


  int steps = int(log2(_total_memory_length/_basic_block_size));
  
  int block_size = _total_memory_length;

  for(int i =0;i<=steps;i++){
    freeList.push_back(LinkedList());
    // freeListMap.insert(make_pair(block_size, i));
    freeListMap[block_size] = i;
 
    block_size=block_size/2;
    }
  // insert ();  
  initial = new char[_total_memory_length];
  BlockHeader* h = (BlockHeader*) initial;
  h->size = _total_memory_length;
  freeList[0].insert(h);
  

 
}

//freeListMap[128] return int index
BuddyAllocator::~BuddyAllocator (){
	delete(initial);
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  // return new char [_length];
  //cout <<"--------------------" << endl<<"Request length: " <<_length <<endl;
  int total_request = _length +sizeof(BlockHeader);
  float factor = log(total_request)/log(2);
  int pow_2_ceil = pow(2, ceil(factor));
  if(pow_2_ceil<basic_block_size)pow_2_ceil=basic_block_size;
  int index = int(log2(total_memory_length/pow_2_ceil)+1);
  BlockHeader* list_head = freeList[index].getHead();
  int moves=0;
  char* freeBlock;
  try
  {
    if (list_head != NULL ){
		freeList[index].remove(list_head);
    //cout <<"--------------------" << endl<<list_head<<endl<<"--------------------"<<endl;
		moves = 0;
    return ((char*)list_head) + sizeof(BlockHeader);
		}
    for (int i = index-1; i>=0;i--){ 
      if (freeList[i].getHead() != NULL ){
        freeBlock = (char*)freeList[i].getHead();
          for(int i = 0;i<moves;i++){
            
            BlockHeader* newBlock=(BlockHeader*)split(freeBlock);
            if(newBlock->size==pow_2_ceil){
              freeList[index-1].remove(newBlock);
              cout<<"--------------------" <<endl<< newBlock<<endl<<"--------------------";
              return ((char*)newBlock) + sizeof(BlockHeader);
              moves=0;
            }
          }		
        }
        moves++;   
      
    }throw "No available memory. ";
  }
  catch (string e)
  {
    cout << "Please request another memory size " << e << '\n';
  }
  return NULL;
}


char* BuddyAllocator::getbuddy (char * addr){
    BlockHeader* x = (BlockHeader*) addr;
    return (((char*)x-initial)^x->size)+initial;
}

bool BuddyAllocator::isvalid (char * addr){
    BlockHeader* x = (BlockHeader*) addr;  
    return (x->Free==true);

}

char* BuddyAllocator::split (char* block){
  BlockHeader* x = (BlockHeader*) block;
  int index = freeListMap[x->size];
  freeList[index].remove((BlockHeader*)block);
  x->size/=2;
  x->next= (BlockHeader*)(block + x->size);
  x->Free=true;

  x->next->size = x->size;
  x->next->Free = true;
  x->next->next = NULL;
 
  freeList[index+1].insert(x->next);
  freeList[index+1].insert(x);

  return (char*) x;
}


int BuddyAllocator::free (char * _a){
    //cout << "FREE CALL: " <<_a<<endl;
    int success=0;
    BlockHeader* x = (BlockHeader*) (_a-sizeof(BlockHeader));//-sizeof(BlockHeader)
    char* buddy = getbuddy((char*)x);
    cout << "(Buddy): " << (unsigned long)buddy << " (x): " << (unsigned long)x << endl;
    
    
    while(isvalid(buddy))
    {
      x = (BlockHeader*)merge((char*)x,buddy); 
      buddy = getbuddy((char*)x);
      success = 1;
    }
    return success;
}


char* BuddyAllocator::merge (char* block1, char* block2){
  cout <<"\n\n MERGE!!!\n\n";
  BlockHeader* i1 = (BlockHeader*)block1;
  BlockHeader* i2 = (BlockHeader*)block2;
  if(block2<block1){
    BlockHeader* temp = i1;
    i1 = i2;
    i2 = temp;
  }
    i2=NULL;
    i1->next=NULL;
    i1->size *= 2;
    i1->Free = true;
    int index = freeListMap[i1->size];
    freeList[index+1].remove(i1);
    freeList[index].insert(i1);
    //freeList[index+1].remove(i1);
  return (char*)i1;
}


void BuddyAllocator::debug() {

  for (int i =0; i<freeList.size();i++){
	  BlockHeader *head = freeList[i].getHead();
    cout << "head " << i << "=" << (unsigned long)head << endl;
  } 
    
    
    
    //!=NULL){
		 // cout<< "\n \n split() Stuff inside freeList: \n \n" << freeList[i].getHead()->size << " (size) ... "<<freeList[i].getHead()->Free << " (Free) ... "<<freeList[i].getHead()->next << " (Next) \n";
		  //cout <<"\n \n Stuff inside freeList: "<<freeList[index+1].getHead()->next->size;
	
	  //}
    //}


}
  // BlockHeader* head=FreeList->getHead();
  // FreeList->setHead(alloc(_total_memory_length));

  // FreeList= new LinkedList[_total_memory_length];

  // LinkedList newList[steps];

// BlockHeader* LinkedList::traverse(){
// 	int total_block = 0;
// 	BlockHeader* curr = head;
// 	BlockHeader* prev =NULL;
// 	cout<<"\n \n";
// 	while(curr!=NULL){
// 		cout<<"[ Block size: " <<curr->size <<"]  ->  ";
// 		prev = curr;
// 		curr=curr->next;
// 		total_block++;
// 	}
// 	cout<<"\n \n total_block: "<<total_block;
// 	return prev;
// }
/* for (int i =0; i<=index+1;i++){
	  if(freeList[i].getHead()!=NULL){
		  cout<< "\n \n split() Stuff inside freeList: \n \n" << freeList[i].getHead()->size << " (size) ... "<<freeList[i].getHead()->Free << " (Free) ... "<<freeList[i].getHead()->next << " (Next) \n";
		  //cout <<"\n \n Stuff inside freeList: "<<freeList[index+1].getHead()->next->size;
	
	  }

  } */

    /* cout << initial << " (initial) ... " << freeList[0]->getHead() << " (h) ";
  freeList[0].insert(h);
  int ind = 0;
  while( freeList[ind].getHead()!=NULL){
    cout<< "\n \n Constructor: \n \n" << freeList[ind].getHead()->size << " (size) ... "<<freeList[ind].getHead()->Free << " (Free) ... "<<freeList[ind].getHead()->next << " (Next) \n";
    ind++;
  } */
  // avail = (BlockHeader*)(initial)+sizeof(head);
  // FreeList->block = new char [total_memory_length/basic_block_size];
  