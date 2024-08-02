/*
 Copyright (c) 2024 neoatomic

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */


//
// Created by zcn on 2024/3/29.
//

#ifndef CJS_CXXJSON_SERIALIZE_H
#define CJS_CXXJSON_SERIALIZE_H

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "cJSON.h"

/**
 * JSON序列化：应用于C++对象
 *
 * 本扩展不是像QJson、cJSON等同类的只生成“键值”信息的又一个C++对象json化工具库，而是基于“键值”库实现的进一步好用。
 * 本扩展的目标是尽可能的简化C++类/结构体的json序列化实现，在使用方式上也能像java中那样容易、便利。
 * 通过在C++类/结构体中声明一个简单的扩展宏，来实现对类/结构体对象的json序列化与反序列化。
 *
 * 本扩展的实现基于c++11、cJSON，所以它也是跨平台的。
 * 使用cJSON，完成了json字符串与“键值”信息的转化；需要注意的是，本扩展内使用的cJSON也已经被扩展了：为了完成json键
 * 与C语言标识符的兼容。这里的扩展cJSON，指的是cJSON源代码上的兼容（而非二进制级别的兼容，没有二进制兼容的原因是我们
 * 对struct cJSON新增加了成员）
 *
 * CJS前缀: c++ JSON Serialize
 *
 *
 * 背景与问题
 * 在C++中，即便于使用cJSON、或者QJson等，一般也只是能从一个json字符串，得到一系列层次化的json键值对。
 * 比如，以jsonStr = "{\"name\":\"Rich\",\"id\":123}"这串代表json格式的字符串为例，使用cJSON库中的
 * cJSON_Parse进行解析后，会得到一个cJSON对象jsonObj，它的成员状态为：
 * jsonObj["name"] == "Rich"
 * jsonObj["id"] == 123
 *
 * 然而，我们其实更需要的是像java中的一些工具库那样（比如阿里的fastjson等），能把jsonStr直接反序列化为我们在业务
 * 层定义的某个类/结构体对象实例；或者把某个类/结构体对象实例，直接序列化为jsonStr字符串。
 * 经常使用C++的同学，可能对java里的序列化/反序列化的使用不熟悉，从而惧怕或者不明就里。然而，一方面基于反射实现的这种便捷，
 * 综合效率上，已经经过了长久验证；另一方面，更明朗的，我们以fastjson为例来说明，其在java里的基本使用如下：
 *
 * package com.neoatomic.example;
 *
 * import com.alibaba.fastjson.*;
 *
 * public class App {
 *     // 假设定义了一个（java里）的类JPerson，对其进行序列化操作:
 *     public static JPerson {
 *         public String name;
 *         public Integer id;
 *     }
 *
 *     public static void main(String[] args) {
 *         JPerson personInfo;
 *         personInfo.name = "Poly";
 *         personInfo.id = 12;
 *         System.out.println(JSON.toJSONString(personInfo)); // 这里输出："{\"name\":\"Poly\",\"id\":12}"
 *
 *         String jsonStr = "{\"name\":\"Rich\",\"id\":123}";
 *         personInfo = JSON.parseObject(jsonStr, JPerson.class);
 *         // 现在，personInfo.name.equals("Rich"); personInfo.id == 123
 *     }
 * }
 *
 * 如上，在java里，由于有反射，所以fastjson能够提供便利的接口toJSONString、parseObject，以方便的对某个对象进行
 * 序列化操作（或者将一串json字符串反序列化以填充这个对象的成员变量值）。
 *
 * 所以，问题来了，如何在C++里也像java一样，便利的对结构体（或者类）对象进行json的序列化操作呢？
 *
 *
 * 方案
 * 由于C++里，目前的标准库仍然没有反射，并且为了让代码在遗留的历史OS上容易的编译，我们经常还是会使用c++11、c++14，
 * 所以，实现这种便利，不应该使用最新的C++标准库，否则仍然会把麻烦留给使用者。
 * 随着时间的推移，10多年前的C++11，也终于有了相对广泛的使用，所以基于c++11、cJSON，本代码实现了如上java中的类似功能：
 * 即对C++里的结构体/类对象，实现对其的json序列化操作（序列化与反序列化）。
 *
 * 然而，由于没有反射，想在C++里“无侵入”的实现、使用这种便捷，由业务层上的简单实现是不可能的；所以，我们采用了以下方法，
 * 来实现这种便利的“扩展”：
 * 在结构体/类里声明一个宏扩展。定义结构体/类后，“顺便的”在其定义内，添加上这个扩展宏（以及宏需要的参数）
 *
 * 相比较以前的实现，本次使用了更简洁的宏扩展方式来达成，所以，仍然无需为这种扩展担忧，因为它的学习曲线几乎是平的。
 * 这也是CV实现在心理上带来的一个积极效果。重点在于看一看下面的这个C++示例：
 *
 * #include <iostream>
 * #include "cxxstruct2json.h"
 *
 * struct TPerson {
 *     std::string name;
 *     int id;
 *
 *     CJS_JSON_SERIALIZE(TPerson, name, id)
 * };
 *
 * int main() {
 *     TPerson personInfo;
 *     personInfo.name = "Rich";
 *     personInfo.id = "123";
 *
 *     std::cout << personInfo.__cjsToJsonString() << std::endl; // 这里输出："{\"name\":\"Rich\",\"id\":123}"
 *
 *     std::string strJson = "{\"name\":\"Poly\",\"id\":12}";
 *     personInfo.__cjsFromJsonString(strJson);
 *     // 现在，personInfo.name == "Poly"; personInfo.id == 12
 *
 *     return 0;
 * }
 *
 * 如上示例，在结构体TPerson中，使用扩展宏，传入结构体名（它主要起一个标识作用，或者想获取类的类型名等信息时，可以使用）、
 * 需要json序列化操作的成员变量名后，即完成了这种扩展。
 * 更多的，这种宏扩展，并不会改变结构体在sizeof时的大小，即即便于你仍然想通过网络直接（将结构体转换为char*）发送结构体对象
 * 实例代表的二进制数据，接收方在接收到时，定义一个没有此扩展宏的同结构体后，用来接收这段二进制流，仍然能原生的直接强制转换为
 * 这个结构体对象实例。（这个在挺多存量C++业务场景是需要的，交互方的系统是什么情况我们其实是不清楚的）
 *
 *
 * 使用注意
 * 一、支持的类型
 * 在几年前的一版实现中，我们使用了冗余的模式，如
 * OLD_CJS_JSON_SERIALIZE(StructName, intFromJsonFuc, numToJsonFunc, x,
 *                                       strFromJsonFuc, strToJsonFunc, y)
 * intToJsonFunc、numToJsonFunc、strFromJsonFuc、strToJsonFunc为一些预置的方法，用它们来正确的对结构体的成员变量x、
 * y等进行序列化操作。
 * 尽管这种冗余，在使用几次后，就会非常的熟练，然而，它也有一些限制，比如，一些编译器，对可变宏参数的数量限制是128个，上面这
 * 种冗余，相当于三分之二的参数不能当成员变量使用，当然实际业务中，我们很少会直接定义一个成员变量数量巨大的结构体，但是，书写上
 * 仍然是有些冗余。另外，不断的改进，（最终向java等的便利靠齐），也是我们继续探索的原因。
 *
 * 所以，为了去掉这种冗余和继续探索，我们需要对结构体/类成员变量使用的常见的基本类型，进行模板实现。你在使用中进行成员变量声明时，
 * 可以使用如下的类型（与cJSON对应）：
 * 1、基础类型。
 * char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long,
 * long long, unsigned long long, float, double, bool
 *
 * 2、基础类型的数组类型。
 * char[]（将其转换为char*字符串，或者使用strcpy进行赋值）
 *
 * 3、std::string类型。
 * 4、std::vector类型。std::vector的元素类型，覆盖了上述1中的基础类型，以及使用了CJS_JSON_SERIALIZE扩展宏的结构体/类。
 * 5、std::map<std::string, T>类型。其中T包含上述1中的基础类型，以及使用了CJS_JSON_SERIALIZE扩展宏的结构体/类。
 * 6、指针。使用了CJS_JSON_SERIALIZE扩展宏的结构体/类指针，目前需要先在外部申请资源后，再进行序列化操作。
 * 7、std::set类型。以[]数组的形式存储于json字符串中，不支持std::set<bool>类型。
 *
 * 二、成员变量的数量
 * 本实现中，我们限定了最多支持63个成员变量的json序列化操作。当然，也可以按照既有的格式，对CJS_CNT1等宏做扩展，以便支持更
 * 多数量的成员变量。
 *
 * 三、杂项
 * 1、__cjsToJsonString的实现上，是不需要依赖于cJSON的：通过循环、递归即可达成结构体/类对象向json字符串的序列化操作。因为本
 * 扩展几乎都是对已有功能的整合和包装，所以在这个实现上，也使用了标准库的to_string等方法，没有像一些同类的其他库使用了自造轮子来
 * 实现。
 * 2、std::map<整型类型, T>类型的未支持。通过模板实现std::map<KeyType, ValueType>以及整型类型的特例化，是可能的，但是，由
 * 于json键是以字符串来表示的，所以本扩展也未实现。
 *
 *
 * 使用步骤
 * 1、在你的工程设置里，使用C++11及以上的标准；
 * 2、在你的源代码里，包含本功能有关的4个源代码文件：
 *     cJSON.h    cJSON.c    cxxJson.h    cxxJson.h
 * 3、在定义结构体/类的.h/.cpp文件开始，#include "cxxJson.h"，以包含本扩展对外的头文件（注意路径）；
 * 4、定义结构体/类，在其中使用宏CJS_JSON_SERIALIZE(<结构体/类名>, 成员变量名1, 成员变量名2, ....)
 * 5、使用结构体/类时，声明一个结构体/类对象obj，使用obj.__cjsFromJsonString(<strJson>)、obj.__cjsToJsonString()实
 * 现结构体/类对象的序列化操作。
 *
 * 简单的实例见方案里的示例。
 *
 * 嵌套的复杂实例，如下：
 * #include <iostream>
 * #include "utils/json/cxxJson.h"
 *
 * struct TBaseType {
 *     char c;
 *     unsigned char uc;
 *     short s;
 *     unsigned short us;
 *     int i;
 *     unsigned int ui;
 *     long l;
 *     unsigned long ul;
 *     long long ll;
 *     unsigned long long ull;
 *     float f;
 *     double d;
 *     bool b;
 *     char chStr[64];
 *     std::string str;
 *
 *     CJS_JSON_SERIALIZE(TBaseType, c, uc, s, us, i, ui,
 *             l, ul, ll, ull, f,
 *             d, b, chStr, str)
 * };
 *
 * struct TVectorType {
 *     std::vector<char> vecC;
 *     std::vector<unsigned char> vecUc;
 *     std::vector<short> vecS;
 *     std::vector<unsigned short> vecUs;
 *     std::vector<int> vecI;
 *     std::vector<unsigned int> vecUi;
 *     std::vector<long> vecL;
 *     std::vector<unsigned long> vecUl;
 *     std::vector<long long> vecLl;
 *     std::vector<unsigned long long> vecUll;
 *     std::vector<float> vecF;
 *     std::vector<double> vecD;
 *     std::vector<bool> vecB;
 *     std::vector<std::string> vecStr;
 *
 *     CJS_JSON_SERIALIZE(TVectorType, vecC, vecUc, vecS, vecUs, vecI,
 *             vecUi, vecL, vecUl, vecLl, vecUll,
 *             vecF, vecD, vecB, vecStr);
 * };
 *
 * struct TStringMapType {
 *     std::map<std::string, char> mapC;
 *     std::map<std::string, unsigned char> mapUc;
 *     std::map<std::string, short> mapS;
 *     std::map<std::string, unsigned short> mapUs;
 *     std::map<std::string, int> mapI;
 *     std::map<std::string, unsigned int> mapUi;
 *     std::map<std::string, long> mapL;
 *     std::map<std::string, unsigned long> mapUl;
 *     std::map<std::string, long long> mapLl;
 *     std::map<std::string, unsigned long long> mapUll;
 *     std::map<std::string, float> mapF;
 *     std::map<std::string, double> mapD;
 *     std::map<std::string, bool> mapB;
 *     std::map<std::string, std::string> mapStr;
 *
 *     CJS_JSON_SERIALIZE(TStringMapType, mapC, mapUc, mapS, mapUs, mapI,
 *             mapUi, mapL, mapUl, mapLl, mapUll,
 *             mapF, mapD, mapB, mapStr)
 * };
 *
 * struct TTreeItem {
 *     long long id;
 *     std::string name;
 *
 *     std::vector<TTreeItem> children;
 *
 *     CJS_JSON_SERIALIZE(TTreeItem, id, name, children)
 * };
 *
 * struct TTree {
 *     long long id;
 *     std::string name;
 *     std::vector<TTreeItem> items;
 *
 *     CJS_JSON_SERIALIZE(TTree, id, name, items)
 * };
 *
 * struct TPortfolio{
 *     int id;
 *     std::string name;
 *     TBaseType baseType;
 *     std::vector<TBaseType> vecBasetype;
 *     std::map<std::string, TBaseType> mapBasetype;
 *     TTree tree;
 *     std::map<std::string, TTree> mapTree;
 *     TTree *pTree;
 *
 *     TPortfolio() {
 *         pTree = new TTree();
 *     }
 *
 *     ~TPortfolio() {
 *         delete pTree;
 *     }
 *
 *     CJS_JSON_SERIALIZE(TPortfolio, id, name, baseType, vecBasetype, mapBasetype,
 *             tree, mapTree, pTree)
 * };
 *
 * class CPortfolio {
 *     int id;
 *     std::string name;
 *     TBaseType baseType;
 *     std::vector<TBaseType> vecBasetype;
 *     std::map<std::string, TBaseType> mapBasetype;
 *     TTree tree;
 *     std::map<std::string, TTree> mapTree;
 *     TTree *pTree;
 *
 * public:
 *     CPortfolio() {
 *         pTree = new TTree();
 *     }
 *
 *     ~CPortfolio() {
 *         delete pTree;
 *     }
 *
 *     CJS_JSON_SERIALIZE(CPortfolio, id, name, baseType, vecBasetype, mapBasetype,
 *             tree, mapTree, pTree)
 * };
 *
 * int main() {
 *     TPortfolio portfolioObj;
 *     portfolioObj.name = "Rich";
 *     portfolioObj.id = 12;
 *     portfolioObj.vecBasetype.push_back(TBaseType());
 *     portfolioObj.vecBasetype.push_back(TBaseType());
 *     portfolioObj.vecBasetype.push_back(TBaseType());
 *     portfolioObj.tree.items.push_back(TTreeItem());
 *     portfolioObj.tree.items[0].children.push_back(TTreeItem());
 *
 *     // struct object TO json string
 *     std::string strJson = portfolioObj.__cjsToJsonString();
 *     std::cout << strJson << std::endl;
 *
 *     // json string TO struct object
 *     portfolioObj.__cjsFromJsonString(strJson);
 *
 *     std::cout << std::endl;
 *     strJson = portfolioObj.__cjsToJsonString();
 *     std::cout << strJson << std::endl;
 *
 *     // class object TO json string
 *     CPortfolio clsPortfolioObj;
 *     std::cout << std::endl;
 *     strJson = clsPortfolioObj.__cjsToJsonString();
 *     std::cout << strJson << std::endl;
 *
 *     return 0;
 * }
 *
 *
 * 局限性
 * 本组件对业务应用结构体/类提供的扩展宏CJS_JSON_SERIALIZE，主要是为了对应cJSON中的若干类型，更加方便的在开发时使用json序列
 * 化；所以除了扩展cJSON（或者QJson）等键值化json功能组件，其余有挺多局限性的（C++的魔幻语法太多了），不限于以下：
 * 1、基础类型的数组。对int、double之类的基础类型，由于与const T&、const T*等模板冲突，目前仍然只能使用std::vector<int>，
 * 或者std::vector<double>等对应std::vector类型来实现。
 * 2、多维数组。本扩展没有考虑多维数组的实现，（因为一维数组也没有实现好），所以，使用时需要折衷。从过去业务经验来看，多维数组未
 * 曾被使用。
 * 3、最多支持127个成员变量的json序列化操作。这个容易扩展。
 * 4、不支持位结构。尽管通过一定的改造能够实现，但目前未实现，好在目前需要json的场景里（我们主要是对应cJSON的几种类型），极少使
 * 用位结构（在spring应用里有吗？）
 * 5、不支持静态成员变量。
 * 6、派生类的不确定。扩展的初衷是作用于配置或者用于数据交互的结构体/类对象上，所以派生类的支持，需要单独的做工作。
 * 7、msvc编译器未实现参数数量为0的情况。
 *
 *
 * 未来
 * 1、局限性中的问题，尝试解决？（或许是模板匹配问题）
 * 2、更多数量的成员变量支持。这个是容易的。
 * 3、c99中宏扩展的实现。这个是繁琐的，由于C语言里没有重载、成员方法等，所以，需要做更多的工作，且扩展宏的形式可能都是不一样的。
 * 4、动态绑定。尽管派生类的支持可能需要做额外的工作，然而，如果想支持动态绑定，则需要在数据协议上做约定，同时还需要集成动态绑定的
 * 实现，这个灵活度非常的高。
 * 5、替换cJSON。cJSON其中的一些方法采用了简单的实现，所以如果确定需要追求更高的效率（但又不想失去便利），可以使用其他的同类库
 * 来替换。然而，需要注意的是，此处提的“效率”，对大多数场景来说，都是不需要的，（抛开剂量漫谈毒性的都是耍流氓），单一的极致效率
 * 通常也不会用这种扩展（我们针对的是综合考虑）。
 *
 *
 * 日志
 * 2024.08.02
 * 新增
 * 添加了对std::set的支持，但是对应的在json字符串里是以[]数组的形式来表示值的，这确实是个怪异的行为，这是因为{}表示的Object，必需要有
 * 键，然而，我们不想给set类型的造个键。
 *
 *
 * 2024.07.02
 * 修改
 * 1、将文件名从cxxJsonSerialize.h/cpp，修改成为cxxJson.h/cpp。注意，很抱歉，这种修改是不兼容的，如果你在过去已经使用了
 * cxxJsonSerialize.h/cpp，那么现在推荐换成cxxJson.h/cpp。
 * 2、对README.md的开始做了说明，用于明确的区别本扩展与“键值”类json工具库的区别。
 * 3、将支持32个成员变量，扩展为63个成员变量。
 * 4、将main.cpp中的测试代码，移动到新建立的test文件夹内。
 * 5、对QT的pro、cmake的CMakeLists.txt文件，做了同步的配置修改。
 *
 * 新增
 * 1、对cJSON.h/c进行了（兼容）新增扩展：扩展了struct cJSON结构体，添加了context成员，以便为键值化过程添加必要的控制：对本扩展，
 * 添加了将json键规则化为C语言标识符（[a-zA-Z0-9_]）的开关；添加了cJSON_ParseEx方法，以对应规则化控制；在cJSON.c里添加了
 * cJSON_ReplaceNonAlphanumericWithUnderscore、cJSON_RegulateKeyForC、cJSON_CopyContext等内容使用的辅助方法；兼容式的修改了
 * parse_object等内部使用的方法。
 * 2、新增了test文件夹，以添加测试代码。
 *
 *
 * 2024.04.03
 * 新增
 * 基于cJSON，实现了本扩展的第一个版本。
 *
 *
 * scribble
 * 清明假期要到了（2024.04.03）：
 * 清明时节，乍暖轻寒。
 * 南去北往，日行千里，
 * 只为共思故人颜。
 *
 */

