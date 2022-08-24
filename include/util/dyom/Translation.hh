#pragma once

#include <sstream>
#include <string>
#include <regex>

#include <windows.h>
#include <wininet.h>

#include "DYOMFileFormat.hh"
#include "Internet.hh"

/* Based on some of the code by captain_n00by/met-nikita
   https://github.com/Parik27/SA.Rainbomizer/pull/170
 */
class DyomTranslator
{
    InternetUtils              internet;
    std::string                translationQueue;
    int                        queueCounter = 0;
    bool                       didTranslate = false;
    std::vector<std::string *> translationOut;

    void ProcessDidTranslate (std::string translated);
    
public:
    DyomTranslator ();
    ~DyomTranslator ();

    void FixupGxtTokens (std::string &text);
    void DecodeSpecialChars (std::string &text);

    std::string TranslateText (const std::string &text);
    void        TranslateDyomFile (DYOM::DYOMFileStructure &file);

    // Queue based translation
    void DoTranslate ();
    void EnqueueTranslation (std::string &text);

    bool
    GetDidTranslate ()
    {
        return didTranslate;
    }

    enum TranslationMethod
    {
        TM_DEFAULT,
        TM_GOOGLECLOUD,
        TM_YANDEXCLOUD,
        TM_MAX
    };

    static inline struct Config
    {
        std::string TranslationChain;
        int TranslationMethod;
        std::string GoogleAPIKey;
        std::string YandexOAuth;
        std::string YandexFolderID;
    } m_Config;

    std::vector<std::string> mTranslationChain;
    std::regex               mResultRegex;
    bool                     mPost;
    std::string              mURLTemplate;
    std::string              mHeaderTemplate;
    std::string              mRequestTemplate;
};