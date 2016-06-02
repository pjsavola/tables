#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <math.h>
#include "MersenneTwister.h"
using namespace std;

MTRand twister;

int N = 8;
int tab[8][4][4];
int pvp[32][32];
int tvt[8][8];

void reset()
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            tvt[i][j] = 0;
            for (int k = 0; k < 4; k++)
                for (int l = 0; l < 4; l++)
                    pvp[4 * i + k][4 * j + l] = 0;
        }
}

/*
  1 2 3 4

1 1 2 3 4
2 2 3 4 1
3 3 4 1 2
4 4 1 2 3
*/
int player(int team, int seat, int round)
{
    int p = team * 4 + (seat - round + 4) % 4;
    return p;
}

void init()
{
    for (int round = 0; round < 4; round++)
        for (int seat = 0; seat < 4; seat++)
        {
            int team[N];
            for (int i = 0; i < N; i++)
            {
                team[i] = i;
            }
            for (int i = 0; i < N; i++)
            {
                int r = twister.randInt(N - 1);
                int t = team[i];
                team[i] = team[r];
                team[r] = t;
            }
            for (int table = 0; table < N; table++)
            {
                tab[table][seat][round] = team[table];
            }
        }
}

void calc()
{
    for (int table = 0; table < N; table++)
        for (int round = 0; round < 4; round++)
            for (int seat = 0; seat < 4; seat++)
                for (int opp = seat + 1; opp < 4; opp++)
                {
                    int team1 = tab[table][seat][round];
                    int team2 = tab[table][opp][round];
                    tvt[team1][team2]++;
                    tvt[team2][team1]++;
                    int player1 = player(team1, seat, round);
                    int player2 = player(team2, opp, round);
                    pvp[player1][player2]++;
                    pvp[player2][player1]++;
                }
}

int badness(int table, int round) {
    double avg = 48.0/(N-1);
    int x = 0;
    int y = 0;
    for (int seat = 0; seat < 4; seat++)
        for (int opp = seat + 1; opp < 4; opp++)
        {
            int team1 = tab[table][seat][round];
            int team2 = tab[table][opp][round];
            if (team1 == team2)
            {
                // cout << "Team has 2 players in 1 table\n";
                return 1000;
            }
            x += (int)fabs(avg - tvt[team1][team2]);
            int player1 = player(team1, seat, round);
            int player2 = player(team2, opp, round);
            if (player1 == player2)
            {
                // cout << "Player is in same table twice\n";
                return 1000;
            }
            int tmp = pvp[player1][player2];
            if (tmp > 0) tmp--;
            y += tmp;
        }
    return 3 * x + y;
}

#if 0 // Old way to calculate "goodness" of a table
double goodness(int table, int round)
{
    double avg = 12.0/(N-1);
    int a = (tab[table][0][round] - tab[table][1][round]);
    a *= (tab[table][0][round] - tab[table][2][round]);
    a *= (tab[table][0][round] - tab[table][3][round]);
    a *= (tab[table][1][round] - tab[table][2][round]);
    a *= (tab[table][1][round] - tab[table][3][round]);
    a *= (tab[table][2][round] - tab[table][3][round]);

    // Horrible. Same team has 2+ players in same table.
    if (a == 0)
        return 0;

    double b = 1000;
    b -= pow(fabs(tvt[tab[table][0][round]][tab[table][1][round]] - avg),1.0);
    b -= pow(fabs(tvt[tab[table][0][round]][tab[table][2][round]] - avg),1.0);
    b -= pow(fabs(tvt[tab[table][0][round]][tab[table][3][round]] - avg),1.0);
    b -= pow(fabs(tvt[tab[table][1][round]][tab[table][2][round]] - avg),1.0);
    b -= pow(fabs(tvt[tab[table][1][round]][tab[table][3][round]] - avg),1.0);
    b -= pow(fabs(tvt[tab[table][2][round]][tab[table][3][round]] - avg),1.0);
    return b;
}
#endif

int largest(int k) {
    int max = 0;
    int bad;
    int j;
    for(int i = 0; i < N; i++) {
        bad = badness(i,k);
        if (bad > max) {
            max = bad;
            j = i;
        }
    }
    return j;
}


// Delete place i from table Y, return the deleted team (round k).
int del(int seat, int table, int round) {
    int team1 = tab[table][seat][round];
    tab[table][seat][round] = -1;
    for(int opp = 0; opp < 4; opp++) {
        if(opp != seat) {
            int team2 = tab[table][opp][round];
            tvt[team1][team2]--;
            tvt[team2][team1]--;
            int player1 = player(team1, seat, round);
            int player2 = player(team2, opp, round);
            pvp[player1][player2]--;
            pvp[player2][player1]--;
        }
    }
    return team1;
}

void put(int seat, int team, int table, int round) {

    // If place is taken...
    if(tab[table][seat][round] != -1) {
        del(seat, table, round);
    }

    tab[table][seat][round] = team;
    for(int opp = 0; opp < 4; opp++) {
        if(opp != seat) {
            int team1 = team;
            int team2 = tab[table][opp][round];
            tvt[team1][team2]++;
            tvt[team2][team1]++;
            int player1 = player(team1, seat, round);
            int player2 = player(team2, opp, round);
            pvp[player1][player2]++;
            pvp[player2][player1]++;
        }
    }
}

