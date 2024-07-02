#include "tstbase.h"

#include <iostream>

void TstBase::test()
{
    std::cout << "test begin [" << __FILE__ << "]" << std::endl;

#ifndef _MSC_VER
    int val = CJS_CNT1();
#endif

    TPortfolio portfolioObj;
    portfolioObj.name = "Rich中文";
    portfolioObj.id = 12;
    portfolioObj.vecBasetype.push_back(TBaseType());
    portfolioObj.vecBasetype.push_back(TBaseType());
    portfolioObj.vecBasetype.push_back(TBaseType());
    portfolioObj.tree.items.push_back(TTreeItem());
    portfolioObj.tree.items[0].children.push_back(TTreeItem());

    // struct object TO json string
    std::string strJson = portfolioObj.__cjsToJsonString();
    std::cout << strJson << std::endl;

    // json string TO struct object
    portfolioObj.__cjsFromJsonString(strJson);

    std::cout << std::endl;
    strJson = portfolioObj.__cjsToJsonString();
    std::cout << strJson << std::endl;

    // class object TO json string
    CPortfolio clsPortfolioObj;
    std::cout << std::endl;
    strJson = clsPortfolioObj.__cjsToJsonString();
    std::cout << strJson << std::endl;

    std::cout << "test end   [" << __FILE__ << "]\n" << std::endl;
}
