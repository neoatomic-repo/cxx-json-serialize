#ifndef TSTBASE_H
#define TSTBASE_H

#include "../../utils/json/cxxJson.h"

/**
 * @brief The TstBase class
 * 使用cxxJson的基础示例
 */

struct TBaseType {
    char c;
    unsigned char uc;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    long long ll;
    unsigned long long ull;
    float f;
    double d;
    bool b;
    char chStr[64];
    std::string str;

    CJS_JSON_SERIALIZE(TBaseType, c, uc, s, us, i,
                       ui, l, ul, ll, ull,
                       f, d, b, chStr, str)
};

struct TVectorType {
    std::vector<char> vecC;
    std::vector<unsigned char> vecUc;
    std::vector<short> vecS;
    std::vector<unsigned short> vecUs;
    std::vector<int> vecI;
    std::vector<unsigned int> vecUi;
    std::vector<long> vecL;
    std::vector<unsigned long> vecUl;
    std::vector<long long> vecLl;
    std::vector<unsigned long long> vecUll;
    std::vector<float> vecF;
    std::vector<double> vecD;
    std::vector<bool> vecB;
    std::vector<std::string> vecStr;

    CJS_JSON_SERIALIZE(TVectorType, vecC, vecUc, vecS, vecUs, vecI,
            vecUi, vecL, vecUl, vecLl, vecUll,
            vecF, vecD, vecB, vecStr)
};

struct TStringMapType {
    std::map<std::string, char> mapC;
    std::map<std::string, unsigned char> mapUc;
    std::map<std::string, short> mapS;
    std::map<std::string, unsigned short> mapUs;
    std::map<std::string, int> mapI;
    std::map<std::string, unsigned int> mapUi;
    std::map<std::string, long> mapL;
    std::map<std::string, unsigned long> mapUl;
    std::map<std::string, long long> mapLl;
    std::map<std::string, unsigned long long> mapUll;
    std::map<std::string, float> mapF;
    std::map<std::string, double> mapD;
    std::map<std::string, bool> mapB;
    std::map<std::string, std::string> mapStr;

    CJS_JSON_SERIALIZE(TStringMapType, mapC, mapUc, mapS, mapUs, mapI,
            mapUi, mapL, mapUl, mapLl, mapUll,
            mapF, mapD, mapB, mapStr)
};

struct TTreeItem {
    long long id;
    std::string name;

    std::vector<TTreeItem> children;

    CJS_JSON_SERIALIZE(TTreeItem, id, name, children)
};

struct TTree {
    long long id;
    std::string name;
    std::vector<TTreeItem> items;

    CJS_JSON_SERIALIZE(TTree, id, name, items)
};

struct TPortfolio{
    int id;
    std::string name;
    TBaseType baseType;
    std::vector<TBaseType> vecBasetype;
    std::map<std::string, TBaseType> mapBasetype;
    TTree tree;
    std::map<std::string, TTree> mapTree;
    TTree *pTree;
    std::set<int> setType;

    TPortfolio() {
        pTree = new TTree();
    }

    ~TPortfolio() {
        delete pTree;
    }

    CJS_JSON_SERIALIZE(TPortfolio, id, name, baseType, vecBasetype, mapBasetype,
            tree, mapTree, pTree, setType)
};

class CPortfolio {
    int id;
    std::string name;
    TBaseType baseType;
    std::vector<TBaseType> vecBasetype;
    std::map<std::string, TBaseType> mapBasetype;
    TTree tree;
    std::map<std::string, TTree> mapTree;
    TTree *pTree;

public:
    CPortfolio() {
        pTree = new TTree();
    }

    ~CPortfolio() {
        delete pTree;
    }

    CJS_JSON_SERIALIZE(CPortfolio, id, name, baseType, vecBasetype, mapBasetype,
            tree, mapTree, pTree)
};

/**
 * msvc编译器，目前没搞定变量为0的情况
 */
#ifndef _MSC_VER

struct TZero
{
    CJS_JSON_SERIALIZE(TZero)
};

#endif

class TstBase
{
public:
    static void test();
};

#endif // TSTBASE_H
