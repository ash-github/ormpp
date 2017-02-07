//
// Created by QY on 2016-12-20.
//
#ifndef UNTITLED3_DBNG_HPP
#define UNTITLED3_DBNG_HPP
#include <vector>
#include <array>
#include "sqlite3.h"
#include "reflection.hpp"
#include "in_binder.hpp"
#include "out_binder.hpp"

class dbng
{
public:
    ~dbng()
    {
 //       int result = sqlite3_finalize(stmt_);
        int result = sqlite3_close(handle_);
        std::cout<<result<<std::endl;
    }

    template<typename T>
    std::vector<T> query(const char* sql)
    {
        int result = sqlite3_prepare_v2(handle_, sql, strlen(sql), &stmt_, nullptr);
        if(result!=SQLITE_OK)
            return {};

        std::vector<T> v;
        while (true)
        {
            result = sqlite3_step(stmt_);
            if (result == SQLITE_DONE)
                break;

            if (result != SQLITE_ROW)
                break;

            T t;
            for_each(t, [this](auto& item, size_t I, bool is_last)
            {
                sqlite::assign(stmt_, item, I);
            });
            v.push_back(std::move(t));
        }

        result = sqlite3_finalize(stmt_);
        return v;
    }

	template<typename T>
	int insert(T&& t, const std::vector<int>& v = {})
	{
		auto sql = make_sql(std::forward<T>(t), "insert", v);

		return excecute(sql.c_str(), t, v);
	}

	template<typename T>
	std::string make_sql(T&& t, const std::string& pre, const std::vector<int>& v = {})
	{
		std::string str_atr = pre+" into ";
		std::string str = "values(";
		using M = Members<std::remove_const_t <std::remove_reference_t<T>>>;
		str_atr += M::name; 
		str_atr += +"(";
		const int size = M::value;
		for (size_t i = 0; i < size; i++)
		{
			if (!v.empty() && std::find(v.begin(), v.end(), i) != v.end())
			{
				if (i == size - 1)
				{
					str_atr[str_atr.length() - 1] = ')';
					str[str.length() - 1] = ')';
				}

				continue;
			}

			str_atr += get_name<T>(i);
			str += "?";
			if (i != size - 1)
			{
				str_atr += ",";
				str += ",";
			}
			else
			{
				str_atr += ") ";
				str += "); ";
			}
		}

		return str_atr + str;
	}

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, std::string> to_str(T&& t)
	{
		return std::to_string(t);
	}

	template<typename T>
	std::enable_if_t<!std::is_arithmetic<T>::value, std::string> to_str(T&& s)
	{
		return std::string("'")+s+ std::string("'");
	}

	template<typename... Args>
	bool fill_condition(std::string& where, Args&&... args)
	{
		int count = 0;
		for (size_t i = 0; i < where.size(); i++)
		{
			if (where[i] == '?')
				count++;
		}
		if (count != sizeof...(Args))
			return false;

		//where.insert(where.find_first_of('?'), );
		std::string s = "";
		std::initializer_list<int>{(
			s = to_str(std::forward<Args>(args)),
		where.replace(where.find_first_of('?'), s.size(), s)
			, 0)...};

		return true;
	}

	template<typename T, typename... Args>
	int update(T&& t, const std::vector<int>& v, std::string& where, Args&&... args)
	{
		bool ok = fill_condition(where, std::forward<Args>(args)...);
		if (!ok)
			return -1;

		std::string str_atr = "update ";
		
		using M = Members<std::remove_const_t <std::remove_reference_t<T>>>;
		str_atr += M::name;
		str_atr += +" set ";
		const int size = M::value;
		for (size_t i = 0; i < size; i++)
		{
			if (!v.empty() && std::find(v.begin(), v.end(), i) != v.end())
			{
				if (i == size - 1)
				{
					str_atr[str_atr.length() - 1] = ' ';
				}

				continue;
			}

			str_atr += get_name<T>(i);
			str_atr += " = ? ";
			if (i != size - 1)
			{
				str_atr += ",";
			}
		}
		str_atr += where;

//		auto sql = make_sql(std::forward<T>(t), "replace", v);
		
		return excecute(str_atr.c_str(), t, v);
	}

    int exceute(const char* sql)
    {
        return sqlite3_exec(handle_, sql, nullptr, nullptr, nullptr);
    }

    int prepare(const char* sql)
    {
        return sqlite3_prepare_v2(handle_, sql, strlen(sql), &stmt_, nullptr);
    }

    template<typename... Args>
    int excecute_args(Args&&... args)
    {
        int result = sqlite::bind<1>(stmt_, std::forward<Args>(args)...);
        result = sqlite3_step(stmt_);
        result = sqlite3_reset(stmt_);
        return result;
    }

    int begin()
    {
        return exceute("BEGIN");
    }

    int ROLLBACK()
    {
        int result = sqlite3_finalize(stmt_);
        return exceute("ROLLBACK");
    }

    int commit()
    {
        int result = sqlite3_finalize(stmt_);
        return exceute("COMMIT");
    }

    template<typename... Args>
    int excecute(const char* sql, Args&&... args)
    {
        int result = sqlite3_prepare_v2(handle_, sql, strlen(sql), &stmt_, nullptr);
        if(result!=SQLITE_OK)
            return result;

        result = sqlite::bind<1>(stmt_, std::forward<Args>(args)...);
        result = sqlite3_step(stmt_);
        result = sqlite3_finalize(stmt_);

        return result;
    }

    template<typename T, typename=std::enable_if_t<std::is_class<T>::value>>
	int excecute(const char* sql, T& t, const std::vector<int>& v)
    {
        int result = sqlite3_prepare_v2(handle_, sql, strlen(sql), &stmt_, nullptr);
		int index = 0;
        for_each(t, [this,&v,&index,&result](auto& item, size_t I, bool is_last)
        {
			index += 1;
			auto it = std::find(v.begin(), v.end(), I);
			if (it != v.end())
			{
				index -= 1;
				return;
			}				

            int r = sqlite::bind(stmt_, item, index);
			if (r != 0)
				result = r;
        });

        result = sqlite3_step(stmt_);
        result = sqlite3_finalize(stmt_);

        return result;
    }

	template<typename T, typename = std::enable_if_t<std::is_class<T>::value>>
	int excecute(const char* sql, T& t)
	{
		int result = sqlite3_prepare_v2(handle_, sql, strlen(sql), &stmt_, nullptr);
		for_each(t, [this](auto& item, size_t I, bool is_last)
		{
			sqlite::bind(stmt_, item, I+1);
		});

		result = sqlite3_step(stmt_);
		result = sqlite3_finalize(stmt_);

		return result;
	}

    int create_database(const char* sql)
    {
        return sqlite3_open(sql, &handle_);
    }
private:
    sqlite3* handle_;
    sqlite3_stmt* stmt_ = nullptr;
};
#endif //UNTITLED3_DBNG_HPP
