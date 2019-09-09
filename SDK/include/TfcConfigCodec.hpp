#pragma once

#include <list>
#include <string>
using std::string;

namespace DiaoBot
{

class TfcConfigCodecImpl;
class TfcConfigCodec
{
public:
    enum Errno
    {
        SUCCESS = 0,
        CANNOT_OPEN_FILE,
        PARSE_FAIL_LINE_TYPE_ERROR,
        PARSE_FAIL_SECTION_TAG_NOT_MATCH,
        SECTION_NOT_FOUND,
        SECTION_DUPLICATED,
        KVPAIR_NOT_FOUND,
        KVPAIR_DUPLICATED
    };

    struct CommentedNode
    {
        std::list<string> Comments;
        void AddComment(const string &comment);
    };

    struct KVPair : CommentedNode
    {
        string Key, Value;
        KVPair(const string &k, const string &v):
            Key(k), Value(v) { }
    };

    struct Node : CommentedNode
    {
        int                 Level;
        string              Tag;
        std::list<Node>     Children;
        std::list<KVPair>   Pairs;
        std::list<string>   RawLines;

        Node(void):
            Level(-1) { }
    };

    TfcConfigCodec(void);
    ~TfcConfigCodec(void);

    int     Parse(const char *str);
    int     ParseFile(const char *filename);
    void    Generate(string &outbuf);

    bool    HasSection(const char *section);
    Node&   GetSection(const char *section);
    bool    HasKV(const char *section, const char *key);
    string  GetKV(const char *section, const char *key);

    int     ClearKVs(const char *section);
    int     EraseKV(const char *section, const char *key);
    int     InsertKV(const char *section, const char *key, const char *value);
    int     OverwriteKV(const char *section, const char *key, const char *value);
    int     ClearRawLines(const char *section);
    int     InsertRawLine(const char *section, const char *line);
    
    int     ClearSections(const char *parent);
    int     EraseSection(const char *parent, const char *tag);
    int     InsertSection(const char *parent, const char *tag);
    
private:
    TfcConfigCodecImpl *PImpl;
    void ResetPImpl(void);
};

string ReadFile(const char *filename);
std::list<string> SplitString(const string &s, char delimeter);

}