# JSON Serialization: Applied to C++ Object

This extension is not another C++ object json tool library like QJson, cJSON, etc., which only generates "key-value" information, but is a further useful tool based on them(now is cJSON).

The goal of this extension is to simplify the json serialization of C++ classes/structs as much as possible, and to use them as easily and conveniently as in java.

JSON serialization and deserialization of class/struct objects is achieved by declaring a simple extension macro in a C++ class/struct definition. 

It should be noted that the cJSON used in this extension has also been extended: in order to complete the compatibility of json keys with C language identifiers. The extension cJSON here refers to compatibility on the source code of cJSON (not ABI, the reason why there is no ABI compatibility is that we have added a new member to struct cJSON)

This extension is published on csdn synchronously, see [the json serialization extension of C++ objects](https://blog.csdn.net/smartcore/article/details/137517125), in which there is also a link to Baidu networkdisk. Due to the intermittent network, if there is a problem, you can also complain to the author under CSDN (try to keep the latest version here).

<br>

The implementation of this extension is based on C++11 and cJSON, so it is also cross-platform.

CJS prefix: c++ JSON Serialize
  
<br>

## Preface

In C++, even if you use cJSON or QJson, you can generally get a series of hierarchical json key-value pairs from a json string.

For example, ``jsonStr`` a json string object (``jsonStr = "{"name":"Rich","id":123}"``), if you use the cJSON_Parse in cJSON library to parse it, you will get a cJSON object jsonObj :

```c++
jsonObj["name"] == "Rich"
jsonObj["id"] == 123
```  

However, what we actually need more is to be able to directly deserialize ``jsonStr`` into a class/struct instance defined by us at the business layer, like some tool libraries in java (such as Alibaba's fastjson, etc.); Or serialize an instance of a class/struct object directly into a jsonStr string.

Mates who use C++ a lot may not be familiar with the use of serialization/deserialization in Java, so they are afraid or unclear. However, on the one hand, this convenience and comprehensive efficiency based on reflection have been verified for a long time; On the other hand, to make it clearer, let's take FastJSON as an example, and its basic usage in Java is as follows:

```java
package com.neoatomic.example;

import com.alibaba.fastjson.*;

public class App {
    // pseudo java class JPerson, serilize it:
    public static JPerson {
        public String name;
        public Integer id;
    }

    public static void main(String[] args) {
        JPerson personInfo;
        personInfo.name = "Poly";
        personInfo.id = 12;
        System.out.println(JSON.toJSONString(personInfo)); // output："{\"name\":\"Poly\",\"id\":12}"

        String jsonStr = "{\"name\":\"Rich\",\"id\":123}";
        personInfo = JSON.parseObject(jsonStr, JPerson.class);
        // now，personInfo.name.equals("Rich"); personInfo.id == 123
    }
}

```

As mentioned above, in Java, fastjson can provide convenient interfaces toJSONString and parseObject to easily serialize an object (or deserialize a string of json to fill in the value of the object's member variables) due to reflection.

So, **the question is, how to perform json serialization operations on struct (or class) objects in C++ like java?**

<br>

## Solution

Since the current standard library in C++ is still no ``reflect``, and in order to make the code easy to compile on the legacy historical OS, we often still use C++11 and C++14, so to achieve this convenience, we should not use the latest C++ standard library, otherwise it will still leave the trouble to the user.

With the passage of time, C++11 has finally been relatively widely used, so based on C++11 and cJSON, this code implements similar functions as in the above java: that is, the json serialization operation (serialization and deserialization) of structs/class objects in C++ is realized.

However, due to the lack of reflection, it is impossible to implement and use this convenience in C++ without intrusion, and it is impossible to implement it by the business layer. So, **we implement this convenient "extension" by declaring a macro extension in struct/class's definition. Once the struct/class is defined, "incidentally" add this extension macro (and the parameters that the macro needs) to its definition.**

Compared to previous implementations, this time it uses a more concise macro extension, so there is still no need to worry about this scaling, as the learning curve is almost flat. This is also a positive psychological effect of CV(ctrl+c ctrl+v) realization. It's important to take a look at the following C++ example:

```c++
#include <iostream>
#include "cxxJson.h"

struct TPerson {
    std::string name;
    int id;

    // add this macro
    CJS_JSON_SERIALIZE(TPerson, name, id)
};

int main() {
    TPerson personInfo;
    personInfo.name = "Rich";
    personInfo.id = "123";

    std::cout << personInfo.__cjsToJsonString() << std::endl; // output："{\"name\":\"Rich\",\"id\":123}"

    std::string strJson = "{\"name\":\"Poly\",\"id\":12}";
    personInfo.__cjsFromJsonString(strJson);
    // now，personInfo.name == "Poly"; personInfo.id == 12

    return 0;
}
```

As shown in the example above, in the struct TPerson, this extension is completed by passing in the struct name (which mainly serves as an identifier, or when you want to get information such as the type name of the class) and the name of the member variable that needs to be JSON serialized.

What's more, this macro extension does not change the size of the struct (``sizeof``), i.e. even if you still want to send the binary data represented by the struct object instance directly over the network (cast the struct to char*), the receiver will define a contemporaneous struct without this extension macro when receiving it, and it will still be able to directly cast the struct object instance natively. (This is needed in a lot of existing C++ business scenarios, and we don't know the remote system's information)

<br>

## Caution

### 1. Types of support  
In an implementation a few years ago, we used redundant patterns such as:  

```c++
OLD_CJS_JSON_SERIALIZE(StructName, intFromJsonFuc, numToJsonFunc, x,
                                      strFromJsonFuc, strToJsonFunc, y)
```  

intToJsonFunc, numToJsonFunc, strFromJsonFuc, and strToJsonFunc are some predefined methods that can be used to correctly serialize the member variables x, y, and so on of the struct.  

Although this kind of redundancy, after using it a few times, it will be very proficient, however, it also has some limitations, for example, some compilers, the number of variable macro parameters is limited to 128, the above redundancy, equivalent to two-thirds of the parameters can not be used as member variables, of course, in actual business, we rarely directly define a structure with a huge number of member variables, however, there is still some redundancy in writing. In addition, continuous improvement, (eventually in line with the convenience of Java and so on) is also the reason why we continue to explore.  

So, in order to get rid of this redundancy and continue to explore, we need to implement the template for the common primitive types used by struct/class member variables. When you declare member variables in use, you can use the following types (corresponding to cJSON):
1. Basic type. char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, bool

2. Array type of the Basic type.
char[] (convert it to a char* string, or use strcpy for assignment)

3. std::string.
4. std::vector. std::vector, which overrides the base type in ``1.``, and the struct/class that uses the CJS_JSON_SERIALIZE macro.
5. std::map<std::string, T> type. where T contains the base type in ``1.``, as well as the struct/class that uses the CJS_JSON_SERIALIZE macro.
6. pointer. The struct/class pointer that uses the CJS_JSON_SERIALIZE macro currently needs to be applied externally before serialization.


### 2. The number of struct/class member variables

In this implementation, we limit the JSON serialization operation to support up to 63 member variables. Of course, macros such as CJS_CNT1 can also be extended to support a larger number of member variables.


### 3. Miscellaneous

1. There is no need to rely on cJSON to implement __cjsToJsonString: the serialization operation from struct/class object to json string can be achieved through loop and recursion. Because this extension is almost all about the integration and packaging of existing functions, this implementation also uses the to_string methods of STL, and does not use self-made wheels like some other libraries.
2. std::map< integer type, T> type is not supported. It is possible to implement std::map<KeyType, ValueType> and special cases of integer types via templates, but this extension is not implemented because the json keys are represented as strings.

<br>

## How To Use  

1. In your project settings, use C++11 and above standards;  
2. In your source code, there are 4 source code files related to this function:  
```cJSON.h cJSON.c cxxJson.h cxxJson.h``` 

3. insert ``#include "cxxJson.h"``(NOTE path)  at start of the .h/.cpp file that defines the struct/class;     
4. Define the struct/class, use the macro CJS_JSON_SERIALIZE (< struct/class name>, member variable name 1, member variable name 2, ....)  
5. When using structs/classes, declare a struct/class object obj, and use obj.__cjsFromJsonString() and obj.__cjsToJsonString() to realize the serialization operation of struct/class objects.  
  
A simple example is shown in the example in ``Solution``.  

An example of a nested defintion, as follows:  

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


## Limitations

The macro CJS_JSON_SERIALIZE provided by this component for business application structs/classes is mainly to correspond to several types in cJSON, making it more convenient to use json serialization during development. Therefore, in addition to extending key-value json functional components such as cJSON (or QJson), there are many limitations (there are too many magic syntax in C++), including but not limited to the following:

1. Arrays of basic types. For basic types such as int and double, due to conflicts with templates such as const T& and const T*, you can still only use std::vector<int>, std::vector<double>, etc.
2. Multi-dimensional arrays. This extension does not consider the implementation of multidimensional arrays, (because one-dimensional arrays are not implemented well), so there is a need for compromises when using them. From past business experience, multidimensional arrays have never been used.
3. Support JSON serialization operations with up to 127 member variables. This is easy. 
4. Bit structure is not supported. Although it can be achieved through some transformation, it is not implemented at present, but fortunately, in the current scenarios that require json (we mainly correspond to several types of cJSON), bit structures are rarely used (is there one in SpringBoot applications?). ）
5. Static member variables are not supported.
6. Uncertainty of derived classes. The original intention of the extension is to act on the struct/class object that is configured or used for data interaction, so the support for derived classes needs to be done separately.
7. The MSVC compiler does not implement the situation that the number of member is 0.

<br>

## Future

1. Problems in limitations, try to solve? (Maybe it's a template matching issue)  

2. More member variables are supported. This is easy.  

3. Implementation of macro expansion in C99. This is cumbersome, because there are no overloads, member methods, etc. in C, so more work needs to be done, and the form of the extension macro may be different.

4. Dynamic binding. Although supporting derived classes may require additional work, if you want to support dynamic binding, you need to make a contract on the data protocol and also need to integrate the implementation of dynamic binding, which is very flexible.  

5. Replace cJSON. Some of the cJSON methods are simple implementations, so if you decide you need to be more efficient (but don't want to lose convenience), you can use other similar libraries instead. However, it is important to note that the "efficiency" mentioned here is not needed for most scenarios; and a single extreme efficiency is usually not used for this kind of expansion (we are aiming at the comprehensive consideration).  


## ChangeLog

2024.07.02   
Revise        
1. Change the file name from cxxJsonSerialize.h/cpp to cxxJson.h/cpp. Note, so many sorry, this change causes incompatible, if you have already used cxxJsonSerialize.h/cpp in the past, it is recommended to change to cxxJson.h/cpp for now.  
2. The beginning of the README.md is explained to clearly distinguish the difference between this extension and the "key-value" JSON tool library.  
3. It will support 32 member variables and expand to 63 member variables.   
4. Move the test code in the main.cpp to the newly created test folder.  
5. The QT .pro and cmake CMakeLists.txt files have been synchronized and modified.  
  
<br>

New    
1. New extensions to cJSON.h/c (compatible): Extended struct cJSON structs and added ``context`` member to add necessary control: For this extension, added a switch to regularize json keys into C identifiers (``[a-zA-Z0-9_]``); Added cJSON_ParseEx methods to correspond to regular controls; In cJSON.c, added some auxiliary methods - cJSON_ReplaceNonAlphanumericWithUnderscore, cJSON_RegulateKeyForC, cJSON_CopyContext, etc.     
2. Added the test folder to add test code.   


<br>  
<br>  
  
2024.04.03   
New  
based on cJSON，first release.  
