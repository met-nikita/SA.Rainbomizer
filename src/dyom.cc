#include "dyom.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include "injector/calling.hpp"
#include <wininet.h>
#include "config.hh"
#include <cstring>
#include "missions.hh"
#include <regex>

using namespace std::literals;

DyomRandomizer *DyomRandomizer::mInstance = nullptr;

/*******************************************************/
CRunningScript *
StoreRandomizedDYOMScript (uint8_t *startIp)
{
    auto dyomRandomizer = DyomRandomizer::GetInstance ();

    auto out = HookManager::CallOriginalAndReturn<
        injector::cstd<CRunningScript *(uint8_t *)>, 0x46683B> (nullptr,
                                                                startIp);
    if (ScriptParams[0] == 91092)
        dyomRandomizer->mDyomScript = out;

    DyomRandomizer::mEnabled = true;
    return out;
}

/*******************************************************/
void
DyomHandleOnScriptOpCodeProcess ()
{
    auto dyomRandomizer = DyomRandomizer::GetInstance ();

    if (DyomRandomizer::mEnabled && dyomRandomizer->mDyomScript)
        dyomRandomizer->HandleDyomScript (dyomRandomizer->mDyomScript);

    HookManager::CallOriginalAndReturn<injector::cstd<void ()>, 0x469FB0> (
        [] { (*((int *) 0xA447F4))++; });
}

/*******************************************************/
size_t
AdjustCodeForDYOM (FILE *file, void *buf, size_t len)
{
    size_t ret = CallAndReturn<size_t, 0x538950> (file, buf, len);

    char *signature = (char *) buf + 0x18CE0;
    if (std::string (signature, 4) == "DYOM")
        {
            uint8_t *slots
                = reinterpret_cast<uint8_t *> ((char *) buf + 0x18D90);
            *slots = 9;

            char *column_name = (char *) buf + 0x18E11;
            strcpy ((char *) buf + 0x18E11, "FILE9");
        }
    return ret;
}

/*******************************************************/
void
DyomRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("DYOMRandomizer", 
        std::pair("UseEnglishOnlyFilter", &m_Config.EnglishOnly)))
        return;

    if (!ConfigManager::ReadConfig ("DYOMRandomizer",
                                    std::pair ("AutoTranslateToEnglish",
                                               &m_Config.Translate)))
        return;
    if (m_Config.Translate)
        {
            if (!ConfigManager::ReadConfig ("DYOMRandomizer",
                                            std::pair ("TranslatorAPIOAuth",
                                                       &m_Config.OAuth)))
                return;

            if (!ConfigManager::ReadConfig ("DYOMRandomizer",
                                            std::pair ("TranslatorAPIFolderID",
                                                       &m_Config.FolderID)))
                return;
        }

    mIAM = "";

    if (!ConfigManager::ReadConfig ("MissionRandomizer"))
        {
            RegisterDelayedHooks (
                {{HOOK_CALL, 0x469FB0,
                  (void *) &DyomHandleOnScriptOpCodeProcess}});

            RegisterDelayedFunction ([] { injector::MakeNOP (0x469fb5, 2); });
        }

    RegisterHooks ({{HOOK_CALL, 0x46683B, (void *) &StoreRandomizedDYOMScript},
                    {HOOK_CALL, 0x468E7F, (void *) AdjustCodeForDYOM}});

    Logger::GetLogger ()->LogMessage ("Intialised DyomRandomizer");
}

/*******************************************************/
bool
ReadRequestResponse (HANDLE request, std::vector<uint8_t> &out)
{
    DWORD dwSize;
    DWORD dwDownloaded;

    for (;;)
        {

            if (!InternetQueryDataAvailable (request, &dwSize, 0, 0))
                {
                    Logger::GetLogger ()->LogMessage (
                        "InternetQueryDataAvailable failed "
                        + std::to_string (GetLastError ()));

                    return false;
                }
            auto lpszData = new TCHAR[dwSize + 1];
            if (!InternetReadFile (request, lpszData, dwSize, &dwDownloaded))
                {
                    Logger::GetLogger ()->LogMessage (
                        "InternetReadFile failed "
                        + std::to_string (GetLastError ()));

                    delete[] lpszData;
                    break;
                }

            out.insert (out.end (), lpszData, lpszData + dwSize);

            if (dwDownloaded == 0)
                break;
        }

    InternetCloseHandle (request);
    return true;
}

