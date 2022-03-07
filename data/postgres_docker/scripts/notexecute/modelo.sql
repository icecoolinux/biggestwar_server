


CREATE TABLE users
(
	username varchar(100) NOT NULL,
	pass varchar(100) NOT NULL,
	
	wins int NOT NULL,
	lost int NOT NULL,
	points int NOT NULL,
	
	PRIMARY KEY (username)
);

CREATE TABLE worlds
(
	worldNumber SERIAL NOT NULL,
	stage int NOT NULL,
	amountplayers int NOT NULL,
	uri varchar(500) NOT NULL,
	port int NOT NULL,
	
	PRIMARY KEY (worldNumber)
);

CREATE TABLE players
(
	username varchar(100) NOT NULL,
	worldNumber int NOT NULL,
	
	equip int NOT NULL,
	token varchar(200) NOT NULL,
	
	xview float NOT NULL,
	yview float NOT NULL,
	
	PRIMARY KEY (username),
	FOREIGN KEY (username) REFERENCES users(username),
	FOREIGN KEY (worldNumber) REFERENCES worlds(worldNumber)
);

CREATE TABLE results
(
	username varchar(100) NOT NULL,
	worldNumber int NOT NULL,
	equip int NOT NULL,
	
	rank int NOT NULL,
	points int NOT NULL,
	surrender int NOT NULL,
	teamWin int NOT NULL,
	winToEnd int NOT NULL,
	
	PRIMARY KEY (username)
);


CREATE TABLE Process
(
	pid int NOT NULL,
	worldNumber int,
	eslogin int,
	
	PRIMARY KEY (pid)
);

CREATE TABLE Servers
(
	url varchar(500) NOT NULL
);

ALTER TABLE users OWNER TO biggestwaruser;
ALTER TABLE worlds OWNER TO biggestwaruser;
ALTER TABLE players OWNER TO biggestwaruser;
ALTER TABLE results OWNER TO biggestwaruser;
ALTER TABLE Process OWNER TO biggestwaruser;
ALTER TABLE Servers OWNER TO biggestwaruser;


-- Add data

INSERT INTO Servers (url) VALUES ('ws://127.0.0.1');


INSERT INTO users (username, pass, wins, lost, points) VALUES ('icecool', 'pass', 0, 0, 0);

-- Make 10000 IA agent users
do $$
begin
for i in 1..9999 loop
INSERT INTO users (username, pass, wins, lost, points) VALUES (CONCAT('AgentIA', cast(i as varchar)), 'pass', 0, 0, 0);
end loop;
end;
$$;

-- Make 10000 IA dummy agent users
do $$
begin
for i in 1..9999 loop
INSERT INTO users (username, pass, wins, lost, points) VALUES (CONCAT('AgentDummy', cast(i as varchar)), 'pass', 0, 0, 0);
end loop;
end;
$$;












