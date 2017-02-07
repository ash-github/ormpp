//
// Created by QY on 2016-12-26.
//

#ifndef UNTITLED3_OUT_BINDER_HPP
#define UNTITLED3_OUT_BINDER_HPP

#include "sqlite3.h"

namespace sqlite
{
    void assign(sqlite3_stmt* stmt, int& item, size_t I)
    {
        item = sqlite3_column_int(stmt, I);
    }

    void assign(sqlite3_stmt* stmt, double & item, size_t I)
    {
        item = sqlite3_column_int(stmt, I);
    }

    void assign(sqlite3_stmt* stmt, char*& item, size_t I)
    {
        item = (char*)sqlite3_column_text(stmt, I);
    }

    void assign(sqlite3_stmt* stmt, std::string& item, size_t I)
    {
        item.reserve(sqlite3_column_bytes(stmt, I));
        item.assign((const char*)sqlite3_column_text(stmt, I), (size_t)sqlite3_column_bytes(stmt, I));
    }

    void assign(sqlite3_stmt* stmt, std::vector<char>& item, size_t I)
    {
        item.reserve(sqlite3_column_bytes(stmt, I));
        auto buf = sqlite3_column_text(stmt, I);
        item.assign(buf,buf+sqlite3_column_bytes(stmt, I));
    }

    template<size_t N>
    void assign(sqlite3_stmt* stmt, std::array<char, N>& item, size_t I)
    {
        memcpy(item.data(), sqlite3_column_text(stmt, I), (size_t)sqlite3_column_bytes(stmt, I));
    }

    template<size_t N>
    void assign(sqlite3_stmt* stmt, char (&item)[N], size_t I)
    {
        memcpy(item, sqlite3_column_text(stmt, I), (size_t)sqlite3_column_bytes(stmt, I));
    }
}
#endif //UNTITLED3_OUT_BINDER_HPP
