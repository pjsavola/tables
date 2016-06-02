#include "tables.h"

Player::Player(int n, Team *t) : mNumber(n), mTeam(t)
{
    t->SetPlayer(n, this);
    for (int i = 0; i < 4; i++) mTable[i] = 0;
}
int Player::GetId() const { return mTeam->GetId() * 4 + mNumber; }
Team* Player::GetTeam() const { return mTeam; }
void Player::SetTable(int r, Table *t)
{
    if (mTable[r])
        mTable[r]->RemovePlayer(this);
    mTable[r] = t;
    t->AddPlayer(this);
}
bool Player::PlaysAgainst(Player *p) const
{
    return mOpponents.find(p) != mOpponents.end();
}
void Player::AddOpponent(Player *p) { mOpponents.insert(p); }

int main(int argc, char *argv[]) {
    int N = 4;
    vector<Team *> teams;
    vector<Player *> players;
    vector<Table *> tables;
    for (int team = 0; team < N; team++)
    {
        Team *t = new Team(team);
        teams.push_back(t);
        for (int player = 0; player < 4; player++)
        {
            Player *p = new Player(player, t);
            players.push_back(p);
        }
        t->Print();
    }
    permute(players);
    for (int round = 0; round < 4; round++)
    {
        for (int table = 0; table < N; table++)
        {
            Table *t = new Table(table, round);
            tables.push_back(t);
            t->Print();
        }
    }

    // Garbage collecting
    while (!tables.empty())
    {
        delete tables.back();
        tables.pop_back();
    }
    while (!players.empty())
    {
        delete players.back();
        players.pop_back();
    }
    while (!teams.empty())
    {
        delete teams.back();
        teams.pop_back();
    }
}
