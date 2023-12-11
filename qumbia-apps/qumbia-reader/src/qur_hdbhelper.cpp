#include "qur_hdbhelper.h"
#include <QDateTime>
#ifdef HAS_CUHDB
#include <cumbiahdbworld.h>
#include <algorithm>
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
        if(data[CuDType::Err].toBool())  // data["err"]
            da[CuDType::Err] = true;  // da["err"]
        if(da.containsKey("time_scale_us") && data.containsKey("time_scale_us")) {
            da["time_scale_us"].append(data["time_scale_us"]);
        }
        if(da.containsKey("progress") && data.containsKey("progress")) {
            int progress;
            data["progress"].to<int>(progress);
            da["progress"] = progress;
        }
        if(da.containsKey(CuDType::Value) && data.containsKey(CuDType::Value)) {  // da.containsKey("value"), data.containsKey("value")
            da[CuDType::Value].append(data[CuDType::Value]);  // da["value"], data["value"]
        }
        if(da.containsKey(CuDType::WriteValue) && data.containsKey(CuDType::WriteValue)) {  // da.containsKey("w_value"), data.containsKey("w_value")
            da[CuDType::WriteValue].append(data[CuDType::WriteValue]);  // da["w_value"], data["w_value"]
        }
        if(data.containsKey("notes_time_scale_us")) {
            da["notes_time_scale_us"].append(data["notes_time_scale_us"]);
        }
        if(data.containsKey("notes"))
            da["notes"].append(data["notes"]);

        if(data[CuDType::Exit].toBool())  // data["exit"]
            da[CuDType::Exit] = true;  // da["exit"]
    }
}

