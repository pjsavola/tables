#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "MersenneTwister.h"
using namespace std;

vector<string> players;
int pc;
int tc;
int rounds = 7;
MTRand twister;

int main()
{
    ifstream in ("input.txt");
    if (in.is_open())
    {
        while (in.good())
        {
            string s;
            in >> s;
            players.push_back(s);
        }
        in.close();
    }
    players.pop_back();
    pc = players.size();
    if (pc % 4 != 0)
    {
        cout << "Player count % 4 != 0" << endl;
        return -1;
    }
    tc = pc / 4;

    int table[tc][4][rounds];
    double record = 1000000;
    int seed = 1001;
    double avg_meet = rounds * 3 / (double) pc;
    double avg_pos = rounds / (double) 4;
    while (seed++ < 2000)
    {
        twister.seed(seed);

        for (int i = 0; i < rounds; i++)
        {
            int player[pc];
            for (int j = 0; j < pc; j++)
            {
                player[j] = j;
            }
            for (int j = 0; j < pc; j++)
            {
                int swap = twister.randInt(pc - 1);
                int p = player[swap];
                player[swap] = player[j];
                player[j] = p;
            }
            for (int j = 0; j < tc; j++)
            {
                for (int k = 0; k < 4; k++)
                {
                    table[j][k][i] = player[j * 4 + k];
                }
            }
        }

        int iters = 10000;
        while (iters-- > 0)
        {
            int pos1, pos2, seat1, seat2, table1, table2, p1, p2;
            int r = twister.randInt(rounds - 1);
            double bad[2];
            vector<string> names;
            for (int b = 0; b < 2; b++)
            {
                // Goodness calc
                int meet[pc][pc];
                int pos[pc][4];
                for (int i = 0; i < pc; i++)
                {
                    for (int j = 0; j < pc; j++)
                    {
                        meet[i][j] = 0;
                    }
                    for (int j = 0; j < 4; j++)
                    {
                        pos[i][j] = 0;
                    }
                }
                for (int i = 0; i < rounds; i++)
                {
                    for (int j = 0; j < tc; j++)
                    {
                        for (int k = 0; k < 4; k++)
                        {
                            pos[table[j][k][i]][k]++;
                            for (int l = 0; l < 4; l++)
                            {
                                if (l == k)
                                {
                                    continue;
                                }
                                int pl1 = table[j][k][i];
                                int pl2 = table[j][l][i];
                                meet[pl1][pl2]++;
                                meet[pl2][pl1]++;
                            }
                        }
                    }
                }
                double badness = 0;
                for (int i = 0; i < pc; i++)
                {
                    for (int j = i + 1; j < pc; j++)
                    {
                        int m = meet[i][j] / 2;
                        double a = abs(m - avg_meet);
                        double x = 5;
                        if (a >= 1)
                        {
                            if (m == 0 && avg_meet >= 1)
                            {
                                x = 50;
                            }
                            badness += x * a;
                        }
                        if (b == 1)
                        {
                            stringstream ss;
                            ss << players[i] << " vs " << players[j] << " " << m;
                            names.push_back(ss.str());
                        }
                    }
                    /*
                    for (int j = 0; j < 4; j++)
                    {
                        double a = abs(pos[i][j] - avg_pos);
                        if (a < 1)
                        {
                            badness += a;
                        }
                     }
                    */
                }
                bad[b] = badness;
                if (b == 0)
                {
                    pos1 = twister.randInt(pc - 1);
                    seat1 = pos1 / tc;
                    table1 = pos1 % tc;
                    p1 = table[table1][seat1][r];
                    vector<int> good;
                    for (int i = 0; i < tc; i++)
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            p2 = table[i][j][r];
                            int m = meet[p1][p2] / 2;
                            double a = abs(m - avg_meet);
                            if (a < 1)
                            {
                                pos2 = i * 4 + j;
                                good.push_back(pos2);
                            }
                            else
                            {
                                for (int k = 0; k < 4; k++)
                                {
                                    if (j == k)
                                    {
                                        continue;
                                    }
                                    m = meet[j][k] / 2;
                                    a = abs(m - avg_meet);
                                    if (a >= 1)
                                    {
                                        pos2 = i * 4 + j;
                                        good.push_back(pos2);
                                    }
                                }
                            }
                        }
                    }
                    if (!good.empty() && twister.randInt(1000) > 600)
                    {
                        int index = twister.randInt(good.size() - 1);
                        pos2 = good[index];
                    }
                    else
                    {
                        pos2 = twister.randInt(pc - 1);
                        while (pos1 == pos2)
                        {
                            pos2 = twister.randInt(pc - 1);
                        }
                    }
                    seat2 = pos2 / tc;
                    table2 = pos2 % tc;
                    p2 = table[table2][seat2][r];

                    table[table1][seat1][r] = p2;
                    table[table2][seat2][r] = p1;
                }
                else
                {
                    bool revert = false;
                    if (bad[0] > bad[1])
                    {
                        if (twister.randInt(1000) > 200)
                        {
                            // Revert
                            table[table1][seat1][r] = p1;
                            table[table2][seat2][r] = p2;
                            revert = true;
                        }
                    }
                    if (!revert && bad[1] < record)
                    {
                        record = bad[1];
                        // Print tables
                        for (int i = 0; i < rounds; i++)
                        {
                            cout << endl;
                            cout << "ROUND " << i << endl;
                            for (int j = 0; j < tc; j++)
                            {
                                cout << "Table " << j << ": ";
                                for (int k = 0; k < 4; k++)
                                {
                                    cout << players[table[j][k][i]] << " ";
                                }
                                cout << endl;
                            }
                        }
                        cout << "Badness: " << bad[1] << endl;
                        for (int i = 0; i < names.size(); i++)
                        {
                            cout << names[i] << endl;
                        }
                    }

                }
            }
        }
    }
}
