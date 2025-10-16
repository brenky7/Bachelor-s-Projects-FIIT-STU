CREATE TYPE loaned_status AS ENUM('arranged', 'shipped_to', 'delivered', 'exhibited', 'stored', 'shipped_back', 'delivered_back');
CREATE TYPE exemplar_status AS ENUM('exhibited', 'stored', 'loaned', 'checking');
CREATE TYPE exposition_status AS ENUM('active', 'preparation', 'closed');

DROP TABLE IF EXISTS partners CASCADE;
CREATE TABLE partners (
	id BIGSERIAL NOT NULL PRIMARY KEY,
	name VARCHAR(50) NOT NULL,
    ico VARCHAR(12) NOT NULL,
    dic VARCHAR(12) NOT NULL,
	description VARCHAR(500)
);

DROP TABLE IF EXISTS category CASCADE;
CREATE TABLE category (
	id SERIAL NOT NULL PRIMARY KEY,
	name VARCHAR(50) NOT NULL
);

DROP TABLE IF EXISTS storage_positions CASCADE;
CREATE TABLE storage_positions (
	id SERIAL NOT NULL PRIMARY KEY,
	name VARCHAR(50) NOT NULL
);

DROP TABLE IF EXISTS expositions CASCADE;
CREATE TABLE expositions (
	id BIGSERIAL NOT NULL PRIMARY KEY,
	name VARCHAR(50) NOT NULL,
	description VARCHAR(500),
	expected_start_date TIMESTAMP NOT NULL,
    real_start_date TIMESTAMP,
	expected_end_date TIMESTAMP NOT NULL,
    real_end_date TIMESTAMP,
    duration INTERVAL,
	state exposition_status DEFAULT 'preparation',
    total_navstevnost INT,
	CONSTRAINT check_expected_date CHECK (expected_end_date > expected_start_date)
);

DROP TABLE IF EXISTS exemplars CASCADE;
CREATE TABLE exemplars (
	id BIGSERIAL NOT NULL PRIMARY KEY,
	name VARCHAR(50) NOT NULL,
	description VARCHAR(500) NOT NULL,
	state exemplar_status DEFAULT 'stored',
    storage_location INT REFERENCES storage_positions(id),
	owner_id BIGINT NOT NULL REFERENCES partners(id),
	category_id INT NOT NULL REFERENCES category(id)
);

DROP TABLE IF EXISTS checkup CASCADE;
CREATE TABLE checkup (
	id BIGSERIAL NOT NULL PRIMARY KEY,
	description VARCHAR(500),
	exemplar_id BIGINT NOT NULL REFERENCES exemplars(id),
    expected_start TIMESTAMP NOT NULL,
    real_start TIMESTAMP,
    expected_end TIMESTAMP NOT NULL,
    real_end TIMESTAMP,
	length INTERVAL	NOT NULL
);

DROP TABLE IF EXISTS loaned_exemplars CASCADE;
CREATE TABLE loaned_exemplars (
	id BIGSERIAL NOT NULL PRIMARY KEY,
    exemplar_id BIGINT NOT NULL REFERENCES exemplars(id),
	state loaned_status DEFAULT 'arranged',
	loan_date TIMESTAMP NOT NULL,
    expected_receive_date TIMESTAMP,
    receive_date TIMESTAMP,
    storage_location INT REFERENCES storage_positions(id),
	loaner_id BIGINT NOT NULL REFERENCES partners(id),
	expected_return_date TIMESTAMP NOT NULL,
    real_return_date TIMESTAMP,
  	CONSTRAINT check_expected_date CHECK (expected_return_date > loan_date),
    CONSTRAINT check_real_date CHECK (real_return_date > loan_date)
);

DROP TABLE IF EXISTS zones CASCADE;
CREATE TABLE zones (
	id SERIAL NOT NULL PRIMARY KEY,
	name VARCHAR(50) NOT NULL
);

DROP TABLE IF EXISTS exposition_exemplar_zone CASCADE;
CREATE TABLE exposition_exemplar_zone (
	exposition_id BIGINT NOT NULL REFERENCES expositions(id),
	exemplar_id BIGINT REFERENCES exemplars(id),
	zone_id INT NOT NULL REFERENCES zones(id),
    CONSTRAINT check_zone UNIQUE (exposition_id, exemplar_id)
);

-- ------------------- Inserty ------------------------

-- Zones
INSERT INTO zones (name) VALUES
('Zone A'),
('Zone B'),
('Zone C'),
('Zone D'),
('Zone E'),
('Zone F'),
('Zone G'),
('Zone H');

-- Partners

INSERT INTO partners (name, ico, dic, description)
VALUES
    ('Museum', '1234567890', 'DIC123', 'Description of our museum'),
    ('Partner B', '0987654321', 'DIC456', 'Description of Partner B'),
    ('Partner C', '1357924680', 'DIC789', 'Description of Partner C'),
    ('Partner D', '2468013579', 'DIC246', 'Description of Partner D'),
    ('Partner E', '2468067890', 'DIC321', 'Description of Partner E'),
    ('Partner F', '0987624680', 'DIC654', 'Description of Partner F'),
    ('Partner G', '1357912345', 'DIC987', 'Description of Partner G'),
    ('Partner H', '5432113579', 'DIC624', 'Description of Partner H');

-- Category
INSERT INTO category (name)
VALUES
    ('Category A'),
    ('Category B'),
    ('Category C'),
    ('Category D'),
    ('Category E'),
    ('Category F'),
    ('Category G'),
    ('Category H');

-- Storage_positions
INSERT INTO storage_positions (name)
VALUES
    ('Storage Position A'),
    ('Storage Position B'),
    ('Storage Position C'),
    ('Storage Position D'),
    ('Storage Position E'),
    ('Storage Position F'),
    ('Storage Position G'),
    ('Storage Position H');



-- ------------------- Zmeny oproti navrhu -------------------

-- expositions
ALTER TABLE expositions DROP CONSTRAINT check_real_date;
ALTER TABLE expositions ALTER COLUMN real_start_date DROP NOT NULL;
ALTER TABLE expositions ALTER COLUMN real_end_date DROP NOT NULL;
ALTER TABLE expositions ALTER COLUMN duration DROP NOT NULL;
ALTER TABLE expositions ALTER COLUMN total_navstevnost DROP NOT NULL;

-- exposition_exemplar_zone
ALTER TABLE exposition_exemplar_zone ALTER COLUMN exemplar_id DROP NOT NULL;