/*******************************************************/
HANDLE
MakeRequest (HANDLE session, const std::string &file)
{
    HANDLE request = HttpOpenRequest (session, "GET", file.c_str (), NULL, NULL,
                                      NULL, INTERNET_FLAG_SECURE, 0);
    HttpSendRequest (request, NULL, 0, NULL, 0);

    return request;
}

/*******************************************************/
HANDLE
MakeRequestPOST (HANDLE session, const std::string &url,
                 const std::string &headers, const std::string &data)
{
    HANDLE request = HttpOpenRequest (session, "POST", url.c_str (), NULL, NULL,
                                      NULL, INTERNET_FLAG_SECURE, 0);
    HttpSendRequest (request, headers.c_str (), headers.length (),
                     (char*)data.c_str (), data.length());

    return request;
}

/*******************************************************/
HANDLE
OpenSession (HINTERNET internet)
{
    return InternetConnect (internet, "dyom.gtagames.nl",
                            INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                            INTERNET_SERVICE_HTTP, 0, 0);
}

/*******************************************************/
HANDLE
OpenSession (HINTERNET internet, const std::string &domain)
{
    return InternetConnect (internet, domain.c_str(),
                            INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                            INTERNET_SERVICE_HTTP, 0, 0);
}

/*******************************************************/
std::string
ReadStringFromRequest (HANDLE request)
{
    std::vector<uint8_t> output;
    ReadRequestResponse (request, output);

    return std::string (output.begin (), output.end ()).c_str ();
}

/*******************************************************/
int
DyomRandomizer::GetTotalNumberOfDYOMMissionPages (HANDLE      session,
                                                  std::string list)
{
    HANDLE      request = MakeRequest (session, list.c_str ());
    std::string lists   = ReadStringFromRequest (request);

    auto start = lists.find ("... <span class=pagelink>");
    start      = lists.find ("\' >", start) + 3;

    auto end = lists.find ("</a>", start);

    return std::stoi (lists.substr (start, end - start));
}

/*******************************************************/
int
CountOccurrencesInString (const std::string &str, const std::string &substr)
{
    int         count = 0;
    std::size_t found = str.find (substr);
    while (found != str.npos)
        {
            found = str.find (substr, found + 1);
            count++;
        }

    return count;
}

/*******************************************************/
std::size_t
GetNthOccurrenceOfString (const std::string &str, const std::string &substr,
                          int n)
{
    int         count = 0;
    std::size_t found = str.find (substr);
    while (found != str.npos)
        {
            if (count == n)
                return found;

            count++;
            found = str.find (substr, found + 1);
        }

    return 0;
}

/*******************************************************/
std::string
DyomRandomizer::GetRandomEntryFromPage (HANDLE session, std::string page)
{
    std::string entries
        = ReadStringFromRequest (MakeRequest (session, page.c_str ()));

    int entries_count = CountOccurrencesInString (entries, "<a href='show/");
    std::size_t start = GetNthOccurrenceOfString (entries, "<a href='show/",
                                                  random (entries_count - 1))
                        + 9;
    std::size_t end = entries.find ("'>", start + 1);

    return entries.substr (start, end - start);
}

/*******************************************************/
bool
DyomRandomizer::ParseMission (HANDLE session, const std::string &url)
{
    std::string mission = ReadStringFromRequest (MakeRequest (session, url));
    if (mission.find ("<a title='download for slot 1'  href='") == mission.npos)
        return false;

    std::vector<uint8_t> output;
    ReadRequestResponse (MakeRequest (session, "download/" + url.substr (5)),
                         output);
    FILE *file = fopen ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (), "wb");
    fwrite (output.data (), 1, output.size (), file);
    fclose (file);
    FILE *file2 = fopen ((CFileMgr::ms_dirName + "\\DYOM8.dat"s).c_str (), "wb");
    fwrite (output.data (), 1, output.size (), file2);
    fclose (file2);

    return true;
}

