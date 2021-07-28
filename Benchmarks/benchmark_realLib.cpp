 #include<benchmark/benchmark.h>
 #include "include/helper.hpp"
 #include  "Real.h"
 #define M 100000
 #define precision 40
 using namespace RealLib;
 using namespace std;
 
 static void BM_Initialize(benchmark::State& state){
    for(auto _:state){
        InitializeRealLib();
        std::cout << fixed << setprecision(precision);
    }
 }
 
 static void BM_add_int(benchmark::State& state){
	 
	 int n=state.range(0);
	 char* ar_a=new char[n+1];
	 char* ar_b=new char[n+1];
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 Real a(ar_a);
         Real b(ar_b);

		 state.ResumeTiming();

		 Real sum=a+b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}

static void BM_add_real(benchmark:: State& state){

     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 char* ar_b=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
        
		 
		 Real a(ar_a);
         Real b(ar_b);
         
		 state.ResumeTiming();

		 Real sum=a+b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_multiply_int(benchmark:: State& state){
	 int n=state.range(0);
	 char* ar_a=new char[n+1];
	 char* ar_b=new char[n+1];
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 Real a(ar_a);
         Real b(ar_b);

		 state.ResumeTiming();

		 Real res=a*b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_multiply_real(benchmark:: State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 char* ar_b=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
        
		 
		 Real a(ar_a);
         Real b(ar_b);
         
		 state.ResumeTiming();

		 Real res=a*b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_divide_int(benchmark:: State& state){
	 int n=state.range(0);
	 char* ar_a=new char[n+1];
	 char* ar_b=new char[n+1];
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 Real a(ar_a);
         Real b(ar_b);

		 state.ResumeTiming();

		 Real res=a/b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_divide_real(benchmark:: State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 char* ar_b=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
        
		 
		 Real a(ar_a);
         Real b(ar_b);
         
		 state.ResumeTiming();

		 Real res=a/b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_power(benchmark::State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 char* ar_b=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
        
		 
		 Real a(ar_a);
         Real b(ar_b);
         
		 state.ResumeTiming();

		 Real res=exp(b*log(a));
	 }
	 delete(ar_a);
	 delete(ar_b);
}

static void BM_sqrt(benchmark::State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
        
		 
		 Real a(ar_a);
         
		 state.ResumeTiming();

		 Real res=sqrt(a);
	 }
	 delete(ar_a);
}

static void BM_exp(benchmark::State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
        
		 
		 Real a(ar_a);
         
		 state.ResumeTiming();

		 Real res=exp(a);
	 }
	 delete(ar_a);
}

static void BM_log(benchmark::State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
        
		 
		 Real a(ar_a);
         
		 state.ResumeTiming();

		 Real res=log(a);
	 }
	 delete(ar_a);
}

static void BM_sin(benchmark::State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
        
		 
		 Real a(ar_a);
         
		 state.ResumeTiming();

		 Real res=sin(a);
	 }
	 delete(ar_a);
}

static void BM_cos(benchmark::State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
        
		 
		 Real a(ar_a);
         
		 state.ResumeTiming();

		 Real res=cos(a);
	 }
	 delete(ar_a);
}

static void BM_tan(benchmark::State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 
	for(auto _:state){
		 
		 state.PauseTiming();
		 
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
        
		 
		 Real a(ar_a);
         
		 state.ResumeTiming();

		 Real res=tan(a);
	 }
	 delete(ar_a);
}

 BENCHMARK(BM_Initialize);
 BENCHMARK(BM_add_int)->RangeMultiplier(10)->Range(10,M);
 BENCHMARK(BM_add_real)->RangeMultiplier(10)->Range(10,M);
 
 BENCHMARK(BM_multiply_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_multiply_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_divide_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_divide_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_power)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_sqrt)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_exp)->RangeMultiplier(8)->Range(8,1024);
 
 BENCHMARK(BM_sin)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_cos)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_tan)->RangeMultiplier(8)->Range(8,1024);
 
 BENCHMARK_MAIN();
