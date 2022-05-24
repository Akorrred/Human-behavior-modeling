#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <clocale>
#include <fstream>
#include <sstream>
#include <random>
#include <string_view>
#include <queue>

#define char string

#define size_t unsigned long long

using namespace std;

struct pair_hash {
    template <class T1, class T2>
    size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // нужно, чтобы нормально работал unordered_map<pair<int, int>, int, pair_hash> coordinate_to_int;
        return h1 ^ h2;
    }
};


unordered_map<string, int> char_to_int;

unordered_map<int, pair<int, int>> int_to_coordinate;
unordered_map<pair<int, int>, int, pair_hash> coordinate_to_int;

bool Is_Russian_Letter(int c) {
    return (c >= 1 && c <= 33);
}

double my_generate_random(double from, double to) {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(from, to);
    double random_double = dis(gen);
    return random_double;
}

int find_symmetric_vertex(int vertex) {
    pair<int, int> coordinate = int_to_coordinate[vertex];
    coordinate.first *= -1; // Пока что отражаю относительно оси Y, в дальнейшем, думаю, можно
    // рандомно выбирать относительно какой оси отражать или же опираться на количество вершин пути на определенной четверти координатной плоскости
    auto search = coordinate_to_int.find(coordinate);
    if (search != coordinate_to_int.end()) {
        return coordinate_to_int[coordinate]; // We found a symmetrical vertex
    } else {
        return -1; // We didn't find a symmetrical vertex
    }

}

class Rat;

class Maze;

class Discrete_Operator {
public:
    double prob = 0;

    virtual void apply(Maze &maze, Rat &rat) {
        cerr << "Error: tried to apply general discrete operator\n";
        exit(1);
    }

    virtual void set_probability(double probability) {
        cerr << "Error: tried to set_probabilities for general discrete operator\n";
        exit(1);
    }

    virtual string get_name() {
        return "general";
    }
};

class Operator_Random : public Discrete_Operator {
public:
    void apply(Maze &maze, Rat &rat) override; // Написано ниже
    void set_probability(double probability) override; // Написано ниже
    string get_name() override {
        return "random";
    }
};

class Operator_Inversion : public Discrete_Operator {
public:
    int max_len_of_path_a_rat_can_invert = 5;

    void apply(Maze &maze, Rat &rate) override; // Написано ниже

    void set_probability(double probability) override; // Написано ниже

    string get_name() override {
        return "inversion";
    }
};

class Operator_Symmetry : public Discrete_Operator {
public:

    void apply(Maze &maze, Rat &rate) override; // Написано ниже

    void set_probability(double probability) override; // Написано ниже

    string get_name() override {
        return "symmetry";
    }

};

class Operator_Compression : public Discrete_Operator {
public:

    void apply(Maze &maze, Rat &rate) override; // Написано ниже

    void set_probability(double probability) override; // Написано ниже

    string get_name() override {
        return "compression";
    }

};

class Operator_Ring : public Discrete_Operator {
public:

    void apply(Maze &maze, Rat &rate) override; // Написано ниже

    void set_probability(double probability) override; // Написано ниже

    string get_name() override {
        return "ring";
    }

};

double approximately_one = 0.9; // Для выведения медиан
double eps = 0.001; // Для подтверждения точности вычисления в double

double INF = exp(100);

enum class abs_direction { // absolute direction
    left, right, up, down, stop, start, left_up, right_up, down_right, down_left, no
};

enum class relative_direction {
    to_left, to_right, forward, backwards, forward_left, forward_right, backwards_left, backwards_right
};


enum class operator_types {
    Discrete_Operator, Operator_Random, Operator_Inversion, Operator_Symmetry, Operator_Compression, Operator_Ring
};


enum class type_of_maze {
    nikolskaya, not_set
};


int n;
unordered_map<int, string> int_to_char;
unordered_map<char, int> eng_to_int;

class Cell {
public:
    int num = 0;
    unordered_map<abs_direction, int> neighs;
};

abs_direction determine_new_direction(abs_direction cur_direction, relative_direction choice) {
    if (choice == relative_direction::forward) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::down;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::left_up;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::right_up;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::down_right;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::down_left;
        }
    }
    if (choice == relative_direction::backwards) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::up;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::down_right;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::down_left;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::left_up;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::right_up;
        }
    }
    if (choice == relative_direction::to_left) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::right;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::down_left;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::left_up;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::right_up;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::down_right;
        }
    }
    if (choice == relative_direction::to_right) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::left;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::right_up;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::down_right;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::down_left;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::left_up;
        }
    }
    if (choice == relative_direction::forward_left) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::down_left;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::right_up;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::left_up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::down_right;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::down;
        }
    }
    if (choice == relative_direction::forward_right) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::left_up;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::down_right;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::right_up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::down_left;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::left;
        }
    }
    if (choice == relative_direction::backwards_left) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::down_right;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::left_up;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::down_left;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::right_up;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::right;
        }
    }
    if (choice == relative_direction::backwards_right) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::right_up;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::down_left;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::down_right;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::left_up;
        }

        if (cur_direction == abs_direction::left_up) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::right_up) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down_right) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::down_left) {
            return abs_direction::up;
        }
    }

    cerr << "Error #2 from determine_new_direction!!!\n";
    exit(1);
} // OK

class Rat {
public:
    int cur_cell_int = 0;
    abs_direction cur_direction;

    unordered_map<relative_direction, double> probabilities_of_relative_dirs;
    unordered_map<abs_direction, double> cur_probabilities_of_abs_dirs;
    type_of_maze typeOfMaze = type_of_maze::not_set;

