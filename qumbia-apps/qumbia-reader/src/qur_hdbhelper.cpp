#include "qur_hdbhelper.h"
#ifdef HAS_CUHDB
#include <cumbiahdbworld.h>
#include <QDateTime>
#endif

QuR_HdbHelper::QuR_HdbHelper()
{

}


void QuR_HdbHelper::mergeResult(const QString &src, const CuData &data)
{
    if(!mMap.contains(src)) {
        mMap[src] = data;
    }
    else {
        CuData &da = mMap[src];
        if(data["err"].toBool())
            da["err"] = true;
        if(da.containsKey("time_scale") && data.containsKey("time_scale")) {
            da["time_scale"].append(data["time_scale"]);
        }
        if(da.containsKey("progress") && data.containsKey("progress")) {
            int progress;
            data["progress"].to<int>(progress);
            da["progress"] = progress;
        }
        if(da.containsKey("value") && data.containsKey("value")) {
            da["value"].append(data["value"]);
        }
        if(da.containsKey("w_value") && data.containsKey("w_value")) {
            da["w_value"].append(data["w_value"]);
        }
    }
}

bool QuR_HdbHelper::isComplete(const QString &name) const
{
    int progress = 0;
    if(mMap.contains(name))
        mMap[name]["progress"].to<int>(progress);
    return progress == 100;
}

bool QuR_HdbHelper::allComplete() const
{
    foreach(QString src, mMap.keys())
        if(!isComplete(src))
            return false;
    return true;
}

CuData QuR_HdbHelper::take(const QString &src)
{
    CuData d;
    if(mMap.contains(src)) {
        d = mMap[src];
        mMap.remove(src);
    }
    return d;
}

QList<CuData> QuR_HdbHelper::takeAll()
{
    QList<CuData> dl;
    dl = mMap.values();
    mMap.clear();
    return dl;
}

void QuR_HdbHelper::print_all(const QList<CuData> & dl, const QString &out_filenam, bool color)
{
#ifdef HAS_CUHDB
    enum Colors { Default = 0, Newline, Green, Cyan, Blue, Yellow, Red, MaxColors = 8 };
    char c[MaxColors][16] { "", "\n", "", "", "", "", "", "" };
    // output on file ?
    FILE *fp;
    out_filenam.isEmpty() ? fp = stdout : fp = fopen(out_filenam.toLatin1().data(), "w");
    bool on_file = out_filenam.size() > 0 && fp;
    if(on_file)
        color = false;

    if(fp == nullptr) {
        perr("QuR_HdbHelper.print_all: failed to open file \"%s\" in write mode: %s", qstoc(out_filenam), strerror(errno));
        return;
    }
    else if(!out_filenam.isEmpty())
        printf("%s*%s output on file \"%s\"\n", c[Green], c[Default], qstoc(out_filenam));
    //
    if(color) {
        strcpy(c[Default], "\e[0m");
        strcpy(c[Newline], "\e[0m\n");
        strcpy(c[Green],   "\e[1;32m");
        strcpy(c[Cyan],     "\e[1;36m");
        strcpy(c[Blue],     "\e[1;34m");
        strcpy(c[Yellow], "\e[1;33m");
        strcpy(c[Red],       "\e[1;31m");
    }

    foreach(const CuData& d, dl) {

//        printf("QuR_HdbHelper.print_all: %s\n", d.toString().c_str());

        const CuVariant &v = d["value"];
        if(v.isValid() && d.containsKey("data_type_str")) {
            fprintf(fp, "%s%s%s", c[Cyan], d["src"].toString().c_str(), c[Default]);
            if(on_file) {
                // write data type and write type on file
                fprintf(fp, ",%s,%s,%s,%s,%s\n", d["data_type_str"].toString().c_str(),
                        d["write_mode_str"].toString().c_str(), d["data_format_str"].toString().c_str(),
                        d["start_date"].toString().c_str(), d["stop_date"].toString().c_str());
            }
            else {
                fprintf(fp, " [ %s%s%s --> %s%s%s ]\n", c[Cyan],
                        d["start_date"].toString().c_str(), c[Default],
                        c[Cyan], d["stop_date"].toString().c_str(), c[Default]);
            }
            std::vector<double> tss = d["time_scale"].toDoubleVector();
            size_t rowcnt = tss.size();
            if(v.getSize() % rowcnt == 0) {
                size_t siz = v.getSize() / rowcnt;
                if(d["data_type_str"].toString() == "double") {
                    std::vector<double> dv = v.toDoubleVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        fprintf(fp, "%s", qstoc(QDateTime::fromSecsSinceEpoch(tss[r]).toString("yyyy-MM-dd hh:mm:ss")));
                        on_file ? fprintf(fp, ",") : fprintf(fp, " ");
                        if(siz > 1 && color) fprintf(fp, "%s{%s ", c[Blue], c[Default]);
                        for(size_t i = 0; i < dv.size() / rowcnt; i++) {
                            fprintf(fp, "%s%f%s", c[Green], dv[r * siz + i], c[Default]);
                            if(i < dv.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && color) fprintf(fp, "%s }%s ", c[Blue], c[Default]);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "int") {
                    // CumbiaHdbWorld::extract_data: long int stored in CuVariant:
                    std::vector<long int> liv = v.toLongIntVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        fprintf(fp, "%s ", qstoc(QDateTime::fromSecsSinceEpoch(tss[r]).toString("yyyy-MM-dd hh:mm:ss")));
                        for(size_t i = 0; i < liv.size() / rowcnt; i++) {
                            fprintf(fp, "%s%ld%s", c[Green], liv[r * siz + i], c[Default]);
                            if(i < liv.size() / rowcnt - 1) fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "uint") {
                    // CumbiaHdbWorld::extract_data: long unsigned int stored in CuVariant:
                    std::vector<unsigned long> luiv = v.toULongIntVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        fprintf(fp, "%s ", qstoc(QDateTime::fromSecsSinceEpoch(tss[r]).toString("yyyy-MM-dd hh:mm:ss")));
                        for(size_t i = 0; i < luiv.size() / rowcnt; i++) {
                            fprintf(fp, "%s%lu%s", c[Green], luiv[r * siz + i], c[Default]);
                            if(i < luiv.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "bool") {
                    std::vector<bool> boov = v.toBoolVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        fprintf(fp, "%s ", qstoc(QDateTime::fromSecsSinceEpoch(tss[r]).toString("yyyy-MM-dd hh:mm:ss")));
                        for(size_t i = 0; i < boov.size() / rowcnt; i++) {
                            fprintf(fp, "%s%s%s", c[Cyan], boov[r * siz + i] ? "true" : "false", c[Default]);
                            if(i < boov.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "string") {
                    std::vector<std::string> str_v = v.toStringVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        fprintf(fp, "%s ", qstoc(QDateTime::fromSecsSinceEpoch(tss[r]).toString("yyyy-MM-dd hh:mm:ss")));
                        for(size_t i = 0; i < str_v.size() / rowcnt; i++) {
                            fprintf(fp, "%s%s%s", c[Cyan], str_v[r * siz + i].c_str(), c[Default]);
                            if(i < str_v.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        fprintf(fp, "\n");
                    }
                }
            }
            else {
                perr("QuR_HdbHelper::print_all: inconsistent number of rows [%ld] and data size [%ld]", rowcnt, v.getSize());
            }
        } // v.isValid() && d.containsKey("data_type_str")

    } // foreach(const CuData& d, dl)

    if(!out_filenam.isEmpty() && fp != nullptr)
        fclose(fp);

#endif
}
