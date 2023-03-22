/**
  ******************************************************************************
  * @file           : adlist.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-22
  ******************************************************************************
  */

#include "src/adlist.h"

namespace redis_simplify {
    List::~List() {
        listNode *current, *next;
        // 指向头指针
        current = m_head;
        while (m_len--) {
            next = current->next;
            // 如果有设置值释放函数，那么调用它
            if (m_free_fun) m_free_fun();
            // 释放节点结构
            delete current;
            current = next;
        }
    }

    void List::listAddNodeHead(void* value) {
        listNode* node = new listNode();
        // 保存值指针
        node->value = value;
        // 添加节点到空链表
        if (m_len == 0) {
            m_head = m_tail = node;
            node->prev = node->next = nullptr;
        } else { // 添加节点到非空链表
            node->prev = nullptr;
            node->next = m_head;
            m_head->prev = node;
            m_head = node;
        }
        // 更新链表节点数
        m_len++;
    }

    void List::listAddNodeTail(void* value) {
        listNode* node = new listNode();

        // 保存值指针
        node->value = value;
        // 目标链表为空
        if (m_len == 0) {
            m_head = m_tail = node;
            node->prev = node->next = nullptr;
            // 目标链表非空
        } else {
            node->prev = m_tail;
            node->next = nullptr;
            m_tail->next = node;
            m_tail = node;
        }
        // 更新链表节点数
        m_len++;
    }

    void List::listInsertNode(listNode* old_node, void* value, int after) {
        listNode* node = new listNode();
        // 保存值
        node->value = value;

        // 将新节点添加到给定节点之后
        if (after) {
            node->prev = old_node;
            node->next = old_node->next;
            // 给定节点是原表尾节点
            if (m_tail == old_node) {
                m_tail = node;
            }
            // 将新节点添加到给定节点之前
        }
        else {
            node->next = old_node;
            node->prev = old_node->prev;
            // 给定节点是原表头节点
            if (m_head == old_node) {
                m_head = node;
            }
        }

        // 更新新节点的前置指针
        if (node->prev != nullptr) {
            node->prev->next = node;
        }
        // 更新新节点的后置指针
        if (node->next != nullptr) {
            node->next->prev = node;
        }

        // 更新链表节点数
        m_len++;
    }

    void List::listDelNode(listNode* node) {
        // 调整前置节点的指针
        if (node->prev)
            node->prev->next = node->next;
        else
            m_head = node->next;

        // 调整后置节点的指针
        if (node->next)
            node->next->prev = node->prev;
        else
            m_tail = node->prev;

        // 释放值
        if (m_free_fun) m_free_fun();

        // 释放节点
        delete node;

        // 链表数减一
        m_len--;
    }

    listIter* List::listGetIterator(int direction) {
        // 为迭代器分配内存
        listIter* iter = new listIter();

        // 根据迭代方向，设置迭代器的起始节点
        if (direction == AL_START_HEAD)
            iter->next = m_head;
        else
            iter->next = m_tail;

        // 记录迭代方向
        iter->direction = direction;

        return iter;
    }

    listNode* List::listNext(listIter* iter) {
        listNode* current = iter->next;

        if (current != nullptr) {
            // 根据方向选择下一个节点
            if (iter->direction == AL_START_HEAD)
                // 保存下一个节点，防止当前节点被删除而造成指针丢失
                iter->next = current->next;
            else
                // 保存下一个节点，防止当前节点被删除而造成指针丢失
                iter->next = current->prev;
        }

        return current;
    }

    void List::listReleaseIterator(listIter* iter) {
        delete iter;
    }

    List* List::listDup(List* orig) {
        // 创建新链表
        List* copy = new List();
        listIter* iter;
        listNode* node;

        // 设置节点值处理函数
        copy->m_dup_fun   = orig->m_dup_fun;
        copy->m_free_fun  = orig->m_free_fun;
        copy->m_match_fun = orig->m_match_fun;

        // 迭代整个输入链表
        iter = orig->listGetIterator(AL_START_HEAD);
        while ((node = listNext(iter)) != nullptr) {
            void* value;

            // 复制节点值到新节点
            if (copy->m_dup_fun) {
                value = copy->m_dup_fun();
                if (value == nullptr) {
                    delete copy;
                    listReleaseIterator(iter);
                    return nullptr;
                }
            }
            else
                value = node->value;

            // 将节点添加到链表
            copy->listAddNodeTail(value);
        }

        // 释放迭代器
        listReleaseIterator(iter);

        // 返回副本
        return copy;
    }

    listNode* List::listSearchKey(void* key) {
        listIter* iter;
        listNode* node;

        // 迭代整个链表
        iter = listGetIterator(AL_START_HEAD);
        while ((node = listNext(iter)) != nullptr) {

            // 对比
            if (m_match_fun) {
                if (m_match_fun(node->value, key)) {
                    listReleaseIterator(iter);
                    // 找到
                    return node;
                }
            }
            else {
                if (key == node->value) {
                    listReleaseIterator(iter);
                    // 找到
                    return node;
                }
            }
        }

        listReleaseIterator(iter);

        // 未找到
        return nullptr;
    }

    listNode* List::listIndex(long index) {
        listNode *n;

        // 如果索引为负数，从表尾开始查找
        if (index < 0) {
            index = (-index) - 1;
            n = m_tail;
            while (index-- && n) n = n->prev;
            // 如果索引为正数，从表头开始查找
        }
        else {
            n = m_head;
            while (index-- && n) n = n->next;
        }

        return n;
    }

    void List::listRewind(listIter* li) {
        li->next = m_head;
        li->direction = AL_START_HEAD;
    }

    void List::listRewindTail(listIter* li) {
        li->next = m_tail;
        li->direction = AL_START_TAIL;
    }

    void List::listRotate() {
        listNode* tail = m_tail;

        if (m_len <= 1) return;

        // 取出表尾节点
        m_tail = tail->prev;
        m_tail->next = nullptr;

        // 插入到表头
        m_head->prev = tail;
        tail->prev = nullptr;
        tail->next = m_head;
        m_head = tail;
    }

    unsigned long List::listLength() const {
        return m_len;
    }

    listNode* List::listFirst() const {
        return m_head;
    }

    listNode* List::listLast() const {
        return m_tail;
    }

    listNode* List::listPrevNode(listNode* node) {
        return node->prev;
    }

    listNode* List::listNextNode(listNode* node) {
        return node->next;
    }

    void* List::listNodeValue(listNode* node) {
        return node->value;
    }

    void List::listSetDupMethod(std::function<void*()> dup) {
        m_dup_fun = std::move(dup);
    }

    void List::listSetFreeMethod(std::function<void()> free) {
        m_free_fun = std::move(free);
    }

    void List::listSetMatchMethod(std::function<bool(void* a, void* b)> match) {
        m_match_fun = std::move(match);
    }

    std::function<void *()> List::listGetDupMethod() {
        return m_dup_fun;
    }

    std::function<void()> List::listGetFree() {
        return m_free_fun;
    }

    std::function<bool(void *, void *)> List::listGetMatchMethod() {
        return m_match_fun;
    }
}