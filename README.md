# A Universal ORM Engine based on Reflection #
ormpp is a modern, universal and easy-to-use ORM engine developed in c++14. ormpp only supports sqlite now, however ormpp can support any database, such as mysql, postgresql,oracle etc. ormpp will support many kind of database in future. ormpp is in developing now.

### Motivation ###
Simplify database operation with compile-time reflection.ormpp provides very simple and universal interface, although the databases type are different.In short ormpp shields the difference of the bottom database.
This library provides a portable cross-platform way of:

- operation of sqlite
- operation of mysql(in the paln)
- operation of postgresql(in the paln)
- operation of any other database

### Tutorial ###
This Tutorial is provided to give you a view of how to use ormpp, now it is only concerned with sqlite.
### Data manipulation
**Create a database.**

	dbng db;
	int result = db.create_database("test.db");//return 0 is successfull, others else are failed.

**Create a table.**

	const char* create_user = "create table if not exists user(id integer PRIMARY KEY AUTOINCREMENT, name text, pwd text,"
		"qq text, sex integer, role integer)";
	int result = db.excecute(create_user);

The excecute method is a universal interface, it can accept any data manipulation sql string. So you can also call excecute for insert, update and delete etc.

**Binding parameters.**

The excecute interface support binding parameters, bellow is an example.

	const char* insert_user = "insert into user(id, name,pwd,qq,sex,role) values(?,?,?,?,?,?);";

	auto result = db.excecute(insert_user, 1, "admin", "admin", "12345", 1, 1);
If the statement needs some later binding parameters, pass the parameters straight forward. ormpp will atomically binding theparameters.

**Operation with object**

If you get tired of filling many parameters, you can choose object instead. ormpp can automatically make sql with compile-time reflection.Bellow is an example of inserting with object.
Firstly, define meta data just as iguana dose.

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
Secondly, directly call excecute interface.

	user _user = { 2,"admin", "admin", "12345", 1, 1 };
	db.insert(_user);

**ignore some fields of an object**

insert interface provides a default parameter which is used to tell ormpp which fields would be ignored.Just need pass the fields indexes.Bellow is an example about ignore specific fields:

	user _user = { 2,"admin", "admin", "12345", 1, 1 };
	db.insert(_user);

	_user.name = "test";
	_user.pwd = "test";
	std::string str_where = "where id= ? ";
	db.update(_user, { 0,5 }, str_where, 2); //ignore the first field and the fifth field

**Query table**

Query table is also very simple, just need call a query interface with a reflection object.

	const char* get_all = "select * from user";
	std::vector<user> parts = db.query<user>(get_all);
How about multiple tables query? Still call the query interface.

	const char* get_all1 = "select * from user,article,contact;";
	std::vector<std::tuple<user, article_t, contact>> parts1 = db.query<user, article_t, contact>(get_all1);
	//you can use auto simply the code

If you query parts of tables, still call the query interface
	
	const char* get_all2 = "select user.*, article.title, contact.author_id from user,article,contact;";
	auto parts2 = db.query<user, std::string, int>(get_all2);

### Full sources:


- https://github.com/qicosmos/ormpp/blob/master/example.cpp

