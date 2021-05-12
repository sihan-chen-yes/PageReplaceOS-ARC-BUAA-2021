//#include "pageReplace.h"
#define MAX_PHY_PAGE 64
#define MAX_PAGE 12
#define LRUGHOST_SIZE MAX_PHY_PAGE / 2
#define LFUGHOST_SIZE MAX_PHY_PAGE / 2
#define get_Page(x) (x>>MAX_PAGE)
#define LRU -1
#define LFU 1
#define NULL 0

int p = MAX_PHY_PAGE / 2;

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

int changeCapacity(int choice) {
    p += choice;
    return 1;
}

int setEmptyPage(long *physic_memery,long vpage) {
    for (int i = 0; i < MAX_PHY_PAGE; ++i) {
        if (EmptyPage[i] == 0) {
            physic_memery[i] = vpage;
            EmptyPage[i] = 1;
            return i;
        }
    }
    return -1;
}

void clearPhysic(int i) {
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