/*******************************************************/
bool
DyomRandomizer::TranslateMission (HANDLE session)
{
    // get IAM token for further use
    HINTERNET   handle = InternetOpen ("123robot", INTERNET_OPEN_TYPE_PRECONFIG,
                                     NULL, NULL, 0);
    HANDLE      session2 = OpenSession (handle, "iam.api.cloud.yandex.net");
    std::string response = ReadStringFromRequest (
        MakeRequestPOST (session2, "/iam/v1/tokens",
                         "Content-Type: application/json",
                         std::string ("{\"yandexPassportOauthToken\":\"")
                             + m_Config.OAuth + "\"}"));
    std::regex  e ("\"iamToken\":\\s*\"(.*?)\"");
    std::cmatch cm;
    if (!std::regex_search (response.c_str (), cm, e))
        return false;
    mIAM = cm[1];
    CloseHandle (handle);
    CloseHandle (session2);
    
    //translator 100% won't get language of every string right, so to determine the language of the mission we will find the language that had most occurences
    std::map<std::string, size_t> lang_histogram;
    using pair_type = decltype(lang_histogram)::value_type;

    //v1.1 - (int)2 - 6 header fields - doesn't have objective text
    //v2.0 v2.1 - (int)3 - 6 header fields - 0x4A7 - objective texts
    //v3.0 v4.0 v4.1 (int)4 - 6 header fields - 0xCCF - objective texts
    //v5.0 - (int)5 - 6 header fields - 0x194F - objective texts
    //v7.0.0 and onward - (int)6 - 7 header fields - 0x194F - objective texts
    //published mission is negative

    byte input[40000];
    byte  output[40000];
    FILE *file = fopen ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (), "rb");
    std::size_t read = fread (input, 1, 40000, file);
    fclose (file);
    if (read < 1) //corrupted
        return false;
    //copy first 4 bytes (version) to the resulting array and variable
    memcpy (output, input, 4);
    INT8       version;
    std::size_t offset;
    UINT8       max_objectives;
    memcpy (&version, input, 1);
    if (version < 0)
        version *= -1;
    if (version < 2 || version > 6) // unsupported version
        return false;
    switch (version)
        {
        case 3:
            offset         = 0x4A7;
            max_objectives = 20;
            break;
        case 4:
            offset         = 0xCCF;
            max_objectives = 50;
            break;
        case 5:
        case 6:
        default:
            offset         = 0x194F;
            max_objectives = 100;
            break;
        }
    std::size_t o_pos = 4;
    //get mission name string
    std::string text = "";
    std::size_t i_pos = 4;
    while (input[i_pos] != 0x00)
        {
            text+=(char)input[i_pos];
            i_pos++;
        }
    std::size_t cut_pos = i_pos;
    //translate the mission name and write it to the resulting array
    std::string* result = TranslateText (session, text);
    std::string  translation = result[0];
    std::string  lang        = result[1];
    ++lang_histogram[lang];
    memcpy (output + o_pos, translation.data (), translation.size ());
    o_pos += translation.size ();
    //remember where to insert language code later
    std::size_t lang_pos = o_pos;
    //write author name as-is
    i_pos++;
    while (input[i_pos] != 0x00)
        {
            i_pos++;
        }
    i_pos++;
    memcpy (output + o_pos, input+cut_pos, i_pos-cut_pos);
    o_pos += i_pos - cut_pos;
    //translate 3 intro text fields
    int counter = 0;
    text        = "";
    while (true)
        {
            if (input[i_pos] != 0x00)
                text += (char) input[i_pos];
            else
                {
                    if (text.length () > 1)
                        {
                            result = TranslateText (session, text);
                            translation = result[0];
                            std::string lang = result[1];
                            ++lang_histogram[lang];
                        }
                    else
                        {
                            translation = text;
                        }
                    memcpy (output + o_pos, translation.c_str (),
                            translation.size ()+1);
                    o_pos += translation.size () + 1;
                    text                    = "";
                    counter++;
                }
            i_pos++;
            if (counter >= 3)
                break;
        }
    if (version != 2)
        {
            cut_pos = i_pos;
            if (version == 6) // skip through soundcode field
                {
                    while (true)
                        {
                            if (input[i_pos] != 0x00)
                                i_pos++;
                            else
                                {
                                    i_pos++;
                                    break;
                                }
                        }
                    memcpy (output + o_pos, input + cut_pos, i_pos - cut_pos);
                    o_pos += i_pos - cut_pos;
                    cut_pos = i_pos;
                }
            // depending on the version objectives texts offset a little bit
            // different to the header
            i_pos += offset;
            // write everything inbetween mission name and objectives texts to
            // the resulting array
            memcpy (output + o_pos, input + cut_pos, i_pos - cut_pos);
            o_pos += i_pos - cut_pos;
            // translate and write objectives to the resulting array (100 max)
            counter = 0;
            text    = "";
            while (true)
                {
                    if (input[i_pos] != 0x00)
                        text += (char) input[i_pos];
                    else
                        {
                            if (text.length () > 1)
                                {
                                    result      = TranslateText (session, text);
                                    translation = result[0];
                                    std::string lang = result[1];
                                    ++lang_histogram[lang];
                                }
                            else
                                {
                                    translation = text;
                                }
                            memcpy (output + o_pos, translation.c_str (),
                                    translation.size () + 1);
                            o_pos += translation.size () + 1;
                            text = "";
                            counter++;
                        }
                    i_pos++;
                    if (counter >= max_objectives)
                        break;
                }
        }
    if (lang_histogram.size() < 1)
        lang = " (unk)";
    else
        {
            auto pr = std::max_element (std::begin (lang_histogram),
                                     std::end (lang_histogram),
                                        [] (const pair_type &p1,
                                            const pair_type &p2) {
                                            return p1.second < p2.second;
                                        });
            lang    = " (" + pr->first + ")";
    }

    //write the rest of the file to the resulting array
    memcpy (output + o_pos, input + i_pos, read - i_pos);
    //rename original file
    std::remove ((CFileMgr::ms_dirName + "\\DYOM9_orig.dat"s).c_str ());
    std::rename ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (),
                 (CFileMgr::ms_dirName + "\\DYOM9_orig.dat"s).c_str ());
    //finally write the file with translated mission
    FILE *file2 = fopen ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (), "wb");
    fwrite (output, 1, lang_pos, file2);
    fwrite (lang.data(), 1, lang.size(), file2);
    fwrite (output+lang_pos, 1, o_pos + read - i_pos - lang_pos, file2);
    fclose (file2);
    FILE *file3
        = fopen ((CFileMgr::ms_dirName + "\\DYOM8.dat"s).c_str (), "wb");
    fwrite (output, 1, lang_pos, file3);
    fwrite (lang.data (), 1, lang.size (), file3);
    fwrite (output + lang_pos, 1, o_pos + read - i_pos - lang_pos, file3);
    fclose (file3);
    return true;
}

