 #include<benchmark/benchmark.h>
 #include<bits/stdc++.h>
 #include "include/helper.hpp"
 #include<NTL/ZZ.h>         // For BigInteger operations
 #include<NTL/RR.h>         // For Floating point operations.
 
 #define M 100000
 #define precision 40
 
 using namespace NTL;
 using namespace std;

static void BM_add_int(benchmark::State& state){		 	 
     
     int n=state.range(0);
	 char* ar_a=new char[n+1];
	 char* ar_b=new char[n+1];
	 
	 for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 ZZ a,b,c;
		 conv(a,ar_a);
		 conv(b,ar_b);
	     

		 state.ResumeTiming();
		 
         c=a+b;
		 
	}
	delete(ar_a);
	delete(ar_b);
}

static void BM_add_real(benchmark:: State& state){

    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	char* ar_b=new char[n+2];
	
	for(auto _:state){
		
		//Pausing timer for variable declaration and constructors.		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 RR a,b,c;
		 conv(a,ar_a);
		 conv(b,ar_b);
        

		 state.ResumeTiming();

		 c=a+b;
		 
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_multiply_int(benchmark:: State& state){
     int n=state.range(0);
	 char* ar_a=new char[n+1];
	 char* ar_b=new char[n+1];
	 
	 for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 ZZ a,b,c;
		 conv(a,ar_a);
		 conv(b,ar_b);
	     

		 state.ResumeTiming();
		 
         c=a*b;
		 
	}
	delete(ar_a);
	delete(ar_b);
}
static void BM_multiply_real(benchmark:: State& state){

     RR::SetPrecision(precision);
     int n=state.range(0);
	 char* ar_a=new char[n+2];
	 char* ar_b=new char[n+2];
	 
	 for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 RR a,b,c;
		 conv(a,ar_a);
		 conv(b,ar_b);
	     

		 state.ResumeTiming();
		 
         c=a*b;
		 
	}
	delete(ar_a);
	delete(ar_b);
}
static void BM_divide_int(benchmark:: State& state){

    int n=state.range(0);
	char* ar_a=new char[n+1];
	char* ar_b=new char[n+1];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 ZZ a,b,c;
		 conv(a,ar_a);
		 conv(b,ar_b);
		 
		 state.ResumeTiming();
         c=a/b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_divide_real(benchmark:: State& state){

    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	char* ar_b=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 RR a,b,c;
		 
		 conv(a,ar_a);
		 conv(b,ar_b);

		 state.ResumeTiming();
 
        c=a/b;
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_power(benchmark::State& state){

    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	char* ar_b=new char[n+2];
	 
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 RR a,b,c;
		 conv(a,ar_a);
		 conv(b,ar_b);
		 
		 state.ResumeTiming();
 
		 c=pow(a,b);

	 }
	 delete(ar_a);
	 delete(ar_b);
}

static void BM_sqrt(benchmark::State& state){

    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
		 RR a,b;
		 conv(a,ar_a);
		 state.ResumeTiming();
		 
		 b=sqrt(a);
	 }
	 delete(ar_a);
}

static void BM_exp(benchmark::State& state){
    
    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
		 RR a,b;
		 conv(a,ar_a);
	     
		 state.ResumeTiming();
		 
		 b=exp(a);

	 }
	 delete(ar_a);
}

static void BM_log(benchmark::State& state){

    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
		 RR a,b;
		 conv(a,ar_a);
	     
		 state.ResumeTiming();
		 
		 b=log(a);
		 
	 }
	 delete(ar_a);
}

static void BM_sin(benchmark::State& state){

    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
		 RR a,b;
		 conv(a,ar_a);
	     
		 state.ResumeTiming();
		 b=sin(a);
		 
	 }
	 delete(ar_a);
}

static void BM_cos(benchmark::State& state){

    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
		 RR a,b;
		 conv(a,ar_a);
	     
		 state.ResumeTiming();
		 
		 b=cos(a);

	 }
	 delete(ar_a);
}

static void BM_tan(benchmark::State& state){
    RR::SetPrecision(precision);
    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
		 RR a,b;
		 conv(a,ar_a);
	     
		 state.ResumeTiming();
		 
		 b=sin(a)/cos(a);

	 }
	 delete(ar_a);
}


 BENCHMARK(BM_add_int)->RangeMultiplier(10)->Range(10,M);
 BENCHMARK(BM_add_real)->RangeMultiplier(10)->Range(10,M);
 
 BENCHMARK(BM_multiply_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_multiply_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_divide_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_divide_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_power)->RangeMultiplier(8)->Range(8,8);
 BENCHMARK(BM_sqrt)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_exp)->RangeMultiplier(8)->Range(8,8);
 
 BENCHMARK(BM_sin)->RangeMultiplier(8)->Range(8,64);
 BENCHMARK(BM_cos)->RangeMultiplier(8)->Range(8,64);
 BENCHMARK(BM_tan)->RangeMultiplier(8)->Range(8,64);
 
 BENCHMARK_MAIN();
