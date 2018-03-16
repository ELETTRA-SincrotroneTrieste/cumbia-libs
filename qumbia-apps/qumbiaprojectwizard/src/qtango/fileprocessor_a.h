#ifndef FILEPROCESSORINTERFACE_H
#define FILEPROCESSORINTERFACE_H

#include <QString>
#include <QList>
#include "definitions.h"

class QString;

/*! \brief quality of the operation
 */
class OpQuality
{
public:
    OpQuality(const QString& ty, const QString& old, const QString& ne,
              const QString& filenam,
              const QString& comm, Quality::Level q, int line) {
        type = ty;
        old_e = old;
        new_e = ne;
        quality = q;
        comment = comm;
        lineno = line;
        file = filenam;
    }

    QString type, old_e, new_e,  comment, file;
    Quality::Level quality;
    int lineno;
};

class FileCmd
{
public:
    FileCmd(const QString& filename) {
        m_filename = filename;
    }

    virtual ~FileCmd() {

    }

    void setDefinitions(const Definitions& d) {
        m_defs = d;
    }

    virtual QString process(const QString& input) = 0;

    /*! \brief returns an identifier for the implementation of FileCmd
     */
    virtual QString name() = 0;

    virtual bool error() {
        return m_err;
    }

    virtual QString errorMessage() {
        return m_msg;
    }

    QList<OpQuality> log() const {
        return m_log;
    }

    QString filename()  const {
        return m_filename;
    }

protected:
    bool m_err;
    QString m_msg;
    QList<OpQuality> m_log;
    Definitions m_defs;

private:
    QString m_filename;


};

/*! Abstract class for file processors
 */
class MacroFileCmd
{
public:
    MacroFileCmd(const QString& contents) {
        m_contents = contents;
        m_err = false;
    }

    virtual ~MacroFileCmd() {
        foreach(FileCmd *cmd, m_cmds)
            delete cmd;
        m_cmds.clear();
    }

    void registerCommand(FileCmd *cmd) {
        m_cmds.append(cmd);
    }

    bool error() const {
        return m_err;
    }

    virtual bool process(const Definitions& defs) {
        if(m_err)
            return !m_err;
        m_msg = "FileProcessorA: no commands";
        foreach(FileCmd *cmd, m_cmds) {
            cmd->setDefinitions(defs);
            printf("\e[1;32m*\e[0m processing %s\n", cmd->name().toStdString().c_str());
            m_contents = cmd->process(m_contents);
            m_err = cmd->error();
            m_log.append(cmd->log());
            if(m_err) {
                m_msg = cmd->errorMessage();
                break;
            }
        }
        if(!m_err)
            m_msg = "";
        return !m_err;
    }

    QString errorMessage() const { return m_msg; }

    QList<OpQuality> log() const {
        return m_log;
    }

    QString contents() const {
        return m_contents;
    }

protected:
    bool m_err;
    QString m_msg;

private:
    QList<OpQuality> m_log;
    QString m_contents;
    QList<FileCmd* > m_cmds;
};

#endif // FILEPROCESSORINTERFACE_H
