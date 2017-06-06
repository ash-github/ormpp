//
// Created by QY on 2016-12-26.
//

#ifndef UNTITLED3_IN_BINDER_HPP
#define UNTITLED3_IN_BINDER_HPP
#include<string.h>
#include "sqlite3.h"

namespace sqlite
{
	template<typename T>
    std::enable_if_t<std::is_integral<T>::value&&!std::is_same<T, int64_t>::value&&!std::is_same<T, uint64_t>::value, int>
		bind(sqlite3_stmt* stmt, T item, size_t I)
    {
        return sqlite3_bind_int(stmt, I, item);
    }

	template<typename T>
	std::enable_if_t<std::is_same<T, int64_t>::value||std::is_same<T, uint64_t>::value, int>
		bind(sqlite3_stmt* stmt, T item, size_t I)
	{
		return sqlite3_bind_int64(stmt, I, item);
	}

    int bind(sqlite3_stmt* stmt, double item, size_t I)
    {
        return sqlite3_bind_double(stmt, I, item);
    }

    int bind(sqlite3_stmt* stmt, const char* item, size_t I)
    {
        return sqlite3_bind_text(stmt, I, item, strlen(item), nullptr);
    }

    int bind(sqlite3_stmt* stmt, const std::nullptr_t  item, size_t I)
    {
        return  0;
        //return sqlite3_bind_blob(stmt, I, item.data(), item.size(), nullptr);
    }

    int bind(sqlite3_stmt* stmt, const std::string& item, size_t I)
    {
        return sqlite3_bind_text(stmt, I, item.data(), item.length(), nullptr);
    }

    int bind(sqlite3_stmt* stmt, const std::vector<char>& item, size_t I)
    {
        return sqlite3_bind_blob(stmt, I, item.data(), item.size(), nullptr);
    }

    template<size_t N>
    int bind(sqlite3_stmt* stmt, const std::array<char, N>& item, size_t I)
    {
        return sqlite3_bind_text(stmt, I, item.data(), item.size(), nullptr);
    }

    template<size_t N>
    int bind(sqlite3_stmt* stmt, const char (&item)[N], size_t I)
    {
        return sqlite3_bind_text(stmt, I, item, strlen(item), nullptr);
    }

    template<size_t I, typename Arg, typename... Args>
    int bind(sqlite3_stmt* stmt, Arg&& arg, Args&&... args)
    {
        auto r = bind(stmt, std::forward<Arg>(arg), I);
        if(r!=SQLITE_OK)
            return r;

        return bind<I+1>(stmt, std::forward<Args>(args)...);
    }

	template<size_t I>
	int bind(sqlite3_stmt* stmt)
	{
		return 0;
	}
}

#endif //UNTITLED3_IN_BINDER_HPP
