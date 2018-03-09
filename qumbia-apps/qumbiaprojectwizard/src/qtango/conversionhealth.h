#ifndef CONVERSIONHEALTH_H
#define CONVERSIONHEALTH_H


class ConversionHealth
{
public:
    ConversionHealth() {
        m_health = 0;
        m_opcnt = 0;
    }

    void healtier() {
        m_health++;
        m_opcnt++;
    }

    void weaker() {
        if(m_health > 0)
            m_health--;
        m_opcnt++;
    }

    float health() const {
        return 100.0 * m_health / (float) m_opcnt;
    }

    enum State { Ok, Warning, Critical};

private:
    int m_health;
    int m_opcnt;
};

#endif // CONVERSIONHEALTH_H