/*******************************************************/
std::string *
DyomRandomizer::TranslateText (HANDLE session, const std::string &text)
{
    std::string *result = new std::string[2] {text, "unk"};
    std::string response = ReadStringFromRequest (
        MakeRequestPOST (session, "/translate/v2/translate",
        std::string ("Content-Type: application/json\r\nAuthorization: Bearer ")
            + mIAM,
        std::string ("{\"folderId\": \"")+m_Config.FolderID+"\",\"texts\": [\""+text+"\"],\"targetLanguageCode\": \"en\"}"));
    std::regex  rtext ("\"text\":\\s*\"(.*?)\"");
    std::cmatch cm;
    if (!std::regex_search (response.c_str (), cm, rtext))
        return result;
    std::string translation = cm[1];
    std::regex  rlang (
        "\"detectedLanguageCode\":\\s*\"(.*?)\"");
    std::string lang = "unk";
    if (std::regex_search (response.c_str (), cm, rlang))
        lang = cm[1];
    //translator tends to break tags with spaces, attempt to fix
    translation = std::regex_replace(translation,std::regex("(~)\\s*([a-zA-Z])\\s*(~)"),"$1$2$3");
    //trim everything above 99 symbols (crashes overwise)
    if (translation.length () > 99)
        {
            translation = translation.substr (0, 99);
    }
    // remove remaining broken tags (crashes overwise)
    std:size_t pos = 0;
    while (true)
        {
            std::size_t tild = translation.find ("~", pos);
            if (tild != translation.npos)
                {
                    if (translation.substr (tild + 2, 1).compare ("~") != 0)
                        {
                            translation = translation.replace (tild, 1, "");
                            pos         = tild;
                        }
                    else
                        pos = tild + 3;
                }
            else
                break;
        }
    result[0] = translation;
    result[1] = lang;
    return result;
}

