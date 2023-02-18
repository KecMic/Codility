#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>      // rand()
#include <set>
#include <unordered_set>
#include <numeric>      // std::iota()
#include <algorithm>    // std::sort, std::unique, std::max
#include <utility>      // std::pair

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


// Codility - Lesson 6-1, Distinct
int solution1_1(vector<int> &A) {
   std::set<int> distinct_nums(A.begin(), A.end());
   //cout << "found the following distinct nums in A: ";
   //for (const auto& n : distinct_nums) { cout << n << ", "; } cout << "\n";
   return distinct_nums.size();
}
int solution1_2(vector<int> &A) {
   std::unordered_set<int> distinct_nums(A.begin(), A.end());
   return distinct_nums.size();
}
int solution1_3(vector<int> &A) {
   // sort followed by unique to "remove" all duplicates
   // for empty vector: A.begin()==A.end()
   //    `sort`   : should be noop/immediate return
   //    `unique` : returns A.begin(), so subsequent difference will be 0 (as it should)
   std::sort(A.begin(),A.end());
   auto end = std::unique(A.begin(),A.end());
   return end-A.begin();
}
int solution1_4(vector<int> &A) {
   if (A.empty()) return 0;

   std::sort(A.begin(),A.end());
   int N_distinct_nums = 1;
   for (size_t i=1; i<A.size(); ++i) {
      if (A[i] != A[i-1])
         N_distinct_nums += 1;
   }

   return N_distinct_nums;
}

// Codility - Lesson 6-2, MaxProductOfThree
/*
   - N is an int within range [3..100,000]
   - each elem of A is an int within range [−1,000..1,000]

   want: (all 3 != 0) && ((2 neg, 1 pos) || (0 neg, 3 pos) || (3 neg with smallest abs))

   max product of sorted array
      -) largest 3 strictly positive (>0) vals (large abs)
      -) largest 3 strictly negative (<0) vals (small abs) (if all vals in A are negative)
         => A[N-3]*A[N-2]*A[N-1]
      -) 2 smallest strictly negative (<0) (large abs) and 1 largest strictly positive (>0) (large abs) val
         => A[0]*A[1]*A[N-1]
*/
int solution2(vector<int> &A) {
   const size_t N = A.size();
   std::sort(A.begin(),A.end());
   //cout << "sorted (in ascending order by default, from smallest to largest): ";
   //for (const auto& e : A) { cout << e << ","; } cout << "\b \n";
   return std::max( A[0]*A[1]*A[N-1], A[N-3]*A[N-2]*A[N-1] );
}

