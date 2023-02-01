#include <iostream>
#include <vector>
#include <cstdlib>      // rand()
#include <unordered_set>
#include <numeric>      // std::iota()
#include <algorithm>    // std::shuffle, std::max_element, std::fill, std::unique, std::find_if
#include <random>       // std::default_random_engine

using std::cout;
using std::endl;
using std::vector;

#define ANSI_COLOR_BG_CYAN  "\e[46m"
#define ANSI_COLOR_RESET    "\e[0m"

long nanos() {
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
   return ts.tv_nsec + ts.tv_sec*1000000000L;
}


// Codility - Lesson 4-1, FrogRiverOne
/*
   initially @pos 0
   goal      @pos X+1
   N and X in [1...100000]
   each A[k] in [1...X]
   A[k] == position where one leaf falls at time k
   => find earliest time k when all positions from 1 to X are covered by leaves
   => min k : all values from 1,...,X appear in A[0],...,A[k] 
*/
int solution1(int X, vector<int> &A) {
   std::unordered_set<int> pos_covered; // position, occurred/covered by leaf
   for (size_t i=0; i<A.size(); ++i) {
      const auto& Ai = A[i];
      if (Ai<=X) {
         pos_covered.insert(Ai);
         if (pos_covered.size()==X)
            return i;
      }
   }

   return -1;
}

// Codility - Lesson 4-2, MaxCounters
/*
   increase(X) – counter X is increased by 1
   max counter – all counters are set to the maximum value of any counter

   A non-empty array A of M integers is given. This array represents consecutive operations:

   if A[K] == X, such that 1<=X<=N, then operation K is increase(X),
   if A[K] == N+1 then operation K is max counter
*/
vector<int> solution2(int N, vector<int> &A) {
   /*
      TODO: still timing out on 1 performance test (0.104 vs 0.100 sec.)
   */
   vector<int> counters(N);

   auto print_counters = [&counters]() {
      cout << "(";
      for (const auto& c : counters) { cout << c << ", "; }
      cout << "\b\b)\n";
   };

   int cur_max = 0;     // has to be set to 0 initially!
   auto max_counter = [&counters,&cur_max]() {
      //const auto max_val = *std::max_element(counters.begin(), counters.end());
      //std::fill(counters.begin(), counters.end(), max_val);
      std::fill(counters.begin(), counters.end(), cur_max);
   };
   bool can_consume_new_max = false; // to avoid std:fill-calls with same max-val
   // TODO: executing lots of max_counter-calls will be costly...
   //       I am missing sth. here...
   // => track max_elem and only execute max_counter, if max is actually new
   
   for (const auto& op : A) {
      //if (1 <= op && op <= N) {
      //if (!(1 > op || op > N)) {
      //if (op <= N) {
      if (op != (N+1)) {
         // increase X
         counters[op-1]++;
         // update current max-value and indicate that next max_counter-call needs to happen
         if (counters[op-1]>cur_max) {
            cur_max = counters[op-1];
            can_consume_new_max = true;
         }
      } else {
         // max counter
         if (can_consume_new_max) {
            max_counter();
            can_consume_new_max = false;
         }
      }
      //print_counters();
   }

   return counters;

   /*
   // other strategy: as all counters are reset on max_counter-call,
   //                 find last max_counter-call in A and go from there
   // BUT: have to know the max so far, which depends on past !!
   auto pos = std::find(A.rbegin(), A.rend(), N+1);
   if (pos != A.rend()) {
      // found at least one 'max counter' op
      auto idx = (pos.base()-A.begin())-1; // for forward-iterators 'std::find(A.begin(), A.end(), N+1);' => pos-A.begin();
      cout << "@idx " << idx << ":" << *pos << "\n";
      
      //std::fill(counters.begin(), counters.end(), max_val);
   }
   return counters;
   */
}


// Codility - Lesson 4-3, MissingInteger
int solution3(vector<int> &A) {

   auto print_vec = [](const vector<int>& v, const std::string& tag="") {
      cout << tag;
      for (const auto& e : v) { cout << e << ", "; } cout << "\b\b \n";
   };
   
   std::sort(A.begin(), A.end());
   auto end_unique = std::unique(A.begin(), A.end());
   //print_vec(A);

   // check if we have a val >=0 at all - if none, return 1 (all vals <0)
   // else: we have at least one val >=0, start calculating diffs from there
   // and find first one that is >1 => return A at this idx + 1,
   // or last_val+1, if no diff > 1
   auto pos = std::find_if(A.begin(), end_unique, [](int i){ return i>=0; });
   if (pos==end_unique || *pos>1) {
      // all < 0 or first elem >=0 is also >1, so 1 is not among vals of A
      return 1;
   } else {

      for (; std::next(pos) != end_unique; ++pos) {
         const int d = *std::next(pos) - *pos;
         if (d>1)
            return *pos + 1;
      }
      return *std::prev(end_unique) + 1;
   }

   return -1; // should never get here!
}

