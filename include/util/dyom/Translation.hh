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
    std::atomic_bool                     didTranslate = false;
    std::vector<std::string *> translationOut;

    void ProcessDidTranslate (std::string translated);
    
public:
    DyomTranslator ();
    ~DyomTranslator () { internet.Close (); }

    void FixupGxtTokens (std::string &text);
    void DecodeSpecialChars (std::string &text);

    std::string TranslateText (const std::string &text);
    void TranslateAsync (std::string &text);
    void        TranslateDyomFile (DYOM::DYOMFileStructure &file);

    // Queue based translation
    void DoTranslate ();
    void EnqueueTranslation (std::string &text);

    bool
    GetDidTranslate ()
    {
        return didTranslate;
    }

    static inline struct Config
    {
        std::string TranslationChain;
    } m_Config;
    std::vector<std::string> mTranslationChain;
};