// Codility - Lesson 6-3, NumberOfDiscIntersections
/*
   - N is an int within range [0..100,000]
   - each elem of A is an int within range [0..2,147,483,647 == 2**31-1]
   - A[j] == radius of disk j, centered at (j,0)
   - discs contain their borders => 2 discs are tangent => they intersect

   => nice O(N) algorithm described here:
         https://stackoverflow.com/questions/30497470/numberofdiscintersections-overflow-in-codility-test
         https://math.stackexchange.com/questions/1871124/codility-numberofdiscintersections-100
   => subsequently is an illustration of the relevant results in each step of the algo
      >> quite beautiful how this works!!! <<

   A = [1,5,2,1,4,0], => N=6

        idx
     [–––+–––]
   idx-r    idx+r  // r := A[idx]

   idx | r | idx-r | idr+r        -4 -3 -2 -1  0  1  2  3  4  5  6  7  8  
   ––––+–––+–––––––+––––––       ––+––+––+––+––+––+––+––+––+––+––+––+––+––
       |   |       |                                                      
    0  | 1 |  -1   |  +1         –––––––––––[##o##]–––––––––––––––––––––––
    1  | 5 |  -4   |  +6         ––[##############o##############]––––––––
    2  | 2 |   0   |  +4         ––––––––––––––[#####o#####]––––––––––––––
    3  | 1 |  +2   |  +4         ––––––––––––––––––––[##o##]––––––––––––––
    4  | 4 |   0   |  +8         ––––––––––––––[###########o###########]––
    5  | 0 |  +5   |  +5         –––––––––––––––––––––––––––––o–––––––––––
   
   +–––+––––––––+––––––––+–––––––––––––––––+–––––––––––––––––+–––––––––––––––––––––+––––––––––––––––––––+
   | i | i-A[i] | i+A[i] | s=[0,0,0,0,0,0] | e=[0,0,0,0,0,0] | res-inc; res        | active-inc; active |
   +–––+––––––––+––––––––+–––––––––––––––––+–––––––––––––––––+–––––––––––––––––––––+––––––––––––––––––––+
   | 0 |   -1   |   +1   |   [1,0,0,0,0,0] |   [0,1,0,0,0,0] | 0·0 + 4·3/2 = 6;  6 |  4; 4              |
   | 1 |   -4   |   +6   |   [2,0,0,0,0,0] |   [0,1,0,0,0,1] | 4·0 + 0     = 0;  6 | -1; 3              |
   | 2 |    0   |   +4   |   [3,0,0,0,0,0] |   [0,1,0,0,1,1] | 3·1 + 0     = 3;  9 |  1; 4              |
   | 3 |   +2   |   +4   |   [3,0,1,0,0,0] |   [0,1,0,0,2,1] | 4·0 + 0     = 0;  9 |  0; 4              |
   | 4 |    0   |   +8   |   [4,0,1,0,0,0] |   [0,1,0,0,2,2] | 4·0 + 0     = 0;  9 | -2; 2              |
   | 5 |   +5   |   +5   |   [4,0,1,0,0,1] |   [0,1,0,0,2,3] | 2·1 + 0     = 2; 11 | -2; 0              |
   +–––+––––––––+––––––––+–––––––––––––––––+–––––––––––––––––+–––––––––––––––––––––+––––––––––––––––––––+
   => result = 11

*/
int solution3(vector<int> &A) {
   const size_t N = A.size();
   
   #if 0
   vector<size_t> ind(N);
   std::iota(ind.begin(),ind.end(),0);
   std::sort(ind.begin(),ind.end(), [&A](int i,int j){ return A[i]>A[j]; });
   cout << "indices sorted:  "; for (const auto& e : ind) { cout << e << ","; } cout << "\b \n";
   cout << "A sorted:        "; for (const auto& e : ind) { cout << A[e] << ","; } cout << "\b \n";
   #endif


   // O(N) solution
   vector<int> start_pts(N);
   vector<int> end_pts(N);
   long res = 0;
   long tmp;
   size_t idx;
   int active = 0;
   
   for (long i=0; i<N; ++i) {
      tmp = i - A[i];
      idx = tmp < 0 ? 0 : tmp;
      start_pts[idx]++;

      tmp = i + A[i];
      idx = tmp >= N ? N-1 : tmp;
      end_pts[idx]++;
   }
   
   for (long i=0; i<N; ++i) {
      // active * start_pts[i]:
      //    intersections between already open (active) disks and newly opened disks
      // (start_pts[i]*(start_pts[i]-1))/2:
      //    intersections among newly opened disks (start_pts[i] choose 2)
      res += active * start_pts[i] + (start_pts[i]*(start_pts[i]-1))/2;
      if (res > 10000000) return -1;
      active += start_pts[i] - end_pts[i];
   }

   return res;


   #if 0
   // brute-force solution O(N^2)
   long N_intersecting_pairs = 0;
   for (size_t i=0; i<N; ++i) {
      for (size_t j=i+1; j<N; ++j) {
         cout << "(" << i << "," << j << ") "; 
         auto disk_centers_distance = j-i;
         long radii_sum = (long)A[i]+A[j];
         if (disk_centers_distance <= radii_sum)
            N_intersecting_pairs++;
      }
   }
   cout << endl;
   return (N_intersecting_pairs > 10000000) ? -1 : N_intersecting_pairs;
   #endif
}

// Codility - Lesson 6-4, Triangle
/*
   - N is an int within range [0..100,000]
   - each elem of A is an int within range [−2,147,483,648..2,147,483,647]
   - a triplet (P,Q,R) is triangular if 0<=P<Q<R<N AND:
      (A[P] + A[Q]) > A[R]
      (A[Q] + A[R]) > A[P]
      (A[R] + A[P]) > A[Q]
   
   sorted A, 0<=P<Q<R<N
      a1 = A[P];
      a2 = A[Q];
      a3 = A[R];
   sorting guarantees a1 <= a2 <= a3, not a1<a2<a3 !!

      ,´                     ,´            
      ' a1 + a2      > a3    ' a1 > a3 - a2
      '      a2 + a3 > a1    ' a1 < a2 + a3
      ' a1      + a3 > a2    ' a1 > a2 - a3
      `.                     `.            
      
      => as a2 <= a3:
            a3-a2 >= 0
            a2-a3 <= 0

      ,´                ,´            
      ' a1 > a3 - a2    ' a1 > a3 - a2
      ' a1 > a2 - a3    ' 
      `.                `.            
   
   => so far, the following has to hold:
       ,´            
       ' a1 < a2 + a3   // p < q+r
       ' a1 > a3 - a2   // p > r-q
       `.            

      => as a1 <= a2 <= a3
            ::: recall: (1 < 2), and (-2 < -1) :::
                                    |
            ~~~~~[-2]~~~~~[-1]~~~~~[0]~~~~~[+1]~~~~~[+2]~~~~~
                                    |
            e.g.:
               a1,a2,a3 = 0,0,0
               a1,a2,a3 = 1,1,1
               a1,a2,a3 = 1,2,3
               
               a1,a2,a3 = -2,-1,0
               a1,a2,a3 = -1,0,1

               a1,a2,a3 = -1,-1,-1
               a1,a2,a3 = -3,-2,-1

         "a1 < a2 + a3" is always valid, EXCEPT
            - if all 3 are 0              (a1==a2==a3==0)
            - if all 3 are <0             (if a3<0, all are <0)
            - if a1==a2 !=0 and a3==0     (a1<0,a2<0 because of ordering)
            - if a1==a3 !=0 and a2==0     (a1<0,a3>0 because of ordering)
            - ...
         "a1 < a2 + a3" is valid,
            - if a1==0, a2!=0 || a3!=0    (a2>=0,a3>=0 because of ordering)
                  but for "a1 > a3 - a2",
                  it must hold 0 > a3 - a2, i.e. it must hold a2 > a3,
                  but from ordering: a2 <= a3, which contradicts a2 > a3
                  so for a1==0, "a1 < a2 + a3" AND "a1 > a3 - a2" cannot be both true
            - if a1>0, a2>0,a3>0          (a2>0,a3>0 because of ordering)
         => "a1 < a2 + a3" in conjunction with "a1 > a3 - a2" is only valid if "a1>0"
         => replace "a1 > a3 - a2" by "a1>0" in conjunction with "a1 > a3 - a2"

   => so the following has to hold:
    ,´            
    ' a1 > 0         // p > 0
    ' a1 > a3 - a2   // p > r-q
    `.            
*/
int solution4(vector<int> &A) {
   const size_t N = A.size();
   if (N<3) return 0;

   std::sort(A.begin(),A.end());
   //cout << "A sorted: "; for (const auto& e : A) { cout << e << ","; } cout << "\b \n";

   for (size_t i=0; i<N-2; ++i) {
      const auto& p = A[i];
      const auto& q = A[i+1];
      const auto& r = A[i+2];
      //if (p<((long)q+r) && p>((long)r-q)) {
      if (p>0 && p>((long)r-q)) {
         //cout << "found a triangular triplet: (?,?,?):(" << p << "," << q << "," << r << ")\n";
         return 1;
      }
   }
   
   return 0;
}