// Codility - Lesson 4-4, PermCheck
int solution4(vector<int> &A) {
   const auto N = A.size();
   std::unordered_set<int> num_covered;
   for (size_t i=0; i<N; ++i) {
      const auto& Ai = A[i];
      if (Ai<=N)
         num_covered.insert(Ai);
      else
         // if Ai>N, cannot be a permutation
         return 0;
   }

   return (num_covered.size()==N) ? 1 : 0;
}

void run_eval_1(int (*solution1)(int X, vector<int> &A), int X, vector<int> &A) {
   long start = nanos();
   int res = solution1(X, A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_2(vector<int> (*solution2)(int N, vector<int> &A), int N, vector<int> &A) {
   long start = nanos();
   vector<int> res = solution2(N, A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " ";
      for (const auto& r : res) { cout << r << " "; }
   cout << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_3(int (*solution3)(vector<int> &A), vector<int> &A) {
   int res;
   long start = nanos();
   res = solution3(A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_4(int (*solution4)(vector<int> &A), vector<int> &A) {
   int res;
   long start = nanos();
   res = solution4(A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}


int main()
{
   {
      cout << "–– solution1 ––\n";
      int X = 5;
      vector<int> arr = { 1, 3, 1, 4, 2, 3, 5, 4 };
      cout << "X = " << X << ", used numbers (" << arr.size() << "): ";
      for (const auto& n : arr) { cout << n << ", "; }
      cout << "\b\b \n";

      cout << "Need to find the smallest k for the given A, s.t. all the positions (1.."<<X<<") have occurred in A[k].\n";
      run_eval_1(solution1, X, arr);
      
      const size_t N = 100;
      vector<int> arr_long(N);
      //srand(time(NULL)); // want different result each run
      for (auto& elem : arr_long) {
         elem = (rand() % X) +1; // [0,X-1]+1 == [1,X]
      }
      cout << "used numbers (" << arr_long.size() << "): ";
      for (const auto& n : arr_long) { cout << n << ", "; }
      cout << "\b\b \n";

      run_eval_1(solution1, X, arr_long);
   }
   cout << endl;

   {
      cout << "–– solution2 ––\n";
      int N = 5;
      vector<int> arr = { 3, 4, 4, 6, 1, 4, 4 };
      cout << "Used numbers:\n";
      for (const auto& n : arr) { cout << n << ", "; }
      cout << "\b\b \n";

      run_eval_2(solution2, N, arr);

      const size_t M = 20;
      vector<int> arr_long(M);
      //srand(time(NULL)); // want different result each run
      for (auto& elem : arr_long) {
         elem = (rand() % N+1) +1; // [0,N]+1 == [1,N+1]
      }
      cout << "long array to test solution2 contains:" << endl;
      for (const auto& elem : arr_long) { cout << elem << ", "; }
      cout << "\b\b \n";

      run_eval_2(solution2, N, arr_long);
   }
   cout << endl;

   {
      cout << "–– solution3 ––\n";
      vector<int> arr1 = { 1, 3, 6, 4, 1, 2 };
      vector<int> arr2 = { 1, 2, 3 };
      vector<int> arr3 = { -1, -3 };
     
      cout << "arr1: "; for (const auto& n : arr1) { cout << n << ", "; } cout << "\b\b \n";
      run_eval_3(solution3, arr1);
      
      cout << "arr2: "; for (const auto& n : arr2) { cout << n << ", "; } cout << "\b\b \n";
      run_eval_3(solution3, arr2);
      
      cout << "arr3: "; for (const auto& n : arr3) { cout << n << ", "; } cout << "\b\b \n";
      run_eval_3(solution3, arr3);
      
      const size_t N = 20;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 2001) -1000; // [0,2000]-1000 == [-1000,+1000]
      }
      cout << "long array to test solution3 contains:" << endl;
      for (const auto& elem : arr_long) {
         cout << elem << ", ";
      }
      cout << "\b\b  " << endl;

      run_eval_3(solution3, arr_long);
   }
   cout << endl;
   
   {
      cout << "–– solution4 ––\n";
      vector<int> arr1 = { 4, 1, 3, 2 };
      vector<int> arr2 = { 4, 1, 3 };
      
      cout << "arr1: "; for (const auto& n : arr1) { cout << n << ", "; } cout << "\b\b \n";
      run_eval_4(solution4, arr1);
      
      cout << "arr2: "; for (const auto& n : arr2) { cout << n << ", "; } cout << "\b\b \n";
      run_eval_4(solution4, arr2);
      
      const size_t N = 60;
      vector<int> arr_long(N);
      std::iota(arr_long.begin(), arr_long.end(), 1);
      std::shuffle(arr_long.begin(), arr_long.end(), std::default_random_engine());
      cout << "long array to test solution4 contains all numbers from 1 to " << N << " randomly shuffled:" << endl;
      for (const auto& elem : arr_long) {
         cout << elem << ", ";
      }
      cout << "\b\b \n";

      run_eval_4(solution4, arr_long);
      
      arr_long[N-N/4] = 1;;
      cout << "long array to test solution4 now contains a second 1 at pos "  << (N-N/4) << endl;

      run_eval_4(solution4, arr_long);
   }
   cout << endl;

   return 0;
}