/**
  * 使用namespace：cjs
  */
#define USING_NS_CJS 1
#ifdef USING_NS_CJS
#define NSCJS cjs::
#else
#define NSCJS
#endif

/**
  * gcc编译默认隐藏内部的重载方法
  */
#ifndef CJS_INF_HIDDEN
#if __GNUC__ >= 4
#define CJS_INF_HIDDEN  __attribute__((visibility("hidden")))
#else
#define CJS_INF_HIDDEN /* nothing */
#endif
#endif


#ifdef USING_NS_CJS
namespace cjs {
#endif

    /**
     * 以下的定义，是实现对象向json字符串转换的功能
     */


    CJS_INF_HIDDEN extern std::string __cjsToJsonString();

/**
 * 模板化：将char数组类型转换为json字符串
 * @tparam T
 * @param obj
 * @return
 */
    template <std::size_t N>
    CJS_INF_HIDDEN std::string __cjsToJsonString(const char (&obj)[N]) {
        return std::string("\"") + std::string(obj) + std::string("\"");
    }



/**
 * 模板化：将元素为某种类型的std::vector转换为json字符串
 * @tparam T
 * @param obj
 * @return
 */
template <typename T>
CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<T>& obj) {
    std::string strRet = "[";
    std::string strSep = "";
    for (int i = 0; i < obj.size(); ++i) {
        strRet += strSep;
        strRet += obj[i].__cjsToJsonString();
        strSep = ",";
    }
    strRet += "]";
    return strRet;
}

