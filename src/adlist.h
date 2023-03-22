//
// Created by zgys on 23-3-21.
//

#ifndef REDIS_SIMPLIFY_ADLIST_H
#define REDIS_SIMPLIFY_ADLIST_H

/* Node, List, and Iterator are the only data structures used currently. */

#include <functional>
#include <memory>

namespace redis_simplify {


/* Directions for iterators -- 迭代器进行迭代的方向 */
#define AL_START_HEAD 0 // 从表头向表尾进行迭代
#define AL_START_TAIL 1 // 从表尾到表头进行迭代


    /* 双端链表节点 */
    typedef struct listNode {

        struct listNode *prev; // 前置节点

        struct listNode *next; // 后置节点

        void* value;           // 节点的值
    } listNode;

    /* 双端链表迭代器 */
    typedef struct listIter {

        listNode *next;       // 当前迭代到的节点

        int direction;        // 迭代的方向

    } listIter;

    class List {
    public:
        typedef std::shared_ptr<List> list_ptr;

        List() = default;
        /* 释放链表中所有节点 T = O(N) */
        ~List();

        /* listAddNodeHead 将一个包含有给定值指针 value 的新节点添加到链表的表头 T = O(1) */
        void listAddNodeHead(void* value);

        /* listAddNodeTail 将一个包含有给定值指针 value 的新节点添加到链表的表尾 T = O(1) */
        void listAddNodeTail(void* value);

        /* listInsertNode 创建一个包含值 value 的新节点，并将它插入到 old_node 的之前或之后
         * 如果 after 为 0 ，将新节点插入到 old_node 之前
         * 如果 after 为 1 ，将新节点插入到 old_node 之后
         * T = O(1)
         * */
        void listInsertNode(listNode* old_node, void* value, int after);

        /* listDelNode 从链表 list 中删除给定节点 node T = O(1)
         * 对节点私有值(private value of the node)的释放工作由调用者进行。该函数一定会成功. */
        void listDelNode(listNode* node);

        /*
        * listGetIterator 为给定链表创建一个迭代器，
        * 之后每次对这个迭代器调用 listNext 都返回被迭代到的链表节点,调用该函数不会失败
        *
        * direction 参数决定了迭代器的迭代方向：
        *  AL_START_HEAD ：从表头向表尾迭代
        *  AL_START_TAIL ：从表尾想表头迭代
        *
        * T = O(1)
        */
        listIter* listGetIterator(int direction);

        /*
         * 复制整个链表。
         *
         * 复制成功返回输入链表的副本，
         * 如果因为内存不足而造成复制失败，返回 NULL 。
         *
         * 如果链表有设置值复制函数 dup ，那么对值的复制将使用复制函数进行，
         * 否则，新节点将和旧节点共享同一个指针。
         *
         * 无论复制是成功还是失败，输入节点都不会修改。
         *
         * T = O(N)
         */
        List* listDup(List* orig);

        /*
         * 查找链表 list 中值和 key 匹配的节点。
         *
         * 对比操作由链表的 match 函数负责进行，
         * 如果没有设置 match 函数，
         * 那么直接通过对比值的指针来决定是否匹配。
         *
         * 如果匹配成功，那么第一个匹配的节点会被返回。
         * 如果没有匹配任何节点，那么返回 NULL 。
         *
         * T = O(N)
         */
        listNode* listSearchKey(void* key);

        /*
         * 返回链表在给定索引上的值。
         *
         * 索引以 0 为起始，也可以是负数， -1 表示链表最后一个节点，诸如此类。
         *
         * 如果索引超出范围(out of range),返回 NULL 。
         *
         * T = O(N)
         */
        listNode* listIndex(long index);

        /*
         * 将迭代器的方向设置为 AL_START_HEAD,
         * 并将迭代指针重新指向表头节点。
         *
         * T = O(1)
         */
        void listRewind(listIter* li);

        /*
         * 将迭代器的方向设置为 AL_START_TAIL,
         * 并将迭代指针重新指向表尾节点。
         *
         * T = O(1)
         */
        void listRewindTail(listIter* li);

        /*
         * 取出链表的表尾节点，并将它移动到表头，成为新的表头节点。
         *
         * T = O(1)
         */
        void listRotate();

    public:
        /*
         * 返回迭代器当前所指向的节点。
         *
         * 删除当前节点是允许的,但不能修改链表里的其他节点。
         *
         * 函数要么返回一个节点,要么返回 NULL,常见的用法是：
         *
         * iter = listGetIterator(list, <direction>);
         * while ((node = listNext(iter)) != NULL) {
         *     doSomethingWith(listNodeValue(node));
         * }
         *
         * T = O(1)
         */
        static listNode* listNext(listIter* iter);

        /*
         * listReleaseIterator 释放迭代器
         *
         * T = O(1)
         */
        static void listReleaseIterator(listIter* iter);

        /*返回给定节点的前置节点 */
        static listNode* listPrevNode(listNode* node);

        /* 返回给定节点的后置节点 */
        static listNode* listNextNode(listNode* node);

        /* 返回给定节点的值 */
        static void* listNodeValue(listNode* node);

    public:
        unsigned long listLength() const;                                        // 返回给定链表所包含的节点数量
        listNode* listFirst() const;                                             // 返回给定链表的表头节点
        listNode* listLast() const;                                              // 返回给定链表的表尾节点

        void listSetDupMethod(std::function<void*()> dup);                       // 将链表 l 的值复制函数设置为 m
        void listSetFreeMethod(std::function<void()> free);                      // 将链表 l 的值释放函数设置为 m
        void listSetMatchMethod(std::function<bool(void* a, void* b)> match);    // 将链表的对比函数设置为 m
        std::function<void *()> listGetDupMethod();                              // 返回给定链表的值复制函数
        std::function<void()> listGetFree();                                     // 返回给定链表的值释放函数
        std::function<bool(void *, void *)> listGetMatchMethod();                // 返回给定链表的值对比函数

    private:
        listNode* m_head = nullptr;    // 表头节点
        listNode* m_tail = nullptr;    // 表尾节点

        std::function<void*()> m_dup_fun = nullptr;                   // 节点值复制函数(C++ 复制应当在重载赋值运算符)
        std::function<void()> m_free_fun = nullptr;                   // 节点值释放函数(C++ 中释放应当在析构中完成)
        std::function<bool(void *, void *)> m_match_fun = nullptr;    // 节点值对比函数(C++ 中释放应当重载大于小于运算符)

        unsigned long m_len = 0;                                      // 链表所包含的节点数量
    };

}

#endif //REDIS_SIMPLIFY_ADLIST_H
