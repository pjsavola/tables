#include <iostream>
#include <set>
#include <map>
#include <vector>
#include "MersenneTwister.h"
using namespace std;

MTRand twister;

class Team;
class Table;

class Player
{
public:
    Player(int n, Team *t);
    int GetId() const;
    Team* GetTeam() const;
    void SetTable(int r, Table *t);
    bool PlaysAgainst(Player *p) const;
    void AddOpponent(Player *p);
private:
    int mNumber;
    Team *mTeam;
    Table *mTable[4];
    set<Player *> mOpponents;
};

class Team
{
public:
    Team(int n) : mId(n) {}
    int GetId() const { return mId; }
    void SetPlayer(int i, Player *p) { mPlayer[i] = p; }
    Player* GetPlayer(int i) const { return mPlayer[i]; }
    void Print()
    {
        cout << "TEAM " << mId << endl;
        for (int i = 0; i < 4; i++)
            if (mPlayer[i])
                cout << mPlayer[i]->GetId() << " ";
            else
                cout << "<empty> ";
        cout << endl;
    }
private:
    int mId;
    Player *mPlayer[4];
    map<Team *, int> mOpponents;
};

class Table
{
public:
  Table(int i, int r) : mId(i), mRound(r)
    {
        for (int i = 0; i < 4; i++) mPlayer[i] = 0;
    }
    bool AddPlayer(Player *p)
    {
        if (!p) return false;
        int seat = (p->GetId() + mRound) % 4;
        for (int i = 0; i < 4; i++)
        {
            Player *a = mPlayer[i];
            if (a)
            {
                if (a == p) return false;
                if (p->PlaysAgainst(a)) return false;
                // Team opponent constraint...
                if (a->GetTeam() == p->GetTeam()) return false;
                if (i == seat) return false;
            }
        }
        p->AddOpponent(mPlayer[seat]);
        mPlayer[seat] = p;
        return true;
    }
    void RemovePlayer(Player *p)
    {
        if (!p) return;
        int seat = (p->GetId() + mRound) % 4;
        mPlayer[seat] = 0;
        for (int i = 0; i < 4; i++)
            if (mPlayer[i] == p)
            {
                mPlayer[i] = 0;
                break;
            }
    }
    void Print()
    {
        cout << "TABLE " << mId << " (round " << mRound << ")" << endl;
        for (int i = 0; i < 4; i++)
            if (mPlayer[i])
                cout << mPlayer[i]->GetId() << " ";
            else
                cout << "<empty> ";
        cout << endl;
    }
private:
    int mId;
    int mRound;
    Player *mPlayer[4];

};

void permute(vector<Player *> &players)
{
    for (unsigned int i = 0; i < players.size(); i++)
    {
        int j = twister.randInt(players.size() - 1);
        Player *p = players[i];
        players[i] = players[j];
        players[j] = p;
    }
}
