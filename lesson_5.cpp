#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>         // rand()
//#include <unordered_map>
#include <array>
#include <algorithm>       // std::max, std::find
#include <time.h>          // clock_gettime, struct timespec

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


// Codility - Lesson 5-1, CountDiv
/*
   returns the number of integers within the range [A..B] that are divisible by K,
   i.e: {i : A<=i<=B, i mod K = 0}

   - A and B are ints   within range [0..2,000,000,000]
   - K is an int        within range [1..2,000,000,000]
   - A<=B

   note: 2**31-1 == 2,147,483,647
*/
long find_aligned_start_pos(long n,long k) {
   // see also: MAKE_ALIGNMENT_SIZE(size) – Wietze: "Automotive Embedded Systems"
   // Find next number, starting from n, that is divisibly by K.
   // n: number to start from (result will be >= n)
   // k: alignment requirement/width of block-size we align to => return_val%k==0
   /*
      e.g.: k=4
          0  1  2  3  4  5  6  7  8  9 10 11 12 13 ...
          v  v  v  v  v  v  v  v  v  v  v  v  v  v  ...
          0  4  4  4  4  8  8  8  8 12 12 12 12 16 ...

   */
   // CAREFUL: n+(k-1) can overflow/undefined behaviour !!!
   return ((n+(k-1))/k)*k;
}
int solution1(int A, int B, int K) {
   // PYTHON: list(range(A,B+1,K))
   /*int cnt = 0;
   bool found_first = false;
   for (int i=A; i<=B; i++) {
      if ((i%K)==0)
         cnt++;
   }
   return cnt;*/

   if (A==B) {
      return (A%K==0);
   } else {
      const long start = find_aligned_start_pos(std::max(A,K),K);
      const int N_vals = (B-start+1);
      return find_aligned_start_pos(N_vals,K)/K + (A==0);
   }
}

// Codility - Lesson 5-2, GenomicRangeQuery
/*
   - DNA sequence: non-empty string S consisting of letters A,C,G,T (corresponding to nucleotides)
      S = S[0]S[1]...S[N-1]
   - each nucleotide has an impact factor (map: c in {A,C,G,T} -> impact_factor[c] in {1,2,3,4})
         std::unordered_map<char,int> impact_factors = {
            {'A',1},
            {'C',2},
            {'G',3},
            {'T',4},
         };
   - goal: answer several (M) queries of the form
            find min impact factor of nucleotides contained in a particular part of the given DNA sequence
   - queries given by non-empty arrays P and Q, each consisting of M ints (P.size()==Q.size()),
     => P[k] and Q[k] are indices of S in [0,N-1]
   - K-th query (0<=K<M):
      find min impact factor of nucleotides contained in S between [begin,end]=[P[K],Q[K]],
         P[K] <= Q[K], with 0<=K<M
*/
vector<int> solution2(string &S, vector<int> &P, vector<int> &Q) {

   const size_t N = S.size();
   // separate prefix_sums for 'A','C','G'
   vector<vector<int>> prefix_sums(3,vector<int>(N+1));
   // add 1 every time the respective letter is encountered
   for (size_t i=1; i<(N+1); ++i) {
      prefix_sums[0][i] = prefix_sums[0][i-1] + (S[i]=='A');
      prefix_sums[1][i] = prefix_sums[1][i-1] + (S[i]=='C');
      prefix_sums[2][i] = prefix_sums[2][i-1] + (S[i]=='G');
   }

   auto idx2letter = [](size_t idx) -> char {
      return idx==0 ? 'A' :
             idx==1 ? 'C' :
             idx==2 ? 'G' :
             '?';
   };
   auto letter2idx = [](char letter) -> size_t {
      return letter=='A' ? 0:
             letter=='C' ? 1:
             letter=='G' ? 2:
             0;
   };

   // checks if letter is in range [P,Q] of S, with letter in ['A','C','G']
   auto letter_is_in_range = [&letter2idx,&prefix_sums](int P,int Q,char letter) -> bool {
      const auto i = letter2idx(letter);
      return ((prefix_sums[i][Q+1] - prefix_sums[i][P]) > 0);
   };
   // compute min in range [P,Q] of S by using prefix sums
   auto min_impact_factor = [&letter_is_in_range](int P,int Q) -> int {
      return letter_is_in_range(P,Q,'A') ? 1 :
             letter_is_in_range(P,Q,'C') ? 2 :
             letter_is_in_range(P,Q,'G') ? 3 :
             4;
   };

   // print all prefix sums
   for (size_t i=0; i<prefix_sums.size(); ++i) {
      cout << "prefix sums of " << idx2letter(i) << ": ";
      for (const auto& e : prefix_sums[i]) { cout << e << ","; }
      cout << "\b \n";
   }

   const size_t M_queries = P.size();
   vector<int> queries_results(M_queries);
   for (size_t k=0; k<M_queries; ++k) {
      queries_results[k] = min_impact_factor(P[k],Q[k]);
   }

   return queries_results;
}


