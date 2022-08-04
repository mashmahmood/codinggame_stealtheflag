// THE CODE IS Extremely MESSY. I did not progress with any plan whatsoever, and kept writing to only accomplish the tasks.
// I will try to refactor the code later. For now, please see the documentation to get a better understanding of mechanisms of this solution: https://github.com/mashmahmood/codinggame_stealtheflag

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

int map[20][35];
int wid, hei;
int flag_r, flag_c; //MY flag
string moves[5]; //The output commands for my minions
int roles[5];   //The roles of my minions (3 = runner, 1 = troller, 0 = defender, 2 = collector, -1 = trapped within grid)


//TO only SORT MY MINIONS On distance to Opponent's flag base
struct Minion{
    int id, dist;
    Minion(int _id = 0, int _d = 0){
        id = _id;
        dist = _d;
    }
};

 
//TO KEEP TRACK OF DATA OF MY MINIONS
struct My_Minion{
    int id, pos_r, pos_c, prev_health, health;
    My_Minion(int _id = -1, int _r = 0, int _c = 0, int _h = 100){
        id = _id;
        pos_r = _r;
        pos_c = _c;
        prev_health = _h;
        health = _h;
    }
};

My_Minion my_mins[5];   //DATA OF MY MINIONS


//COMPARE FUNCTION TO SORT MINIONS (on distance to op flag base)
struct CompareMinion {
    bool operator()(Minion const& c1, Minion const& c2)
    {
        return c1.dist > c2.dist;
    }
};


//I use limited BFS to find near escape (away from opponent's fire) or find paths with many coins

//I use this struct to use limited BFS on cells, with a counter called step.
struct State{
    int pos_r, pos_c, step;
    State(int _r, int _c, int _s){
        pos_r = _r;
        pos_c = _c;
        step = _s;
    }
};

//For coins, I use an additional counter which counts the number of coins in that BFS PATH
struct Coin_State{
    int pos_r, pos_c, step, cnt;
    Coin_State(int _r, int _c, int _s, int _cnt){
        pos_r = _r;
        pos_c = _c;
        step = _s;
        cnt = _cnt;
    }
};

//Calculate Shortest path by "limited" BFS from (i, j) to (t_r, t_c)
//Limited BFS means, if the length of the path crosses a limit, it will return no path 
int shortestPath(int i, int j, int t_r, int t_c){
    queue<State> Q;
    int visited[20][35];
    for (int p = 0; p < 20; p++){
        for (int q = 0; q < 35; q++)
            visited[p][q] = 0;
    }
    Q.push(State(i, j, 0));

    while(!Q.empty())
    {
        State s = Q.front();
        Q.pop();
        //debug: cerr << s.pos_r << " " << s.pos_c << endl;

        if (s.pos_r == t_r && s.pos_c == t_c)
            return s.step;

        if (visited[s.pos_r][s.pos_c] || !map[s.pos_r][s.pos_c])
            continue;

        visited[s.pos_r][s.pos_c] = 1;

        if (s.pos_r != 0)
            Q.push(State(s.pos_r-1, s.pos_c, s.step+1));
        if (s.pos_r != hei-1)
            Q.push(State(s.pos_r+1, s.pos_c, s.step+1));
        if (s.pos_c != 0)
            Q.push(State(s.pos_r, s.pos_c-1, s.step+1));
        if (s.pos_c != wid-1)
            Q.push(State(s.pos_r, s.pos_c+1, s.step+1));

        
    }
    
    return -1;
}

//By BFS, figure out paths where there are more coins
pair<int,int> goodCoinPath(int i, int j){
    queue<Coin_State> Q;
    int visited[20][35];
    Coin_State best = Coin_State(-1, -1, 0, -1);

    for (int p = 0; p < 20; p++){
        for (int q = 0; q < 35; q++)
            visited[p][q] = 0;
    }
    Q.push(Coin_State(i, j, 0, 0));

    while(!Q.empty())
    {
        Coin_State s = Q.front();
        Q.pop();
        //debug: cerr << s.pos_r << " " << s.pos_c << endl;


        if (visited[s.pos_r][s.pos_c] || !map[s.pos_r][s.pos_c] || s.step > 10){
            continue;
        }        
        visited[s.pos_r][s.pos_c] = 1;
        
        if (map[s.pos_r][s.pos_c] == 2){
            s.cnt++;
        }
            //return {s.pos_r, s.pos_c};
        if (s.cnt > best.cnt){
            best = s;
        }

        if (s.pos_r != 0)
            Q.push(Coin_State(s.pos_r-1, s.pos_c, s.step+1, s.cnt));
        if (s.pos_r != hei-1)
            Q.push(Coin_State(s.pos_r+1, s.pos_c, s.step+1, s.cnt));
        if (s.pos_c != 0)
            Q.push(Coin_State(s.pos_r, s.pos_c-1, s.step+1, s.cnt));
        if (s.pos_c != wid-1)
            Q.push(Coin_State(s.pos_r, s.pos_c+1, s.step+1, s.cnt));
        
    }
    
    if (best.pos_c == -1)
        return {-1, -1};
    else{
        map[best.pos_r][best.pos_c] = 1;
        return {best.pos_r, best.pos_c};
    }
}