    void set_probabilities_of_relative_dirs(double pr_forward, double pr_right, double pr_left, double pr_back, double pr_forward_left,
                                            double pr_forward_right, double pr_back_left, double pr_back_right) {
        if (abs(pr_forward + pr_right + pr_left + pr_back
                + pr_forward_left + pr_forward_right + pr_back_left + pr_back_right - 1) > eps) {
            cerr << "Error! Incorrect probabilities for a rat!\n";
            exit(1);
        }
        probabilities_of_relative_dirs[relative_direction::to_right] = pr_right;
        probabilities_of_relative_dirs[relative_direction::to_left] = pr_left;
        probabilities_of_relative_dirs[relative_direction::forward] = pr_forward;
        probabilities_of_relative_dirs[relative_direction::backwards] = pr_back;
        probabilities_of_relative_dirs[relative_direction::forward_left] = pr_forward_left;
        probabilities_of_relative_dirs[relative_direction::forward_right] = pr_forward_right;
        probabilities_of_relative_dirs[relative_direction::backwards_left] = pr_back_left;
        probabilities_of_relative_dirs[relative_direction::backwards_right] = pr_back_right;
    } // OK



    bool completed_route() {
        if (typeOfMaze == type_of_maze::nikolskaya) {
            return (cur_cell_int == char_to_int["Р"]) || (cur_cell_int == char_to_int["Т"]);
        } else if (typeOfMaze == type_of_maze::not_set) {
            cerr << "Error in bool completed_route() typeOfMaze == not_set\n";
        } else {
            cerr << "Error in bool completed_route() because of strange typeOfMaze\n";
        }
        exit(1);

    }

    abs_direction make_decision() {
        {
            double check_sum = 0;
            for (auto &pair_direction_double: cur_probabilities_of_abs_dirs) {
                double cur_prob = pair_direction_double.second;
                if (cur_prob < 0.0) {
                    cerr << "Error! Negative probability in make_decision!\n";
                    exit(1);
                }
                check_sum += cur_prob;
            }
        }

        // Link: https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
        double random_from_0_to_1 = my_generate_random(0.0, 1.0);
        double cur_sum = 0.0;
        for (auto &pair_direction_double: cur_probabilities_of_abs_dirs) {
            abs_direction dir = pair_direction_double.first;
            double prob = pair_direction_double.second;
            if (prob != 0.0) {
                cur_sum += prob;
                if (cur_sum + eps > random_from_0_to_1) {
                    return dir;
                }
            }
        }
        cerr << "Error in make_decision. NO direction had been chosen\n";
        exit(1);
    }
}; // OK

class Maze {
public:
    type_of_maze typeOfMaze = type_of_maze::not_set;

    vector<Cell> cells;
    vector<int> cur_generated_route;
    vector<vector<int>> vector_of_generated_routes;

    vector<string> vector_of_real_routes;
    map<string, vector<vector<string>>> rats_name_to_experiment_results;
    map<string, vector<string>> name_to_route;

    Discrete_Operator discrete_operator;
    Operator_Random operator_random;
    Operator_Inversion operator_inversion;
    Operator_Symmetry operator_symmetry;
    Operator_Compression operator_compression;
    Operator_Ring operator_ring;

    unordered_map<string, Discrete_Operator *> map_from_name_to_operator;
    unordered_map<string, double> operators_to_probabilities;

    bool flag_has_to_stop = false;