void run_eval(int (*solution)(vector<int> &A), vector<int> &A) {
   long start = nanos();
   int res = solution(A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}


int main()
{
   {
      cout << "–– solution1 ––\n";
      vector<int> A = {2, 1, 1, 2, 3, 1};
      cout << "A:  ";
      for (const auto& elem : A) { cout << elem << ","; } cout << "\b \n";
      
      const size_t N = 20;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 21) -10; // [0,20]-10 == [-10,+10]
      }
      cout << "long array to test solution1 contains: \n";
      for (const auto& elem : arr_long) { cout << elem << ","; } cout << "\b \n";
      
      using T = std::pair<int(*)(vector<int> &A),string>;
      auto zipped = {
         T{solution1_1,"solution1_1"},
         T{solution1_2,"solution1_2"},
         T{solution1_3,"solution1_3"},
         T{solution1_4,"solution1_4"}
      };
      for (const auto& [sol,label] : zipped) {
         cout << "–– " << label << ":\n";
         run_eval(sol,A);
         run_eval(sol,arr_long);
      }
   }
   cout << endl;

   {
      cout << "–– solution2 ––\n";
      vector<int> A1 = {-3, 1, 2, -2, 5, 6};
      vector<int> A2 = {-3, 1, 2, -2, 5};
      vector<int> A3 = {-3, 1, 2, -2, 5, 6, 3};
      vector<int> A4 = {-5, 5, -5, 4};
      vector<int> A5 = {4, 7, 3, 2, 1, -3, -5};
      vector<int> A6 = {4, 5, 1, 0};
      vector<int> A7 = {-1, -2, -3, -4, -5};
      using T = std::pair<vector<int>&,string>;
      auto zipped = {
         T{A1,"A1: "},
         T{A2,"A2: "},
         T{A3,"A3: "},
         T{A4,"A4: "},
         T{A5,"A5: "},
         T{A6,"A6: "},
         T{A7,"A7: "}
      };
      for (const auto& [A,label] : zipped) {
         cout << label;
         for (const auto& elem : A) { cout << elem << ","; } cout << "\b \n";
         run_eval(solution2, A);
      }

      const size_t N = 20;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 21) -10; // [0,20]-10 == [-10,+10]
      }
      cout << "long array to test solution2 contains:\n";
      for (const auto& elem : arr_long) { cout << elem << ","; } cout << "\b \n";
      run_eval(solution2, arr_long);
   }
   cout << endl;

   {
      cout << "–– solution3 ––\n";
      vector<int> arr = { 1, 5, 2, 1, 4, 0 }; 
      cout << "arr: "; for (const auto& n : arr) { cout << n << ","; } cout << "\b \n";
      run_eval(solution3, arr);
      
      const size_t N = 10;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 21); // [0,20]
      }
      cout << "long array to test solution3 contains: ";
      for (const auto& elem : arr_long) { cout << elem << ","; } cout << "\b \n";
      run_eval(solution3, arr_long);
   }
   cout << endl;

   {
      cout << "–– solution4 ––\n";
      vector<int> arr1 = { 10, 2, 5, 1, 8, 20 };
      cout << "arr1: "; for (const auto& n : arr1) { cout << n << ","; } cout << "\b \n";
      run_eval(solution4, arr1);
      
      const size_t N = 100;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 2001)-1000; // [0,2000]-1000 == [-1000,1000]
      }
      cout << "long array to test solution3 contains: ";
      for (const auto& elem : arr_long) { cout << elem << ", "; } cout << "\b\b \n";
      run_eval(solution4, arr_long);
   }
   cout << endl;
   
   return 0;
}