// Escape from opponent's fire using BFS to determine safe cells
// danger_r and danger_c are consecutively row and column of opponent's position.
pair<int,int> nearestSafe(int i, int j, int danger_r, int danger_c){
    queue<State> Q;
    int visited[20][35];
    for (int p = 0; p < 20; p++){
        for (int q = 0; q < 35; q++)
            visited[p][q] = 0;
    }
    Q.push(State(i, j, 0));

    while(!Q.empty())
    {
        State s = Q.front();
        Q.pop();
        //debug: cerr << s.pos_r << " " << s.pos_c << endl;

        if (map[s.pos_r][s.pos_c] && (s.pos_r != danger_r || danger_r == -1) && (s.pos_c != danger_c || danger_c == -1))
            return {s.pos_r, s.pos_c};

        if (visited[s.pos_r][s.pos_c] || !map[s.pos_r][s.pos_c] || s.step > 1){
            visited[s.pos_r][s.pos_c] = 1;
            continue;
        }        

        if (s.pos_r != 0)
            Q.push(State(s.pos_r-1, s.pos_c, s.step+1));
        if (s.pos_r != hei-1)
            Q.push(State(s.pos_r+1, s.pos_c, s.step+1));
        if (s.pos_c != 0)
            Q.push(State(s.pos_r, s.pos_c-1, s.step+1));
        if (s.pos_c != wid-1)
            Q.push(State(s.pos_r, s.pos_c+1, s.step+1));
        
    }
    
    return {-1, -1};
}