/**
 * 特例化：将char型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(char obj);

/**
 * 特例化：将unsigned char型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(unsigned char obj);

/**
 * 特例化：将short型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(short obj);

/**
 * 特例化：将unsigned short型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(unsigned short obj);

/**
 * 特例化：将int型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(int obj);

/**
 * 特例化：将unsigned int型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(unsigned int obj);

/**
 * 特例化：将long型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(long obj);

/**
 * 特例化：将unsigned long型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(unsigned long obj);

/**
 * 特例化：将long long型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(long long obj);

/**
 * 特例化：将unsigned long long型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(unsigned long long obj);

/**
 * 特例化：将float型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(float obj);

/**
 * 特例化：将double型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(double obj);

/**
 * 特例化：将bool型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(bool obj);

/**
 * 特例化：将char*型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const char* obj);

/**
 * 特例化：将std::string型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::string& obj);

CJS_INF_HIDDEN extern std::string __cjsToJsonString(std::string& obj);

/**
 * 特例化：将std::vector<char>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<char>& obj);

/**
 * 特例化：将std::vector<unsigned char>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<unsigned char>& obj);

/**
 * 特例化：将std::vector<short>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<short>& obj);

/**
 * 特例化：将std::vector<unsigned short>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<unsigned short>& obj);

/**
 * 特例化：将std::vector<int>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<int>& obj);

/**
 * 特例化：将std::vector<unsigned int>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<unsigned int>& obj);

/**
 * 特例化：将std::vector<long>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<long>& obj);

/**
 * 特例化：将std::vector<unsigned long>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<unsigned long>& obj);

/**
 * 特例化：将std::vector<long long>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<long long>& obj);

/**
 * 特例化：将std::vector<unsigned long long>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<unsigned long long>& obj);

/**
 * 特例化：将std::vector<float>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<float>& obj);

/**
 * 特例化：将std::vector<double>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<double>& obj);

/**
 * 特例化：将std::vector<bool>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<bool>& obj);

/**
 * 特例化：将std::vector<std::string>型转换json字符串
 * @param obj
 * @return
 */
CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::vector<std::string>& obj);

/**
 * 模板化：将某种指针类型转换为json字符串
 * @tparam T
 * @param obj
 * @return
 */
    template <typename T>
    CJS_INF_HIDDEN std::string __cjsToJsonString(const T* obj) {
        if (!obj) return "\"\"";
        return obj->__cjsToJsonString();
    }

    template <typename T>
    CJS_INF_HIDDEN std::string __cjsToJsonString( T* obj) {
        if (!obj) return "\"\"";
        return obj->__cjsToJsonString();
    }

/**
 * 模板化：将某种类型转换为json字符串
 * @tparam T
 * @param obj
 * @return
 */
    template <typename T>
    CJS_INF_HIDDEN std::string __cjsToJsonString(const T &obj) {
        return obj.__cjsToJsonString();
    }

    template <typename T>
    CJS_INF_HIDDEN std::string __cjsToJsonString(T &obj) {
        return obj.__cjsToJsonString();
    }


    /**
     * 模板化：将元素为某种类型的std::map转换为json字符串
     * @tparam T
     * @param obj
     * @return
     */
    template <typename T>
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, T>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (typename std::map<std::string, T>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += (*it).second.__cjsToJsonString() + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    /**
     * 模板特例化：将std::map<std::string, char>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, char>& obj);

    /**
     * 模板特例化：将std::map<std::string, unsigned char>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, unsigned char>& obj);

    /**
     * 模板特例化：将std::map<std::string, short>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, short>& obj);

    /**
     * 模板特例化：将std::map<std::string, unsigned short>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, unsigned short>& obj);

    /**
     * 模板特例化：将std::map<std::string, int>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, int>& obj);

    /**
     * 模板特例化：将std::map<std::string, unsigned int>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, unsigned int>& obj);

    /**
     * 模板特例化：将std::map<std::string, long>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, long>& obj);

    /**
     * 模板特例化：将std::map<std::string, unsigned long>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, unsigned long>& obj);

    /**
     * 模板特例化：将std::map<std::string, long long>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, long long>& obj);

    /**
     * 模板特例化：将std::map<std::string, unsigned long long>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, unsigned long long>& obj);

    /**
     * 模板特例化：将std::map<std::string, float>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, float >& obj);

    /**
     * 模板特例化：将std::map<std::string, double>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, double >& obj);

    /**
     * 模板特例化：将std::map<std::string, bool>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, bool >& obj);

    /**
     * 模板特例化：将std::map<std::string, std::string>转换为json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::map<std::string, std::string>& obj);

    /**
     * 模板化：将元素为某种类型的std::set转换为json字符串
     * @tparam T
     * @param obj
     * @return
     */
    template <typename T>
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::set<T>& obj) {
        std::string strRet = "[";
        std::string strSep = "";
        for (const T& item : obj) {
            strRet += strSep;
            strRet += std::string("{\"");
            strRet += item.__cjsToJsonString();
            strRet += std::string("}");
            strSep = ",";
        }
        strRet += "]";
        return strRet;
    }

    /**
     * 特例化：将std::set<char>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<char>& obj);

    /**
     * 特例化：将std::set<unsigned char>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<unsigned char>& obj);

    /**
     * 特例化：将std::set<short>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<short>& obj);

    /**
     * 特例化：将std::set<unsigned short>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<unsigned short>& obj);

    /**
     * 特例化：将std::set<int>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<int>& obj);

    /**
     * 特例化：将std::set<unsigned int>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<unsigned int>& obj);

    /**
     * 特例化：将std::set<long>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<long>& obj);

    /**
     * 特例化：将std::set<unsigned long>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<unsigned long>& obj);

    /**
     * 特例化：将std::set<long long>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<long long>& obj);

    /**
     * 特例化：将std::set<unsigned long long>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<unsigned long long>& obj);

    /**
     * 特例化：将std::set<float>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<float>& obj);

    /**
     * 特例化：将std::set<double>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<double>& obj);

    /**
     * 特例化：将std::set<std::string>型转换json字符串
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern std::string __cjsToJsonString(const std::set<std::string>& obj);

    /**
     * 以下的定义，是实现cJSON对象向C++（结构体）类型对象转换的功能
     */

    /**
     * 特例化：jsonObj对象转换为char型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, char& obj);

    /**
     * 特例化：jsonObj对象转换为unsigned char型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned char& obj);

    /**
     * 特例化：jsonObj对象转换为short型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, short& obj);

    /**
     * 特例化：jsonObj对象转换为unsigned short型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned short& obj);

    /**
     * 特例化：jsonObj对象转换为int型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, int& obj);

    /**
     * 特例化：jsonObj对象转换为unsigned int型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned int& obj);

    /**
     * 特例化：jsonObj对象转换为long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, long& obj);

    /**
     * 特例化：jsonObj对象转换为unsigned long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned long& obj);

    /**
     * 特例化：jsonObj对象转换为long long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, long long& obj);

    /**
     * 特例化：jsonObj对象转换为unsigned long long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned long long& obj);

    /**
     * 特例化：jsonObj对象转换为float型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, float& obj);

    /**
     * 特例化：jsonObj对象转换为double型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, double& obj);

    /**
     * 特例化：jsonObj对象转换为bool型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, bool& obj);

    /**
     * 特例化：jsonObj对象转换为char*型变量（字符串）
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, char* obj);

    /**
     * 特例化：jsonObj对象转换为std::string型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::string& obj);

    /**
     * 模板：jsonObj对象转换为T型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    template <typename T>
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, T& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_Object)
            obj.__cjsFromJsonObject(jsonObjItem);
    }

    /**
     * 模板：jsonObj对象转换为T*型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    template <typename T>
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, T* obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_Object && obj) {
            obj->__cjsFromJsonObject(jsonObjItem);
        }
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<char>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned char>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<short>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned short>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<std::vector<int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<int>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<std::vector<unsigned int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned int>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<long long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned long long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<float>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<float>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<double>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<double>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<bool>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<bool>& obj);

    /**
     * 特例化：jsonObj对象转换为std::vector<std::string>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<std::string>& obj);

    /**
     * 模板化：jsonObj对象转换为std::vector<T>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    template <typename T>
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<T>& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_Array) {
            obj.clear();
            int cnt = cJSON_GetArraySize(jsonObjItem);
            for (int i = 0; i < cnt; ++i) {
                T objItem;
                objItem.__cjsFromJsonObject(cJSON_GetArrayItem(jsonObjItem, i));
                obj.push_back(objItem);
            }
        }
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, char>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned char>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, short>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned short>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, std::map<std::string, int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, int>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, std::map<std::string, unsigned int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned int>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, long long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned long long>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, float>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, float>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, double>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, double>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, bool>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, bool>& obj);

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, std::string>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, std::string>& obj);

    /**
     * 模板化：jsonObj对象转换为std::map<std::string, T>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    template <typename T>
    CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, T>& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_Object) {
            obj.clear();
            cJSON* child = jsonObjItem->child;
            while (child)
            {
                T objItem;
                objItem.__cjsFromJsonObject(child);
                obj[child->string] = objItem;
                child = child->next;
            }
        }
    }

    /**
         * 特例化：jsonObj对象转换为std::set<char>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<char>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<unsigned char>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<unsigned char>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<short>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<short>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<unsigned short>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<unsigned short>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<std::set<int>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<int>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<std::set<unsigned int>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<unsigned int>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<long>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<long>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<unsigned long>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<unsigned long>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<long long>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<long long>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<unsigned long long>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<unsigned long long>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<float>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<float>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<double>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<double>& obj);

        /**
         * 特例化：jsonObj对象转换为std::set<std::string>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<std::string>& obj);

        /**
         * 模板化：jsonObj对象转换为std::set<T>型变量
         * @param jsonObj
         * @param obj
         * @return
         */
        template <typename T>
        CJS_INF_HIDDEN extern void __cjsFromJsonObject(cJSON* jsonObjItem, std::set<T>& obj) {
            if (jsonObjItem && jsonObjItem->type == cJSON_Array) {
                obj.clear();
                int cnt = cJSON_GetArraySize(jsonObjItem);
                for (int i = 0; i < cnt; ++i) {
                    T objItem;
                    objItem.__cjsFromJsonObject(cJSON_GetArrayItem(jsonObjItem, i));
                    obj.insert(objItem);
                }
            }
        }

    CJS_INF_HIDDEN extern void __cjsSetCJsonContext(cJSON* item);


