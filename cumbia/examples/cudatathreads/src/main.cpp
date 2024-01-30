#include <thread>
#include <cudata.h>

#define VERSION "1.0"

class Ob {
public:
    Ob(const std::string& nam) : onam(nam) {}
    ~Ob() {
        cuprintf("[0x%lx] \e[1;31m~Ob %p\e[0m \"%s\" : \"%s\"\n",
               pthread_self(), this, onam.c_str(), msg.c_str());
    }

    Ob(const Ob&other) {
        cuprintf("[0x%lx] Ob %p [\e[1;32mcopy constructor from %p [%s]\e[0m]\n",
               pthread_self(), this, &other, other.onam.c_str());
        onam = other.onam;
        msg = other.msg;
    }
    Ob(const Ob &&other) {
        cuprintf("[0x%lx] Ob %p [\e[1;36mmove constructor from %p [%s]\e[0m]\n",
               pthread_self(), this, &other, other.onam.c_str());
        onam = std::move(other.onam);
        msg = std::move(other.msg);
    }

    std::string msg;
    std::string onam;
};

int f1(const CuData& rd, CuData cd,
       std::vector<CuData> vv,
       const std::vector<CuData> &vref,
       Ob ob,
       const Ob& obref,
       const std::string& nam, int cy, int idx) {

    cd["idx"] = idx;
    CuData cp2(cd);
    cp2[CuDType::Name] = cd.s(CuDType::Name) + "_copy";  // cp2["name"], cd.s("name")
    cp2["idx"] = 10 * idx;
    std::vector<double> vd;
    vd.reserve(100);
    for(int i = 0; i < 100; i++)
        vd.push_back(10 * idx + i);
    cp2["vd"] = vd;
    CuData cp4;
    {
        CuData cp3(cp2);
        cp3["vd"] = vd;
        cp4 = cp3;
    }
    CuData cp5;
    {
        CuData cp6(rd);
        cp5 = cp6;
    }
    cp5[CuDType::Name] = "cp5";  // cp5["name"]
    vd.clear();
    vd.reserve(100);
    for(int i = 0; i < 100; i++)
        vd.push_back(10 * idx + 2*i);
    cp4["vd"] = vd;
    cp4[CuDType::Name] = "cp4";  // cp4["name"]

    std::vector <CuData> vrefcopy = vref;
    for(int k = 0; k < 100; k++)
        vv[k]["j"] = vv[k]["j"].toInt() * 10;


    //cuprintf("\n\n");
    ob.msg = "| in thread " + std::to_string(cy) + "." + std::to_string(idx) + " | ";
    cuprintf("\n[0x%lx] f1: test object [%p] \"%s\" msg \"%s\"\n",
           pthread_self(), &ob, ob.onam.c_str(), ob.msg.c_str());
    cuprintf("\n[0x%lx] f1: test \e[1;35mreference\e[0m object [%p] \"%s\" msg \"%s\"\n",
           pthread_self(), &obref, obref.onam.c_str(), obref.msg.c_str());

    return cy;
}

int main(int argc, char *argv[])
{
    if(argc < 3) {
        cuprintf("usage: %s NUM_EXT_CYCLES   NUM_INT_CYCLES\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[2]);
    int cy = 0, cycnt = atoi(argv[1]);
    while(cy < cycnt) {
        std::vector<std::thread *> threads;
        threads.resize(n);
        for(int i = 0; i < n; i++) {
            CuData da1(CuDType::Name, "da1." + std::to_string(cy) + "." + std::to_string(i));  // ("name", "da1." + std::to_string(cy)
            da1.set(CuDType::Mode, "const ref").set("cycle", cy);  // ("mode", "const ref")
            CuData da2(CuDType::Name, "da2."  + std::to_string(cy) + "." + std::to_string(i));  // ("name", "da2."  + std::to_string(cy)
            da2.set(CuDType::Mode, "copy").set("cycle", cy);  // ("mode", "copy")
            std::vector<CuData> vv(100), vref(100);
            for(int j=0; j< 100; j++) {
                vv[j]["j"] = j;
                vref[j]["j"] = j;
            }

            Ob o("o:" + std::to_string(cy) + "." + std::to_string(i));
            o.msg = ("object will be passed by copy");
            Ob oref("oref:" + std::to_string(cy) + "." + std::to_string(i));
            oref.msg = ("oref will be passed by reference");



            threads[i] = new std::thread(f1, da1, da2, vv, vref,
                                         o, oref,
                                         std::string("th ") + std::to_string(i),
                                         cy, i);
            o.msg += " - after thread call";
            oref.msg += " -after thread call";

            cuprintf("[0x%lx] \e[1;34mmain thread\e[0m: o %p %s %s | oref %p %s %s\n",
                   pthread_self(), &o, o.onam.c_str(), o.msg.c_str(),
                   &oref, oref.onam.c_str(), oref.msg.c_str());
        }
        for(int i = 0; i < n; i++) {
            threads[i]->join();
            delete threads[i];
        }

        cy++;
    }


    return 0;
}
