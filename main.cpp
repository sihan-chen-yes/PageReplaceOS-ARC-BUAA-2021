//#include "pageReplace.h"
#define MAX_PHY_PAGE 64
//change
#include <time.h>
using namespace std;
#define MAX_PAGE 12
#define LRUGHOST_SIZE MAX_PHY_PAGE / 2
#define LFUGHOST_SIZE MAX_PHY_PAGE / 2
#define get_Page(x) (x>>MAX_PAGE)
#define LRU -1
#define LFU 1
#include <iostream>

int p = MAX_PHY_PAGE / 2;
//change !!
int fault = 0;
class Block {
public:
    long vpage;
    int ppage;
    Block *pre;
    Block *next;
    Block();
};
Block::Block() {
    vpage = 0;
    ppage = 0;
    pre = NULL;
    next = NULL;
}

class GBlock {
public:
    long vpage;
    GBlock *pre;
    GBlock *next;
    GBlock();
};
GBlock::GBlock() {
    vpage = 0;
    pre = NULL;
    next = NULL;
}

//class EmptyPage {
//public:
//    int num;
//    EmptyPage *next;
//    EmptyPage()
//};

Block *LRUHead = new Block;
Block *LRUTail = new Block;
Block *LFUHead = new Block;
Block *LFUTail = new Block;
GBlock *LRUGHead = new GBlock;
GBlock *LRUGTail = new GBlock;
GBlock *LFUGHead = new GBlock;
GBlock *LFUGTail = new GBlock;
//EmptyPage *EmptyHead = new

int LRUCnt;
int LFUCnt;
int LRUGCnt;
int LFUGCnt;
int EmptyPage[64];

int changeCapacity(int choice) {
    p += choice;
    if (p > MAX_PHY_PAGE || p < 0) {
        printf("pmove WAWAWAWA%d",p);
    }
    return 1;
}

int setEmptyPage(long *physic_memery,long vpage) {
    //change !!!!!!!!!
    fault++;
    for (int i = 0; i < MAX_PHY_PAGE; ++i) {
        if (EmptyPage[i] == 0) {
            physic_memery[i] = vpage;
            EmptyPage[i] = 1;
            return i;
        }
    }
    printf("alloc WA!\n");
    return -1;
}

void clearPhysic(int i,long *physic_memery) {
    EmptyPage[i] = 0;
}

void removeListTail(int choice,Block *b) {
    if (choice == LRU) {
        b->pre->next = LRUTail;
        LRUTail->pre = b->pre;
        LRUCnt--;
        return;
    } else {
        b->pre->next = LFUTail;
        LFUTail->pre = b->pre;
        LFUCnt--;
        return;
    }
}

void removeGListTail(int choice,GBlock *gb) {
    if (choice == LRU) {
        gb->pre->next = LRUGTail;
        LRUGTail->pre = gb->pre;
        delete gb;
        LRUGCnt--;
        return;
    } else {
        gb->pre->next = LFUGTail;
        LFUGTail->pre = gb->pre;
        delete gb;
        LFUGCnt--;
        return;
    }
}

void removeListIn(int choice,Block *b) {
    if (choice == LRU) {
        if (b->next == LRUTail) {
            removeListTail(LRU,b);
            return;
        } else if (b->pre == LRUHead) {
            b->next->pre = LRUHead;
            LRUHead->next = b->next;
            LRUCnt--;
            return;
        } else {
            b->pre->next = b->next;
            b->next->pre = b->pre;
            LRUCnt--;
            return;
        }
    } else {
        if (b->next == LFUTail) {
            removeListTail(LFU,b);
            return;
        } else if (b->pre == LFUHead) {
            b->next->pre = LFUHead;
            LFUHead->next = b->next;
            LFUCnt--;
            return;
        } else {
            b->pre->next = b->next;
            b->next->pre = b->pre;
            LFUCnt--;
            return;
        }
    }
}

