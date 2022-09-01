//------------------------------------------------------------------------------
// 
//     
//
//------------------------------------------------------------------------------

#include <cstdio>
#include <cstring>
#include <cmath>
#include <immintrin.h>
#include "mdlist.h"

#define SET_ADPINV(_p)    ((Node *)(((uintptr_t)(_p)) | 1))
#define CLR_ADPINV(_p)    ((Node *)(((uintptr_t)(_p)) & ~1))
#define IS_ADPINV(_p)     (((uintptr_t)(_p)) & 1)

//------------------------------------------------------------------------------
template<int D>
inline void MDList::KeyToCoord(uint32_t key, uint8_t coord[])
{
    const static uint32_t basis[32] = {0xffffffff, 0x10000, 0x800, 0x100, 0x80, 0x40, 0x20, 0x10, 0xC, 0xA, 0x8, 0x7, 0x6, 0x5, 0x5, 0x4,
                                        0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x2};

    uint32_t quotient = key;

    for (int i = D - 1; i >= 0 ; --i) 
    {
        coord[i] = quotient % basis[D - 1];
        quotient /= basis[D - 1];
    }
}

//------------------------------------------------------------------------------
MDList::MDList()
: m_head(new Node)
{
    memset(m_head, 0, sizeof(Node));
}

MDList::~MDList()
{
    std::string prefix;
    Traverse(m_head, NULL, 0, prefix);
}


//------------------------------------------------------------------------------
void MDList::Insert(uint32_t key)
{
    //Allocate new node
    //Decompose Key into mutli-dimension coordinates
    Node* new_node = new Node;
    new_node->m_key = key;
    KeyToCoord<DIMENSION>(key, new_node->m_coord);

    Node* pred = NULL;      //pred node
    Node* curr = m_head;    //curr node
    uint32_t dim = 0;       //the dimension of curr node
    uint32_t pred_dim = 0;  //the dimesnion of pred node
    
    LocatePred(new_node->m_coord, pred, curr, dim, pred_dim);

    //We are not updating existing node, remove this line to allow update
    if(dim == DIMENSION)
    {
        return; 
    }

    FillNewNode(new_node, pred, curr, dim, pred_dim);
    pred->m_child[pred_dim] = new_node;
    FinishInserting(new_node, curr, dim, pred_dim);
}


inline void MDList::LocatePred(uint8_t coord[], Node*& pred, Node*& curr, uint32_t& dim, uint32_t& pred_dim)
{
    //Locate the proper position to insert
    //traverse list from low dim to high dim
    while(dim < DIMENSION)
    {
        //Loacate predecessor and successor
        while(curr && coord[dim] > curr->m_coord[dim])
        {
            pred_dim = dim;
            pred = curr;
            curr = curr->m_child[dim];
        }

        //no successor has greater coord at this dimension
        //the position after pred is the insertion position
        if(curr == NULL || coord[dim] < curr->m_coord[dim]) 
        {
            //done searching
            break;
        }
        //continue to search in the next dimension 
        //if coord[dim] of new_node overlaps with that of curr node
        else
        {
            //dim only increases if two coords are exactly the same
            ++dim;
        }
    }
}


inline void MDList::FillNewNode(Node* new_node, Node* pred, Node* curr, uint32_t dim, uint32_t pred_dim)
{
    //Fill values for new_node, m_child is set to 1 for all children before pred_dim
    //pred_dim is the dimension where new_node is inserted, all dimension before that are invalid for new_node
    for(uint32_t i = 0; i < pred_dim; ++i)
    {
        new_node->m_child[i] = (Node*)0x1;
    }
    //be careful with the length of memset, should be DIMENSION - pred_dim NOT (DIMENSION - 1 - pred_dim)
    memset(new_node->m_child + pred_dim, 0, sizeof(Node*) * (DIMENSION - pred_dim));
    if(dim < DIMENSION)
    {
        //If curr is marked for deletion or overriden, we donnot link it. 
        //Instead, we adopt ALL of its children
        new_node->m_child[dim] = curr;
    }
}

inline void MDList::FinishInserting(Node* n, Node* curr, uint32_t dim, uint32_t pred_dim)
{
    for (uint32_t i = pred_dim; i < dim; ++i) 
    {
        Node* child = curr->m_child[i];
        curr->m_child[i] = SET_ADPINV(child);
        n->m_child[i] = child;
    }
}


//------------------------------------------------------------------------------
bool MDList::Delete(uint32_t key)
{
    uint8_t coord[DIMENSION];
    KeyToCoord<DIMENSION>(key, coord);
    Node* pred = NULL;      //pred node
    Node* curr = m_head;    //curr node
    uint32_t dim = 0;       //the dimension of curr node
    uint32_t pred_dim = 0;  //the dimesnion of pred node

    LocatePred(coord, pred, curr, dim, pred_dim);

    Node* nextChild = NULL;
    uint32_t child_dim = 0;

    if(dim == DIMENSION)
    {
        for(int i = DIMENSION - 1; i >= 0; i--)
        {
            if(curr->m_child[i] != NULL)
            {
                nextChild = curr->m_child[i];
                child_dim = i;
                break;
            }
        }

        pred->m_child[pred_dim] = nextChild;
        if(nextChild != NULL)
        {
            for(uint32_t i = pred_dim; i < child_dim; i++)
            {
                nextChild->m_child[i] = curr->m_child[i];
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}


//------------------------------------------------------------------------------
bool MDList::Find(uint32_t key)
{
    uint8_t coord[DIMENSION];
    KeyToCoord<DIMENSION>(key, coord);
    Node* pred = NULL;      //pred node
    Node* curr = m_head;    //curr node
    uint32_t dim = 0;       //the dimension of curr node
    uint32_t pred_dim = 0;  //the dimesnion of pred node

    LocatePred(coord, pred, curr, dim, pred_dim);

    return dim == DIMENSION;
}


//------------------------------------------------------------------------------
inline void MDList::Traverse(Node* n, Node* parent, int dim, std::string& prefix)
{
    printf("%s", prefix.c_str());
    printf("Node [%p] Key [%u] DIM [%d] of Parent[%p]\n", n, n->m_key, dim, parent);

    //traverse from last dimension up to current dim
    //The valid children include child nodes up to dim
    //e.g. a node on dimension 3 has only valid children on dimensions 3~8
    for (int i = DIMENSION - 1; i >= dim; --i) 
    {
        Node* child = n->m_child[i];

        if(child != NULL)
        {
            prefix.push_back('|');
            prefix.insert(prefix.size(), i, ' ');

            Traverse(child, n, i, prefix);

            prefix.erase(prefix.size() - i - 1, i + 1);
        }
    }
}