// Codility - Lesson 5-3, MinAvgTwoSlice
// goal: find starting position of a slice whose average is minimal
/*
   - array/vector A (of N elems), given by {a_0, a_1, ..., a_{N-1}}
   - prefix sum p_k defined by 'p_k := p_{k-1}+a_{k-1}, p_0:=0, k=0,..,N'

   A `slice` of array/vector A defined by a pair of ints (P,Q), s.t. 0<=P<Q<N.
      => P and Q are indices of A in {0,1,..,N-1}
      => P and Q are both inclusive
      => P in {0,1,..,N-2}
      => Q in {1,2,..,N-1}
   
   The `sum of slice` (P,Q), with 0<=P<Q<N, is:       (A[P] + A[P+1] + ... + A[Q])
      def slice_sum(prefix_sums,P,Q): return (prefix_sums[P+1]-prefix_sums[Q])
   
   The `average of slice` (P,Q), with 0<=P<Q<N, is:   (A[P] + A[P+1] + ... + A[Q]) / (Q-P+1)
      def slice_average(prefix_sums,P,Q): return (prefix_sums[P+1]-prefix_sums[Q]) / (Q-P+1)
   
      A (N elems): a_{0:N-1}
            +—————+—————+—————+—————+—————————+
            | a_0 | a_1 | a_2 | ... | a_{N-1} |
            +—————+—————+—————+—————+—————————+

      prefix_sums (N+1 elems): p_{0:N}
      +—————+—————+—————————+—————————————+—————+—————————————————————+
      | 0   | a_0 | a_0+a_1 | a_0+a_1+a_2 | ... | a_0+a_1+...+a_{N-1} |
      +—————+—————+—————————+—————————————+—————+—————————————————————+
        p_0   p_1   p_2       p_3           ...   p_{N}
   
   - strategy: check all total sums of same len, starting with longest
         
         1) |··········|   (0,N-1)

         2) |·········#|   (0,N-2)
            |#·········|   (1,N-1)
         
         3) |········##|   (0,N-3)
            |#········#|   (1,N-2)
            |##········|   (2,N-1)
         
         4) |·······###|   (0,N-4)
            |#·······##|   (1,N-3)
            |##·······#|   (2,N-2)
            |###·······|   (3,N-1)
      => this is still O(N^2) !!
   - all possible slices (P,Q) s.t. 0<=P<Q<N (a slice contains at least 2 elements)
      #slices == #entries of all elements of an upper triangular matrix of size (N-1)x(N-1)
              == #(entries above diagonal) + #(entries of diagonal)
              == (sum_{n=1}^{N-2} n)  +  (N-1)
              == (N-2)(N-1)/2 + (N-1)
              or
              == sum_{n=1}^{N-1} n == (N-1)N/2
              == num of pairs (P,Q)

      p = 0, 1, 2, ..., N-2
      q = 1, 2, 3, ..., N-1
      (p,q) in

         (0,1), (0,2), (0,3), ..., (0,N-2), (0,N-1)   // (N-1)-1+1 = (N-1) terms
         (1,2), (1,3), (1,4), ..., (1,N-2), (1,N-1)   // (N-1)-2+1 = (N-2) terms
         ...
         (N-3,N-2), (N-3,N-1)
         (N-2,N-1)
   - key to solve this: only have to consider 2-elem and 3-elem slices:
         (note: max_elem >= avg >= min_elem, max_elem==avg==min_elem <=> all elems are identical)
         https://codesays.com/2014/solution-to-min-avg-two-slice-by-codility/
         https://math.stackexchange.com/questions/2682379/will-the-length-of-a-minimum-average-slice-of-a-numeric-array-ever-be-greater-th
*/
vector<int> calc_prefix_sums(const vector<int> &A) {
   const size_t N = A.size();
   vector<int> prefix_sums(N+1);
   prefix_sums[0] = 0;
   for (size_t i=1; i<(N+1); ++i) {
      prefix_sums[i] = prefix_sums[i-1] + A[i-1];
   }
   return prefix_sums;
}
int slice_sum(const vector<int>& prefix_sums, int P, int Q) {
   return (prefix_sums[Q+1]-prefix_sums[P]);
}
float slice_average(const vector<int>& prefix_sums, int P, int Q) {
   // preconditions:
   // (A.size()==N) && (prefix_sums.size()==N+1) &&
   // (P<Q) && (P in {0,1,..,N-2}) && (Q in {1,2,..,N-1})
   const int N_elems = Q-P+1;
   const float avg = slice_sum(prefix_sums,P,Q) / (float)N_elems;
   cout << "P,Q -> avg: " << P << "," << Q << " -> " << avg << "\n";
   return avg;
}
int solution3(vector<int> &A) {
   
   const size_t N = A.size();
   float min_avg_val = (A[0]+A[1])/2.0;
   int   min_avg_pos = 0;

   /*
      |–––––| e.g.: N=5

      |––   | i=0
      |–––  |

      | ––  | i=1
      | ––– |

      |  –– | i=2
      |  –––|

      |   ––| last 2-elem slice
   */
   
   for (size_t i=0; i<(N-2); ++i) {
      const int sum2 = A[i]+A[i+1];
      const int sum3 = sum2+A[i+2];
      const float avg2 = sum2/2.0;
      const float avg3 = sum3/3.0;
      // 2-elem slice
      if (avg2 < min_avg_val) {
         min_avg_val = avg2;
         min_avg_pos = i;
      }
      // 3-elem slice
      if (avg3 < min_avg_val) {
         min_avg_val = avg3;
         min_avg_pos = i;
      }
   }

   // last 2-elem slice
   const int sum2 = A[N-2]+A[N-1];
   const float avg2 = sum2/2.0;
   if (avg2 < min_avg_val) {
      min_avg_val = avg2;
      min_avg_pos = N-2;
   }

   return min_avg_pos;
}