void removeGListIn(int choice,GBlock *gb) {
    if (choice == LRU) {
        if (gb->next == LRUGTail) {
            removeGListTail(LRU,gb);
            return;
        } else if (gb->pre == LRUGHead) {
            gb->next->pre = LRUGHead;
            LRUGHead->next = gb->next;
            delete gb;
            LRUGCnt--;
            return;
        } else {
            gb->pre->next = gb->next;
            gb->next->pre = gb->pre;
            delete gb;
            LRUGCnt--;
            return;
        }
    } else {
        if (gb->next == LFUGTail) {
            removeGListTail(LFU,gb);
            return;
        } else if (gb->pre == LFUGHead) {
            gb->next->pre = LFUGHead;
            LFUGHead->next = gb->next;
            delete gb;
            LFUGCnt--;
            return;
        } else {
            gb->pre->next = gb->next;
            gb->next->pre = gb->pre;
            delete gb;
            LFUGCnt--;
            return;
        }
    }
}

void getFront(Block *b) {
    if (b->pre != LFUHead) {
        b->pre->next = b->next;
        b->next->pre = b->pre;
        LFUHead->next->pre = b;
        b->next = LFUHead->next;
        b->pre = LFUHead;
        LFUHead->next = b;
    }
    return;
}

void freeGListTail(int choice) {
    if (choice == LRU) {
        while (LRUGCnt > LRUGHOST_SIZE) {
            removeGListTail(LRU,LRUGTail->pre);
        }
    } else {
        while (LFUGCnt > LFUGHOST_SIZE) {
            removeGListTail(LFU,LFUGTail->pre);
        }
    }
}

void insertListHead(int choice,Block *b) {
    if (choice == LRU) {
        LRUHead->next->pre = b;
        b->next = LRUHead->next;
        LRUHead->next = b;
        b->pre = LRUHead;
        LRUCnt++;
    } else {
        LFUHead->next->pre = b;
        b->next = LFUHead->next;
        LFUHead->next = b;
        b->pre = LFUHead;
        LFUCnt++;
    }
    return;
}

void insertGListHead(int choice,GBlock *gb) {
    if (choice == LRU) {
        LRUGHead->next->pre = gb;
        gb->next = LRUGHead->next;
        LRUGHead->next = gb;
        gb->pre = LRUGHead;
        LRUGCnt++;
    } else {
        LFUGHead->next->pre = gb;
        gb->next = LFUGHead->next;
        LFUGHead->next = gb;
        gb->pre = LFUGHead;
        LFUGCnt++;
    }
    return;
}

void freeListTail(int choice,long *physic_memery) {
    Block *replaced;
    GBlock *replacedGhost = new GBlock();
    if (choice == LRU) {
        replaced = LRUTail->pre;
        clearPhysic(replaced->ppage,physic_memery);
        replacedGhost->vpage = replaced->vpage;
        removeListTail(LRU,replaced);
        delete replaced;
        insertGListHead(LRU,replacedGhost);
        freeGListTail(LRU);
    } else {
        replaced = LFUTail->pre;
        clearPhysic(replaced->ppage,physic_memery);
        replacedGhost->vpage = replaced->vpage;
        removeListTail(LFU,replaced);
        delete replaced;
        insertGListHead(LFU,replacedGhost);
        freeGListTail(LFU);
    }
}

void init() {
    LRUHead->next = LRUTail;
    LRUTail->pre = LRUHead;
    LFUHead->next = LFUTail;
    LFUTail->pre = LFUHead;
    LRUGHead->next = LRUGTail;
    LRUGTail->pre = LRUGHead;
    LFUGHead->next = LFUGTail;
    LFUGTail->pre = LFUGHead;
}

