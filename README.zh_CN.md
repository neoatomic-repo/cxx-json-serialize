# JSON序列化：应用于C++对象

本扩展不是像QJson、cJSON等同类的只生成“键值”信息的又一个C++对象json化工具库，而是基于“键值”信息的进一步好用。

本扩展的目标是尽可能的简化C++类/结构体的json序列化实现，在使用方式上也能像java中那样容易、便利。

通过在C++类/结构体中声明一个简单的扩展宏，来实现对类/结构体对象的json序列化与反序列化。使用cJSON，完成了json字符串与“键值”信息的转化；

需要注意的是，本扩展内使用的cJSON也已经被扩展了：为了完成json键与C语言标识符的兼容。这里的扩展cJSON，指的是cJSON源代码上的兼容（而非二进制级别的兼容，没有二进制兼容的原因是我们对struct cJSON新增加了成员）

本扩展同步发文于csdn上，见[C++对象的json序列化扩展](https://blog.csdn.net/smartcore/article/details/137517125)，在其中也有百度网盘的链接。由于网络的断续，若有问题，也可在csdn下向作者吐槽（这里尽量保持最新版本）。

<br>

本扩展的实现基于c++11、cJSON，所以它也是跨平台的。

CJS前缀: c++ JSON Serialize

  
<br>

## 背景与问题

在C++中，即便于使用cJSON、或者QJson等，一般也只是能从一个json字符串，得到一系列层次化的json键值对。  

比如，以jsonStr = "{\"name\":\"Rich\",\"id\":123}"这串代表json格式的字符串为例，使用cJSON库中的cJSON_Parse进行解析后，会得到一个cJSON对象jsonObj，它的成员状态为：  
```c++
jsonObj["name"] == "Rich"
jsonObj["id"] == 123
```  

然而，我们其实更需要的是像java中的一些工具库那样（比如阿里的fastjson等），能把jsonStr直接反序列化为我们在业务层定义的某个类/结构体对象实例；或者把某个类/结构体对象实例，直接序列化为jsonStr字符串。  

经常使用C++的同学，可能对java里的序列化/反序列化的使用不熟悉，从而惧怕或者不明就里。然而，一方面基于反射实现的这种便捷，综合效率上，已经经过了长久验证；另一方面，更明朗的，我们以fastjson为例来说明，其在java里的基本使用如下：  

```java
package com.neoatomic.example;

import com.alibaba.fastjson.*;

public class App {
    // 假设定义了一个（java里）的类JPerson，对其进行序列化操作:
    public static JPerson {
        public String name;
        public Integer id;
    }

    public static void main(String[] args) {
        JPerson personInfo;
        personInfo.name = "Poly";
        personInfo.id = 12;
        System.out.println(JSON.toJSONString(personInfo)); // 这里输出："{\"name\":\"Poly\",\"id\":12}"

        String jsonStr = "{\"name\":\"Rich\",\"id\":123}";
        personInfo = JSON.parseObject(jsonStr, JPerson.class);
        // 现在，personInfo.name.equals("Rich"); personInfo.id == 123
    }
}

```

如上，在java里，由于有反射，所以fastjson能够提供便利的接口toJSONString、parseObject，以方便的对某个对象进行序列化操作（或者将一串json字符串反序列化以填充这个对象的成员变量值）。  

所以，**问题来了，如何在C++里也像java一样，便利的对结构体（或者类）对象进行json的序列化操作呢？**

<br>

## 方案

由于C++里，目前的标准库仍然没有反射，并且为了让代码在遗留的历史OS上容易的编译，我们经常还是会使用c++11、c++14，所以，实现这种便利，不应该使用最新的C++标准库，否则仍然会把麻烦留给使用者。  

随着时间的推移，10多年前的C++11，也终于有了相对广泛的使用，所以基于c++11、cJSON，本代码实现了如上java中的类似功能：即对C++里的结构体/类对象，实现对其的json序列化操作（序列化与反序列化）。  


然而，由于没有反射，想在C++里“无侵入”的实现、使用这种便捷，由业务层上的简单实现是不可能的；所以，**我们采用了以下方法，来实现这种便利的“扩展”：在结构体/类里声明一个宏扩展。定义结构体/类后，“顺便的”在其定义内，添加上这个扩展宏（以及宏需要的参数）**  

相比较以前的实现，本次使用了更简洁的宏扩展方式来达成，所以，仍然无需为这种扩展担忧，因为它的学习曲线几乎是平的。这也是CV实现在心理上带来的一个积极效果。重点在于看一看下面的这个C++示例：  

```c++
#include <iostream>
#include "cxxJson.h"

struct TPerson {
    std::string name;
    int id;

    CJS_JSON_SERIALIZE(TPerson, name, id)
};

int main() {
    TPerson personInfo;
    personInfo.name = "Rich";
    personInfo.id = "123";

    std::cout << personInfo.__cjsToJsonString() << std::endl; // 这里输出："{\"name\":\"Rich\",\"id\":123}"

    std::string strJson = "{\"name\":\"Poly\",\"id\":12}";
    personInfo.__cjsFromJsonString(strJson);
    // 现在，personInfo.name == "Poly"; personInfo.id == 12

    return 0;
}
```

如上示例，在结构体TPerson中，使用扩展宏，传入结构体名（它主要起一个标识作用，或者想获取类的类型名等信息时，可以使用）、需要json序列化操作的成员变量名后，即完成了这种扩展。  

更多的，这种宏扩展，并不会改变结构体在sizeof时的大小，即即便于你仍然想通过网络直接（将结构体转换为char*）发送结构体对象实例代表的二进制数据，接收方在接收到时，定义一个没有此扩展宏的同结构体后，用来接收这段二进制流，仍然能原生的直接强制转换为这个结构体对象实例。（这个在挺多存量C++业务场景是需要的，交互方的系统是什么情况我们其实是不清楚的）  

<br>

## 使用注意

### 一、支持的类型
在几年前的一版实现中，我们使用了冗余的模式，如  
```c++
OLD_CJS_JSON_SERIALIZE(StructName, intFromJsonFuc, numToJsonFunc, x,
                                      strFromJsonFuc, strToJsonFunc, y)
```  
intToJsonFunc、numToJsonFunc、strFromJsonFuc、strToJsonFunc为一些预置的方法，用它们来正确的对结构体的成员变量x、y等进行序列化操作。  

尽管这种冗余，在使用几次后，就会非常的熟练，然而，它也有一些限制，比如，一些编译器，对可变宏参数的数量限制是128个，上面这种冗余，相当于三分之二的参数不能当成员变量使用，当然实际业务中，我们很少会直接定义一个成员变量数量巨大的结构体，但是，书写上仍然是有些冗余。另外，不断的改进，（最终向java等的便利靠齐），也是我们继续探索的原因。  


所以，为了去掉这种冗余和继续探索，我们需要对结构体/类成员变量使用的常见的基本类型，进行模板实现。你在使用中进行成员变量声明时，可以使用如下的类型（与cJSON对应）：  
1、基础类型。
char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, bool  

2、基础类型的数组类型。  
char[]（将其转换为char*字符串，或者使用strcpy进行赋值）  

3、std::string类型。  
4、std::vector类型。std::vector的元素类型，覆盖了上述1中的基础类型，以及使用了CJS_JSON_SERIALIZE扩展宏的结构体/类。  
5、std::map<std::string, T>类型。其中T包含上述1中的基础类型，以及使用了CJS_JSON_SERIALIZE扩展宏的结构体/类。  
6、指针。使用了CJS_JSON_SERIALIZE扩展宏的结构体/类指针，目前需要先在外部申请资源后，再进行序列化操作。  


### 二、成员变量的数量

本实现中，我们限定了最多支持63个成员变量的json序列化操作。当然，也可以按照既有的格式，对CJS_CNT1等宏做扩展，以便支持更多数量的成员变量。


### 三、杂项

1、__cjsToJsonString的实现上，是不需要依赖于cJSON的：通过循环、递归即可达成结构体/类对象向json字符串的序列化操作。因为本扩展几乎都是对已有功能的整合和包装，所以在这个实现上，也使用了标准库的to_string等方法，没有像一些同类的其他库使用了自造轮子来实现。  
2、std::map<整型类型, T>类型的未支持。通过模板实现std::map<KeyType, ValueType>以及整型类型的特例化，是可能的，但是，由于json键是以字符串来表示的，所以本扩展也未实现。

<br>

## 使用步骤  

1、在你的工程设置里，使用C++11及以上的标准；  
2、在你的源代码里，包含本功能有关的4个源代码文件：  
    cJSON.h    cJSON.c    cxxJson.h    cxxJson.h
3、在定义结构体/类的.h/.cpp文件开始，#include "cxxJson.h"，以包含本扩展对外的头文件（注意路径）；
4、定义结构体/类，在其中使用宏CJS_JSON_SERIALIZE(<结构体/类名>, 成员变量名1, 成员变量名2, ....)  
5、使用结构体/类时，声明一个结构体/类对象obj，使用obj.__cjsFromJsonString(<strJson>)、obj.__cjsToJsonString()实现结构体/类对象的序列化操作。  


简单的实例见方案里的示例。  


嵌套的复杂示例，如下：  

```c++
#include <iostream>
#include "utils/json/cxxJson.h"

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

    CJS_JSON_SERIALIZE(TBaseType, c, uc, s, us, i, ui,
            l, ul, ll, ull, f,
            d, b, chStr, str)
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
            vecF, vecD, vecB, vecStr);
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

    TPortfolio() {
        pTree = new TTree();
    }

    ~TPortfolio() {
        delete pTree;
    }

    CJS_JSON_SERIALIZE(TPortfolio, id, name, baseType, vecBasetype, mapBasetype,
            tree, mapTree, pTree)
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

int main() {
    TPortfolio portfolioObj;
    portfolioObj.name = "Rich";
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

    return 0;
}

```


## 局限性

本组件对业务应用结构体/类提供的扩展宏CJS_JSON_SERIALIZE，主要是为了对应cJSON中的若干类型，更加方便的在开发时使用json序列化；所以除了扩展cJSON（或者QJson）等键值化json功能组件，其余有挺多局限性的（C++的魔幻语法太多了），不限于以下：  

1、基础类型的数组。对int、double之类的基础类型，由于与const T&、const T*等模板冲突，目前仍然只能使用std::vector<int>，或者std::vector<double>等对应std::vector类型来实现。  
2、多维数组。本扩展没有考虑多维数组的实现，（因为一维数组也没有实现好），所以，使用时需要折衷。从过去业务经验来看，多维数组未曾被使用。  
3、最多支持127个成员变量的json序列化操作。这个容易扩展。
4、不支持位结构。尽管通过一定的改造能够实现，但目前未实现，好在目前需要json的场景里（我们主要是对应cJSON的几种类型），极少使用位结构（在spring应用里有吗？）  
5、不支持静态成员变量。  
6、派生类的不确定。扩展的初衷是作用于配置或者用于数据交互的结构体/类对象上，所以派生类的支持，需要单独的做工作。  
7、msvc编译器未实现参数数量为0的情况。  

<br>

## 未来

1、局限性中的问题，尝试解决？（或许是模板匹配问题）  
2、更多数量的成员变量支持。这个是容易的。  
3、c99中宏扩展的实现。这个是繁琐的，由于C语言里没有重载、成员方法等，所以，需要做更多的工作，且扩展宏的形式可能都是不一样的。  
4、动态绑定。尽管派生类的支持可能需要做额外的工作，然而，如果想支持动态绑定，则需要在数据协议上做约定，同时还需要集成动态绑定的实现，这个灵活度非常的高。  
5、替换cJSON。cJSON其中的一些方法采用了简单的实现，所以如果确定需要追求更高的效率（但又不想失去便利），可以使用其他的同类库来替换。然而，需要注意的是，此处提的“效率”，对大多数场景来说，都是不需要的，（抛开剂量漫谈毒性的都是耍流氓），单一的极致效率通常也不会用这种扩展（我们针对的是综合考虑）。  


## 日志

2024.07.02  
修改  
1、将文件名从cxxJsonSerialize.h/cpp，修改成为cxxJson.h/cpp。注意，很抱歉，这种修改是不兼容的，如果你在过去已经使用了cxxJsonSerialize.h/cpp，那么现在推荐换成cxxJson.h/cpp。  
2、对README.md的开始做了说明，用于明确的区别本扩展与“键值”类json工具库的区别。  
3、将支持32个成员变量，扩展为63个成员变量。
4、将main.cpp中的测试代码，移动到新建立的test文件夹内。  
5、对QT的pro、cmake的CMakeLists.txt文件，做了同步的配置修改。  
  
<br>
新增  
1、对cJSON.h/c进行了（兼容）新增扩展：扩展了struct cJSON结构体，添加了context成员，以便为键值化过程添加必要的控制：对本扩展，添加了将json键规则化为C语言标识符（[a-zA-Z0-9_]）的开关；添加了cJSON_ParseEx方法，以对应规则化控制；在cJSON.c里添加了cJSON_ReplaceNonAlphanumericWithUnderscore、cJSON_RegulateKeyForC、cJSON_CopyContext等内容使用的辅助方法；兼容式的修改了parse_object等内部使用的方法。  
2、新增了test文件夹，以添加测试代码。  
  

<br>  
<br>  
  
2024.04.03   
新增  
基于cJSON，实现了本扩展的第一个版本。  