// Codility - Lesson 5-4, PassingCars
/*
   array A of size N, only 0s and 1s
      0: car traveling east   -->
      1: car traveling west   <--
   (P,Q): pair of cars, 0<=P<Q<N
   a pair of cars (P,Q) is passing, when P:0, Q:1
   
   goal: find the number of pairs of passing cars

   other strategy:
   - first find all indices k where A[k]==0
      (https://stackoverflow.com/questions/25846235/finding-the-indexes-of-all-occurrences-of-an-element-in-a-vector)
   - then only iterate over those zero-indices

*/
// find all indices i in vector vec, for which vec[i]==val
template<typename T>
vector<int> find_all_indices_of(const vector<T>& vec, const T& val) {
   vector<int> indices;
   typename vector<T>::const_iterator it = vec.begin();
   
   while ((it=std::find(it,vec.end(),val)) != vec.end()) {
      indices.push_back( it-vec.begin() );
      ++it;
   }
   
   return indices;
}
int solution4(vector<int> &A) {
   const size_t N = A.size();
   const std::vector<int> prefix_sums = calc_prefix_sums(A);
   long cnt = 0;
   
   for (size_t p=0; p<N-1; ++p) {
      if (A[p]==1) continue;
      
      // here: A[p]==0
      cnt += slice_sum(prefix_sums,p,N-1); // == (sum_{q=p+1}^{N-1} A[q]), but in O(1) !!!
      //for (size_t q=p+1; q<N; ++q) {
      //   if (A[q]==1) cnt++;
      //}
   }

   return (cnt > 1000000000) ? -1 : cnt;
}

