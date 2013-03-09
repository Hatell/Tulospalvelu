-- tulospalvelu.sql
-- vi: et sw=2 fileencoding=utf8
--
-- SQLite tulospalvelu kuvaus


DROP TABLE IF EXISTS luettu_emit_rasti;
DROP TABLE IF EXISTS luettu_emit;

DROP TABLE IF EXISTS rasti;

DROP TABLE IF EXISTS valiaika;
DROP TABLE IF EXISTS tulos;

DROP TABLE IF EXISTS tulos_tila;
DROP TABLE IF EXISTS sarja;
DROP TABLE IF EXISTS emit;
DROP TABLE IF EXISTS kilpailija;
DROP TABLE IF EXISTS tapahtuma;


CREATE TABLE tapahtuma (
        id INTEGER NOT NULL, -- PRIMARY KEY

        nimi TEXT NOT NULL,

        PRIMARY KEY (id)
);

CREATE TABLE kilpailija (
        id INTEGER NOT NULL, -- PRIMARY KEY

        nimi TEXT NOT NULL,

        PRIMARY KEY (id)
);

CREATE TABLE emit (
        id VARCHAR(5) NOT NULL, -- PRIMARY KEY

        vuosi INTEGER(2) NOT NULL,
        kuukausi INTEGER(2) NOT NULL,

        kilpailija INTEGER,

        FOREIGN KEY (kilpailija) REFERENCES kilpailija(id)
                ON UPDATE CASCADE
                ON DELETE SET NULL,

        PRIMARY KEY (id)
);

CREATE TABLE sarja (
        id INTEGER NOT NULL, -- PRIMARY KEY

        nimi TEXT NOT NULL,

        tapahtuma INTEGER NOT NULL,

        FOREIGN KEY (tapahtuma) REFERENCES tapahtuma(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        PRIMARY KEY (id)
);

CREATE TABLE tulos_tila (
        id INTEGER NOT NULL,

        nimi TEXT NOT NULL,

        PRIMARY KEY (id)
);

CREATE TABLE tulos (
        id INTEGER NOT NULL,


        tapahtuma INTEGER NOT NULL,
        emit VARCHAR(5) NOT NULL,
        kilpailija INTEGER NOT NULL,
        sarja INTEGER NOT NULL,
        tila INTEGER NOT NULL,

        aika TIME NOT NULL,

        maaliaika DATETIME NOT NULL,

        FOREIGN KEY (tapahtuma) REFERENCES tapahtuma(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        FOREIGN KEY (emit) REFERENCES emit(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        FOREIGN KEY (kilpailija) REFERENCES kilpailija(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        FOREIGN KEY (sarja) REFERENCES sarja(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        FOREIGN KEY (tila) REFERENCES tulos_tila(id)
                ON UPDATE CASCADE
                ON DELETE RESTRICT,

        PRIMARY KEY (id)
);

CREATE TABLE valiaika (
        id INTEGER NOT NULL, -- PRIMARY KEY

        tulos INTEGER NOT NULL,

        numero INTEGER NOT NULL,
        koodi INTEGER NOT NULL,
        aika TIME NOT NULL,

        FOREIGN KEY (tulos) REFERENCES tulos(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        PRIMARY KEY (id)
);

CREATE UNIQUE INDEX valiaika_index ON valiaika (tulos, numero, koodi, aika);

CREATE TABLE rasti (
        id INTEGER NOT NULL,

        sarja INTEGER NOT NULL,

        numero INTEGER NOT NULL,
        koodi INTEGER NOT NULL,

        -- Sakkoaika annetaan sekuntteina
        sakkoaika INTEGER NOT NULL DEFAULT -1,

        FOREIGN KEY (sarja) REFERENCES sarja(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        PRIMARY KEY (id)
);

CREATE TABLE luettu_emit (
        id INTEGER NOT NULL, -- PRIMARY KEY

        tapahtuma INTEGER NOT NULL,

        emit VARCHAR(5) NOT NULL,

        luettu DATETIME NOT NULL,

        tulos INTEGER,

        FOREIGN KEY (tapahtuma) REFERENCES tapahtuma(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        FOREIGN KEY (emit) REFERENCES emit(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        FOREIGN KEY (tulos) REFERENCES tulos(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        PRIMARY KEY (id)
);

CREATE TABLE luettu_emit_rasti (
        id INTEGER NOT NULL, -- PRIMARY KEY

        luettu_emit INTEGER NOT NULL,

        numero INTEGER NOT NULL,

        koodi INTEGER NOT NULL,
        aika INTEGER NOT NULL,

        FOREIGN KEY (luettu_emit) REFERENCES luettu_emit(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE,

        PRIMARY KEY (id)
);