#ifdef USING_NS_CJS
}   // namespace cjs {
#endif

// for msvc compiler compatible
#define CJS_MSVC_EXPAND( x ) x

/**
 * 返回“不定参数”的个数对应的数值，内部使用
 */
#define CJS_CNT1_ARGS(p01, p11, p21, p31, p41, p51, p61, p71, p81, p91, \
                        p101, p111, p121, p131, p141, p151, p161, p171, p181, p191, \
                        p201, p211, p221, p231, p241, p251, p261, p271, p281, p291, \
                        p301, p311, p321, p331, p341, p351, p361, p371, p381, p391, \
                        p401, p411, p421, p431, p441, p451, p461, p471, p481, p491, \
                        p501, p511, p521, p531, p541, p551, p561, p571, p581, p591, \
                        p601, p611, p621, p631, p641, \
                        n, ...) CJS_MSVC_EXPAND(n)

/**
 * 返回“不定参数”的个数对应的数值
 */
#define CJS_CNT1(...) CJS_MSVC_EXPAND(CJS_CNT1_ARGS(0, ##__VA_ARGS__, \
                        64, 63, 62, 61, 60, \
                        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
                        49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
                        39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                        29, 28, 27, 26, 25, 24, 23, \
                        22, 21, 20, 19, 18, 17, 16, 15, 14, 13, \
                        12, 11, 10, 9, 8, 7, 6, 5, 4, 3, \
                        2, 1, 0))


