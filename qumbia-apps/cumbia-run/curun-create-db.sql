CREATE TABLE IF NOT EXISTS apps (
	name TEXT NOT NULL,
	path TEXT NOT NULL,
	exename TEXT,
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


CREATE TABLE IF NOT EXISTS bin_suffixes (
        suffix TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS binaries (
        app_name TEXT NOT NULL,
	bin_name TEXT NOT NULL,
	PRIMARY KEY(app_name, bin_name)
);