    Maze() {
        // Делаем биекцию между буквами (буквы обозначают участки лабиринта)
        // и числами.
        int_to_char[0] = "_";
        int_to_char[1] = "А";
        int_to_char[2] = "Б";
        int_to_char[3] = "В";
        int_to_char[4] = "Г";
        int_to_char[5] = "Д";
        int_to_char[6] = "Е";
        int_to_char[7] = "Ё";
        int_to_char[8] = "Ж";
        int_to_char[9] = "З";
        int_to_char[10] = "И";
        int_to_char[11] = "Й";
        int_to_char[12] = "К";
        int_to_char[13] = "Л";
        int_to_char[14] = "М";
        int_to_char[15] = "Н";
        int_to_char[16] = "О";
        int_to_char[17] = "П";
        int_to_char[18] = "Р";
        int_to_char[19] = "С";
        int_to_char[20] = "Т";
        int_to_char[21] = "У";
        int_to_char[22] = "Ф";
        int_to_char[23] = "Х";
        int_to_char[24] = "Ц";
        int_to_char[25] = "Ч";
        int_to_char[26] = "Ш";
        int_to_char[27] = "Щ";
        int_to_char[28] = "Ъ";
        int_to_char[29] = "Ы";
        int_to_char[30] = "Ь";
        int_to_char[31] = "Э";
        int_to_char[32] = "Ю";
        int_to_char[33] = "Я";
        int_to_char[34] = "R";
        int_to_char[35] = "S";
        int_to_char[36] = "W";
        int_to_char[37] = "Z";
        int_to_char[38] = "D";
        int_to_char[39] = "F";
        int_to_char[40] = "Q";
        int_to_char[41] = "V";
        int_to_char[42] = "U";
        int_to_char[43] = "I";
        int_to_char[44] = "J";
        int_to_char[45] = "G";
        int_to_char[46] = "L";
        int_to_char[47] = "f";
        int_to_char[48] = "d";

        eng_to_int["A"] = 1;
        eng_to_int["B"] = 2;
        eng_to_int["C"] = 3;
        eng_to_int["D"] = 4;
        eng_to_int["E"] = 5;
        eng_to_int["F"] = 6;
        eng_to_int["G"] = 8;
        eng_to_int["H"] = 9;
        eng_to_int["I"] = 10;
        eng_to_int["J"] = 12;
        eng_to_int["K"] = 13;
        eng_to_int["L"] = 14;
        eng_to_int["M"] = 16;
        eng_to_int["N"] = 18;
        eng_to_int["O"] = 19;
        eng_to_int["P"] = 20;
        eng_to_int["Q"] = 21;
        eng_to_int["R"] = 22;
        eng_to_int["S"] = 23;
        eng_to_int["T"] = 24;
        eng_to_int["U"] = 26;
        eng_to_int["V"] = 27;
        eng_to_int["W"] = 29;
        eng_to_int["X"] = 31;
        eng_to_int["Y"] = 33;

        int_to_coordinate[34] = {0, 0};
        int_to_coordinate[16] = {0, 1};
        int_to_coordinate[29] = {0, 2};
        int_to_coordinate[22] = {0, -1};
        int_to_coordinate[41] = {0, -2};

        int_to_coordinate[10] = {-1, 0};
        int_to_coordinate[23] = {-1, -2};

        int_to_coordinate[36] = {-2, 0};
        int_to_coordinate[13] = {-2, 1};
        int_to_coordinate[37] = {-2, 2};
        int_to_coordinate[2] = {-2, 3};
        int_to_coordinate[40] = {-2, -2};
        int_to_coordinate[1] = {-2, -3};
        int_to_coordinate[12] = {-2, -1};

        int_to_coordinate[6] = {-3, 2};
        int_to_coordinate[5] = {-3, -2};

        int_to_coordinate[38] = {-4, 2};
        int_to_coordinate[18] = {-4, 3};
        int_to_coordinate[26] = {-4, 1};
        int_to_coordinate[19] = {-4, -1};
        int_to_coordinate[39] = {-4, -2};

        int_to_coordinate[14] = {1, 0};
        int_to_coordinate[24] = {1, -2};

        int_to_coordinate[35] = {2, 0};
        int_to_coordinate[31] = {2, 1};
        int_to_coordinate[45] = {2, 2};
        int_to_coordinate[3] = {2, 3};
        int_to_coordinate[33] = {2, -1};
        int_to_coordinate[42] = {2, -2};
        int_to_coordinate[4] = {2, -3};

        int_to_coordinate[8] = {3, 2};
        int_to_coordinate[9] = {3, -2};

        int_to_coordinate[21] = {4, -1};
        int_to_coordinate[27] = {4, 1};
        int_to_coordinate[44] = {4, 2};
        int_to_coordinate[20] = {4, 3};
        int_to_coordinate[43] = {4, -2};

        // Каждой точке сопоставили координату

        n = int_to_char.size();
        {
            for (size_t i = 0; i < int_to_char.size(); ++i) {
                char_to_int[int_to_char[i]] = i;
            }
        }
        cells.resize(n);
        // Сопоставляем каждой букве и координате число
        {
            for (size_t i = 0; i < int_to_coordinate.size(); ++i) {
                coordinate_to_int[int_to_coordinate[i]] = i;
            }
        }

    } // OK

    void construct(istream &input, ostream &output) {
        string line;
        while (getline(input, line)) {
            stringstream ss(line);
            char cur_cell_char, left_neigh_char, left_up, up_neigh_char, right_up, right_neigh_char,
                    down_right, down_neigh_char, down_left;
            ss >> cur_cell_char >> left_neigh_char >> left_up >> up_neigh_char >> right_up >> right_neigh_char >>
               down_right >> down_neigh_char >> down_left;
            int cur_cell_int = char_to_int[cur_cell_char];

            int left_neigh_int = char_to_int[left_neigh_char];
            int left_up_int = char_to_int[left_up];
            int up_neigh_int = char_to_int[up_neigh_char];
            int right_up_int = char_to_int[right_up];
            int right_neigh_int = char_to_int[right_neigh_char];
            int down_right_int = char_to_int[down_right];
            int down_neigh_int = char_to_int[down_neigh_char];
            int down_left_int = char_to_int[down_left];

            add_neighs_to_cell(cur_cell_int, left_neigh_int, left_up_int, up_neigh_int, right_up_int,
                               right_neigh_int, down_right_int, down_neigh_int, down_left_int);
        }
    } // OK

    void add_neighs_to_cell(int cur_int, int left_int, int left_up_int, int up_int, int right_up_int, int right_int,
                            int down_right_int, int down_int, int down_left_int) {
        cells[cur_int].num = cur_int;
        cells[cur_int].neighs[abs_direction::left] = left_int;
        cells[cur_int].neighs[abs_direction::left_up] = left_up_int;
        cells[cur_int].neighs[abs_direction::up] = up_int;
        cells[cur_int].neighs[abs_direction::right_up] = right_up_int;
        cells[cur_int].neighs[abs_direction::right] = right_int;
        cells[cur_int].neighs[abs_direction::down_right] = down_right_int;
        cells[cur_int].neighs[abs_direction::down] = down_int;
        cells[cur_int].neighs[abs_direction::down_left] = down_left_int;
    } // OK


    void print_info_about_cells_in_letters(ostream &output) {
        for (size_t i = 0; i < n; ++i) {
            output << "Клетка " << int_to_char[i] << ' ';
            if (cells[i].num == 0) {
                output << "не существует";
            } else {
                unordered_map<abs_direction, int> &neighs = cells[i].neighs;
                output << i << " имеет соседей "
                       << int_to_char[neighs[abs_direction::left]] << ' '
                       << int_to_char[neighs[abs_direction::left_up]] << ' '
                       << int_to_char[neighs[abs_direction::up]] << ' '
                       << int_to_char[neighs[abs_direction::right_up]] << ' '
                       << int_to_char[neighs[abs_direction::right]] << ' '
                       << int_to_char[neighs[abs_direction::down_right]] << ' '
                       << int_to_char[neighs[abs_direction::down]] << ' '
                       << int_to_char[neighs[abs_direction::down_left]];
            }
            output << '\n';
        }
    } // OK