struct Result {
    int seed;
    int badness;
    int worst;
    int tvt;
    int pvp;
};

bool comp1(Result r1, Result r2) { return r1.badness < r2.badness; }
bool comp2(Result r1, Result r2) { return r1.worst < r2.worst; }

int main(int argc, char *argv[])
{
    int seed = -1;
    bool just_once = false;
    if (argv[1])
    {
        seed = atoi(argv[1]);
    }
    if (seed != -1)
    {
        just_once = true;
        seed--;
    }
    vector<Result> vec;
    while (++seed < 20000)
    {
        twister.seed(seed);
        reset();
        init();
        calc();
        /*
        for(int k = 0; k < 4; k++) {
            cout << "Round " << k << "\n";
            for(int i = 0; i < N; i++) {
                cout << "Initial badness of " << i << ": "
                     << badness(i,k) << "\n";
            }
        }
        */

        int cancel = 0;
        int C,D;
        int tog;
        int c = 0;
        int x, r, k, i, j;
        int counter = 1000;
        while(c < counter) {
            k = twister.randInt(3);
            x = largest(k);
            r = x;
            while (r == x)
                r = twister.randInt(N - 1);
            tog = badness(x, k) + badness(r, k);
            i = twister.randInt(3);
            C = del(i,x,k);
            D = del(i,r,k);
            put(i,D,x,k);
            put(i,C,r,k);

            if(badness(x,k) + badness(r,k) > tog) {
                put(i,C,x,k);
                put(i,D,r,k);
                cancel++;
            }

            c++;
        }

        //cout << "Seed " << seed << " badness: ";
        Result result;
        int bad_sum = 0;
        int lrgst = 0;
        for(k = 0; k < 4; k++)
        {
            int lrg = largest(k);
            int bad = badness(lrg,k);
            if (bad > lrgst)
                lrgst = bad;
            for(i = 0; i < N; i++)
                bad_sum += badness(i,k);
        }

        int pvp_max = 0;
        int pvp_sum = 0;
        for (int p1 = 0; p1 < 4 * N; p1++)
            for (int p2 = p1 + 1; p2 < 4 * N; p2++) {
                pvp_sum += pvp[p1][p2];
                if (pvp[p1][p2] > pvp_max)
                    pvp_max = pvp[p1][p2];
            }
        if (pvp_sum != 24 * N)
        {
            cout << "WARNING: Wrong pvp sum!\n";
        }

        int tvt_max = 0;
        int tvt_sum = 0;
        for (int t1 = 0; t1 < N; t1++)
            for (int t2 = t1 + 1; t2 < N; t2++) {
                tvt_sum += tvt[t1][t2];
                if (tvt[t1][t2] > tvt_max)
                    tvt_max = tvt[t1][t2];
            }
        if (tvt_sum != 24 * N)
        {
            cout << "WARNING: Wrong tvt sum!";
            cout << " Seed: " << seed << " Sum: " << tvt_sum << "\n";
        }

        if (just_once)
        {
            cout << "== SUMMARY ==" << endl;
            cout << "Seed: " << seed << endl;
            cout << "Total badness: " << bad_sum << endl;
            cout << "Worst table: " << lrgst << endl;
            cout << "PvP max: " << pvp_max << endl;
            cout << "TvT max: " << tvt_max << endl;
            break;
        }

        result.seed = seed;
        result.badness = bad_sum;
        result.worst = lrgst;
        result.pvp = pvp_max;
        result.tvt = tvt_max;

        vec.push_back(result);
        //cout << bad_sum << "\n";
        //cout << "Worst table: " << lrgst << "\n";


    }
    if (!just_once)
    {
        sort(vec.begin(), vec.end(), comp1);
        cout << "TvT optimal: " << 48.0 / (N - 1) << endl;
        cout << "PvP optimal: " << 12.0 / (N - 1) << endl;
        cout << "Best tables (average):\n";
        for (int i = 0; i < 10; i++)
        {
            cout << vec[i].seed << ": " << vec[i].badness << " / ";
            cout << vec[i].worst << " pvp: " << vec[i].pvp;
            cout << " tvt: " << vec[i].tvt << endl;
        }
        sort(vec.begin(), vec.end(), comp2);
        cout << "Best worst tables:\n";
        for (int i = 0; i < 10; i++)
        {
            cout << vec[i].seed << ": " << vec[i].badness << " / ";
            cout << vec[i].worst << " pvp: " << vec[i].pvp;
            cout << " tvt: " << vec[i].tvt << endl;
        }
    }
    else
    {
        cout << "== TABLES -> PLAYERS ==" << endl;
        map<pair<int, int>, int> pmap;
        for (int k = 0; k < 4; k++) {
            cout << "Round " << k << "\n";
            for (int i = 0; i < N; i++) {
                cout << "Table " << i << " : ";
                for (int j = 0; j < 4; j++) {
                    int team = tab[i][j][k];
                    int player = team * 4 + (j - k + 4) % 4;
                    pmap[make_pair(player, k)] = i;
                    cout << player << " ";
                }
                cout << "\n";
            }
        }
        cout << "== PLAYERS -> TABLES ==" << endl;
        for (int i = 0; i < 4 * N; i++) {
            cout << "Player " << i << " : ";
            for (int k = 0; k < 4; k++) {
                cout << pmap[make_pair(i, k)] << " ";
            }
            cout << "\n";
        }
    }
}
