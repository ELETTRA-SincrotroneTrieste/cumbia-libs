#ifndef BOTPLOTGENERATOR_H
#define BOTPLOTGENERATOR_H

#include "volatileoperation.h"
#include <QByteArray>
#include <vector>

class CuData;

class BotPlotGenerator : public VolatileOperation
{
public:
    enum Type { PlotGen = 0x04 };

    BotPlotGenerator(int chat_id, const CuData& data);

    ~BotPlotGenerator();

    QByteArray generate() const;

    // VolatileOperation interface
public:
    void consume(TBotMsgDecoder::Type t);
    int type() const;
    QString name() const;
    void signalTtlExpired();

private:
    std::vector<double> m_data;
    int m_chat_id;
    QString m_source;
};

#endif // BOTPLOTGENERATOR_H