    vector<int> direct(int from, int to) { // BFS для поиска кратчайшего расстояния между вершинами
        vector<int> ans;
        vector<int> len(49, 50);
        vector<int> parent(49, -1);
        len[from] = 0;
        queue<int> q;
        q.push(from);
        while (!q.empty()) {
            int cur = q.front();
            q.pop();
            if (cur == to) {
                for (int i = to; i != -1; i = parent[i]) {
                    ans.push_back(i);
                }
                reverse(ans.begin(), ans.end());
                return ans;
            }
            unordered_map<abs_direction, int> &neighs = cells[cur].neighs;

            if (neighs[abs_direction::left] != 0 && len[neighs[abs_direction::left]] == 49 + 1) {
                len[neighs[abs_direction::left]] = len[cur] + 1;
                q.push(neighs[abs_direction::left]);
                parent[neighs[abs_direction::left]] = cur;
            }
            if (neighs[abs_direction::left_up] != 0 && len[neighs[abs_direction::left_up]] == 49 + 1) {
                len[neighs[abs_direction::left_up]] = len[cur] + 1;
                q.push(neighs[abs_direction::left_up]);
                parent[neighs[abs_direction::left_up]] = cur;
            }
            if (neighs[abs_direction::up] != 0 && len[neighs[abs_direction::up]] == 49 + 1) {
                len[neighs[abs_direction::up]] = len[cur] + 1;
                q.push(neighs[abs_direction::up]);
                parent[neighs[abs_direction::up]] = cur;
            }
            if (neighs[abs_direction::right_up] != 0 && len[neighs[abs_direction::right_up]] == 49 + 1) {
                len[neighs[abs_direction::right_up]] = len[cur] + 1;
                q.push(neighs[abs_direction::right_up]);
                parent[neighs[abs_direction::right_up]] = cur;
            }
            if (neighs[abs_direction::right] != 0 && len[neighs[abs_direction::right]] == 49 + 1) {
                len[neighs[abs_direction::right]] = len[cur] + 1;
                q.push(neighs[abs_direction::right]);
                parent[neighs[abs_direction::right]] = cur;
            }
            if (neighs[abs_direction::down_right] != 0 && len[neighs[abs_direction::down_right]] == 49 + 1) {
                len[neighs[abs_direction::down_right]] = len[cur] + 1;
                q.push(neighs[abs_direction::down_right]);
                parent[neighs[abs_direction::down_right]] = cur;
            }
            if (neighs[abs_direction::down] != 0 && len[neighs[abs_direction::down]] == 49 + 1) {
                len[neighs[abs_direction::down]] = len[cur] + 1;
                q.push(neighs[abs_direction::down]);
                parent[neighs[abs_direction::down]] = cur;
            }
            if (neighs[abs_direction::down_left] != 0 && len[neighs[abs_direction::down_left]] == 49 + 1) {
                len[neighs[abs_direction::down_left]] = len[cur] + 1;
                q.push(neighs[abs_direction::down_left]);
                parent[neighs[abs_direction::down_left]] = cur;
            }

        }
        if (len[to] == 49 + 1) {
            return {-1};
        } else {
            for (int i = to; i != -1; i = parent[i]) {
                ans.push_back(i);
            }
            reverse(ans.begin(), ans.end());
            return ans;
        }
    }

    string choose_an_operator() {
        {
            // Проверка того, что данные корректные
            double sum_of_probs = 0;
            for (auto &pair_string_double: operators_to_probabilities) {
                double el = pair_string_double.second;
                if (el < 0) {
                    cerr << "Error in choose_an_operator. There is a negative probability!\n";
                    exit(1);
                }
                sum_of_probs += el;
            }
            if (abs(1 - sum_of_probs) > eps) {
                cerr << "Error in choose_an_operator. Sum of probabilities is not equal to 1.0!\n";
                exit(1);
            }
        }
        double random_from_0_to_1 = my_generate_random(0, 1.0);
        double cur_sum = 0.0;
        for (auto &pair_string_double: operators_to_probabilities) {
            string name_of_operator = pair_string_double.first;
            double prob = pair_string_double.second;
            if (prob != 0.0) {
                cur_sum += prob;
                if (cur_sum + eps > random_from_0_to_1) {
                    return name_of_operator;
                }
            }
        }
        cerr << "Error in choose_an_operator. NO operator had been chosen\n";
        exit(1);
    }

