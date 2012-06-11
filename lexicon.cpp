/**
 * @file lexicon.cpp
 */

#include "lexicon.h"

Lexicon::Lexicon(const string & lexiconFile):
    _lexiconFilename(lexiconFile)
{
    _entries = new unordered_map<TermID, TokenData>();
    readLexicon();
}

Lexicon::~Lexicon()
{
    delete _entries;
}

Lexicon::Lexicon(const Lexicon & other)
{
    _entries = new unordered_map<TermID, TokenData>(*other._entries);
    _lexiconFilename = other._lexiconFilename;
}

const Lexicon & Lexicon::operator=(const Lexicon & other)
{
    if(this != &other)
    {
        delete _entries;
        _entries = other._entries;
        _lexiconFilename = other._lexiconFilename;
    }
    return *this;
}

TokenData Lexicon::getInfo(TermID termID) const
{
    unordered_map<TermID, TokenData>::const_iterator it = _entries->find(termID);
    if(it == _entries->end())
    {
        cerr << "[Lexicon]: warning: termID lookup failed" << endl;
        return TokenData();
    }
    return it->second;
}

void Lexicon::save() const
{
    ofstream outfile(_lexiconFilename);
    if(outfile.good())
    {
        unordered_map<TermID, TokenData>::const_iterator it;
        for(it = _entries->begin(); it != _entries->end(); ++it)
        {
            string line = toString(it->first) + " ";
            TokenData data = it->second;
            line += toString(data.idf) + " ";
            line += toString(data.totalFreq) + " ";
            line += toString(data.postingIndex) + " ";
            line += toString(data.postingBit) + " ";
            line += toString(data.postingLength) + "\n";
            outfile << line;
        }
        outfile.close();
    }
    else
    {
        cerr << "[Lexicon]: error writing lexicon to disk" << endl;
    }
}

template <class T>
string Lexicon::toString(T value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

void Lexicon::addTerm(TermID term, TokenData tokenData)
{
    _entries->insert(make_pair(term, tokenData));
}

void Lexicon::readLexicon()
{
    Parser parser(_lexiconFilename, "\n");
    if(!parser.isValid())
        return;

    while(parser.hasNext())
    {
        istringstream line(parser.next());
        vector<string> items;
        copy(std::istream_iterator<string>(line),
             std::istream_iterator<string>(),
             std::back_inserter<vector<string> >(items));
        TermID termID;
        TokenData data;
        istringstream(items[0]) >> termID;
        istringstream(items[1]) >> data.idf;
        istringstream(items[2]) >> data.totalFreq;
        istringstream(items[3]) >> data.postingIndex;
        istringstream(items[4]) >> data.postingBit;
        istringstream(items[5]) >> data.postingLength;
        addTerm(termID, data);
    }
}