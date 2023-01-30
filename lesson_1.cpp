#include <iostream>
using std::cout;
using std::endl;
#include <string>
#include <vector>
#include <iomanip>   // setw
#include <cmath>     // ceil,log2
#include <cassert>
#include <time.h>    // clock_gettime
#include <bitset>
#include <algorithm> // max

#define ANSI_COLOR_CYAN     "\e[96;1m"
#define ANSI_COLOR_RED      "\e[31;1m"
#define ANSI_COLOR_YELLOW   "\e[93;1m"
#define ANSI_COLOR_RESET    "\e[0m"


long nanos() {
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
   return ts.tv_nsec + ts.tv_sec*1000000000L;
}

// get the clocks's resolution on this machine
void print_clock_resolution(clockid_t clockid) {
   struct timespec ts;
   clock_getres(clockid,&ts);
   cout << "resolution of clock with clockid=" << clockid << " (sec,nsec): " << ts.tv_sec << "," << ts.tv_nsec << "\n";
}

// ––– Codility - Lesson 1 –––
/*
   assumptions:
      N in [1,..,2**31-1]
         => need at least 4-byte int
         => N contains at least one 1 (one set bit)
   optimization ideas:
      depending on whether lower or higher numbers appear most often:
         lower  nums more frequent => start at MSB, shift <<
         higher nums more frequent => start at LSB, shift >>
*/
int solution_1(int N) {
   //–––––––––––––––––––––––––––––––––––
   // check bit by bit, starting at LSB 
   //–––––––––––––––––––––––––––––––––––
   int N_orig = N;

   // get rid of all lower bit 0's
   while ((N & 0x01) == 0) {
      N = N >> 1;
   }

   // current LSB **has to be** a 1, at this point
   assert( N & 0x01 );
   
   // Checking special case of only one 1 for early out,
   // when current LSB is 1 and N >> 1 is 0 => then I'm done, as no gap will be found,
   // otherwise there are at least two 1's
   if ( ((N & 0x01) == 1) && ((N >> 1) == 0) )  {
      cout << ANSI_COLOR_RED << "only one 1 in " << N_orig << ANSI_COLOR_RESET << "\n";
      return 0;
   }

   // contains at least two 1s
   int cur_gap_len = 0;
   int max_gap_len = 0;
   // advance one more => this acts as start marker of first 1 found - there will be at least one other 1
   N = N >> 1;

   for (; N; N>>=1) {
      if ((N & 0x01) == 1) {
         // found '1'
         // check if current sequence is largest
         if (cur_gap_len > max_gap_len) {
            max_gap_len = cur_gap_len;
         }
         // reset cur_gap_len => end of current sequence automatically indicates start of new one
         cur_gap_len = 0;
      } else {
         // found '0'
         cur_gap_len++;
      }
   }

   return max_gap_len;
}

int solution_2(int N) {
   int num_bits_needed = (int) std::ceil(std::log2(N));
   //cout << "to represent " << N << ", need " << num_bits_needed << " bits\n";
   
   // note: bitset stores bits in 'little-endian style', i.e.: std::bitset<4> bs(num); bs[0] == LSB in num
   auto bs = std::bitset<32>(N);

   int cur_gap_len = 0;
   int max_gap_len = 0;
   bool found_one = false;

   for (int i=0; i<num_bits_needed; ++i) {
      if (found_one && bs[i]==0) {
      //if (found_one && (N&(1<<i))==0) {
         cur_gap_len++;
      }
      else if (bs[i]==1) {
      //else if ((N&(1<<i))==1) {
         if (found_one) {
            max_gap_len = std::max(max_gap_len,cur_gap_len);
            cur_gap_len = 0;
         } else {
            found_one = true;
         }
      }
   }
   // note: using std::bitset<N>::operator[] **is slower** than using plain << + &
   
   return max_gap_len;
}

void run_eval(int(*solution)(int N), int n, const int N_BITS) {
   long start = nanos();
   int max_gap_len = solution(n);
   long elapsed_ns = nanos() - start;
   auto fw = (int) std::ceil(std::log10(1<<N_BITS));
   cout << "num: " << ANSI_COLOR_YELLOW << std::setw(fw) << n << ANSI_COLOR_RESET << " => longest binary gap: ";
   cout << ANSI_COLOR_CYAN << std::setw(2) << max_gap_len << ANSI_COLOR_RESET << "\n";
   cout << "took " << elapsed_ns << " ns -- (num from bitset: " << std::bitset<32>(n) << ")\n";
}


int main()
{
   print_clock_resolution(CLOCK_MONOTONIC_RAW);

   // inputs
   const int N_BITS = 16; // 1<<16 == 65536
   std::vector<int> nums = {
      9, 529, 20, 15, 32, 1041,
      (1<<N_BITS)-1, 
      (1<<N_BITS), 
      (int) std::bitset<32>("1100000000001101").to_ulong()
    };
   //int nums[] = { ... }
   //size_t N_nums = sizeof(nums)/sizeof(nums[0])
   for (int i=0; i<2; ++i) {
      nums.push_back( rand() % (1<<N_BITS) );
   }

   cout << "––– solution 1 –––\n";
   for (const auto& e : nums) {
      run_eval(solution_1,e,N_BITS);
   }

   cout << "––– solution 2 –––\n";
   for (const auto& e : nums) {
      run_eval(solution_2,e,N_BITS);
   }
   
   return 0;
}