#define CJS_CONCAT_IMPL(a, b) a##b
#define CJS_CONCAT(a, b) CJS_MSVC_EXPAND(CJS_CONCAT_IMPL(a, b))
#define CJS_STRING_IMPL(s) #s
#define CJS_STRING(s) CJS_MSVC_EXPAND(CJS_STRING_IMPL(s))

/**
 * 对不定个数参数宏中的参数，构造一个字符串数组
 * 比如：
 * CJS_MAKE_STRING_ARRAY(x, y, zz)，展开后：
 * const char* memberNameArray[] = { "x", "y", "zz" };
 *
 */
#define CJS_MAKE_STRING0(...) CJS_MSVC_EXPAND(nullptr)
#define CJS_MAKE_STRING1(x) CJS_STRING(x)
#define CJS_MAKE_STRING2(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING1(__VA_ARGS__))
#define CJS_MAKE_STRING3(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING2(__VA_ARGS__))
#define CJS_MAKE_STRING4(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING3(__VA_ARGS__))
#define CJS_MAKE_STRING5(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING4(__VA_ARGS__))
#define CJS_MAKE_STRING6(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING5(__VA_ARGS__))
#define CJS_MAKE_STRING7(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING6(__VA_ARGS__))
#define CJS_MAKE_STRING8(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING7(__VA_ARGS__))
#define CJS_MAKE_STRING9(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING8(__VA_ARGS__))
#define CJS_MAKE_STRING10(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING9(__VA_ARGS__))
#define CJS_MAKE_STRING11(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING10(__VA_ARGS__))
#define CJS_MAKE_STRING12(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING11(__VA_ARGS__))
#define CJS_MAKE_STRING13(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING12(__VA_ARGS__))
#define CJS_MAKE_STRING14(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING13(__VA_ARGS__))
#define CJS_MAKE_STRING15(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING14(__VA_ARGS__))
#define CJS_MAKE_STRING16(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING15(__VA_ARGS__))
#define CJS_MAKE_STRING17(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING16(__VA_ARGS__))
#define CJS_MAKE_STRING18(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING17(__VA_ARGS__))
#define CJS_MAKE_STRING19(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING18(__VA_ARGS__))
#define CJS_MAKE_STRING20(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING19(__VA_ARGS__))
#define CJS_MAKE_STRING21(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING20(__VA_ARGS__))
#define CJS_MAKE_STRING22(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING21(__VA_ARGS__))
#define CJS_MAKE_STRING23(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING22(__VA_ARGS__))
#define CJS_MAKE_STRING24(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING23(__VA_ARGS__))
#define CJS_MAKE_STRING25(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING24(__VA_ARGS__))
#define CJS_MAKE_STRING26(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING25(__VA_ARGS__))
#define CJS_MAKE_STRING27(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING26(__VA_ARGS__))
#define CJS_MAKE_STRING28(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING27(__VA_ARGS__))
#define CJS_MAKE_STRING29(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING28(__VA_ARGS__))
#define CJS_MAKE_STRING30(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING29(__VA_ARGS__))
#define CJS_MAKE_STRING31(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING30(__VA_ARGS__))
#define CJS_MAKE_STRING32(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING31(__VA_ARGS__))
#define CJS_MAKE_STRING33(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING32(__VA_ARGS__))
#define CJS_MAKE_STRING34(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING33(__VA_ARGS__))
#define CJS_MAKE_STRING35(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING34(__VA_ARGS__))
#define CJS_MAKE_STRING36(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING35(__VA_ARGS__))
#define CJS_MAKE_STRING37(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING36(__VA_ARGS__))
#define CJS_MAKE_STRING38(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING37(__VA_ARGS__))
#define CJS_MAKE_STRING39(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING38(__VA_ARGS__))
#define CJS_MAKE_STRING40(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING39(__VA_ARGS__))
#define CJS_MAKE_STRING41(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING40(__VA_ARGS__))
#define CJS_MAKE_STRING42(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING41(__VA_ARGS__))
#define CJS_MAKE_STRING43(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING42(__VA_ARGS__))
#define CJS_MAKE_STRING44(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING43(__VA_ARGS__))
#define CJS_MAKE_STRING45(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING44(__VA_ARGS__))
#define CJS_MAKE_STRING46(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING45(__VA_ARGS__))
#define CJS_MAKE_STRING47(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING46(__VA_ARGS__))
#define CJS_MAKE_STRING48(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING47(__VA_ARGS__))
#define CJS_MAKE_STRING49(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING48(__VA_ARGS__))
#define CJS_MAKE_STRING50(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING49(__VA_ARGS__))
#define CJS_MAKE_STRING51(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING50(__VA_ARGS__))
#define CJS_MAKE_STRING52(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING51(__VA_ARGS__))
#define CJS_MAKE_STRING53(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING52(__VA_ARGS__))
#define CJS_MAKE_STRING54(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING53(__VA_ARGS__))
#define CJS_MAKE_STRING55(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING54(__VA_ARGS__))
#define CJS_MAKE_STRING56(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING55(__VA_ARGS__))
#define CJS_MAKE_STRING57(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING56(__VA_ARGS__))
#define CJS_MAKE_STRING58(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING57(__VA_ARGS__))
#define CJS_MAKE_STRING59(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING58(__VA_ARGS__))
#define CJS_MAKE_STRING60(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING59(__VA_ARGS__))
#define CJS_MAKE_STRING61(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING60(__VA_ARGS__))
#define CJS_MAKE_STRING62(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING61(__VA_ARGS__))
#define CJS_MAKE_STRING63(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING62(__VA_ARGS__))
#define CJS_MAKE_STRING64(x, ...) CJS_STRING(x),CJS_MSVC_EXPAND(CJS_MAKE_STRING63(__VA_ARGS__))

