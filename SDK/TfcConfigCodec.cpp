#include <cstdio>
#include <sstream>
#include <algorithm>
#include "TfcConfigCodec.hpp"

namespace DiaoBot
{

string ReadFile(const char *filename)
{
    string ret;
    char buf[1024];
    FILE *fp = fopen(filename, "r");
    if (fp == nullptr)
        return string();
    size_t len;
    while ((len = fread(buf, 1, 1024, fp)) != 0)
        ret.append(buf, len);
    fclose(fp);
    return ret;
}

inline static void Trim(string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) 
    {
        return !std::isspace(c);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {
        return !std::isspace(c);
    }).base(), s.end());
}

inline string TrimCopy(string s)
{
    Trim(s);
    return s;
}

inline string GeneratePath(const std::list<string> &stack)
{
    string path;
    for (const auto &i : stack)
    {
        path.append(i);
        path.push_back('\\');
    }
    path.pop_back();
    return path;
}

static const char * SpaceTable[] =
{
    "",
    "    ",
    "        ",
    "            ",
    "                ",
    "                    ",
    "                        ",
    "                            ",
    "                                ",
    "                                    ",
    "                                        "
};

inline static const char * GetLeadingSpaces(const string &s)
{
    int count = 0;
    for (auto i : s)
        if (i == '\\')
            ++count;
    return SpaceTable[count];
}

inline static const char * GetLeadingSpaces(int level)
{
    return SpaceTable[level];
}

std::list<string> SplitString(const string &s, char delimeter)
{
    string i;
    std::list<string> list;
    std::stringstream ss(s);
    while (std::getline(ss, i, delimeter))
        list.push_back(i);
    return list;
}

class TfcConfigCodecImpl
{
    using Node = TfcConfigCodec::Node;
    using KVPair = TfcConfigCodec::KVPair;

    Node RootNode;
    std::list<TfcConfigCodec::CommentedNode*> CommentedNodeStack;

    enum class LineType
    {
        ERROR,
        SECTION_BEGIN,
        SECTION_END,
        KVPAIR
    };

    LineType DetectLineType(const string &line)
    {
        if (line[0] == '<')
        {
            if (line.back() != '>')
                return LineType::ERROR;
            return line[1] == '/' ? LineType::SECTION_END : LineType::SECTION_BEGIN;
        }
        if (line.find('=') != string::npos)
            return LineType::KVPAIR;
        return LineType::ERROR;
    }

    int ParseImpl(std::istringstream &iss, Node &node)
    {
        int ret = TfcConfigCodec::SUCCESS;
        for (string line; std::getline(iss, line); )
        {
            Trim(line);
            if (line.length() == 0)
                continue;
            if (line[0] == '#')
            {
                if (this->CommentedNodeStack.size() == 0)
                    return TfcConfigCodec::PARSE_FAIL_SECTION_TAG_NOT_MATCH;
                this->CommentedNodeStack.back()->AddComment(line);
                continue;
            }
            switch (this->DetectLineType(line))
            {
            case TfcConfigCodecImpl::LineType::KVPAIR:
            {
                auto pos = line.find('=');
                auto k = TrimCopy(line.substr(0, pos));
                auto v = TrimCopy(line.c_str() + pos + 1);
                node.Pairs.push_back(KVPair(k, v));
                this->CommentedNodeStack.pop_back();
                this->CommentedNodeStack.push_back((TfcConfigCodec::CommentedNode*)&node.Pairs.back());
                break;
            }
            case TfcConfigCodecImpl::LineType::SECTION_BEGIN:
            {
                node.Children.push_back(Node());
                auto &ref = node.Children.back();
                this->CommentedNodeStack.push_back((TfcConfigCodec::CommentedNode*)&ref);
                ref.Level = node.Level + 1;
                ref.Tag = line.substr(1, line.length() - 2);
                ret = this->ParseImpl(iss, ref);
                if (ret != TfcConfigCodec::SUCCESS)
                    return ret;
                break;
            }
            case TfcConfigCodecImpl::LineType::SECTION_END:
                if (line.substr(2, line.length() - 3) != node.Tag)
                    return TfcConfigCodec::PARSE_FAIL_SECTION_TAG_NOT_MATCH;
                this->CommentedNodeStack.pop_back();
                return TfcConfigCodec::SUCCESS;
            case TfcConfigCodecImpl::LineType::ERROR:
            default:
                return TfcConfigCodec::PARSE_FAIL_LINE_TYPE_ERROR;
            }
        }
        return ret;
    }

    void GenerateImpl(string &s, const Node &node)
    {
        auto spaces = GetLeadingSpaces(node.Level);
        auto innerspaces = GetLeadingSpaces(node.Level + 1);
        s.append(spaces).append("<").append(node.Tag).append(">\n");
        for (const auto &comment : node.Comments)
        {
            s.append(innerspaces).append(comment).push_back('\n');
        }
        for (const auto &i : node.RawLines)
        {
            s.append(innerspaces).append(i).append("\n");
        }
        for (const auto &i : node.Pairs)
        {
            s.append(innerspaces).append(i.Key).append(" = ").append(i.Value).append("\n");
            for (const auto &comment : i.Comments)
            {
                s.append(innerspaces).append(comment).push_back('\n');
            }
        }
        for (const auto &i : node.Children)
        {
            this->GenerateImpl(s, i);
        }            
        s.append(spaces).append("</").append(node.Tag).append(">\n");
    }