bool QuR_HdbHelper::isComplete(const QString &name) const
{
    if(mMap.contains(name))
        return mMap[name]["exit"].toBool();
    return false;
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

#define HAS_CUHDB 1

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
        size_t notes_idx = 0;
        std::vector<double> notes_tss = d["notes_time_scale_us"].toDoubleVector();
        std::vector<std::string> notes = d["notes"].toStringVector();
        const CuVariant &v = d[CuDType::Value];  // d["value"]
        std::string d1 = d["start_date"].toString(), d2 = d["stop_date"].toString();
        if(d.containsKey("query")) { // query
            fprintf(fp, "%s%s%s\n", c[Cyan], d.s("query").c_str(), c[Default]);
            const std::vector<std::string> &results = d["results"].toStringVector();
            for(const std::string& r : results)
                fprintf(fp, "%s\n", r.c_str());
            if(results.size() == 0)
                fprintf(fp, "[ no results ]\n");
        }
        else if(d.containsKey("find_pattern")) {
            // search was performed
            fprintf(fp, "search_pattern,%s%s%s\n", c[Blue], d.s("find_pattern").c_str(), c[Default]);
            const std::vector<std::string>& srcs = d["sources"].toStringVector();
            for(const std::string& s : srcs)
                fprintf(fp, "%s\n", s.c_str());
            if(srcs.size() == 0)
                fprintf(fp, "[ 0 results ]\n");
        }
        else if(v.isValid() && d.containsKey("data_type_str")) {
            fprintf(fp, "%s%s%s", c[Cyan], d[CuDType::Src].toString().c_str(), c[Default]);  // d["src"]
            if(on_file) {
                // write data type and write type on file
                fprintf(fp, ",%s,%s,%s,%s,%s\n", d["data_type_str"].toString().c_str(),
                        d["write_mode_str"].toString().c_str(), d[CuDType::DataFormatStr].toString().c_str(),  // d["dfs"]
                        d1.c_str(), d2.c_str());
            }
            else {
                d1 != d2 ? fprintf(fp, " [ %s%s%s --> %s%s%s ]\n", c[Cyan], d1.c_str(), c[Default], c[Cyan], d2.c_str(), c[Default])
                          : fprintf(fp, " [ %s%s%s ]\n", c[Cyan], d1.c_str(), c[Default]);
            }
            std::vector<double> tss = d["time_scale_us"].toDoubleVector();
            size_t rowcnt = tss.size();
            if(v.getSize() % rowcnt == 0) {
                size_t siz = v.getSize() / rowcnt;
                if(d["data_type_str"].toString() == "double") {
                    std::vector<double> dv = v.toDoubleVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        while(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r]) {
                            print_note(fp, notes_tss, notes, notes_idx++, on_file);
                        }
                        fprintf(fp, "%s", qstoc(QDateTime::fromMSecsSinceEpoch(tss[r] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz")));
                        on_file ? fprintf(fp, ",") : fprintf(fp, " ");
                        if(siz > 1 && color) fprintf(fp, "%s{%s ", c[Blue], c[Default]);
                        for(size_t i = 0; i < dv.size() / rowcnt; i++) {
                            fprintf(fp, "%s%f%s", c[Green], dv[r * siz + i], c[Default]);
                            if(i < dv.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && color) fprintf(fp, "%s }%s ", c[Blue], c[Default]);
                        if(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r])
                            print_note(fp, notes_tss, notes, r, on_file);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "int") {
                    // CumbiaHdbWorld::extract_data: long int stored in CuVariant:
                    std::vector<long int> liv = v.toLongIntVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        while(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r]) {
                            print_note(fp, notes_tss, notes, notes_idx++, on_file);
                        }
                        fprintf(fp, "%s", qstoc(QDateTime::fromMSecsSinceEpoch(tss[r] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz")));
                        on_file ? fprintf(fp, ",") : fprintf(fp, " ");
                        if(siz > 1 && color) fprintf(fp, "%s{%s ", c[Blue], c[Default]);
                        for(size_t i = 0; i < liv.size() / rowcnt; i++) {
                            fprintf(fp, "%s%ld%s", c[Green], liv[r * siz + i], c[Default]);
                            if(i < liv.size() / rowcnt - 1) fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        if(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r])
                            print_note(fp, notes_tss, notes, r, on_file);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "uint") {
                    // CumbiaHdbWorld::extract_data: long unsigned int stored in CuVariant:
                    std::vector<unsigned long> luiv = v.toULongIntVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        while(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r]) {
                            print_note(fp, notes_tss, notes, notes_idx++, on_file);
                        }
                        fprintf(fp, "%s", qstoc(QDateTime::fromMSecsSinceEpoch(tss[r] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz")));
                        on_file ? fprintf(fp, ",") : fprintf(fp, " ");
                        if(siz > 1 && color) fprintf(fp, "%s{%s ", c[Blue], c[Default]);
                        for(size_t i = 0; i < luiv.size() / rowcnt; i++) {
                            fprintf(fp, "%s%lu%s", c[Green], luiv[r * siz + i], c[Default]);
                            if(i < luiv.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        if(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r])
                            print_note(fp, notes_tss, notes, r, on_file);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "bool") {
                    std::vector<bool> boov = v.toBoolVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        while(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r]) {
                            print_note(fp, notes_tss, notes, notes_idx++, on_file);
                        }
                        fprintf(fp, "%s", qstoc(QDateTime::fromMSecsSinceEpoch(tss[r] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz")));
                        on_file ? fprintf(fp, ",") : fprintf(fp, " ");
                        if(siz > 1 && color) fprintf(fp, "%s{%s ", c[Blue], c[Default]);
                        for(size_t i = 0; i < boov.size() / rowcnt; i++) {
                            fprintf(fp, "%s%s%s", c[Cyan], boov[r * siz + i] ? "true" : "false", c[Default]);
                            if(i < boov.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        if(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r])
                            print_note(fp, notes_tss, notes, r, on_file);
                        fprintf(fp, "\n");
                    }
                }
                if(d["data_type_str"].toString() == "string") {
                    std::vector<std::string> str_v = v.toStringVector();
                    for(size_t r = 0; r < rowcnt; r++) {
                        while(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r]) {
                            print_note(fp, notes_tss, notes, notes_idx++, on_file);
                        }
                        fprintf(fp, "%s", qstoc(QDateTime::fromMSecsSinceEpoch(tss[r] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz")));
                        on_file ? fprintf(fp, ",") : fprintf(fp, " ");
                        if(siz > 1 && color) fprintf(fp, "%s{%s ", c[Blue], c[Default]);
                        for(size_t i = 0; i < str_v.size() / rowcnt; i++) {
                            fprintf(fp, "%s%s%s", c[Cyan], str_v[r * siz + i].c_str(), c[Default]);
                            if(i < str_v.size() / rowcnt - 1)  fprintf(fp, ", ");
                        }
                        if(siz > 1 && !color) fprintf(fp, "%s}%s ", c[Blue], c[Default]);
                        if(notes_tss.size() > notes_idx && notes_tss[notes_idx] < tss[r])
                            print_note(fp, notes_tss, notes, r, on_file);
                        fprintf(fp, "\n");
                    }
                }
            }
            else {
                perr("QuR_HdbHelper::print_all: inconsistent number of rows [%ld] and data size [%ld]", rowcnt, v.getSize());
            }
        } // v.isValid() && d.containsKey("data_type_str")

        // print notes whose timestamp is after last valid data timestamp
        while(notes_tss.size() > notes_idx) {
            print_note(fp, notes_tss, notes, notes_idx++, on_file);
        }

    } // foreach(const CuData& d, dl)

    if(!out_filenam.isEmpty() && fp != nullptr)
        fclose(fp);

#endif
}

void QuR_HdbHelper::print_note(FILE *fp, const std::vector<double> &notes_ts,
                               const std::vector<std::string> &notes, size_t index, bool on_file)
{
    if(index < notes_ts.size()) {
        const char* ts = QDateTime::fromMSecsSinceEpoch(notes_ts[index] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data();
        on_file ? fprintf(fp, "%s,%s\n", ts, notes[index].c_str()) :
                  fprintf(fp, "%s \e[1;31m%s\e[0m\n", ts, notes[index].c_str());
    }
}