int main()
{
    int height;
    int width;
    cin >> height >> width; cin.ignore();
    wid = width;
    hei = height;

    for (int i = 0; i < 5; i++)
        moves[i] = "";

    // MAP DATA
    for (int i = 0; i < height; i++) {
        string row;
        cin >> row; cin.ignore();
        for (int j = 0; j < width; j++)
            if (row[j] == '.')
                map[i][j] = 2;
            else
                map[i][j] = 0;
    }
    int my_flag_base_r;
    int my_flag_base_c;
    cin >> my_flag_base_r >> my_flag_base_c; cin.ignore();
    int opponent_flag_base_r;
    int opponent_flag_base_c;
    cin >> opponent_flag_base_r >> opponent_flag_base_c; cin.ignore();
    flag_r = opponent_flag_base_r;
    flag_c = opponent_flag_base_c;


    map[my_flag_base_r][my_flag_base_c] = 1;
    map[opponent_flag_base_r][opponent_flag_base_c] = 1;

    string fire_name;
    int fire_price;
    int fire_damage;
    cin >> fire_name >> fire_price >> fire_damage; cin.ignore();
    string freeze_name;
    int freeze_price;
    int freeze_damage;
    cin >> freeze_name >> freeze_price >> freeze_damage; cin.ignore();
    string mine_name;
    int mine_price;
    int mine_damage;
    cin >> mine_name >> mine_price >> mine_damage; cin.ignore();

    
    bool firstMove = true;
    int defender = -1;
    int troller = -1;
    int runner = -1;
    int defFireSequence = 4;
    int trollFireSequence = 4;

    int troll_position_r = opponent_flag_base_r;
    int troll_position_c = opponent_flag_base_c;
    int defender_position_r = my_flag_base_r;
    int defender_position_c = my_flag_base_c;
    int defender_target_pos_r = my_flag_base_r;
    int defender_target_pos_c = my_flag_base_c;
    int runner_pos_r;
    int runner_pos_c;

    
    //Determine where the opponent defender will most probably be
    if (map[opponent_flag_base_r+1][opponent_flag_base_c])
        troll_position_r++;
    else if (map[opponent_flag_base_r-1][opponent_flag_base_c])
        troll_position_r--;
    else if (map[opponent_flag_base_r][opponent_flag_base_c+1])
        troll_position_c++;
    else if (map[opponent_flag_base_r][opponent_flag_base_c-1])
        troll_position_c--;

    //Determine a good spot for my defender to defend my flag
    if (map[my_flag_base_r+1][my_flag_base_c])
        defender_target_pos_r++;
    else if (map[my_flag_base_r-1][my_flag_base_c])
        defender_target_pos_r--;
    else if (map[my_flag_base_r][my_flag_base_c+1])
        defender_target_pos_c++;
    else if (map[my_flag_base_r][my_flag_base_c-1]){
        defender_target_pos_c--;
    }

    
    bool attacking = false; //Have we commenced attacking = is our runner/ attacker nearby opponent flag base
    int collect_timer = 16; // A Timer of the game, for some weird reason I had set it to 16 and not changed
    int dodge_counter[5] = {2, 2, 2, 2, 2}; // Number of dodges (jiggles), i-th minion will do
    bool enough_score = false;  // DO I have enough score to fire
    bool defenderReached = false;   // Have my defender reached the position to defend my flag
    bool trollReached = false;  // Is the troller (2nd runner) nearby opponent flag base
    int runner_shoot = 8;   // A counter to wait for turns in between using freeze by runner
    bool botKiller = false; // IS our opponent playing in aggressive mode
    bool extremeBotKiller = false;  // very aggressive mode
    bool botKillerCheck = false;    // we complete an initial check to determine if our opponent is playing aggressive
    float botTimer = 15;    // If our opponent uses power ups within these turns, he/ she most likely is playing aggressive
    bool defSeen = false;   // Have our defender seen an enemy for at least 2 turns
    bool runnerSeen = false;   // Have our runner seen an enemy for at lesat 2 turns
    bool firstShoot = false;    // Is our defender shooting for the first time... This helps differentiate opponents who use jiggle and who do not
    bool runnerReached = false;    // Is our runner nearby opponent flag base
    int defSeenTimer = 3;   // timer for Last when did my defender seen an enemy
    bool tooFarTroll = true;    // Is our 2nd runner too far away from my main runner

    // game loop
    while (1) {
        int my_score;
        int opponent_score;
        cin >> my_score >> opponent_score; cin.ignore();
        if (my_score >= 7)
            enough_score = true;
        if (my_score < 7)
            enough_score = false;

        int my_flag_pos_r;
        int my_flag_pos_c;
        int my_flag_carrier;
        cin >> my_flag_pos_r >> my_flag_pos_c >> my_flag_carrier; cin.ignore();
        int opponent_flag_pos_r;
        int opponent_flag_pos_c;
        int opponent_flag_carrier;
        cin >> opponent_flag_pos_r >> opponent_flag_pos_c >> opponent_flag_carrier; cin.ignore();
        if (opponent_flag_carrier != -1){
            flag_r = opponent_flag_pos_r;
            flag_c = opponent_flag_pos_c;
            runner = opponent_flag_carrier;
        }
        if (my_flag_carrier != -1)
            defenderReached = true;

        
        priority_queue<Minion, vector<Minion>, CompareMinion> PQ;

        bool isDefenderAlive = false;
        bool isTrollerAlive = false;
        bool isRunnerAlive = false;
        bool needSaving = false;
        

        for (int i = 0; i < 5; i++)
            my_mins[i].id = -1;

        int my_alive_minion_cnt;
        cin >> my_alive_minion_cnt; cin.ignore();
        
        for (int i = 0; i < my_alive_minion_cnt; i++) {
            int id;
            int pos_r;
            int pos_c;
            int health;
            int timeout;
            cin >> id >> pos_r >> pos_c >> health >> timeout; cin.ignore();

            int target_x = opponent_flag_pos_r, target_y = opponent_flag_pos_c;
            map[pos_r][pos_c] = 1;  // there is no coin in this cell as our bot is already in this position

            my_mins[id].id = id;
            my_mins[id].pos_r = pos_r;
            my_mins[id].pos_c = pos_c;
            my_mins[id].health = health;

            if (pos_c == troll_position_c)
                runnerReached = true;
            
            if (firstMove){
                int distance = shortestPath(pos_r, pos_c, opponent_flag_pos_r, opponent_flag_pos_c);
                if (distance != -1)
                    PQ.push(Minion(id, distance));
                else
                    roles[id] = -1;
            }
            else{
                if (defender == id){
                    isDefenderAlive = true;
                    
                    target_x = defender_target_pos_r;
                    target_y = defender_target_pos_c;
                    defender_position_r = pos_r;
                    defender_position_c = pos_c;

                    if (pos_r == defender_target_pos_r && pos_c == defender_target_pos_c)
                        defenderReached = true;

                    if (id == opponent_flag_carrier){
                        defender = -1;
                        runner = id;
                        roles[id] = 3;
                    }
                    if (id == 1)
                            cerr << defender_target_pos_r;
                }
                if (troller == id){
                    if (abs(pos_c - runner_pos_c) < 4 && abs(pos_r - runner_pos_r) < 3)
                        tooFarTroll = false;
                    if (abs(pos_c - runner_pos_c) > 5 || abs(pos_r - runner_pos_r) > 4)
                        tooFarTroll = true;

                    if (opponent_flag_carrier != -1 && my_mins[runner].health < 10 && health > 52){
                        needSaving = true;
                    }
                    else{
                        if (!trollReached){
                            target_x = troll_position_r;
                            target_y = troll_position_c;
                        }
                        else{
                            if (target_x + 2 < hei)
                            {
                                if (map[target_x+2][target_y])
                                    target_x += 2;
                            }
                            else if (target_x + 1 < hei){
                                if (map[target_x+1][target_y])
                                    target_x += 1;
                            }
                        }
                    }
                    isTrollerAlive = true;

                    if (abs(pos_r - troll_position_r) < 6 && abs(pos_c - troll_position_c) < 8){
                        trollReached = true;
                    }

                    if (id == opponent_flag_carrier){
                        troller = -1;
                        runner = id;
                        roles[id] = 3;
                    }
                }
                if (runner == id){
                    isRunnerAlive = true;
                    runner_pos_r = pos_r;
                    runner_pos_c = pos_c;
                    if (abs(pos_c - opponent_flag_pos_c) < 6)
                        attacking = true;
                }

                bool collecting = true;

                if (runner != id && (roles[id] == 2 || collect_timer > 0 || !enough_score || id == troller || roles[id] == -1)){
                    //cerr << id << endl;
                    if (id == defender && (collect_timer < 7 || !defenderReached || my_flag_carrier != -1))
                        collecting = false;

                    if (id == troller && 
                            (
                                opponent_flag_carrier == -1 && 
                                 ((!trollReached && (collect_timer > 6 || collect_timer < -10)) || (tooFarTroll && my_score > 35))
                            ) 
                        )
                        collecting = false;
                    if (id == troller && needSaving)
                        collecting = false;
                    if (id == troller && !enough_score && my_alive_minion_cnt < 3)
                        collecting = false;

                    //cerr << isRunnerAlive << " r" << endl;
                    if (collecting){
                        //cerr << id << endl;
                        pair<int, int> pairI = goodCoinPath(pos_r, pos_c);
                        if (pairI.first != -1 && !(pairI.first == pos_r && pairI.second == pos_c)){
                            target_x = pairI.first;
                            target_y = pairI.second;
                            map[target_x][target_y] = 1;
                        }
                    }
                }

                if (id == runner){
                    if (opponent_flag_carrier != -1 || (runner_pos_r == opponent_flag_pos_r && runner_pos_c == opponent_flag_pos_c)){
                        target_x = my_flag_base_r;
                        target_y = my_flag_base_c;
                    }
                }
            }
            
            moves[id] = "MOVE " + to_string(id) + " " + to_string(target_x) + " " + to_string(target_y);
            
        }


        if (firstMove){
            //cerr << "hello" << endl;
            isRunnerAlive = true;
            isDefenderAlive = true;
            isTrollerAlive = true;

            firstMove = false;  

            Minion m = PQ.top();
            PQ.pop();
            roles[m.id] = 3;
            runner = m.id;
            //dodge_counter[runner] = 3;

            
            if (PQ.size() != 0 ){
                m = PQ.top();
                PQ.pop();
            }
            /*
            if (my_alive_minion_cnt == 3 && m.dist > 20){
                roles[m.id] = 0;
                defender = m.id;

                if (PQ.size() != 0 ){
                    m = PQ.top();
                    PQ.pop();
                    roles[m.id] = 1;
                    troller = m.id;
                }
            }
            else*/
            
            roles[m.id] = 1;
            troller = m.id;

            while(PQ.size() > 1){
                m = PQ.top();
                roles[m.id] = 2;
                PQ.pop();
            }

            if (PQ.size() != 0 ){
                m = PQ.top();
                PQ.pop();
                roles[m.id] = 0;
                defender = m.id;
            }
            //


            cerr << runner << troller << defender;
        }
        else{
            if (!isDefenderAlive){
                defender = -1;
            }
            if (!isTrollerAlive){
                troller = -1;
            }
            if (!isRunnerAlive){
                runner = -1;
                
            }

            if (my_alive_minion_cnt == 2 && my_score < 14 && abs(my_flag_pos_c - opponent_flag_base_c) > 24 && my_flag_carrier  == -1 && defSeenTimer < 0){
                roles[defender] = 2;
                defender = -1;
            }
        }
        

        int visible_minion_cnt;
        cin >> visible_minion_cnt; cin.ignore();
        bool runnerOnMove = false;  //will our runner be jiggling
        bool trollerOnMove = false; //will our 2nd runner be jiggling
        string updateDef = "";  //this is to store new move (for defender)... if proper conditions fulfil, then override the previous MOVE command with this new command
        string updateRun = "";  //similar (for runner)

        bool defnewSeen = false;  // Defender seen an enemy this frame
        bool runnerSeenTmp = false; // Runner seen an enemy this frame
        
        for (int i = 0; i < visible_minion_cnt; i++) {
            int id;
            int pos_r;
            int pos_c;
            int health;
            int timeout;
            cin >> id >> pos_r >> pos_c >> health >> timeout; cin.ignore();

            //DODGE SYSTEM
            
            int danger_r, danger_c;
            for (int j = 0; j < 5; j++){
                danger_r = -1; danger_c = -1;   // stores which row and column is our enemy in

                if (my_mins[j].id != -1 && (extremeBotKiller || my_mins[j].id != defender)){

                    int from, to;
                    
                    bool seen = true;
                    if (pos_c != my_mins[j].pos_c && pos_r != my_mins[j].pos_r)  
                        seen = false;  
                    else{

                        if (pos_c == my_mins[j].pos_c){
                            //cerr << "how tho" << my_mins[j].id << endl;
                            //cerr << my_mins[j].id << " " << id << endl;

                            if (my_mins[j].pos_r < pos_r)
                            {
                                from = my_mins[j].pos_r;
                                to = pos_r;
                            }
                            else{
                                from = pos_r;
                                to = my_mins[j].pos_r;
                            }

                            for (int z = from; z < to; z++){
                                if (!map[z][pos_c])
                                {
                                    seen = false;
                                    break;
                                }
                            }
                        }
                        else
                            seen = false;

                        if (seen)
                            danger_c = pos_c;

                        seen = true;

                        if (pos_r == my_mins[j].pos_r){
                            if (my_mins[j].pos_c < pos_c)
                            {
                                from = my_mins[j].pos_c;
                                to = pos_c;
                            }
                            else{
                                from = pos_c;
                                to = my_mins[j].pos_c;
                            }

                            for (int z = from; z < to; z++){
                                if (!map[pos_r][z])
                                {
                                    seen = false;
                                    break;
                                }
                            }
                        }
                        else
                            seen = false;

                        if (seen)
                            danger_r = pos_r;
                    }

                    //cerr << danger_r << " " << danger_c << " " << my_mins[j].id << my_mins[j].pos_c << endl;

                    if (danger_r != -1 || danger_c != -1)
                    {
                        bool defend_myself = false;

                        if (id != my_flag_carrier)
                        {
                            if (roles[my_mins[j].id] == 2 && !extremeBotKiller && (my_score <= 7 || my_alive_minion_cnt <= 3))
                                continue;
                            if (my_mins[j].id == runner && opponent_flag_carrier == -1 && abs(my_mins[j].pos_c - opponent_flag_pos_c) < 3 && abs(opponent_flag_base_c - opponent_flag_pos_c) < 3 ){
                                int s = shortestPath(runner_pos_r, runner_pos_c, opponent_flag_pos_r, opponent_flag_pos_c);
                                if (s < 3)
                                    continue;
                            }

                            if (dodge_counter[my_mins[j].id] > 0 && (opponent_score >= 6 || collect_timer > 0) && ((pos_c == troll_position_c && pos_r == troll_position_r) || my_mins[j].health < 100 || botKiller) && abs(my_flag_base_c - opponent_flag_pos_c) > 7 && timeout == 0){
                                
                                if (my_mins[j].id == runner && opponent_score / 7 < my_mins[j].health / 24 && abs(my_mins[j].pos_c - opponent_flag_base_c) < 12)
                                    continue;

                                if (my_mins[j].id == opponent_flag_carrier && opponent_score/7 < my_mins[j].health/24)
                                    continue;

                                if (my_mins[j].id == opponent_flag_carrier && !botKiller && (my_mins[j].health > 30 || abs(my_mins[j].pos_c - opponent_flag_base_c) < 4) )
                                    continue; 
                                pair<int, int> new_move = nearestSafe(my_mins[j].pos_r, my_mins[j].pos_c, danger_r, danger_c);
                                if (new_move.first != -1){
                                    if (!(my_mins[j].pos_r == new_move.first && my_mins[j].pos_c == new_move.second))
                                    {
                                        
                                        if (my_mins[j].id == runner){
                                            if (opponent_flag_carrier != -1 && runnerReached && (new_move.second - my_mins[j].pos_c)*(my_flag_base_c - my_mins[j].pos_c) < 0)
                                                continue;
                                            runnerOnMove = true;
                                        }
                                        if (my_mins[j].id == troller)
                                            trollerOnMove = true;
                                        
                                        dodge_counter[my_mins[j].id]--;
                                        moves[my_mins[j].id] = "MOVE " + to_string(my_mins[j].id) + " " + to_string(new_move.first) + " " + to_string(new_move.second);
                                    }
                                }
                                else if (runner != my_mins[i].id){
                                    defend_myself = true;
                                }
                            }
                        }
                        if (my_score >= 7 && (defend_myself || id == my_flag_carrier)) {
                            
                            bool toShoot = true;
                            if (my_mins[j].id != runner)
                            {
                                bool friendly_fire = true;
                                //cerr << danger_c << " "<< danger_r << endl;
                                //cerr << runner_pos_c << " " << defender_position_c << endl;
                                if (runner_pos_r != my_mins[j].pos_r && runner_pos_c != my_mins[j].pos_c)
                                    friendly_fire = false;

                                if (runner_pos_r == my_mins[j].pos_r){
                                    //cerr << "whoa" << endl;
                                    if (my_mins[j].pos_c < runner_pos_c)
                                    {
                                        from = my_mins[j].pos_c;
                                        to = runner_pos_c;
                                    }
                                    else{
                                        from = runner_pos_c;
                                        to = my_mins[j].pos_c;
                                    }

                                    for (int z = from; z < to; z++){
                                        if (!map[runner_pos_r][z])
                                        {
                                            friendly_fire = false;
                                            break;
                                        }
                                    }
                                }

                                if (runner_pos_c == my_mins[j].pos_c){
                                    
                                    if (my_mins[j].pos_r < runner_pos_r)
                                    {
                                        from = my_mins[j].pos_r;
                                        to = runner_pos_r;
                                    }
                                    else{
                                        from = runner_pos_r;
                                        to = my_mins[j].pos_r;
                                    }

                                    for (int z = from; z < to; z++){
                                        if (!map[z][runner_pos_c])
                                        {
                                            friendly_fire = false;
                                            break;
                                        }
                                    }
                                }
                                if (friendly_fire)
                                    toShoot = false;
                            }
                            else{
                                
                                if (runner != opponent_flag_carrier || health < 25){
                                    toShoot = true;
                                    //cerr << runner;
                                }
                            }

                            if (defend_myself && my_mins[j].health / 24 < health / 24)
                                toShoot = false;

                            if (runner == my_mins[j].id && id != my_flag_carrier)
                                toShoot = false;

                            if (toShoot){
                                if (id == my_flag_carrier && abs(my_flag_pos_c - opponent_flag_base_c) < 10 && my_score >= 12 && health > 25){
                                    if (timeout == 0)
                                        moves[my_mins[j].id] = "FREEZE " + to_string(my_mins[j].id);
                                    //cerr << "really?" << my_mins[j].id << endl;
                                    //cerr << "taskete" << endl;
                                }
                                else if (id == my_flag_carrier)
                                    moves[my_mins[j].id] = "FIRE " + to_string(my_mins[j].id);
                            }

                            if (id == my_flag_carrier && !isDefenderAlive && my_mins[j].id != runner){
                                roles[my_mins[j].id] = 0;
                                defender = my_mins[j].id;
                                isDefenderAlive = true;
                            }
                        }
                    }
                    
                    if ((danger_r != -1 || danger_c != -1) && my_mins[j].id == runner){
                        runnerSeenTmp = true;
                        runnerSeen = true;
                    }
                }
            }
   

            //WORK OF DEFENDER
            bool shoot = false;
            bool friendly_fire = true;
            
            if (my_score >= 7 && defenderReached && (pos_r == defender_position_r || pos_c == defender_position_c))
                shoot = true;
            if (my_flag_carrier != -1 && id != my_flag_carrier)
                shoot = false;
            if (defender != -1 && shoot){
                int from, to;
                //cerr << runner_pos_c << " " << defender_position_c << endl;
                if (health < my_mins[runner].health && my_score / 7 > health / 24)
                    friendly_fire = false;

                if (runner_pos_r != defender_position_r && runner_pos_c != defender_position_c)
                    friendly_fire = false;

                if (runner_pos_r == defender_position_r){
                    //cerr << "whoa" << endl;
                    if (defender_position_c < runner_pos_c)
                    {
                        from = defender_position_c;
                        to = runner_pos_c;
                    }
                    else{
                        from = runner_pos_c;
                        to = defender_position_c;
                    }

                    for (int z = from; z < to; z++){
                        if (!map[runner_pos_r][z])
                        {
                            friendly_fire = false;
                            break;
                        }
                    }
                }

                if (runner_pos_c == defender_position_c){
                    
                    if (defender_position_r < runner_pos_r)
                    {
                        from = defender_position_r;
                        to = runner_pos_r;
                    }
                    else{
                        from = runner_pos_r;
                        to = defender_position_r;
                    }

                    for (int z = from; z < to; z++){
                        if (!map[z][runner_pos_c])
                        {
                            friendly_fire = false;
                            break;
                        }
                    }
                }

                if (pos_r == defender_position_r && pos_c == defender_position_c)
                    shoot = true;
                else {

                    if (pos_c == defender_position_c){
                        
                        if (defender_position_r < pos_r)
                        {
                            from = defender_position_r;
                            to = pos_r;
                        }
                        else{
                            from = pos_r;
                            to = defender_position_r;
                        }

                        for (int z = from; z < to; z++){
                            if (!map[z][pos_c])
                            {
                                shoot = false;
                                break;
                            }
                        }
                    }

                    if (pos_r == defender_position_r){
                        if (defender_position_c < pos_c)
                        {
                            from = defender_position_c;
                            to = pos_c;
                        }
                        else{
                            from = pos_c;
                            to = defender_position_c;
                        }

                        for (int z = from; z < to; z++){
                            if (!map[pos_r][z])
                            {
                                shoot = false;
                                break;
                            }
                        }
                    }
                    if (shoot){
                        defnewSeen = true;
                        if (id == my_flag_carrier)
                            defSeenTimer = 3;
                    }
                }
                if (shoot && (!friendly_fire || (my_mins[runner].health > 25 && health <= 24 && id == my_flag_carrier))){/*
                    if (defFireSequence == 4 && my_score >= 19){
                        updateDef = "FREEZE " + to_string(defender);
                        //freezeTime = 4;
                    }
                    */

                    if (defSeen || (my_flag_carrier != -1) || !firstShoot){
                        //cerr << defSeen << endl;
                        firstShoot = true;
                        if ( !(my_flag_pos_r == my_flag_base_r && my_flag_pos_c == my_flag_base_c)
                            && (my_score / 7 < health / 24 || (abs(my_flag_pos_c - opponent_flag_base_c) < 5) && health > 24) 
                            && (abs(my_flag_pos_c - opponent_flag_base_c) < 15 || my_alive_minion_cnt <= 3) 
                            && my_score >= 12){
                            if (timeout == 0)
                                updateDef = "FREEZE " + to_string(defender);
                        }
                        else
                            updateDef = "FIRE " + to_string(defender);
                    }
                    /*
                    defFireSequence -= 1;
                    if (defFireSequence < 0)
                        defFireSequence = 4;
                    */
                }
            }

            //WORK OF RUNNER
            shoot = false;
            
            if (my_score >= 7 &&  runnerSeenTmp && timeout == 0 && !extremeBotKiller && (id == my_flag_carrier || ( (dodge_counter[runner] <= 0 || !runnerOnMove) && opponent_score >= 7 ) )){
                if ( (pos_r == troll_position_r && (pos_c == troll_position_c) && my_score >= 45 && abs(runner_pos_c - opponent_flag_pos_c) < 5) 
                || ( abs(my_flag_pos_c - opponent_flag_base_c) > 24 && runnerReached && abs(runner_pos_c - pos_c) < 4 && abs(runner_pos_c - opponent_flag_pos_c) < 5  && my_score >= 45)
                || ( id == my_flag_carrier) )
                    shoot = true;
            }
            if (runner != -1 && shoot){
                if (pos_r != runner_pos_r && pos_c != runner_pos_c)
                    shoot = false;
                else{
                    int from, to;
                    if (pos_c == runner_pos_c){
                        
                        if (runner_pos_r < pos_r)
                        {
                            from = runner_pos_r;
                            to = pos_r;
                        }
                        else{
                            from = pos_r;
                            to = runner_pos_r;
                        }

                        for (int z = from; z < to; z++){
                            if (!map[z][pos_c])
                            {
                                shoot = false;
                                break;
                            }
                        }
                    }

                    if (pos_r == runner_pos_r){
                        if (runner_pos_c < pos_c)
                        {
                            from = runner_pos_c;
                            to = pos_c;
                        }
                        else{
                            from = pos_c;
                            to = runner_pos_c;
                        }

                        for (int z = from; z < to; z++){
                            if (!map[pos_r][z])
                            {
                                shoot = false;
                                break;
                            }
                        }
                    }
                }
                cerr << runner_pos_r << " " << runner_pos_c << " " << pos_r << " " << pos_c << endl;
                if (shoot){
                    if (runner_shoot == 8 && isRunnerAlive){
                        if (id == my_flag_carrier && my_score/7 >= health/24){
                            //cerr << "what now" << endl;
                            updateRun = "FIRE " + to_string(runner);
                        }
                        else{
                            if (opponent_flag_carrier == -1 || (opponent_flag_carrier != -1 && (my_score > 46 || id == my_flag_carrier) )) {
                                
                                if (my_score >= 12)
                                    updateRun = "FREEZE " + to_string(runner);
                                runner_shoot -= 1;
                            }
                        }
                    }
                }
            }
        }

        
        bool defShooting = false;
        //bool runShooting = false;
        bool trollShooting = false;
        if (updateDef.compare("") != 0 && isDefenderAlive){
            moves[defender] = updateDef;
            defShooting = true;
        }


        if (updateRun.compare("") != 0 && abs(my_flag_base_c - runner_pos_c) > 5){
            if (!defShooting || my_score >= 19)
                moves[runner] = updateRun;
        }

        int visible_coin_cnt;
        cin >> visible_coin_cnt; cin.ignore();
        for (int i = 0; i < visible_coin_cnt; i++) {
            int pos_r;
            int pos_c;
            cin >> pos_r >> pos_c; cin.ignore();
        }
    
        bool first = true;
        for (int i = 0; i < 5; i++) {
            if (my_mins[i].health < my_mins[i].prev_health){
                if (runner == i){
                    if (extremeBotKiller)
                        dodge_counter[i] = 4;
                    else if (botKiller)
                        dodge_counter[i] = 3;
                    else if (opponent_flag_carrier != -1 && my_mins[i].health > 10)
                        dodge_counter[i] = 2;
                    else
                        dodge_counter[i] = 1;
                }
                else{
                    if (extremeBotKiller)
                        dodge_counter[i] = 4;
                    else if (botKiller)
                        dodge_counter[i] = 3;
                    else
                        dodge_counter[i] = 2;
                }

                if (botTimer > 0){
                    botKiller = true;
                    for (int j = 0; j < 5; j++){
                        if (runner == j)
                            dodge_counter[j] = 5;
                        else
                            dodge_counter[j] = 4;
                    }

                    if (botTimer > 8){
                        extremeBotKiller = true;
                        for (int j = 0; j < 5; j++){
                            if (runner == j)
                                dodge_counter[j] = 6;
                            else
                                dodge_counter[j] = 5;
                        }
                    }
                    botTimer = -1;
                    botKillerCheck = true;
                }
                my_mins[i].prev_health = my_mins[i].health;
            }
           
            if (moves[i].compare("") != 0){
                if (!first)
                    cout << " | ";
                cout << moves[i];
                first = false;
            }
            moves[i] = "";
        }
        cout << endl;
        collect_timer -= 1;
        
        if (!defnewSeen)
            defSeen = false;
        else
            defSeen = true;

        if (!runnerSeenTmp)
            runnerSeen = false;
        defSeenTimer -=1;

        if (runner_shoot != 8)
            runner_shoot -= 1;
        if (runner_shoot < 0)
            runner_shoot = 8;
        
        if (!isRunnerAlive){
            priority_queue<Minion, vector<Minion>, CompareMinion> Q;
            for (int i = 0; i < 5; i++){
                if (my_mins[i].id != -1){
                    int distance = shortestPath(my_mins[i].pos_r, my_mins[i].pos_c, flag_r, flag_c);
                    if (distance != -1)
                        Q.push(Minion(my_mins[i].id, distance));
                }
            }
            Minion new_runner = Q.top();
            runner = new_runner.id;
            roles[new_runner.id] = 3;
            if (botKiller)
                dodge_counter[runner] = 4;

            if (runner == defender)
                defender = -1;
            if (runner == troller){
                troller = -1;
                isTrollerAlive = false;
            }
            isRunnerAlive = true;
        }

        int mini_id = -1;
        int mini_value = 50;
        if (opponent_flag_carrier != -1 && my_mins[runner].health < 20){
            for (int j = 0; j < 5; j++){
                if (my_mins[j].id != -1 && my_mins[j].health > 20 && abs(my_mins[runner].pos_c - my_mins[j].pos_c) < mini_value)
                {
                    mini_id = j;
                    mini_value = abs(my_mins[runner].pos_c - my_mins[j].pos_c);
                }
            }
            needSaving = true;
            if (mini_id != -1){
                troller = mini_id;
                roles[troller] = 1;
            }
        }

        
        if (!(my_flag_pos_r == my_flag_base_r && my_flag_pos_c == my_flag_base_c)){
            defender_target_pos_r = my_flag_pos_r;
            defender_target_pos_c = my_flag_pos_c;
        }

        botTimer -= 1;
        //freezeTime -= 1;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
    }
}