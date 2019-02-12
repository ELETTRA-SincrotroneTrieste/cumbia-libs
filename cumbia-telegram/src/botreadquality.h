#ifndef BOTREADQUALITY_H
#define BOTREADQUALITY_H


class BotReadQuality
{
public:
    enum Quality { Invalid = -2, ReadError = -1, Valid, Warning, Alarm, Changing, Undefined };

    BotReadQuality();

    BotReadQuality(const BotReadQuality& other);

    void fromTango(bool err, int tango_quality);

    void fromEval(bool err, bool eval);

    Quality quality() const;

    bool operator == (const BotReadQuality& other) const;

    bool operator != (const BotReadQuality &other) const;

private:
    Quality m_q;
};

#endif // BOTREADQUALITY_H