    void conduct_experiment(Rat &roger) {
        cout << "\n\nnew experiment began\n\n";
        char start_char = "0";
        if (typeOfMaze == type_of_maze::nikolskaya) {
            start_char = "Ы";
            roger.cur_direction = abs_direction::down;
        }
        if (start_char == "0") {
            cerr << "Error in     void conduct_experiment(Rat &roger) :: strange start_char\n";
            exit(1);
        }
        roger.cur_cell_int = char_to_int[start_char];

        int i = 0;
        bool took_a = false;
        bool took_g = false;
        while (!roger.completed_route() && i <= 1000) {
            string chosen_operator = choose_an_operator();
            // Здесь можно выводить информацию от том, какой дискретный оператор в данный момент работает.
            // chosen_operator - это имя дискретного оператора, применяемого в данный момент.
            cout << chosen_operator;

            if (chosen_operator != "random" && chosen_operator != "inversion"
                && chosen_operator != "symmetry" && chosen_operator != "compression" && chosen_operator != "ring") {
                cerr << "Error: operator other than random or inversion has been chosen!..\n";
                exit(1);
            }
            map_from_name_to_operator[chosen_operator]->apply(*this, roger);
            if (roger.cur_cell_int == 1) {
                took_a = true;
            }
            if (roger.cur_cell_int == 4) {
                took_g = true;
            }

            if (chosen_operator == "random") {
                cout << " ";
                if (roger.cur_direction == abs_direction::right) {
                    cout << "right";
                } else if (roger.cur_direction == abs_direction::up) {
                    cout << "up";
                } else if (roger.cur_direction == abs_direction::left) {
                    cout << "left";
                } else if (roger.cur_direction == abs_direction::down) {
                    cout << "down";
                } else if (roger.cur_direction == abs_direction::left_up) {
                    cout << "left_up";
                } else if (roger.cur_direction == abs_direction::right_up) {
                    cout << "right_up";
                } else if (roger.cur_direction == abs_direction::down_right) {
                    cout << "down_right";
                } else if (roger.cur_direction == abs_direction::down_left) {
                    cout << "down_left";
                }

            } else if (chosen_operator == "inversion" || chosen_operator == "symmetry"
                       || chosen_operator == "compression" || chosen_operator == "ring") {
                cout << " applied";
            } else{
                cerr << "Error: strange operator!\n";
                exit(1);
            }
            cout << '\n';
            ++i;
        }
        if (i > 1000) {
            flag_has_to_stop = true;
        }
    }

    abs_direction get_abs_direction_from_cell_number_to_cell_number(int from_num, int to_num) {
        vector<abs_direction> all_abs_directions{abs_direction::left,
                                                 abs_direction::up,
                                                 abs_direction::right,
                                                 abs_direction::down,
                                                 abs_direction::left_up,
                                                 abs_direction::right_up,
                                                 abs_direction::down_right,
                                                 abs_direction::down_left};

        for (auto cur_abs_dir: all_abs_directions) {
            if (cells[from_num].neighs[cur_abs_dir] == to_num) {
                return cur_abs_dir;
            }
        }
        return abs_direction::down;
    }
    void construct_map_from_name_to_operator() {
        map_from_name_to_operator[discrete_operator.get_name()] = &discrete_operator;
        map_from_name_to_operator[operator_random.get_name()] = &operator_random;
        map_from_name_to_operator[operator_inversion.get_name()] = &operator_inversion;
        map_from_name_to_operator[operator_symmetry.get_name()] = &operator_symmetry;
        map_from_name_to_operator[operator_compression.get_name()] = &operator_compression;
        map_from_name_to_operator[operator_ring.get_name()] = &operator_ring;
    }

    void conduct_series_of_experiments_memorizing_routes(size_t number_of_experiments, Rat& roger) {
        vector_of_generated_routes.clear();
        flag_has_to_stop = false;

        roger.typeOfMaze = typeOfMaze;

        long long counter_of_wrong_routes = 0;
        for (size_t i = 0; i < number_of_experiments; ++i) {
            conduct_experiment(roger);
            if (flag_has_to_stop) {
                return;
            }
            vector<int> route_to_be_added = cur_generated_route;
            cur_generated_route.clear(); // чтобы маршруты не сливались в один
            if (route_to_be_added.size() < 1550) {
                vector_of_generated_routes.push_back(route_to_be_added);
            } else {
                if (++counter_of_wrong_routes > 0.3 * number_of_experiments) {
                    flag_has_to_stop = true;
                    return;
                }
            }
        }
    }

    void set_probabilities_for_operators(vector<double> vec) {
        if (vec.size() != 5) {
            cerr << "Error in set_probabilities_for_operators. Wrong size of vector of doubles!\n";
            exit(1);
        }
        double sum_of_probs = 0;
        for (double el: vec) {
            if (el < 0) {
                cerr << "Error in set_probabilities_for_operators. There is a negative probability!\n";
                exit(1);
            }
            sum_of_probs += el;
        }
        if (abs(1 - sum_of_probs) > eps) {
            cerr << "Error in set_probabilities_for_operators. Sum of probabilities is not equal to 1.0!\n";
            exit(1);
        }
        operators_to_probabilities[discrete_operator.get_name()] = vec[0];
        operators_to_probabilities[operator_random.get_name()] = vec[1];
        operators_to_probabilities[operator_inversion.get_name()] = vec[2];
        operators_to_probabilities[operator_symmetry.get_name()] = vec[3];
        operators_to_probabilities[operator_compression.get_name()] = vec[4];
        //operators_to_probabilities[operator_ring.get_name()] = vec[5];
    }


    vector<vector<int>> print_all_generated_routes(ostream &o) {
        for (size_t i = 0; i < vector_of_generated_routes.size(); ++i) {
            o << "\nRoute " << i +  1 << ":\n";
            for (int j = 0; j < vector_of_generated_routes[vector_of_generated_routes.size() - i - 1].size(); ++j) {
                o << int_to_char[vector_of_generated_routes[vector_of_generated_routes.size() - i - 1][j]];
            }
            o << "\n";
        }
        return vector_of_generated_routes;
    } // OK

    vector<vector<int>> return_all_generated_routes() {
        return vector_of_generated_routes;
    } // OK

