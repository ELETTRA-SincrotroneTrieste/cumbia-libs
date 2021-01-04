CREATE TABLE IF NOT EXISTS apps (
	name TEXT NOT NULL,
	path TEXT NOT NULL,
	PRIMARY KEY(name,path)
);

CREATE TABLE IF NOT EXISTS history (
	id INT NOT NULL,
	env TEXT,
	args TEXT,
	datetime DATETIME NOT NULL,
	stored INT DEFAULT 0,
	tag TEXT,
    PRIMARY KEY(id,env,args)
);


CREATE TABLE IF NOT EXISTS test (
	id INT NOT NULL,
	env TEXT,
	args TEXT,
	datetime DATETIME NOT NULL,
    PRIMARY KEY(id,env,args)
);


