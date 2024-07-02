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

#include "cxxJson.h"

#include <string.h>

#ifdef USING_NS_CJS
namespace cjs {
#endif

    CJS_INF_HIDDEN std::string __cjsToJsonString() {
        return std::string("\"\"");
    }

/**
 * 特例化：将char型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(char obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将unsigned char型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(unsigned char obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将short型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(short obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将unsigned short型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(unsigned short obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将int型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(int obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将unsigned int型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(unsigned int obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将long型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(long obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将unsigned long型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(unsigned long obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将long long型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(long long obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将unsigned long long型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(unsigned long long obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将float型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(float obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将double型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(double obj) {
        return std::to_string(obj);
    }

/**
 * 特例化：将char*型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const char *obj) {
        if (!obj) return "\"\"";
        return "\"" + std::string(obj) + "\"";
    }

/**
 * 特例化：将std::string型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::string &obj) {
        return "\"" + obj + "\"";
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(std::string &obj) {
        return "\"" + obj + "\"";
    }

/**
 * 特例化：将bool型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(bool obj) {
        return obj ? "true" : "false";
    }

    template<typename T>
    static std::string __cjsToJsonString_vector_internal(const std::vector<T> &obj) {
        std::string strRet = "[";
        std::string strSep = "";
        for (int i = 0; i < obj.size(); ++i) {
            strRet += strSep;
            strRet += std::to_string(obj[i]);
            strSep = ",";
        }
        strRet += "]";
        return strRet;
    }

/**
 * 特例化：将std::vector<char>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<char> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<unsigned char>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<unsigned char> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<short>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<short> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<unsigned short>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<unsigned short> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<int>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<int> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<unsigned int>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<unsigned int> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<long>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<long> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<unsigned long>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<unsigned long> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<long long>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<long long> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<unsigned long long>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<unsigned long long> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<float>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<float> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<double>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<double> &obj) {
        return __cjsToJsonString_vector_internal(obj);
    }

/**
 * 特例化：将std::vector<bool>型转换json字符串
 * @param obj
 * @return
 */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<bool> &obj) {
        std::string strRet = "[";
        std::string strSep = "";
        for (int i = 0; i < obj.size(); ++i) {
            strRet += strSep;
            strRet += (obj[i] ? "true" : "false");
            strSep = ",";
        }
        strRet += "]";
        return strRet;
    }

    /**
    * 特例化：将std::vector<std::string>型转换json字符串
    * @param obj
    * @return
    */
    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::vector<std::string> &obj) {
        std::string strRet = "[";
        std::string strSep = "";
        for (int i = 0; i < obj.size(); ++i) {
            strRet += strSep;
            strRet += std::string("\"") + obj[i] + std::string("\"");
            strSep = ",";
        }
        strRet += "]";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, char>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, char>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, unsigned char>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, unsigned char>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, short>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, short>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, unsigned short>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, unsigned short>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, int>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, int>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, unsigned int>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, unsigned int>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, long>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, long>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, unsigned long>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, unsigned long>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, long long>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, long long>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, unsigned long long>& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, unsigned long long>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, float >& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, float>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, double >& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, double>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::to_string((*it).second) + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, bool >& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, bool>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::string((*it).second ? "true" : "false") + std::string("}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }

    CJS_INF_HIDDEN std::string __cjsToJsonString(const std::map<std::string, std::string >& obj) {
        std::string strRet = "{";
        std::string strSep = "";
        for (std::map<std::string, std::string>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
            strRet += strSep;
            strRet += std::string("{\"") + (*it).first + std::string("\":");
            strRet += std::string("\"") + (*it).second + std::string("\"}");
            strSep = ",";
        }
        strRet += "}";
        return strRet;
    }


    /**
     * convert json object to struct/baseType object
     */
    /**
    * 特例化：jsonObj对象转换为char型变量
    * @param jsonObj
    * @param obj
    * @return
    */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, char& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = (char)std::stoi(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (char)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为unsigned char型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned char& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = (unsigned char)std::stoi(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (unsigned char)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为short型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, short& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = (short)std::stoi(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (short)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为unsigned short型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned short& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = (unsigned short)std::stoi(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (unsigned short)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为int型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, int& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = std::stoi(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (int)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为unsigned int型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned int& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = (unsigned int)std::stoul(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (unsigned int)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, long& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = std::stol(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (long)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为unsigned long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned long& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = std::stoul(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (unsigned long)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为long long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, long long& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = std::stoll(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (long long)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为unsigned long long型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, unsigned long long& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = std::stoull(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Int) {
            obj = (unsigned long long)(jsonObjItem->valueint);
        }
    }

    /**
     * 特例化：jsonObj对象转换为float型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, float& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = std::stof(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Double) {
            obj = (float)(jsonObjItem->valuedouble);
        }
    }

    /**
     * 特例化：jsonObj对象转换为double型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, double& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj = std::stod(jsonObjItem->valuestring);
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_Double) {
            obj = (double)(jsonObjItem->valuedouble);
        }
    }

    /**
     * 特例化：jsonObj对象转换为bool型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, bool& obj) {
        obj = false;
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            const char *pszTrue = "true";
            char *pszValue = jsonObjItem->valuestring;
            while (*pszTrue && *pszValue) {
                if (::tolower(*pszValue) != *pszTrue) {
                    return;
                }
                pszValue++;
                pszTrue++;
            }
            if (!*pszTrue) {
                obj = true;
            }
        }
        else if (jsonObjItem && jsonObjItem->type == cJSON_True) {
            obj = true;
        }
    }

    /**
     * 特例化：jsonObj对象转换为char*型变量（字符串）
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, char* obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String && obj) {
            strcpy(obj, jsonObjItem->valuestring);
        }
    }

    /**
     * 特例化：jsonObj对象转换为std::string型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::string& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_String) {
            obj.assign(jsonObjItem->valuestring);
        }
    }

    template <typename T>
    static void __cjsFromJsonObject_vector_internal(cJSON *jsonObjItem, std::vector<T> &obj) {
        if (!jsonObjItem || jsonObjItem->type != cJSON_Array) {
            return;
        }

        obj.clear();
        int cnt = cJSON_GetArraySize(jsonObjItem);
        for (int i = 0; i < cnt; ++i) {
            T val;
            __cjsFromJsonObject(cJSON_GetArrayItem(jsonObjItem, i), val);
            obj.push_back(val);
        }
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<char>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned char>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<short>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned short>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<std::vector<int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<int>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<std::vector<unsigned int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned int>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<long>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned long>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<long long>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<unsigned long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<unsigned long long>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<float>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<float>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<double>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<double>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<bool>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<bool>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::vector<std::string>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::vector<std::string>& obj) {
        __cjsFromJsonObject_vector_internal(jsonObjItem, obj);
    }

    template <typename T>
    static void __cjsFromJsonObject_map_internal(cJSON* jsonObjItem, std::map<std::string, T>& obj) {
        if (jsonObjItem && jsonObjItem->type == cJSON_Object) {
            obj.clear();
            cJSON* child = jsonObjItem->child;
            while (child)
            {
                T objItemValue;
                __cjsFromJsonObject(child, objItemValue);
                obj[child->string] = objItemValue;
                child = child->next;
            }
        }
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, char>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned char>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned char>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, short>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned short>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned short>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, std::map<std::string, int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, int>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, std::map<std::string, unsigned int>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned int>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, long>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned long>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, long long>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, unsigned long long>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, unsigned long long>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, float>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, float>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, double>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, double>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, bool>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, bool>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    /**
     * 特例化：jsonObj对象转换为std::map<std::string, std::string>型变量
     * @param jsonObj
     * @param obj
     * @return
     */
    CJS_INF_HIDDEN void __cjsFromJsonObject(cJSON* jsonObjItem, std::map<std::string, std::string>& obj) {
        __cjsFromJsonObject_map_internal(jsonObjItem, obj);
    }

    CJS_INF_HIDDEN void __cjsSetCJsonContext(cJSON *item)
    {
        cJSON_SetContextRegulateKeyForC(item, 1);
    }


#ifdef USING_NS_CJS
}
#endif