    int process_random_move_of_rat(
            Rat *rat_ptr) { // функция, отвечающая за перемещение крысы из текущего участка лабиринта в соседний.
        // Возвращает char -- букву (русскую или английскую), характеризующую позицию крысы в лабиринте
        int cur_cell_int = rat_ptr->cur_cell_int;
        abs_direction cur_direction = rat_ptr->cur_direction;
        // int trying_to_move_to_cell = cells[cur_cell_int].neighs[cur_direction];
        vector<relative_direction> all_relative_directions = {relative_direction::forward,
                                                              relative_direction::to_left,
                                                              relative_direction::to_right,
                                                              relative_direction::backwards,
                                                              relative_direction::forward_left,
                                                              relative_direction::forward_right,
                                                              relative_direction::backwards_left,
                                                              relative_direction::backwards_right};
        double sum_of_probabilities_impossible = 0;
        for (relative_direction choice: all_relative_directions) {
            abs_direction new_direction = determine_new_direction(cur_direction, choice);
            if (0 == cells[cur_cell_int].neighs[new_direction]) {
                sum_of_probabilities_impossible += rat_ptr->probabilities_of_relative_dirs[choice];
                rat_ptr->cur_probabilities_of_abs_dirs[new_direction] = 0;
            }
        } // насчитали суммарную вероятность направлений, в которых двигаться невозможно

        if (abs(1 - sum_of_probabilities_impossible) < eps) {
            cerr
                    << "Error in function process_move_of_rat! We have no possible directions! (In processing rat move we almost divided by zero!)\n";
            cerr << "cur_cell = " << rat_ptr->cur_cell_int << "\n";
            exit(1);
        }
        for (relative_direction choice: all_relative_directions) {
            abs_direction new_direction = determine_new_direction(cur_direction, choice);
            if (0 != cells[cur_cell_int].neighs[new_direction]) {
                rat_ptr->cur_probabilities_of_abs_dirs[new_direction] =
                        rat_ptr->probabilities_of_relative_dirs[choice] *
                        (1 + sum_of_probabilities_impossible / (1 - sum_of_probabilities_impossible));
            }
        }
        abs_direction dir = rat_ptr->make_decision();
        int new_cell_int = cells[cur_cell_int].neighs[dir];
        rat_ptr->cur_cell_int = new_cell_int;
        rat_ptr->cur_direction = dir;
        cout << "NEW CELL " << new_cell_int << "\n";
        return new_cell_int;
    } // OK


    unordered_map<string, int> read_eng(istream &in) {
        string line;
        unordered_map<string, int> dir_to_cnt;
        while (getline(in, line)) {
            stringstream ss(line);
            string route;
            ss >> route;
            for (int i = 1; i < route.size() - 1; ++i) {
                if (route[i] != '+' && route[i + 1] != '+') {
                    std::string from(1, route[i]);
                    std::string to(1, route[i + 1]);
                    if (from != to && eng_to_int[from] >= 1 && eng_to_int[to] >= 1) {
                        string dir_str;
                        abs_direction direction =get_abs_direction_from_cell_number_to_cell_number(eng_to_int[from], eng_to_int[to]);
                        if (direction == abs_direction::left) {
                            dir_str = "left";
                        }
                        if (direction == abs_direction::right) {
                            dir_str = "right";
                        }
                        if (direction == abs_direction::up) {
                            dir_str = "up";
                        }
                        if (direction == abs_direction::down) {
                            dir_str = "down";
                        }
                        if (direction == abs_direction::left_up) {
                            dir_str = "left_up";
                        }
                        if (direction == abs_direction::right_up) {
                            dir_str = "right_up";
                        }
                        if (direction == abs_direction::down_left) {
                            dir_str = "down_left";
                        }
                        if (direction == abs_direction::down_right) {
                            dir_str = "down_right";
                        }
                        dir_to_cnt[dir_str]++;
                    }
                }
            }
        }
        return dir_to_cnt;
    } // OK

};

void Operator_Random::apply(Maze &maze, Rat &rat) {
    // cout << "Discrete operator of RANDOM applied!\n";
    maze.cur_generated_route.push_back(maze.process_random_move_of_rat(&rat));
}

void Operator_Random::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator RANDOM's probability has been set to " << prob << "\n";
}

void Operator_Inversion::apply(Maze &maze, Rat &rat) {
    // К текущему пути добавить последние len_of_path_to_inverse "русских" вершин в обратном порядке.
    // Начинаем с предпоследней вершины.

    int len_of_path_to_invert = round(my_generate_random(0.5 + eps, max_len_of_path_a_rat_can_invert + 0.5 - eps));
    int len_of_path_to_invert_saved = len_of_path_to_invert;
    // написать функцию, которая проходит по пути назад и добавляет максимум len_of_path_to_inverse "русских" вершин

    vector<int> &path = maze.cur_generated_route;
    int len_of_path = int(path.size());
    int i = len_of_path - 2; // начинаем с предпоследней вершины
    if (i < 1) {
        return;
    }

    while (len_of_path_to_invert > 0 && i >= 0) {

        int str_path = path[i];

        if (Is_Russian_Letter(str_path)) {
            --len_of_path_to_invert;
        }
        path.push_back(path[i]);
        --i;
    }

    if (path.size() >= 2) {
        rat.cur_cell_int = path[path.size() - 1];
        int str_path2 = path[path.size() - 2];
        int from_num = str_path2;
        int to_num = rat.cur_cell_int;

        if (from_num != to_num) {
            rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);
        }
    }
}

void Operator_Inversion::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator INVERSION's probability has been set to " << prob << "\n";
}

