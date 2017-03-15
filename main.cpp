#include <iostream>
#include "dbng.hpp"

struct user
{
	int id;
	std::string name;
	std::string pwd;
	std::string qq;
	int sex;
	int role;
};
REFLECTION(user, id, name, pwd, qq, sex, role);

struct article_t
{
	int id;
	int type;
	std::string title;
	std::string content;
	std::string desc;
	int category;
	int author_id;
	int pv;
};
REFLECTION(article_t, id, type, title, content, desc, category, author_id, pv);

struct contact
{
	int id;
	std::string title;
	std::string content;
	std::string tms;
	int author_id;
};
REFLECTION(contact, id, title, content, tms, author_id);

void test_sqlite()
{
	const char* create_article = "create table if not exists article(id integer PRIMARY KEY AUTOINCREMENT, type integer NOT NULL,"
		"title text NOT NULL, content blob, desc text, category integer, author_id integer, pv integer);";
	const char* create_user = "create table if not exists user(id integer PRIMARY KEY AUTOINCREMENT, name text, pwd text,"
		"qq text, sex integer, role integer)";
	const char* create_article_type = "create table if not exists article_type(id integer PRIMARY KEY AUTOINCREMENT, name text, info test)";
	const char* create_article_contact = "create table if not exists contact(id integer PRIMARY KEY AUTOINCREMENT, title text, content text,"
		"tm text, author_id int)";
	const char* create_article_comment = "create table if not exists commet(id integer PRIMARY KEY AUTOINCREMENT, title text, content text,"
		"tm text, author_id int, to_id int)";

	dbng db;
	int result = db.create_database("test.db");

	result = db.excecute(create_article);
	result = db.excecute(create_user);
	result = db.excecute(create_article_type);
	result = db.excecute(create_article_contact);
	result = db.excecute(create_article_comment);

	const char* get_all = "select * from user";
	auto parts = db.query<user>(get_all);

	const char* get_all1 = "select * from user,article,contact;";
	auto parts1 = db.query<user, article_t, contact>(get_all1);

	const char* get_all2 = "select user.*, article.title, contact.author_id from user,article,contact;";
	auto parts2 = db.query<user, std::string, int>(get_all2);

	const char* left_join = "select article.*,user.id  from article left join user on user.id=article.id";
	auto parts3 = db.query<article_t, int>(left_join);

	const char* insert_user = "insert into user(id, name,pwd,qq,sex,role) values(?,?,?,?,?,?);";
	const char* insert_article = "insert into article(type,title,content,desc,category,author_id,pv) values(?,?,?,?,?,?,?);";
	const char* insert_contact = "insert into contact(title,content,tm,author_id) values(?,?,?,?);";
	result = db.excecute(insert_contact, "admin", "admin", 5, 6);
	result = db.excecute(insert_article, 1, "test", "content", "desc", 3, 4, 2);
	result = db.excecute(insert_user, "admin", "admin", "12345", 1, 1);	

	const char* del_users = "delete from user";
	result = db.excecute(del_users);
	user _user = { 2,"admin", "admin", "12345", 1, 1 };
	db.insert(_user);
	//_user.id = 3;
	_user.name = "test";
	_user.pwd = "test";
	std::string str_where = "where id= ? ";
	db.update(_user, { 0,5 }, str_where, 2);
	result = db.excecute(insert_user, _user);
}

int main()
{
	test_sqlite();

	return 0;
}