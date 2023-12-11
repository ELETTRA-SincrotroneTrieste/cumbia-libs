#include "cucontrolsfactorypool_p.h"


CuControlsFactoryPool_P::CuControlsFactoryPool_P() {
    _r.store(1);
}

CuControlsFactoryPool_P::CuControlsFactoryPool_P(const CuControlsFactoryPool_P &other) {
    rmap = other.rmap;
    wmap = other.wmap;
    m_dom_patterns = other.m_dom_patterns;
    re_map = other.re_map;
    default_domain = other.default_domain;
    _r.store(1);
}

CuControlsFactoryPool_P::~CuControlsFactoryPool_P() {
//    printf("\e[1;31mX\e[0m ~CuControlsFactoryPool_P [Private] %p\n", this);
}

//CuControlsFactoryPool_P &CuControlsFactoryPool_P::operator=(const CuControlsFactoryPool_P &other)
//{
//    if(this != &other) {
//        rmap = other.rmap;
//        wmap = other.wmap;
//        m_dom_patterns = other.m_dom_patterns;
//        re_map = other.re_map;
//        default_domain = other.default_domain;
//    }
//    return *this;
//}