void Operator_Symmetry::apply(Maze &maze, Rat &rat) {
    // Проверяем, можно ли построить симметричный путь, т.е. для каждой точки пройденного пути проверяем наличие симметричной
    // Ищем кратчайшее расстояние от последней точки текущего пути до первой точки симметричного и идем
    vector<int> &path = maze.cur_generated_route;
    int len = len = 7; // взял пока что такую константу
    if (path.size() < len) {
        //len = path.size();
        return;
    }
    vector<int> symmetric_path;

    for (int i = path.size() - len; i < path.size(); ++i) { // находим для последних len точек пути симметричные им
        int symmetric_vertex = find_symmetric_vertex(path[i]);
        if (symmetric_vertex != -1) {
            symmetric_path.push_back(symmetric_vertex);
        } else {
            return;
        }
    }
    vector<int> direction_to_symmetric_path = maze.direct(path[path.size() - 1], symmetric_path[0]); // строим кратчайший путь
    for (int i = 1; i < direction_to_symmetric_path.size(); ++i) {
        path.push_back(direction_to_symmetric_path[i]);
    }
    for (int i = 1; i < symmetric_path.size(); ++i) {
        path.push_back(symmetric_path[i]);
    }
    if (path.size() >= 2) {
        rat.cur_cell_int = path[path.size() - 1];
        int str_path2 = path[path.size() - 2];
        int from_num = str_path2;
        int to_num = rat.cur_cell_int;
        if (from_num != to_num) {
            rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);
        }
    }

}

void Operator_Symmetry::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator Symmetry's probability has been set to " << prob << "\n";
}

void Operator_Compression::apply(Maze &maze, Rat &rat) {
    // Берем последние len точек пути и проверяем, можем ли мы добраться от path.size() - len до последней точки путем короче
    vector<int> &path = maze.cur_generated_route;
    if (path.size() < 3) {
        return;
    }
    int len = 5; // опять же случайная константа
    if (path.size() < len) {
        return;
    }

    vector<int> path_to_compress, shortest_distance = {};
    for (int i = path.size() - len; i < path.size(); ++i) {
        path_to_compress.push_back(path[i]);
    }
    int prev = 0;
    bool A = false, G = false;

    shortest_distance.push_back(path_to_compress[0]);

    for (int i = 0; i < path_to_compress.size(); ++i) {
        if (path_to_compress[i] == 1 && !A) {
            A = true;
            if (path_to_compress[prev] != path_to_compress[i]) {
                vector<int> shortest_distance_to_A = maze.direct(path_to_compress[prev], path_to_compress[i]);
                for (int j = 1; j < shortest_distance_to_A.size(); ++j) {
                    shortest_distance.push_back(shortest_distance_to_A[j]);
                }
            }
            prev = i;
        } else if (path_to_compress[i] == 4 && !G) {
            G = true;
            if (path_to_compress[prev] != path_to_compress[i]) {
                vector<int> shortest_distance_to_G = maze.direct(path_to_compress[prev], path_to_compress[i]);
                for (int j = 1; j < shortest_distance_to_G.size(); ++j) {
                    shortest_distance.push_back(shortest_distance_to_G[j]);
                }
            }
            prev = i;
        }
    }
    if (path_to_compress[prev] != path_to_compress[path_to_compress.size() - 1]) {
        vector<int> last_short = maze.direct(path_to_compress[prev], path_to_compress[path_to_compress.size() - 1]);
        for (int j = 1; j < last_short.size(); ++j) {
            shortest_distance.push_back(last_short[j]);
        }
    }


    if (len > shortest_distance.size()) { // если получилось сжать - переписываем путь
        for (int i = 0; i < len; ++i) {
            if (path.size() != 0) {
                path.pop_back();
            }
        }
        for (int i = 0; i < shortest_distance.size(); ++i) {
            path.push_back(shortest_distance[i]);
        }

        if (shortest_distance.size() == 1) {
            rat.cur_cell_int = path[path.size() - 1];
        } else {
            rat.cur_cell_int = path[path.size() - 1];
            int str_path2 = path[path.size() - 2];
            int from_num = str_path2;
            int to_num = rat.cur_cell_int;
            rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);
        }
    }

}

void Operator_Compression::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator Compression's probability has been set to " << prob << "\n";
}

void Operator_Ring::apply(Maze &maze, Rat &rat) {
    // Оператор применяется, если мы находимся в кормушке,
    // далее мы ищем кратчайший путь от кормушки до вершины path[path.size() - len_of_ring] и идем туда
    vector<int> &path = maze.cur_generated_route;
    if (path[path.size() - 1] != 1 || path[path.size() - 1] != 4) {
        return;
    }
    int len_of_ring = 5;
    if (path.size() < len_of_ring) {
        return;
    }
    vector<int> path_to_make_ring = maze.direct(path[path.size() - 1], path[path.size() - len_of_ring]);

    for (int i = 1; i < path_to_make_ring.size(); ++i) {
        path.push_back(path_to_make_ring[i]);
    }

    rat.cur_cell_int = path[path.size() - 1];
    int str_path2 = path[path.size() - 2];
    int from_num = str_path2;
    int to_num = rat.cur_cell_int;
    rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);

}

void Operator_Ring::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator Ring's probability has been set to " << prob << "\n";
}


double step = 0.1;
double acc = 0.01;
std::vector<std::vector<double>> gen_grid(std::vector<double> l_bound, std::vector<double> u_bound) {
    std::vector<std::vector<double>> grid;
    grid.reserve(1000);
    std::vector<double> probs(4);
    probs[0] = l_bound[0];
    while (probs[0] < u_bound[0] + acc) {
        probs[1] = l_bound[1];
        while (probs[1] < u_bound[1] + acc) {
            probs[2] = l_bound[2];
            while (probs[2] < u_bound[2] + acc) {
                probs[3] = l_bound[3];
                while (probs[3] < min(u_bound[3], 1. - probs[0] - probs[1] - probs[2]) + acc) {
                    if (probs[3] + probs[2] + probs[1] + probs[0] < 1 + acc &&
                        probs[3] + probs[2] + probs[1] + probs[0] > 1 - acc) {
                        grid.push_back(probs);
                    }
                    probs[3] += step;
                }
                probs[2] += step;
            }
            probs[1] += step;
        }
        probs[0] += step;
    }
    return grid;
}

