#include <iostream>
#include <vector>
#include <cstdlib>         // rand()
#include <unordered_map>
#include <numeric>         // std::iota(), std::accumulate
#include <algorithm>       // std::shuffle, std:min_element
#include <random>          // std::default_random_engine

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


// Codility - Lesson 3-1, FrogJmp
int solution1(int X, int Y, int D) {
   int dist2cover = Y - X;
   int numDjumps = (dist2cover/D);

   // if (dist2cover % D)==0 => distance is exactly numDjumps jumps,
   // otherwise not exactly, but numDjumps jumps + D, i.e. numDjumps+1 jumps
   return (dist2cover % D)==0 ? numDjumps : (numDjumps+1);
}

// Codility - Lesson 3-2, PermMissingElem
int solution2(vector<int> &A) {
   const size_t N = A.size();
   int num_buckets = N+1;
   std::unordered_map<int,int> map_num_cnt(num_buckets);
   cout << "hashmap with " << map_num_cnt.bucket_count() << " buckets\n";

   // init map with key in [1,N+1] with 0 
   for (unsigned int i = 1; i <= N+1; ++i)
      map_num_cnt[i] = 0;

   // increment where number is found
   for (const auto& n : A)
      map_num_cnt[n]++;

   // print map
   for (const auto& e : map_num_cnt)
      cout << e.first << " –> " << e.second << "\n";

   // the one that's still has value 0 is our result
   for (const auto& e : map_num_cnt) {
      if (e.second == 0)
         return e.first;
   }

   return -1;
}


// Codility - Lesson 3-3, TapeEquilibrium
// N>=2, 0<P<N
// part1: A[0], A[1], ..., A[P-1]
// part2: A[P], A[P+1], ..., A[N-1]
int solution3(vector<int> &A) {
   const size_t N = A.size();
   const size_t NUM_SPLITS = N-1; // 0<P<N => P \in {1,...,N-1}

   // N is assumed to contain at least 2 elements, so don't need this
   //if (N==1) return -1;

   vector<int> diffs(NUM_SPLITS);

   /*// version 1:
   for (unsigned int P=1; P<N; ++P) {
      // std::accumulate computes sum of [start,end) 
      auto left_sum = std::accumulate(A.begin(), A.begin()+P, 0);
      auto right_sum = std::accumulate(A.begin()+P, A.end(), 0);
      auto diff = std::abs(left_sum-right_sum);
      //cout << "P=" << P << ", diff=" << diff << endl;
      diffs[P-1] = diff;
   }*/

   /*
      version 2: much more efficient variant, where 
         on left side: add one additional value each run
         on right side: subtract last first value and add one new value
      => only 3 additions per P
   */
   // init for P=1
   auto left_sum  = A[0];
   auto right_sum = std::accumulate(A.begin()+1, A.end(), 0);
   diffs[0] = std::abs(left_sum-right_sum);

   for (unsigned int P=2; P<N; ++P) {
      left_sum  = left_sum + A[P-1];
      right_sum = right_sum - A[P-1];
      auto diff = std::abs(left_sum-right_sum);
      diffs[P-1] = diff;
   }

   // print
   //for (unsigned int P=1; P<N; ++P)
   //   cout << "P=" << P << ", diff=" << diffs[P-1] << "\n";

   auto min_val = *std::min_element(diffs.begin(),diffs.end());
   return min_val;
}

void run_eval_1(int (*solution1)(int X, int Y, int D), int X, int Y, int D) {
   long start = nanos();
   int res = solution1(X, Y, D);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_2(int (*solution2)(vector<int> &A), vector<int> &A) {
   long start = nanos();
   int res = solution2(A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_3(int (*solution3)(vector<int> &A), vector<int> &A) {
   long start = nanos();
   int res = solution3(A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}


int main()
{
   {
      cout << "–– solution1 ––\n";
      int X, Y, D;

      X = 10; Y = 85; D = 30;
      cout << "X=" << X << ", Y=" << Y << ", need to cover a distance of " << Y-X << " with jumps à " << D << "\n";
      run_eval_1(solution1, X, Y, D);

      X = 25; Y = 85; D = 30;
      cout << "X=" << X << ", Y=" << Y << ", need to cover a distance of " << Y-X << " with jumps à " << D << "\n";
      run_eval_1(solution1, X, Y, D);
   }
   cout << endl;

   {
      cout << "–– solution2 ––\n";
      vector<int> arr = {2, 3, 1, 5};
      
      cout << "used numbers (" << arr.size() << "): ";
      for (const auto& n : arr) {
         cout << n << ", ";
      }
      cout << "\b\b  \n";
      
      run_eval_2(solution2, arr);
      
      const size_t N = 100;
      vector<int> arr_long(N+1);
      std::iota(arr_long.begin(), arr_long.end(), 1);
      std::shuffle(arr_long.begin(), arr_long.end(), std::default_random_engine());
      const size_t idx2delete = rand() % (N+1);
      
      cout << "deleting element in vector @idx " << idx2delete << " with value " << arr_long[idx2delete] << "\n";
      arr_long.erase(arr_long.begin()+idx2delete);
      
      cout << "used numbers (" << arr_long.size() << "):\n";
      for (const auto& n : arr_long) {
         cout << n << ", ";
      }
      cout << "\b\b  \n";
      
      run_eval_2(solution2, arr_long);
   }
   cout << endl;
   
   {
      cout << "–– solution3 ––\n";
      vector<int> arr = {3, 1, 2, 4, 3};
      run_eval_3(solution3, arr);
      
      const size_t N = 1000;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 2001) -1000; // [0,2000]-1000 == [-1000,+1000]
      }

      cout << "used numbers (" << arr_long.size() << "):\n";
      for (const auto& elem : arr_long) {
         cout << elem << ", ";
      }
      cout << "\b\b  \n";
      
      run_eval_3(solution3, arr_long);
   }
   cout << endl;

   return 0;
}