int WA = 0;
void pageReplace(long * physic_memery, long nwAdd) {
    static int first = 0;
    if (first == 0) {
        init();
    }
    first = 1;
    long page = get_Page(nwAdd);
    Block *bptr;
    GBlock *gbptr;
    for (bptr = LRUHead; bptr->next != LRUTail;) {
        bptr = bptr->next;
        if (bptr->vpage == page) {
            removeListIn(LRU, bptr);
            if (p == MAX_PHY_PAGE) {
                changeCapacity(-1);
                insertListHead(LFU, bptr);
                freeGListTail(LRU);
                return;
            }
            insertListHead(LFU, bptr);
            if (LFUCnt > MAX_PHY_PAGE - p) {
                freeListTail(LFU,physic_memery);
            }
            return;
        }
    }
    for (bptr = LFUHead; bptr->next != LFUTail;) {
        bptr = bptr->next;
        if (bptr->vpage == page) {
            getFront(bptr);
            return;
        }
    }
    bptr = new Block;
    bptr->vpage = page;
    for (gbptr = LRUGHead; gbptr->next != LRUGTail;) {
        gbptr = gbptr->next;
        if (gbptr->vpage == page) {
            if (p < MAX_PHY_PAGE) {
                changeCapacity(1);
                removeGListIn(LRU, gbptr);
                if (LFUCnt > MAX_PHY_PAGE - p) {
                    freeListTail(LFU,physic_memery);
                }
                bptr->ppage = setEmptyPage(physic_memery, page);
                insertListHead(LRU, bptr);
                return;
            } else {
                removeGListIn(LRU, gbptr);
                freeListTail(LRU,physic_memery);
                bptr->ppage = setEmptyPage(physic_memery, page);
                insertListHead(LRU, bptr);
                return;
            }
        }
    }
    for (gbptr = LFUGHead; gbptr->next != LFUGTail;) {
        gbptr = gbptr->next;
        if (gbptr->vpage == page) {
            if (p > 0) {
                changeCapacity(-1);
                removeGListIn(LFU, gbptr);
                if (LRUCnt > p) {
                    freeListTail(LRU,physic_memery);
                }
                bptr->ppage = setEmptyPage(physic_memery, page);
                insertListHead(LFU, bptr);
                return;
            } else {
                removeGListIn(LFU, gbptr);
                freeListTail(LFU,physic_memery);
                bptr->ppage = setEmptyPage(physic_memery, page);
                insertListHead(LFU, bptr);
                return;
            }

        }
    }
    if (p == 0) {
        changeCapacity(1);
        freeListTail(LFU,physic_memery);
        bptr->ppage = setEmptyPage(physic_memery, page);
        insertListHead(LRU, bptr);
        return;
    } else {
        if (LRUCnt < p) {
            bptr->ppage = setEmptyPage(physic_memery, page);
            insertListHead(LRU, bptr);
            return;
        } else {
            freeListTail(LRU,physic_memery);
            bptr->ppage = setEmptyPage(physic_memery, page);
            insertListHead(LRU, bptr);
            return;
        }
    }
}


long test[1700000];
int main() {
    //change !!!
    long pm[MAX_PHY_PAGE] = {0};
    int find = 0;
    long num;
    int loop;
    int i = 0;
    clock_t start,end,duration = 0;
    cin >> loop;
    while (i < loop) {
        cin >> num;
        test[i++] = num;
    }
    for (int i = 0; i < loop; ++i) {
        long page = test[i];
        long vpage = get_Page(page);
        start = clock();
        find = 0;
        pageReplace(pm,page);
//        printf("%d\n",i);
        if (WA == 1) {
            printf("%d",i);
            return 1;
        }
        end = clock();
        duration += (end - start);
        for (int j = 0; j < MAX_PHY_PAGE; ++j) {
            if (pm[j] == vpage) {
                find = 1;
                break;
            }
        }
        if (find == 0) {
            printf("%d WA \n",i + 1);
            cout<<"WAWAWAWAWAWAWAWAWAWAWAWAWAWA not in CACHE!!! END";
            return 0;
        }
//        if ()
//        printf("the %d is :\n",i + 1);
//        for (int j = 0; j < LRUGList.size(); ++j) {
//            printf("%d ",LRUGList.at(j));
//        }
//        printf("\n");
//        for (int j = 0; j < LRUList.size(); ++j) {
//            printf("%d ",LRUList.at(j).vpage);
//        }
//        printf("\n");
//        for (int j = 0; j < LFUList.size(); ++j) {
//            printf("%d ",LFUList.at(j).vpage);
//        }
//        printf("\n");
//        for (int j = 0; j < LFUGList.size(); ++j) {
//            printf("%d ",LFUGList.at(j));
//        }
//        printf("\n");
    }
    cout << "time cost: " << duration << endl;
    printf("%d : now the fault is %d\n",loop,fault);
    return 0;
}