void run_eval_1(int (*solution1)(int A, int B, int K), int A, int B, int K) {
   long start = nanos();
   int res = solution1(A, B, K);
   long elapsed_ns = nanos() - start;
   cout << "[A,B],K: " << "[" << A << "," << B << "]," << K << "\n";
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_2(vector<int> (*solution2)(string &S, vector<int> &P, vector<int> &Q), string &S, vector<int> &P, vector<int> &Q) {
   long start = nanos();
   vector<int> res = solution2(S, P, Q);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " ";
      for (const auto& r : res) { cout << r << " "; }
      cout << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_3(int (*solution3)(vector<int> &A), vector<int> &A) {
   long start = nanos();
   int res = solution3(A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

void run_eval_4(int (*solution4)(vector<int> &A), vector<int> &A) {
   long start = nanos();
   int res = solution4(A);
   long elapsed_ns = nanos() - start;
   cout << "result: " << ANSI_COLOR_BG_CYAN << " " << res << " " << ANSI_COLOR_RESET;
   cout << ", took " << elapsed_ns << " ns, or " << elapsed_ns/1000.0 << " µs\n";
}

string generate_long_dna_string() {
   // 1<<10 == 1024
   //    1K == 1024
   //    1M == 1024**2
   //    1G == 1024**3
   const long long BUF_SIZE = 20; // (1<<10)*200 ≈ 200000
   cout << "creating random DNA string of length " << BUF_SIZE << endl;
   std::array<char,BUF_SIZE> buf;
   std::array<char,4> dna_char_lut = {'A','C','G','T'};
   for (char& c : buf) {
      // generate random char in set ['A','C','G','T']
      c = dna_char_lut[rand()%4];
   }
   buf.back() = '\0';
   string str(buf.data());
   return str;
}


int main()
{
   {
      cout << "–– solution1 ––\n";
      int A, B, K;
      
      A=6, B=11, K=2;
      run_eval_1(solution1, A, B, K);
      
      A=0, B=20, K=2;
      run_eval_1(solution1, A, B, K);

      A=11, B=14, K=2;
      run_eval_1(solution1, A, B, K);

      A=1, B=13, K=4;
      run_eval_1(solution1, A, B, K);
   }
   cout << endl;
   
   {
      cout << "–– solution2 ––\n";
      string S = "CAGCCTA";
      vector<int> P = {2,5,0};
      vector<int> Q = {4,5,6};

      auto print_P_Q = [&P,&Q]() {
         cout << "Using P=";
         for (const auto& elem : P) { cout << elem << ","; } cout << "\b \n";
         cout << "Using Q=";
         for (const auto& elem : Q) { cout << elem << ","; } cout << "\b \n";
      };
      
      cout << "Using string (of size " << S.size() << "): " << S << "\n";
      print_P_Q();
      run_eval_2(solution2, S, P, Q);


      S = generate_long_dna_string();
      const int M_queries = 20;
      P.resize(M_queries);
      Q.resize(M_queries);
      for (int i=0; i<M_queries; ++i) {
         P[i] = rand() % S.size(); // [0..N − 1];
         // make sure that Q[i] >= P[i]
         do {
            Q[i] = rand() % S.size();
         } while (!(Q[i]>=P[i]));
      }
      
      cout << "Using string (of size " << S.size() << "): " << S << "\n";
      print_P_Q();
      run_eval_2(solution2, S, P, Q);
   }
   cout << endl;

   {
      cout << "–– solution3 ––\n";
      vector<int> arr = { 4, 2, 2, 5, 1, 5, 8 };
      cout << "arr: ";
      for (const auto& n : arr) { cout << n << ","; } cout << "\b \n";
      
      run_eval_3(solution3, arr);
      
      const size_t N = 20;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 2001) -1000; // [0,2000]-1000 == [-1000,+1000]
      }
      cout << "long array (of size " << arr_long.size() << ") to test solution3 contains:\n";
      for (const auto& elem : arr_long) { cout << elem << ", "; } cout << "\b\b \n";
      
      run_eval_3(solution3, arr_long);
   }
   cout << endl;
   
   {
      cout << "–– solution4 ––\n";
      vector<int> arr1 = { 0, 1, 0, 1, 1 };
      cout << "arr1: ";
      for (const auto& n : arr1) { cout << n << ", "; } cout << "\b\b \n";
      
      run_eval_4(solution4, arr1);
      
      const size_t N = 60;
      vector<int> arr_long(N);
      for (auto& elem : arr_long) {
         elem = (rand() % 2); // [0,1]
      }
      cout << "long array (of size " << arr_long.size() << ") to test solution4 contains:\n";
      for (const auto& elem : arr_long) { cout << elem << " "; } cout << "\n";

      vector<int> indices = find_all_indices_of(arr_long,0);
      cout << "all " << indices.size() << " indices where val==0: ";
      for (const auto& e : indices) { cout << e << " "; } cout << "\n";


      run_eval_4(solution4, arr_long);
   }
   cout << endl;
   
   return 0;
}
