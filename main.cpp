//#include "pageReplace.h"
#pragma GCC optimize(3)

const int MAX_PHY_PAGE (64);
//change
#include <time.h>
using namespace std;
const int MAX_PAGE (12);
const int LRUGHOST_SIZE (12);
const int LFUGHOST_SIZE (LRUGHOST_SIZE);
#define get_Page(x) (x>>MAX_PAGE)
const int LRU (-1);
const int LFU (1);
#include <iostream>

int p (MAX_PHY_PAGE / 2);

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

Block *LRUHead = new Block;
Block *LRUTail = new Block;
Block *LFUHead = new Block;
Block *LFUTail = new Block;
GBlock *LRUGHead = new GBlock;
GBlock *LRUGTail = new GBlock;
GBlock *LFUGHead = new GBlock;
GBlock *LFUGTail = new GBlock;

int LRUCnt;
int LFUCnt;
int LRUGCnt;
int LFUGCnt;
int EmptyPage[64];

inline int changeCapacity(int choice) {
    p += choice;
    return 1;
}

inline int setEmptyPage(long *physic_memery,long vpage) {
    for (register int i = 0; i < MAX_PHY_PAGE; ++i) {
        if (EmptyPage[i] == 0) {
            physic_memery[i] = vpage;
            EmptyPage[i] = 1;
            return i;
        }
    }
    return -1;
}

inline void clearPhysic(int i) {
    EmptyPage[i] = 0;
}

void removeListTail(int choice,Block *b) {
    if (choice == LRU) {
        b->pre->next = LRUTail;
        LRUTail->pre = b->pre;
        --LRUCnt;
        return;
    } else {
        b->pre->next = LFUTail;
        LFUTail->pre = b->pre;
        --LFUCnt;
        return;
    }
}

inline void removeGListTail(int choice,GBlock *gb) {
    if (choice == LRU) {
        gb->pre->next = LRUGTail;
        LRUGTail->pre = gb->pre;
        delete gb;
        --LRUGCnt;
        return;
    } else {
        gb->pre->next = LFUGTail;
        LFUGTail->pre = gb->pre;
        delete gb;
        --LFUGCnt;
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
            --LRUCnt;
            return;
        } else {
            b->pre->next = b->next;
            b->next->pre = b->pre;
            --LRUCnt;
            return;
        }
    } else {
        if (b->next == LFUTail) {
            removeListTail(LFU,b);
            return;
        } else if (b->pre == LFUHead) {
            b->next->pre = LFUHead;
            LFUHead->next = b->next;
            --LFUCnt;
            return;
        } else {
            b->pre->next = b->next;
            b->next->pre = b->pre;
            --LFUCnt;
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
            --LRUGCnt;
            return;
        } else {
            gb->pre->next = gb->next;
            gb->next->pre = gb->pre;
            delete gb;
            --LRUGCnt;
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
            --LFUGCnt;
            return;
        } else {
            gb->pre->next = gb->next;
            gb->next->pre = gb->pre;
            delete gb;
            --LFUGCnt;
            return;
        }
    }
}

inline void getFront(Block *b) {
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

inline void freeGListTail(int choice) {
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
        ++LRUCnt;
    } else {
        LFUHead->next->pre = b;
        b->next = LFUHead->next;
        LFUHead->next = b;
        b->pre = LFUHead;
        ++LFUCnt;
    }
    return;
}

void insertGListHead(int choice,GBlock *gb) {
    if (choice == LRU) {
        LRUGHead->next->pre = gb;
        gb->next = LRUGHead->next;
        LRUGHead->next = gb;
        gb->pre = LRUGHead;
        ++LRUGCnt;
    } else {
        LFUGHead->next->pre = gb;
        gb->next = LFUGHead->next;
        LFUGHead->next = gb;
        gb->pre = LFUGHead;
        ++LFUGCnt;
    }
    return;
}

void freeListTail(int choice,long *physic_memery) {
    Block *replaced;
    GBlock *replacedGhost = new GBlock();
    if (choice == LRU) {
        replaced = LRUTail->pre;
        clearPhysic(replaced->ppage);
        replacedGhost->vpage = replaced->vpage;
        removeListTail(LRU,replaced);
        delete replaced;
        insertGListHead(LRU,replacedGhost);
        freeGListTail(LRU);
    } else {
        replaced = LFUTail->pre;
        clearPhysic(replaced->ppage);
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

void pageReplace(long * physic_memery, long nwAdd) {
    static int first = 0;
    if (first == 0) {
        init();
    }
    first = 1;
    long page = get_Page(nwAdd);
    register Block *bptr;
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
    register GBlock *gbptr;
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


long test[10000000];
int main() {
    long pm[MAX_PHY_PAGE] = {0};
    int find;
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
        find = 0;
        start = clock();
        pageReplace(pm,page);
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
    }
    cout << "time cost: " << duration << endl;
    return 0;
}