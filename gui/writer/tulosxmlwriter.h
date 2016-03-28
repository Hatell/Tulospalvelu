#ifndef TULOSXMLWRITER_H
#define TULOSXMLWRITER_H

#include <QtCore>

#include <QXmlStreamWriter>

#include "type/sarja.h"
#include "type/tulos.h"
#include "type/valiaika.h"

#include "makrot.h"

class TulosXMLWriter
{
public:
    explicit TulosXMLWriter(QIODevice* device);
    explicit TulosXMLWriter(QString* str);
    explicit TulosXMLWriter(QByteArray* bytearray);

    void writeStartXML();
    void writeEventClass(const QList<Tulos>& tulokset, const Sarja* sarja);
    void writeEndXML();

private:
    QXmlStreamWriter m_stream;

    void writeCompetitor(const Tulos& tulos, const Sarja* sarja);
};

#endif // TULOSXMLWRITER_H
