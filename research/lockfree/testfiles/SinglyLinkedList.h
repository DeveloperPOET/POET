#include "TATAS.h"
#include <vector>

#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H
template <class T>
class SinglyLinkedListWrap  
  ;
template <class T>
class SinglyLinkedEntryWrap  
  {
    T o;
    SinglyLinkedEntryWrap<T>* next;
    public:
    SinglyLinkedEntryWrap (const T& _o) : o(_o),next(0) {
       
    }
    ~SinglyLinkedEntryWrap() {
       
    }
    T& GetEntry() {
       return o;
    }
    friend class SinglyLinkedListWrap<T>;
  };
template <class T>
class SinglyLinkedListWrap  
  {
    SinglyLinkedEntryWrap<T>* head;SinglyLinkedEntryWrap<T>* end;
    unsigned count;
    public:
    public:
    SinglyLinkedListWrap () : head(0),end(0),count(0) {
       
    }
    public:
    SinglyLinkedListWrap (const SinglyLinkedListWrap<T>& that)  {
       head = end = 0;
       count = 0;
       *this+=that;
    }
    ~SinglyLinkedListWrap() {
       DeleteAll();
    }
    void Reverse() {
       int __ret__3=0;
       
       { 
         AcquireLock acq(lock1);
       
       if (count==0)  
         {
            __ret__3 = 1;
         }
       if (__ret__3!=1)  
         {
            SinglyLinkedEntryWrap<T>* tmp=head;
            SinglyLinkedEntryWrap<T>* tmp1=tmp->next;
            while (tmp!=end)  
              {
                 SinglyLinkedEntryWrap<T>* tmp2=tmp;
                 tmp = tmp1;
                 tmp1 = tmp->next;
                 tmp->next = tmp2;
              }
            head->next = 0;
            end = head;
            head = tmp;
         }
        }
       return ;
    }
    SinglyLinkedEntryWrap<T>* AppendLast(const T& o) {
       SinglyLinkedEntryWrap<T>* e=new SinglyLinkedEntryWrap<T> (o);
       
       { 
         AcquireLock acq(lock1);
       
       ++count;
       if (end==0)  
         {
            head = end = e;
         }
       else  
         {
            end->next = e;
            end = e;
         }
        }
       return e;
    }
    SinglyLinkedEntryWrap<T>* PushFirst(const T& o) {
       SinglyLinkedEntryWrap<T>* e=new SinglyLinkedEntryWrap<T> (o);
       
       { 
         AcquireLock acq(lock1);
       
       ++count;
       if (head==0)  
         {
            head = end = e;
         }
       else  
         {
            e->next = head;
            head = e;
         }
        }
       return e;
    }
    void PopFirst() {
       
       { 
         AcquireLock acq(lock1);
       
       if (count>0)  
         {
            --count;
            SinglyLinkedEntryWrap<T>* e=head;
            if (head==end)  
              {
                 head = end = 0;
              }
            else  
              {
                 head = head->next;
              }
            
         }
        }
    }
    void DeleteAll() {
       
       { 
         AcquireLock acq(lock1);
       
       while (head!=0)  
         {
            if (count>0)  
              {
                 --count;
                 SinglyLinkedEntryWrap<T>* e=head;
                 if (head==end)  
                   {
                      head = end = 0;
                   }
                 else  
                   {
                      head = head->next;
                   }
                 
              }
         }
        }
    }
    unsigned size() const{
       
       { 
         AcquireLock acq(lock1);
       
       return count;
        }
    }
    
    
    SinglyLinkedEntryWrap<T>* Next(SinglyLinkedEntryWrap<T>* cur) const{
       
       { 
         AcquireLock acq(lock1);
       
       
        }
       return cur->next;
    }
    class Iterator  
      {
        const SinglyLinkedListWrap<T>* list;
        SinglyLinkedEntryWrap<T>* cur;
        public:
        public:
        Iterator (const SinglyLinkedListWrap<T>& l) : list(&l) {
           cur = l.First();
        }
        public:
        Iterator (const Iterator& that) : list(that.list),cur(that.cur) {
           
        }
        Iterator& operator =(const Iterator& that) {
           list = that.list;
           cur = that.cur;
        }
        public:
        Iterator () : list(0),cur(0) {
           
        }
        T& Current() const{
           return (cur->GetEntry)();
        }
        T& operator *() const{
           return Current();
        }
        bool ReachEnd() const{
           return cur==0;
        }
        void Reset() {
           if (list!=0)  
             {
                cur = (list->First)();
             }
        }
        void Advance() {
           if (cur!=0)  
             {
                cur = (list->Next)(cur);
             }
        }
        void operator ++() {
           Advance();
        }
        void operator ++(int) {
           Advance();
        }
      };
    private:
    mutable TATAS lock1;
  };
#endif