double cmp(vector<int> real, vector<int> my) {
    double len;
    double penalty = 0;
    if (real.size() < my.size()) {
        len = real.size();
        penalty = (my.size() - real.size()) * 0,1;
    } else {
        len = my.size();
        penalty = (real.size() - my.size()) * 0,1;
    }
    double cnt = 0;
    for (size_t i = 0; i < len; ++i) {
        if (real[i] == my[i]) {
            cnt += 1.0;
        } else if (real[i] == find_symmetric_vertex(my[i])) {
            cnt += 0.5;
        }
    }
    cnt -= penalty;
    return cnt / len;
}

vector<double> gen_cmp(vector<vector<int>> real_routes, vector<vector<int>> my_routes) {

    vector<double> ans;

    for (int i = 0; i < real_routes.size(); ++i) {
        for (int j = 0; j < my_routes.size(); ++j) {
            ans.push_back(cmp(real_routes[i], my_routes[j]));
        }
    }
    return ans;
}

vector<vector<double>> good_probs(Maze &maze, Rat &roger) {

    double step = 0.1, maxi = 0.9, minim = 0.05;
    std::vector<std::vector<double>> all = gen_grid({0.1, 0.1, 0.1, 0.1}, {0.9, 0.9, 0.9, 0.9}); //создаем всевозможные комбинации вероятностей
    vector<vector<double>> ans_probs;
    cout << all.size() << "\n";

    std::ifstream data_read("data_eng.txt");
    string line;
    vector<vector<int>> real_routes;
    while (getline(data_read, line)) {
        stringstream ss(line);
        string route;
        ss >> route;
        vector<int> real_route;
        for (int i = 0; i < route.size(); ++i) {
            std::string route_i(1, route[i]);
            if (eng_to_int[route_i] != 0) {
                real_route.push_back(eng_to_int[route_i]);
            }
        }
        real_routes.push_back(real_route);
    }
    data_read.close();

    int min = 1000;
    int delimiter = 100;
    roger.set_probabilities_of_relative_dirs(18.25 / delimiter, 3.32 / delimiter, 4.15 / delimiter, 11.03 / delimiter, 16.98 /delimiter, 14.96 / delimiter, 15.34 / delimiter, 15.97 /delimiter);

    for (auto prob: all) {
        //int delimiter = 100;
        //roger.set_probabilities_of_relative_dirs(18.25 / delimiter, 3.32 / delimiter, 4.15 / delimiter, 11.03 / delimiter, 16.98 /delimiter, 14.96 / delimiter, 15.34 / delimiter, 15.97 /delimiter);
        maze.set_probabilities_for_operators({0.0, prob[0], prob[1], prob[2], prob[3]});
        maze.conduct_series_of_experiments_memorizing_routes(1, roger);
        vector<vector<int>> my_routes = maze.return_all_generated_routes();
        while (my_routes.empty()) {
            maze.conduct_series_of_experiments_memorizing_routes(1, roger);
            vector<vector<int>> my_routes = maze.return_all_generated_routes();
        }


        vector<double> accur = gen_cmp(real_routes, my_routes);
        double max = 0.0;
        for (auto i : accur) {
            if (i > max) {
                max = i;
            }
        }
        ans_probs.push_back({max, prob[0], prob[1], prob[2], prob[3]});
    }
    return ans_probs; // возвращаем оптимальные вероятности
}

int main() {
    setlocale(LC_ALL, "ru");
    Maze maze = Maze();
    std::ifstream in_for_maze("Nikolskaya_maze_info.txt");
    maze.typeOfMaze = type_of_maze::nikolskaya;

    if (in_for_maze.is_open()) {
        cout << "File with maze description has been opened for input successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file with maze description for input!\n";
        exit(1);
    }

    std::ofstream out;          // поток для записи
    out.open("output2.txt"); // окрываем файл для записи
    if (in_for_maze.is_open()) {
        cout << "File for output opened successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file for output!\n";
        exit(1);
    }

    maze.construct(in_for_maze, out);
    in_for_maze.close();
    maze.print_info_about_cells_in_letters(
            out); // В данный момент применяется в программе, но для ускорения мы не будем выводить пути


    /*std::ifstream in_for_experiments("Nikolskaya_experiments.txt");
    if (in_for_experiments.is_open()) {
        cout << "File for input of experiment_results opened successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file of experiment_results for input!\n";
        exit(1);
    }*/
    cout << "Started reading experiment results!\n";
    // maze.read_experiment_results(in_for_experiments, out);
    cout << "Finished reading experiment results!\n";
    cout << "Started writing experiment results!\n";
    // maze.write_real_experiment_results_to_one_vector();
    cout << "Finished writing experiment results!\n";

    Rat roger;
    maze.construct_map_from_name_to_operator();

    double delimiter = 100;
    roger.set_probabilities_of_relative_dirs(18.25 / delimiter, 3.32 / delimiter, 4.15 / delimiter, 11.03 / delimiter, 16.98 /delimiter, 14.96 / delimiter, 15.34 / delimiter, 15.97 /delimiter);
    vector<vector<double>> ans = good_probs(maze, roger);
    cout << "cmp() and good_probs() values:\n";
    for (auto i : ans) {
        for (auto j : i) {
            cout << j << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    out.close();
    out << "\nDONE!\n";
    std::cout << "End of program" << "\n";
    return 0;
}
