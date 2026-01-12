#include "searchable_tree_bag.hpp"

searchable_tree_bag::searchable_tree_bag()
{

}

searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& copy)
:tree_bag(copy){
    
}

searchable_tree_bag& searchable_tree_bag:: operator=(const searchable_tree_bag& other)
{
    if(this != &other)
        tree_bag::operator=(other);
    return *this;
}

bool searchable_tree_bag::search(node *node, int value) const
{
    if(node == nullptr)
        return false;
    if(node->value == value)
        return true;

    if(node->value < value)
        return(search(node->r, value));
    else
        return(search(node->l, value));
    
}

bool searchable_tree_bag::has(int value) const
{
    return(search(tree, value));
}

searchable_tree_bag::~searchable_tree_bag()
{
}