#define CJS_MAKE_STRING_ARRAY(...)  \
        const char* memberNameArray[] = { CJS_MSVC_EXPAND(CJS_CONCAT(CJS_MAKE_STRING, CJS_CNT1(__VA_ARGS__))(__VA_ARGS__)) };

#define CJS_MAKE_STATIC_STRING_ARRAY(...)  \
        static const char* memberNameArray[] = { CJS_MSVC_EXPAND(CJS_CONCAT(CJS_MAKE_STRING, CJS_CNT1(__VA_ARGS__))(__VA_ARGS__)) };


/**
 * 对不定个数参数宏中的参数，顺序的扩展为N条语句，以实现objectToJsonString（对象转换为json字符串）：
 * 比如：
 * CJS_OP_OBJECT_TO_JSON_STRING_WRAPPER(opFunc, x, y, zz)，展开后：
 * opFunc(x); opFunc(y); opFunc(zz);
 *
 */
#define CJS_OP_OBJECT_TO_JSON_STRING0(...)
#define CJS_OP_OBJECT_TO_JSON_STRING(opFunc, x) CJS_MSVC_EXPAND(std::string("\"") + std::string(CJS_STRING(x)) + std::string("\":") + opFunc(x))
#define CJS_OP_OBJECT_TO_JSON_STRING1(opFunc,x) strRet += CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING(opFunc, x))
#define CJS_OP_OBJECT_TO_JSON_STRING2(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING1(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING3(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING2(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING4(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING3(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING5(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING4(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING6(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING5(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING7(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING6(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING8(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING7(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING9(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING8(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING10(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING9(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING11(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING10(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING12(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING11(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING13(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING12(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING14(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING13(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING15(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING14(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING16(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING15(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING17(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING16(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING18(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING17(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING19(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING18(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING20(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING19(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING21(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING20(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING22(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING21(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING23(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING22(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING24(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING23(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING25(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING24(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING26(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING25(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING27(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING26(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING28(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING27(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING29(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING28(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING30(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING29(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING31(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING30(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING32(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING31(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING33(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING32(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING34(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING33(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING35(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING34(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING36(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING35(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING37(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING36(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING38(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING37(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING39(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING38(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING40(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING39(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING41(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING40(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING42(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING41(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING43(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING42(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING44(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING43(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING45(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING44(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING46(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING45(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING47(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING46(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING48(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING47(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING49(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING48(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING50(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING49(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING51(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING50(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING52(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING51(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING53(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING52(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING54(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING53(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING55(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING54(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING56(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING55(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING57(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING56(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING58(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING57(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING59(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING58(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING60(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING59(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING61(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING60(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING62(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING61(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING63(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING62(opFunc,__VA_ARGS__))
#define CJS_OP_OBJECT_TO_JSON_STRING64(opFunc,x, ...) strRet += CJS_OP_OBJECT_TO_JSON_STRING(opFunc,x) + std::string(","); CJS_MSVC_EXPAND(CJS_OP_OBJECT_TO_JSON_STRING63(opFunc,__VA_ARGS__))

