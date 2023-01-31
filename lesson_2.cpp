#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>      // rand()
#include <map>
#include <unordered_map>
#include <numeric>      // iota()
#include <algorithm>    // shuffle, rotate
#include <random>       // default_random_engine
#include <iomanip>      // setw

using std::cout;
using std::endl;
using std::string;
using std::vector;

#define ANSI_COLOR_BG_CYAN  "\e[46m"
#define ANSI_COLOR_RESET    "\e[0m"

long nanos() {
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
   return ts.tv_nsec + ts.tv_sec*1000000000L;
}


// Codility - Lesson 2-1, Cyclic Rotation
vector<int> solution1_1(vector<int> &A, int K) {
   const size_t N = A.size();
   vector<int> res = vector<int>(N);
   for (size_t i = 0; i < N; ++i) {
      res[(i+K)%N] = A[i];
   }
   return res;
}

vector<int> solution1_2(vector<int> &A, int K) {
   if (A.empty()) return A;
   // std::rotate rotates left. Rotating K right == roating (N-K) left
   const int N = A.size();
   int offset = N-(K%N);
   std::rotate(A.begin(),A.begin()+offset,A.end());
   return A;
}

// modulo '%' as in Python
int mod_idx_wrap(int num, int N) {
   return num >= 0 ? (num%N) : ((num%N+N)%N);
}

// Codility - Lesson 2-2, OddOccurrencesInArray
/*
   - non-empty array A contains an odd number of elements
   - each element of the array can be paired with another element that has the same value,
     except for one element that is left unpaired
   - TODO: return the value of the unpaired element
*/
template<typename K, typename V>
using map = std::unordered_map<K,V>; // std::map<K,V>, std::unordered_map<K,V>

int solution2(vector<int> &A) {
   // one element of map is: (key, value) <-> (int, num_occurrences)
   map<int,int> map_num_occurrences;

   for (const auto& n : A) {
      // increment num of occurrences of number (and add entry, if it doesn't exist yet)
      map_num_occurrences[n]++;        
   }

   //for (const auto& e : map_num_occurrences) {
   //   cout << e.first << ": " << e.second << endl;
   //}

   auto is_odd = [](int n) { return (n%2); };

   for (const auto& e : map_num_occurrences) {
      if (is_odd(e.second)) {
         return e.first;
      }
   }

   return -1;
}

void run_eval_1(vector<int> (*solution1)(vector<int> &A, int K), vector<int> &A, int K) {
   vector<int> res;
   long start = nanos();
   res = solution1(A, K);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " ";
   for (const auto& n : res) {
         cout << n << ", ";
   }
   cout << "\b\b " << ANSI_COLOR_RESET << "\n";
   cout << "took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_2(int (*solution2)(vector<int> &A), vector<int> &A) {
   int res;
   long start = nanos();
   res = solution2(A);
   long elapsed_ns = nanos() - start;
   cout << "=> result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

// all elems appear twice, except for one
// make sure N is odd!!!
vector<int> generate_sample_data(const size_t N) {

   vector<int> arr_long(N);
   vector<int> indices(N);
   // 0,..,N-1
   std::iota(indices.begin(), indices.end(), 0);
   // shuffle, so adjacent elems in indices are randomly apart
   std::shuffle(indices.begin(), indices.end(), std::default_random_engine());

   // generate random number in interval [1,1 000 000 000]
   const int max_val = 1000000000;
   auto gen_num = [max_val]() { return (rand() % max_val) + 1; };
   
   for (size_t i = 0; i < N/2; ++i) {
      const int num = gen_num();
      //cout << "num " << num << " @indices " << "(" << indices[2*i  ] << "," << indices[2*i+1] << ")" << endl;
      // choose 2 random indices, where to put them
      arr_long[indices[2*i  ]] = num;
      arr_long[indices[2*i+1]] = num;
   }
   //cout << "last remaining indices=" << indices.back() << endl;
   arr_long[indices.back()] = gen_num();
   return arr_long;
}

int main()
{
   {
      cout << "–– mod_idx_wrap ––\n";
      const int N = 5;
      vector<int> nums = {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
      for (const auto& n : nums) {
         cout << "n%N: " << std::setw(2) << n << " % " << N << " = " << mod_idx_wrap(n,N) << "\n";
      }
   }
   cout << endl;

   // task 1
   {
      vector<int> arr = {3, 8, 9, 7, 6};
      int K = 2;

      cout << "Used numbers: ";
      for (const auto& n : arr) {
         cout << n << ", ";
      }
      cout << "\b\b  \n";
      cout << "K=" << K << "\n";

      cout << "–– solution1_1 ––\n";
      run_eval_1(solution1_1, arr, K);

      cout << "–– solution1_2 ––\n";
      run_eval_1(solution1_2, arr, K);
   }
   cout << endl;

   // task 2
   {
      vector<int> arr = {9, 3, 9, 3, 9, 7, 9};

      const size_t N = 1001; // make sure this is odd!!!
      auto arr_long = generate_sample_data(N);

      cout << "Used numbers: ";
      for (const auto& n : arr) {
         cout << n << ", ";
      }
      cout << "\b\b  \n";
      
      cout << "–– solution2 (on small array) ––\n";
      run_eval_2(solution2, arr);

      cout << "–– solution2 (on bigger array of size " << arr_long.size() << ") ––\n";
      run_eval_2(solution2, arr_long);
   }

   return 0;
}