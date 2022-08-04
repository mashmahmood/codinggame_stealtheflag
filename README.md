# Steal the Flag
This was my solution for the game 'Steal the Flag', for the AI Contest organized in BUET CSE FEST 2022, in which it ranked 5th.
In the game, your bots will play functioning on your code, with opponents' bots (1 v 1 battle) functioning with codes written by opponents.

* I am not involved in any way, making of the game. I just participated in the contest where I wrote solutions and I am presenting them here. *

## For a basic understanding of rules and objectives of the game, visit here: 
https://www.codingame.com/contribute/view/24899fba3db2b90428fb4f3ecb1505df954dd

```
Note: This is a work in progress. This solution was written during the contest, for which I had no planning upfront and I just went along with the flow. So my code is a total mess. I will try to clean up my code. 
Here, I try to present the key ideas that I used in the solution. I will not dive into full depths of my code as that would make things complicated and probably incomprehensible. So, I will rather give an overview of my strategies and try to give a breakdown of approaches for the solution.
```
** At the end of text, I have added some description of key words I have used. **

### Basic summary of what to do / How to play
Many parameters will be given to you (as ** inputs **), such as position and helath of your minions and of * visible * opponent's minions , flag's positions and map data. You have to ** output ** commands for each of your minion for every ** turn **. The commands are:
```
  MOVE minionID x y: the minion with the identifier minionID moves towards the given cell following the shortest path.
  FIRE minionID: the minion with the identifier minionID uses FIRE.
  FREEZE minionID: the minion with the identifier minionID uses FREEZE.
  MINE minionID r c: the minion with the identifier minionID plants a mine at the cell (r, c) i.e at row r and column c.
  WAIT minionID: the minion with the identifier minionID does nothing.
```

### Identifying Key Points of the Game and Selecting Strategy
Since this is a battle of AI, it is important to analyze the elements and parameters of the game. Also, it is important to identify and predict the game, our opponents and properly utilizing the parameters given by the game.

Now, if you are familiar with the game, you may have noticed the main following conditions for winning.
- Capture the flag or,
- Destroy all Opponent Bots

For beginning, let us consider a few ** strategies ** one can take to win the game:
- Send at least one bot to capture the opponent flag
- Defending our own flag with one of our bots (using Power Ups to destroy or delay enemy bots)
- Good coin collection system by bots, since they allow us to use power ups
- Try to destroy all enemy bots using Power Ups (Fire)

A player can implement ** one ** of those above mentioned strategies or all of them or a ** good combination ** of them.

#### Randomness
The map for 1 v 1 battles were random. So, it is very difficult to obtain one single good solution that would dominate every other bots. For example, my solution very often was defeated by much lower ranked solutions in some maps. Especially, there are maps when the path between the flag bases contains a very straight path (without much turn and twists). In those maps, the minions get very exposed. So, if not handled properly, results would not be good. And my solution many times have failed in those conditions. That indicates, my solution has some major flaws. What I tried to do was play with probability. So, I tried to maintain that my solution would probably have a chance of winning against all sorts of bots.