#define CJS_OP_OBJECT_TO_JSON_STRING_WRAPPER(opFunc, ...)	\
			std::string strRet = "{";	\
			CJS_MSVC_EXPAND(CJS_CONCAT(CJS_OP_OBJECT_TO_JSON_STRING, CJS_CNT1(__VA_ARGS__))(opFunc, __VA_ARGS__));	\
			strRet += "}";


/**
 * 对不定个数参数宏中的参数，顺序的扩展为N条语句，以实现object from JsonString（从cJSON对象转换为结构体对象）：
 * 比如：
 * CJS_OP_FROM_JSON_OBJECT_WRAPPER(opFunc, x, y, zz)，展开后：
 * jsonObjItem = cJSON_GetObjectItem(jsonObj, "x"); opFunc(jsonObjItem, x);
 * jsonObjItem = cJSON_GetObjectItem(jsonObj, "y"); opFunc(jsonObjItem, y);
 * jsonObjItem = cJSON_GetObjectItem(jsonObj, "zz"); opFunc(jsonObjItem, zz);
 *
 */
#define CJS_OP_FROM_JSON_OBJECT0(...)
#define CJS_OP_FROM_JSON_OBJECT(opFunc, x) CJS_MSVC_EXPAND(jsonObjItem = cJSON_GetObjectItem(jsonObj, CJS_STRING(x)); opFunc(jsonObjItem, x))
#define CJS_OP_FROM_JSON_OBJECT1(opFunc,x) CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT(opFunc, x))
#define CJS_OP_FROM_JSON_OBJECT2(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT1(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT3(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT2(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT4(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT3(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT5(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT4(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT6(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT5(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT7(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT6(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT8(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT7(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT9(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT8(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT10(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT9(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT11(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT10(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT12(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT11(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT13(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT12(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT14(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT13(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT15(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT14(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT16(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT15(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT17(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT16(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT18(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT17(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT19(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT18(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT20(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT19(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT21(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT20(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT22(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT21(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT23(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT22(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT24(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT23(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT25(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT24(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT26(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT25(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT27(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT26(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT28(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT27(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT29(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT28(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT30(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT29(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT31(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT30(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT32(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT31(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT33(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT32(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT34(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT33(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT35(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT34(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT36(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT35(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT37(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT36(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT38(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT37(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT39(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT38(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT40(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT39(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT41(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT40(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT42(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT41(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT43(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT42(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT44(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT43(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT45(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT44(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT46(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT45(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT47(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT46(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT48(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT47(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT49(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT48(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT50(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT49(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT51(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT50(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT52(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT51(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT53(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT52(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT54(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT53(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT55(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT54(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT56(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT55(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT57(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT56(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT58(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT57(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT59(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT58(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT60(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT59(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT61(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT60(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT62(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT61(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT63(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT62(opFunc,__VA_ARGS__))
#define CJS_OP_FROM_JSON_OBJECT64(opFunc,x, ...) CJS_OP_FROM_JSON_OBJECT(opFunc,x); CJS_MSVC_EXPAND(CJS_OP_FROM_JSON_OBJECT63(opFunc,__VA_ARGS__))

#define CJS_OP_FROM_JSON_OBJECT_WRAPPER(opFunc, ...)	\
			CJS_MSVC_EXPAND(CJS_CONCAT(CJS_OP_FROM_JSON_OBJECT, CJS_CNT1(__VA_ARGS__))(opFunc, __VA_ARGS__));



/**
 * 对外的宏：CJS_JSON_SERIALIZE
 *
 * 在一个C++结构体的定义里，使用CJS_JSON_SERIALIZE进行扩展声明，传入当前结构体的定义名称，以及成员
 * 比如，定义一个TPeron结构体：
 * struct TPerson {
 *     std::string name;
 *     int id;
 *
 *     CJS_JSON_SERIALIZE(TPerson, name, id)
 * };
 * 然后，就可以在其他需要使用TPerson obj的地方，使用obj.__cjsFromJsonString(), obj.__cjsToJsonString进行序列化操
 * 了。
 */

#define CJS_JSON_SERIALIZE(StructName, ...)  \
        public: \
        const char* __cjsRealTypeName() const {  \
            return typeid(StructName).name();    \
        }   \
        void __cjsFromJsonString(const std::string& strJson) {   \
            cJSON* jsonObj = cJSON_ParseEx(strJson.c_str(), cjs::__cjsSetCJsonContext);  \
            this->__cjsFromJsonObject(jsonObj); \
            cJSON_Delete(jsonObj);  \
        }   \
        std::string __cjsToJsonString() const {   \
            CJS_OP_OBJECT_TO_JSON_STRING_WRAPPER(CJS_MSVC_EXPAND(NSCJS)__cjsToJsonString, __VA_ARGS__)   \
            return strRet;   \
        }   \
        void __cjsFromJsonObject(cJSON* jsonObj) {   \
            cJSON* jsonObjItem = nullptr;   \
            CJS_OP_FROM_JSON_OBJECT_WRAPPER(CJS_MSVC_EXPAND(NSCJS)__cjsFromJsonObject, __VA_ARGS__);    \
        }   \
        cJSON* __cjsToJsonObject() const {   \
            return cJSON_ParseEx(this->__cjsToJsonString().c_str(), cjs::__cjsSetCJsonContext);   \
        }   \
        const char** __cjsGetJsonMembersName(int& cnt) const {   \
            CJS_MAKE_STATIC_STRING_ARRAY(__VA_ARGS__) \
            cnt = CJS_CNT1(__VA_ARGS__); \
            return memberNameArray; \
        }   \
        int __cjsCountOfJsonMembers() {  \
            return CJS_CNT1(__VA_ARGS__); \
        }

#endif //CJS_CXXJSON_SERIALIZE_H