    Node * GetSectionPointer(const char *s)
    {
        Node *p = &RootNode;
        auto sections = SplitString(s, '\\');
        for (const auto &i : sections)
        {
            bool is_found = false;
            for (auto &j : p->Children)
            {
                if (j.Tag == i)
                {
                    p = &j;
                    is_found = true;
                    break;
                }
            }
            if (!is_found)
                return nullptr;
        }
        return p;
    }

    KVPair * GetKVPairPointer(const char *section, const char *key)
    {
        auto p = GetSectionPointer(section);
        for (auto &i : p->Pairs)
            if (i.Key == key)
                return &i;
        return nullptr;
    }

    friend class TfcConfigCodec;
};

void TfcConfigCodec::ResetPImpl(void)
{
    delete PImpl;
    PImpl = new TfcConfigCodecImpl;
}

void TfcConfigCodec::CommentedNode::AddComment(const string &comment)
{
    Comments.push_back(comment);
}

TfcConfigCodec::TfcConfigCodec(void)
{
    PImpl = new TfcConfigCodecImpl;
}

TfcConfigCodec::~TfcConfigCodec(void)
{
    delete PImpl;
}

int TfcConfigCodec::Parse(const char *str)
{
    this->ResetPImpl();
    std::istringstream iss;
    iss.str(str);
    return PImpl->ParseImpl(iss, PImpl->RootNode);
}

int TfcConfigCodec::ParseFile(const char *filename)
{
    auto str = ReadFile(filename);
    if (str.length() == 0)
        return CANNOT_OPEN_FILE;
    return this->Parse(str.c_str());
}

void TfcConfigCodec::Generate(string &s)
{
    for (const auto &i : PImpl->RootNode.Children)
    {
        PImpl->GenerateImpl(s, i);
    }
}

bool TfcConfigCodec::HasKV(const char *section, const char *key)
{
    return PImpl->GetKVPairPointer(section, key) != nullptr;
}

string TfcConfigCodec::GetKV(const char *section, const char *key)
{
    auto p = PImpl->GetKVPairPointer(section, key);
    return p == nullptr ? string() : p->Value;
}

bool TfcConfigCodec::HasSection(const char *section)
{
    return PImpl->GetSectionPointer(section) != nullptr;
}

TfcConfigCodec::Node& TfcConfigCodec::GetSection(const char *section)
{
    return *(PImpl->GetSectionPointer(section));
}

int TfcConfigCodec::ClearKVs(const char *section)
{
    auto p = PImpl->GetSectionPointer(section);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    p->Pairs.clear();
    return SUCCESS;
}

int TfcConfigCodec::EraseKV(const char *section, const char *key)
{
    auto p = PImpl->GetSectionPointer(section);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    for (auto it = p->Pairs.begin(); it != p->Pairs.end(); ++it)
    {
        if (it->Key != key)
            continue;
        p->Pairs.erase(it);
        return SUCCESS;
    }
    return KVPAIR_NOT_FOUND;
}

int TfcConfigCodec::InsertKV(const char *section, const char *key, const char *value)
{
    auto p = PImpl->GetSectionPointer(section);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    for (const auto &i : p->Pairs)
        if (i.Key == key)
            return KVPAIR_DUPLICATED;
    p->Pairs.push_back(KVPair(key, value));
    return SUCCESS;
}

int TfcConfigCodec::OverwriteKV(const char *section, const char *key, const char *value)
{
    auto p = PImpl->GetSectionPointer(section);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    for (auto &i : p->Pairs)
    {
        if (i.Key == key)
        {
            i.Value = value;
            return SUCCESS;
        }
    }
    return KVPAIR_NOT_FOUND;
}

int TfcConfigCodec::ClearRawLines(const char *section)
{
    auto p = PImpl->GetSectionPointer(section);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    p->RawLines.clear();
    return SUCCESS;
}

int TfcConfigCodec::InsertRawLine(const char *section, const char *line)
{
    auto p = PImpl->GetSectionPointer(section);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    p->RawLines.push_back(line);
    return SUCCESS;
}

int TfcConfigCodec::ClearSections(const char *parent)
{
    auto p = PImpl->GetSectionPointer(parent);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    p->Children.clear();
    return SUCCESS;
}

int TfcConfigCodec::EraseSection(const char *parent, const char *tag)
{
    auto p = PImpl->GetSectionPointer(parent);
    if (p == nullptr)
        return SECTION_NOT_FOUND;
    for (auto it = p->Children.begin(); it != p->Children.end(); ++it)
    {
        if (it->Tag == tag)
        {
            p->Children.erase(it);
            return SUCCESS;
        }
    }
    return SECTION_NOT_FOUND;
}

int TfcConfigCodec::InsertSection(const char *parent, const char *tag)
{
    string full(parent);
    full.append("\\").append(tag);
    if (this->HasSection(full.c_str()))
        return SECTION_DUPLICATED;
    auto p = PImpl->GetSectionPointer(parent);
    // elog("parent=%s, full=%s, p=%x", parent, full.c_str(), p);
    p->Children.push_back(Node());
    auto &r = p->Children.back();
    r.Tag = tag;
    r.Level = p->Level + 1;
    return SUCCESS;
}

}