### Summary of my strategies
1. I send one bot (closest to opponent's flag base) to capture the opponent flag (I call it ** runner **)
2. I send another of my bot which try to follow my runner closely. So that, if my runner fails to collect opponent flag, it can jump on the task. This bot plays dual roles. Other objective of it is to collect coins. (I call it ** troller ** )
3. I send one to defend my flag.
4. If there are more bots (more than 3), they will primarily collect coins. If my runner and troller are destroyed they may get assigned to those jobs.
5. My runner freezes the opponent defender of their flag if it senses danger (my minion's health is down or opponent is using fire too frequently.) 
6. My defender mainly uses fire to destroy opponent bots if they are in close vicinity of my flag. When there is not enough * credit * (score) to destroy opponent bot, it may use freeze to delay the opponent if possible.
7. If my flag is captured, my defender will follow the carrier of * my flag *.
8. My bots try to avoid fire of opponent bots by ** Jiggling **. But that is *only* on few conditions. One condition is, if they sense our opponent is playing ** rough ** (using power ups very frequently at the beinning phase of the game).

Note: I have not used the power up "MINE" at all. And I only use BFS without limit for the first turn (as there is 1000 ms to send output), but for other times, I mostly use with limited steps (as there is 50 ms to send output) ...

### Moving on to my solution
There are a lot of parameters to handle.
Firstly, there is map information and initial position of all bots and flags, position of flag bases.

#### Analyzing Map Data
For Map, I created a 2D array to keep track of empty cells, cells with coins and walls.
```
    for (int i = 0; i < height; i++) {
        string row;
        cin >> row; cin.ignore();
        for (int j = 0; j < width; j++)
            // 2 represents cell with coin, 1 represents empty cell, 0 represents wall
            // Initially, all cells (not walls) are filled with coins
            
            if (row[j] == '.')
                map[i][j] = 2;  
            else
                map[i][j] = 0;
    }
```
Also, based on where the * opponent flag base * and * my flag base * is, I determine where to send my * runner * and * defender *.

#### Keeping track of data for my bots
To keep track of data of my minions (bots; since they are called minions in the game, I will be referencing them as such in later text),
I used struct.
```
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
  
```

In the code, you can see another struct with similar name,
```
  struct Minion{
      int id, dist;
      Minion(int _id = 0, int _d = 0){
          id = _id;
          dist = _d;
      }
  };
```
This struct was ** not ** used to keep track of the data, rather to ** Sort ** my minions on distance to opponent flag base. So that, we can send the closest to capture the opponent flag. Also, send the farthest (from opponent's flag, which implies * most of the time *, closest to my flag base) to defend my flag. And assign rest of the bots to collect coins.

#### Using BFS to determine (to sort my minions and assign roles/ nearest safe cell to jiggle/ good coin path)
```
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
//Limited BFS means, if the length of the path crosses a limit, it will return no path (-1), otherwise, return the length of that path
int shortestPath(int i, int j, int t_r, int t_c)

//By BFS, figure out paths where there are more coins, return the indices (r, c) of the cell to move
pair<int,int> goodCoinPath(int i, int j)

// Escape from opponent's fire using BFS to determine safe cells
// danger_r and danger_c are consecutively row and column of opponent's position.
// return the indices (r, c) of the cell to move
pair<int,int> nearestSafe(int i, int j, int danger_r, int danger_c)
```

#### Giving Commands to my Minions
1. For my alive minions, depending on their roles, on each turn, they are initially given a location to move to (but that is not yet sent to output)
```
for (int i = 0; i < my_alive_minion_cnt; i++) {
            int id;
            int pos_r;
            int pos_c;
            int health;
            int timeout;
            
            int target_x = opponent_flag_pos_r, target_y = opponent_flag_pos_c;
            //...
            //...
            if (defender == id){
                    target_x = defender_target_pos_r;
                    target_y = defender_target_pos_c;
            }
            
            if (troller == id){
                //..
            }
            //...
            
            moves[id] = "MOVE " + to_string(id) + " " + to_string(target_x) + " " + to_string(target_y); // These are not the final outputs
}
```
2. Later, check for every visible opponent minion and for each of them... check every of MY minions... and compare positions and determine if my j-th bot can directly see the i-th opponent bot... based on that take some decisions (Do nothing/ Jiggle/ Fire/ Freeze). Fire/ Freeze is commonly used only if the seen opponent bot is carrying my flag. (This portion of the code is the largest... Because I did not write clean code and did not reuse codes... rather kept rewriting stuffs)

```
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
          danger_r = -1; danger_c = -1; // stores which row and column is our enemy in
          
          // conditions to check if this (mine) bot (j-th) has seen i-th enemy bot
          
          /* 
            if the above condition is true, then check for sub conditions, such as,
            
            is the enemy carrying my flag/ 
            is the enemy close to my flag/ 
            do i have enough score to destroy opponent or should i just use freeze to delay/
            which of MY bots has seen the enemy, because if it's my runner who is carrying opponent flag, it may be better to just ignore/
            if it's my defender and the opponent is trying to take my flag away/ 
            if I am going to use fire, will I cause friendly fire and would that friendly fire be costly, like, maybe destroy my OWN RUNNER/
            check credit of both opponent and me to determine what's the best action/
            check dodge counter -- have this minion of mine already dodged/ jiggled a few times/
            check if opponent previously played very aggressively -- then probably better to dodge/
            if my runner is carrying opponent flag and is in danger i.e., low health,
            
            .... and a few more of these types of micro adjustments/ strategies which I took based on performance of my bots against opponent bots
            
            Based on those subconditions, following decisions are made
            - do nothing
            - move away (jiggle)
            - use power (fire/ freeze)
          */
        }
    }
```
#### Output and preparation for next iteration, update timers
- In this segment, a major task that I do is check if a minion with important role (runner/ 2nd runner/ defender) is ** dead **, then assign those roles to nearest (did not use BFS here, just column position to check distance) minions available.
- Another thing I do here is, if my runner is in danger (low health), send 2nd runner close to him so that in case of failure, 2nd minion can carry on the show.
- Also, here I take some decisions based on previous events and game timer -- such as if the opponent has played aggressively
- Update variables for next iteration such as timers, health of my minions, dodge counters...
- Lastly, finalized moves are sent to output

### A Story and a little Summary
From 15 July to 22 July the contest was held in the BUET CSE FEST 2022. During this 1 week, the game went through levels like evolution. The participants were really competitive and really pushed each other to their limits to come up with better solutions. The steps of evolution of the game were like this,
- Default (just send bots to capture flag and return)
- Defend and Attack (players took measures of defending their flags, along with good coin collection system)
- Good use of freeze
- Very Aggressive play
- Escape Aggressive play by Jiggling (also waste credits of opponent)
- Countering escape strategy by not firing immediately

These steps of evolution were not like always progressively better. It's just that some of the counter measures were very adaptive with some measures. So, it was a continous back and forth intense battle between the competitors.

#### I think there are more opportunities to create a much better and optimized solution that would perform better. If the contest went on for a little longer, we probably might have seen some new steps of evolution in the game by now. But, for now, it is as it is. Until, someone comes with more clever solution. Credits to the ones who created this game.

### KEYWORDS
- Runner: The bot that I send to capture opponent flag
- Defender: The bot that I send to defend my own flag
- Troller: The bot that I send along with runner so that if the runner fails, it will take that task. So it's a 2nd runner.
- Collector: The bots that I primarily use to collect coins.