/*******************************************************/
void
DyomRandomizer::DownloadRandomMission ()
{
    if (InternetAttemptConnect (0) == ERROR_SUCCESS)
        {
            HINTERNET handle
                = InternetOpen ("123robot", INTERNET_OPEN_TYPE_PRECONFIG, NULL,
                                NULL, 0);

            HANDLE session = OpenSession (handle);

            std::string list;

            if (m_Config.EnglishOnly)
                list = "list?english=1&";
            else
                list = random (100) > 38 ? "list?" : "list_d?";

            int total_pages = GetTotalNumberOfDYOMMissionPages (session, list);
            while (!ParseMission (
                session,
                GetRandomEntryFromPage (session, list + "page="
                                                     + std::to_string (random (
                                                         total_pages)))))
                ;

            CloseHandle (session);
            CloseHandle (handle);

            if (m_Config.Translate)
                {
                    HINTERNET handle2
                        = InternetOpen ("123robot",
                                        INTERNET_OPEN_TYPE_PRECONFIG, NULL,
                                        NULL, 0);

                    HANDLE session2 = OpenSession (handle2, "translate.api.cloud.yandex.net");
                    TranslateMission (session2);
                    CloseHandle (session2);
                    CloseHandle (handle2);
                }
        }
}

/*******************************************************/
void
DyomRandomizer::HandleDyomScript (CRunningScript *scr)
{
    if (!CGame::bMissionPackGame)
        {
            mDyomScript = nullptr;
            return;
        }

    static int previousOffset = 0;
    int currentOffset = scr->m_pCurrentIP - (unsigned char *) ScriptSpace;

    if (currentOffset != previousOffset)
        {
            if (currentOffset == 103522)
                {
                    if ((char *) &ScriptSpace[10918] == "DYOM9.dat"s)
                        DownloadRandomMission ();
                }
            if (currentOffset == 101718 || currentOffset == 101738)
                {
                    if (ScriptSpace[11439] == 9)
                        {
                            // Random Mission
                            strcpy ((char *) ScriptSpace[9889],
                                    "~n~Random Mission");

                            if (currentOffset == 101738)
                                scr->m_pCurrentIP
                                    = (unsigned char *) ScriptSpace + 101718;
                        }
                }
        }
}

/*******************************************************/
void
DyomRandomizer::DestroyInstance ()
{
    if (DyomRandomizer::mInstance)
        delete DyomRandomizer::mInstance;
}

/*******************************************************/
DyomRandomizer *
DyomRandomizer::GetInstance ()
{
    if (!DyomRandomizer::mInstance)
        {
            DyomRandomizer::mInstance = new DyomRandomizer ();
            atexit (&DyomRandomizer::DestroyInstance);
        }
    return DyomRandomizer::mInstance;
}

bool DyomRandomizer::mEnabled = false;
