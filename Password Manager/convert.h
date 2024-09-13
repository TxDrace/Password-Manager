#pragma once

#ifndef COVERT_H
#define COVERT_H

#include <iostream>
#include <string>

template <class T>
std::unique_ptr<T[]> string2char(const std::string& in)
{
    std::unique_ptr<T[]> res(new T[in.size() + 1]); // Must plus 1 for the null terminator '\0' of the array
    strcpy((char*)res.get(), in.c_str());
    res[in.size()] = '\0'; // Add null terminator
    return res;
}

#endif
