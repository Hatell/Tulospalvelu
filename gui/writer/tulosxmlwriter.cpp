#include "tulosxmlwriter.h"

TulosXMLWriter::TulosXMLWriter(QIODevice *device) :
    m_stream(device)
{
}

TulosXMLWriter::TulosXMLWriter(QString *str) :
    m_stream(str)
{
}

TulosXMLWriter::TulosXMLWriter(QByteArray *bytearray) :
    m_stream(bytearray)
{
}

void TulosXMLWriter::writeStartXML()
{
    m_stream.setAutoFormatting(true);
    m_stream.setAutoFormattingIndent(2);

    m_stream.setCodec("UTF-8");
    m_stream.writeStartDocument(); // Ei anna coodausta

//    m_stream.writeProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    m_stream.writeProcessingInstruction("xml-stylesheet", "type=\"text/xsl\" href=\"tlsemittyyli.xsl\"");

    m_stream.writeStartElement("Event");

    m_stream.writeTextElement("EventName", Tapahtuma::tapahtuma()->nimi());
}

void TulosXMLWriter::writeEndXML()
{
    m_stream.writeEndElement(); // Event
    m_stream.writeEndDocument();
}

void TulosXMLWriter::writeEventClass(const QList<Tulos> &tulokset, const Sarja *sarja)
{
    m_stream.writeStartElement("EventClass");
    m_stream.writeEmptyElement("EventCode");

    m_stream.writeTextElement("ClassName", sarja->getNimi());

    foreach (Tulos t, tulokset) {
        writeCompetitor(t, sarja);
    }

    m_stream.writeEndElement(); // EventClass
}

void TulosXMLWriter::writeCompetitor(const Tulos &tulos, const Sarja *sarja)
{
    m_stream.writeStartElement("Competitor");

    m_stream.writeTextElement("StartNumber", QString::number(tulos.m_id));
    m_stream.writeEmptyElement("EventCode");
    m_stream.writeTextElement("ClassName", sarja->getNimi());
    m_stream.writeTextElement("Order", QString::number(tulos.m_sija));
    m_stream.writeTextElement("Name", tulos.m_kilpailija);
    m_stream.writeTextElement("Emit", tulos.m_emit);
    m_stream.writeEmptyElement("ClubID");
    m_stream.writeEmptyElement("ClubName");
    if (tulos.m_tila == Tulos::Hyvaksytty) {
        m_stream.writeTextElement("StartTime", tulos.m_maaliaika.time().addSecs(tulos.m_aika.secsTo(QTime(0, 0))).toString("HH:mm:ss"));
        m_stream.writeTextElement("STSecs", QString::number(-tulos.m_maaliaika.time().addSecs(tulos.m_aika.secsTo(QTime(0, 0))).secsTo(QTime(0, 0))));
        m_stream.writeTextElement("Rank", QString::number(tulos.m_sija));
        m_stream.writeTextElement("Time", tulos.m_aika.toString("HH:mm:ss"));
        m_stream.writeTextElement("TSecs", QString::number(-tulos.m_aika.secsTo(QTime(0, 0))));
        m_stream.writeEmptyElement("Status");
    } else {
        m_stream.writeTextElement("Status", "DQ");
    }

    m_stream.writeStartElement("SplitTimes");

    foreach (Rasti r, sarja->getRastit()) {
        Valiaika v(QVariant(), 0, 0, QTime(), 0);
        bool found = false;

        foreach (v, tulos.m_valiajat) {
            if (r.sisaltaa(v.m_koodi)) {
                found = true;
                break;
            }
        }

        if (!found) {
            continue;
        }

        m_stream.writeStartElement("Control");

        m_stream.writeTextElement("ControlOrder", QString::number(r.getNumero()));
        if (sarja->getMaalirasti().sisaltaa(v.m_koodi)) {
            m_stream.writeTextElement("CCode", "200");
        } else {
            m_stream.writeTextElement("CCode", QString::number(r.getKoodi()));
        }
        m_stream.writeTextElement("ControlTime", v.m_aika.toString("HH:mm:ss"));
        m_stream.writeTextElement("CTSecs", QString::number(-v.m_aika.secsTo(QTime(0, 0))));

        m_stream.writeEndElement(); // Control
    }

    m_stream.writeEndElement(); // SplitTimes
    m_stream.writeEndElement(); // Competitor
}
