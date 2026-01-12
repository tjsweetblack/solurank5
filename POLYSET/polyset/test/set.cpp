#include "set.hpp"

set::set(searchable_bag& other): bag(other)
{

}

void set::insert (int value)
{
     if(!(bag.has(value)))
        bag.insert(value);
}

void set::insert (int *array, int size)
{
    for(int i = 0; i < size; i++)
        insert(array[i]);
}

void set::print() const
{
    bag.print();
}

void set::clear()
{
    bag.clear();
}

bool set::has(int value) const
{
    return(bag.has(value));
}

searchable_bag& set::get_bag()
{
    return(bag);
}

set::~set()
{
}